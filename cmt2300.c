#include "cmt2300.h"
#include "_cmt2300Reg.h"
#define Cmt2300DelayTime_us_SpiAftCsEn 2
#define Cmt2300DelayTime_us_SpiBfCsDis 2
#define Cmt2300DelayTime_us_SpiAftCsDis 2

#define Cmt2300DelayTime_us_FifoAftCsEn 2
#define Cmt2300DelayTime_us_FifoBfCsDis 3
#define Cmt2300DelayTime_us_FifoAftCsDis 5

#define Cmt2300DelayTime_us_aftReset 30000
#define Cmt2300DelayTime_us_swFromStb2RxTx 360

#define Cmt2300DelayTick_us_aftSwBfCheck 20
#define Cmt2300Timeout_Tick_sw 50
#define Cmt2300DelayTick_us_aftTx 100
#define Cmt2300Timeout_Tick_tx 50
#define Cmt2300Timeout_us_tx 50000

#define Cmt2300_ChipFsm_Idle 0
#define Cmt2300_ChipFsm_Sleep 1
#define Cmt2300_ChipFsm_Standby 2
#define Cmt2300_ChipFsm_Rfs 3
#define Cmt2300_ChipFsm_Tfs 4
#define Cmt2300_ChipFsm_Rx 5
#define Cmt2300_ChipFsm_Tx 6
#define Cmt2300_ChipFsm_Locking 8

#define Cmt2300_ModeSwitchCmd_standby 0b00000010
#define Cmt2300_ModeSwitchCmd_rfs____ 0b00000100
#define Cmt2300_ModeSwitchCmd_rf_____ 0b00001000
#define Cmt2300_ModeSwitchCmd_sleep__ 0b00010000
#define Cmt2300_ModeSwitchCmd_tfs____ 0b00100000
#define Cmt2300_ModeSwitchCmd_tx_____ 0b01000000
#define Cmt2300_ModeSwitchCmd_swRxTx_ 0b10000000
#define Cmt2300_ModeSwitchCmd_

static const uint8_t Cmt2300_ModeSwitchCmds[] =
    {
        Cmt2300_ModeSwitchCmd_sleep__, Cmt2300_ModeSwitchCmd_standby,
        Cmt2300_ModeSwitchCmd_rfs____, Cmt2300_ModeSwitchCmd_tfs____,
        Cmt2300_ModeSwitchCmd_rf_____, Cmt2300_ModeSwitchCmd_tx_____};

// 初始化流程
// Cmt2300Fsm_Boot 上电：等待初始化开始（进软复位）命令
// Cmt2300Fsm_Rst 软复位：结束后等30ms进寄存器配置
// Cmt2300Fsm_Init 寄存器配置
enum
{
    Cmt2300Fsm_Boot = 0,
    Cmt2300Fsm_Rst,
    Cmt2300Fsm_RstWait,

    // init
    // Cmt2300Fsm_Init,
    Cmt2300Fsm_Init_mdf_Sta,
    Cmt2300Fsm_Init_mdf_lockEn,
    Cmt2300Fsm_Init_mdf_lfoscDis,
    Cmt2300Fsm_Init_clrInt1,
    Cmt2300Fsm_Init_clrInt2,
    Cmt2300Fsm_Init_cfgReg,
    Cmt2300Fsm_Init_goSlp,

    Cmt2300Fsm_WaitSpi,

    /// chip fsm
    // 设置状态
    Cmt2300Fsm_ChipFsmSet,
    // 等spi发完
    Cmt2300Fsm_ChipFsmWaitCmdSpi,
    // 等一段时间后读状态
    Cmt2300Fsm_ChipFsmGetReg,
    // 判断有没有到达状态
    Cmt2300Fsm_ChipFsmJudgeReg,

    // reg modify
    Cmt2300Fsm_RegMdf_read,
    Cmt2300Fsm_RegMdf_write,

    Cmt2300Fsm_FifoWrEn,
    Cmt2300Fsm_FifoWriteData,
    Cmt2300Fsm_TxData,
    Cmt2300Fsm_TxStartTimer,
    Cmt2300Fsm_TxWaitIntSignal,
    Cmt2300Fsm_TxEndCheck2Stb,

    Cmt2300Fsm_ready2Tx,
    Cmt2300Fsm_ready2Rx,
    Cmt2300Fsm_UnderRx,
    Cmt2300Fsm_Rx1ByteOk,
    // Cmt2300Fsm_TxWaitModeChange,
    // Cmt2300Fsm_TxWaitTimeout,
    // Cmt2300Fsm_TxCheckTxOk,
    // Cmt2300Fsm_dbg,
    Cmt2300Fsm_RunUsrCbk,
    Cmt2300Fsm_Idle,
    Cmt2300Fsm_,
};

enum
{
    Cmt2300_SpiFsm_Idle = 0,
    Cmt2300_SpiFsm_Required,

    Cmt2300_SpiFsm_DelayAfterCsEn,
    Cmt2300_SpiFsm_DataTrans,
    Cmt2300_SpiFsm_WaitBus,
    Cmt2300_SpiFsm_DelayBeforeCsDis,
    Cmt2300_SpiFsm_DelayAfterCsDis,
    // Cmt2300_SpiFsm_FifoWrite,
    // Cmt2300_SpiFsm_,
    // Cmt2300_SpiFsm_,

    Cmt2300_SpiFsm_RegReadRequired,
    Cmt2300_SpiFsm_RegWriteRequired,
    Cmt2300_SpiFsm_FifoReadRequired,
    Cmt2300_SpiFsm_FifoWriteRequired,
    Cmt2300_SpiFsm_,
};

// extern const uint16_t cmtRegCfgDataLen;
extern const uint16_t cmtRegCfgData[];

static void cmt2300OnSpiTxCpltCbk(void *handle) { ((cmt2300Handle_t *)handle)->busBusy = 0; }

// #define cmt2300SpiTx(_handle, _buf, _len) (_handle)->cfg.api.spiWriteReadAsync(Cmt2300_SpiDir_Tx, (_buf), (_len), cmt2300OnSpiTxCpltCbk, (_handle))

static inline void cmt2300ModifyReg(cmt2300Handle_t *handle, uint8_t reg, uint8_t set, uint8_t clr, uint8_t target)
{
    // 内部函数，不会在spi busy时调用，不检查spiFsm
    handle->regBuf[0] = reg | 0x80;
    handle->regBuf[2] = set;
    handle->regBuf[3] = clr;

    handle->isRx = 1;
    handle->isOperateFifo = 0;
    handle->spiFsm = Cmt2300_SpiFsm_Required;
    handle->fsm = Cmt2300Fsm_RegMdf_read;
    handle->targetFsm = target;
}

static inline void cmt2300SendRegBuf(cmt2300Handle_t *handle, uint8_t reg, uint8_t d, uint8_t target)
{
    // handle->fsm = Cmt2300Fsm_DelayAfterCsEn;
    // 内部函数，不会在spi busy时调用，不检查spiFsm
    // if (handle->spiFsm != Cmt2300_SpiFsm_Idle)
    //     return;
    handle->regBuf[0] = reg;
    handle->regBuf[1] = d;
    handle->isRx = 0;
    handle->isOperateFifo = 0;
    handle->spiFsm = Cmt2300_SpiFsm_Required;
    handle->fsm = Cmt2300Fsm_WaitSpi;
    handle->targetFsm = target;
}

static inline void cmt2300ReadRegBuf(cmt2300Handle_t *handle, uint8_t reg, uint8_t target)
{
    // if (handle->spiFsm != Cmt2300_SpiFsm_Idle)
    //     return;
    handle->isRx = 1;
    handle->isOperateFifo = 0;
    // handle->cfg.api.gpioWrite(Cmt2300_PinId_SpiCs, Cmt2300_PinLevel_Low);
    // handle->cfg.api.usTimerCtrl(Cmt2300_TimerCtrlState_RstAndStart);
    handle->regBuf[0] = reg | 0x80;
    handle->fsm = Cmt2300Fsm_WaitSpi;
    handle->spiFsm = Cmt2300_SpiFsm_Required;
    handle->targetFsm = target;
}

static inline void cmt2300SetChipFsm(cmt2300Handle_t *handle, uint8_t fsm, uint8_t target)
{
    // if (handle->spiFsm != Cmt2300_SpiFsm_Idle)
    //     return;
    handle->targetChipFsm = fsm;
    handle->regBuf[0] = Cmt2300_RegAdd_Ctrl_ModeCtrl;
    handle->regBuf[1] = Cmt2300_ModeSwitchCmds[fsm - 1];
    handle->isRx = 0;
    handle->isOperateFifo = 0;
    handle->spiFsm = Cmt2300_SpiFsm_Required;
    handle->fsm = Cmt2300Fsm_ChipFsmWaitCmdSpi;
    handle->targetFsm = target;

    handle->regBuf[3] = 0;
}

static inline void cmt2300GetChipFsm(cmt2300Handle_t *handle)
{
    // if (handle->spiFsm != Cmt2300_SpiFsm_Idle)
        // return 1;
    handle->regBuf[0] = Cmt2300_RegAdd_Ctrl_ModeState | 0x80;
    handle->isRx = 1;
    handle->isOperateFifo = 0;
    handle->spiFsm = Cmt2300_SpiFsm_Required;
    handle->fsm = Cmt2300Fsm_ChipFsmJudgeReg;
    // return 0;
}

// 读写寄存器可以收发2字节
// 读写FIFO只可以收发1字节. such foolish chip go fuck ur self.
// 中断信号来了再读fifo
static inline void cmtSpiLoop(cmt2300Handle_t *handle)
{
    switch (handle->spiFsm)
    {
    // 只有在这个状态的时候才能操作spi
    case Cmt2300_SpiFsm_Idle:
        break;

    // 发起传输请求了
    case Cmt2300_SpiFsm_Required:
        handle->isSpiDoingRegRx = 0;
        // CS使能
        if (handle->isOperateFifo)
            handle->cfg.api.gpioWrite(Cmt2300_PinId_FifoCs, Cmt2300_PinLevel_Low);
        else
            handle->cfg.api.gpioWrite(Cmt2300_PinId_SpiCs, Cmt2300_PinLevel_Low);
        // 开始计时
        handle->cfg.api.usTimerCtrl(Cmt2300_TimerCtrlState_RstAndStart);

        handle->spiFsm = Cmt2300_SpiFsm_DelayAfterCsEn;
        break;

    // CS使能后延时
    case Cmt2300_SpiFsm_DelayAfterCsEn:
    {
        uint32_t waitTime;
        waitTime = ((handle->isOperateFifo) ? Cmt2300DelayTime_us_FifoAftCsEn : Cmt2300DelayTime_us_SpiAftCsEn);

        if (handle->cfg.api.usTimerGetCnt() <= waitTime)
            break;

        handle->spiFsm = Cmt2300_SpiFsm_DataTrans;
    }
    break;

    // 延时完了收发数据
    case Cmt2300_SpiFsm_DataTrans:
    {
        uint8_t mode;
        uint8_t len;
        uint8_t *buf;

        mode = Cmt2300_SpiDir_Tx;
        len = 1;

        if (handle->isOperateFifo)
        {
            if (handle->isRx)
            {
                buf = handle->rxBuf;
                --(handle->rxLen);
                ++(handle->rxBuf);
                mode = Cmt2300_SpiDir_Rx;
                // handle->isRx = 0;
            }
            else
            {
                buf = handle->txBuf;
                ++(handle->txBuf);
                --(handle->txLen);
            }
        }
        else
        {
            if (handle->isSpiDoingRegRx)
            {
                buf = handle->regBuf + 1;
                len = 1;
                mode = Cmt2300_SpiDir_Rx;
            }
            else
            {
                buf = handle->regBuf;

                if (handle->isRx)
                    len = 1;
                else
                    len = 2;
            }
        }

        handle->busBusy = 1;
        handle->cfg.api.spiWriteReadAsync(mode, buf, len, cmt2300OnSpiTxCpltCbk, handle);
        handle->spiFsm = Cmt2300_SpiFsm_WaitBus;
    }
    break;

    case Cmt2300_SpiFsm_WaitBus:
        if (handle->busBusy)
            break;

        if ((handle->isRx) && (!(handle->isOperateFifo)))
        {
            handle->isSpiDoingRegRx = 1;
            handle->isRx = 0;
            handle->spiFsm = Cmt2300_SpiFsm_DataTrans;
        }
        else
        {
            handle->spiFsm = Cmt2300_SpiFsm_DelayBeforeCsDis;
            handle->cfg.api.usTimerCtrl(Cmt2300_TimerCtrlState_RstAndStart);
        }

        break;

    case Cmt2300_SpiFsm_DelayBeforeCsDis:
    {
        uint32_t waitTime;
        waitTime = ((handle->isOperateFifo) ? Cmt2300DelayTime_us_FifoBfCsDis : Cmt2300DelayTime_us_SpiBfCsDis);
        if (handle->cfg.api.usTimerGetCnt() <= waitTime)
            break;
    }
        handle->cfg.api.gpioWrite(((handle->isOperateFifo) ? Cmt2300_PinId_FifoCs : Cmt2300_PinId_SpiCs), Cmt2300_PinLevel_High);
        handle->cfg.api.usTimerCtrl(Cmt2300_TimerCtrlState_RstAndStart);
        handle->spiFsm = Cmt2300_SpiFsm_DelayAfterCsDis;
        break;

    case Cmt2300_SpiFsm_DelayAfterCsDis:
    {
        uint32_t waitTime;
        waitTime = ((handle->isOperateFifo) ? Cmt2300DelayTime_us_FifoAftCsDis : Cmt2300DelayTime_us_SpiAftCsDis);
        if (handle->cfg.api.usTimerGetCnt() <= waitTime)
            break;
    }
        if ((handle->isOperateFifo) && (!(handle->isRx)) && (handle->txLen))
        {
            handle->spiFsm = Cmt2300_SpiFsm_Required;
            break;
        }
        handle->cfg.api.usTimerCtrl(Cmt2300_TimerCtrlState_Stop);
        handle->spiFsm = Cmt2300_SpiFsm_Idle;
        break;

    default:
        break;
    }
}

// note
// 寄存器要在standby状态配置，配完进sleep生效

void cmt2300Loop(cmt2300Handle_t *handle)
{
    cmtSpiLoop(handle);
    switch (handle->fsm)
    {
    case Cmt2300Fsm_Boot:
        break;

        // 初始化过程：
        // 1. 先软复位
        // 2. 等一段时间
        // 3. 进standby
        // 4.1. Disable RSTN_IN
        // 4.2. Enable LOCKING_EN
        // 4.3. Diable LFOSC
        // 4.5. 清除中断
        // 4.6. 配寄存器
        // 4.7. 配gpio
        // 4.8. 配中断
        // 5. 进sleep

    // 等30ms
    case Cmt2300Fsm_Rst:
        handle->fsm = Cmt2300Fsm_RstWait;
        handle->cfg.api.usTimerCtrl(Cmt2300_TimerCtrlState_RstAndStart);
        break;
    case Cmt2300Fsm_RstWait:
        if (handle->cfg.api.usTimerGetCnt() <= Cmt2300DelayTime_us_aftReset)
            break;

        // handle->targetChipFsm = ;
        cmt2300SetChipFsm(handle, Cmt2300_ChipFsm_Standby, Cmt2300Fsm_Init_mdf_Sta);
        break;

    case Cmt2300Fsm_Init_mdf_Sta:
        cmt2300ModifyReg(handle, Cmt2300_RegAdd_Ctrl_ModeState, CMT2300A_MASK_CFG_RETAIN, ~CMT2300A_MASK_RSTN_IN_EN, Cmt2300Fsm_Init_mdf_lockEn);
        break;

    case Cmt2300Fsm_Init_mdf_lockEn:
        cmt2300ModifyReg(handle, Cmt2300_RegAdd_Ctrl_EnCtrl, CMT2300A_MASK_LOCKING_EN, 0xff, Cmt2300Fsm_Init_mdf_lfoscDis);
        // cmt2300ReadRegBuf(handle, Cmt2300_RegAdd_Ctrl_ModeState, Cmt2300Fsm_Idle);
        break;

    case Cmt2300Fsm_Init_mdf_lfoscDis:
        cmt2300ModifyReg(handle, Cmt2300_RegAdd_Sys2, 0,
                         (uint8_t)(~(CMT2300A_MASK_LFOSC_RECAL_EN | CMT2300A_MASK_LFOSC_CAL1_EN | CMT2300A_MASK_LFOSC_CAL2_EN)),
                         Cmt2300Fsm_Init_clrInt1);
        break;

    case Cmt2300Fsm_Init_clrInt1:
        cmt2300SendRegBuf(handle, Cmt2300_RegAdd_Ctrl_Int1Clr, 0x3f, Cmt2300Fsm_Init_clrInt2);
        break;
    case Cmt2300Fsm_Init_clrInt2:
        handle->regBuf[3] = 0;
        cmt2300SendRegBuf(handle, Cmt2300_RegAdd_Ctrl_Int2Clr, 0x3f, Cmt2300Fsm_Init_cfgReg);
        break;

    case Cmt2300Fsm_Init_cfgReg:
    {
        uint16_t rd = cmtRegCfgData[(handle->regBuf[3])];
        if (0xffff == rd)
            handle->fsm = Cmt2300Fsm_Init_goSlp;
        else
        {
            cmt2300SendRegBuf(handle, ((rd >> 8) & 0xff), (rd & 0xff), Cmt2300Fsm_Init_cfgReg);
            ++(handle->regBuf[3]);
        }
    }
    break;

    case Cmt2300Fsm_Init_goSlp:
        cmt2300SetChipFsm(handle, Cmt2300_ChipFsm_Sleep, Cmt2300Fsm_RunUsrCbk);
        break;

    case Cmt2300Fsm_WaitSpi:
        if (Cmt2300_SpiFsm_Idle == handle->spiFsm)
            handle->fsm = handle->targetFsm;
        break;

    case Cmt2300Fsm_ChipFsmWaitCmdSpi:
        if (Cmt2300_SpiFsm_Idle == handle->spiFsm)
        {
            handle->fsm = Cmt2300Fsm_ChipFsmGetReg;
            handle->cfg.api.usTimerCtrl(Cmt2300_TimerCtrlState_RstAndStart);
        }
        break;

    case Cmt2300Fsm_ChipFsmGetReg:
        if (handle->cfg.api.usTimerGetCnt() <= Cmt2300DelayTick_us_aftSwBfCheck)
            break;
        cmt2300GetChipFsm(handle);
        break;

    case Cmt2300Fsm_ChipFsmJudgeReg:
        if (Cmt2300_SpiFsm_Idle == handle->spiFsm)
        {
            if ((handle->regBuf[1] & 0x0f) == handle->targetChipFsm)
                handle->fsm = handle->targetFsm;
            else
            {
                ++(handle->regBuf[3]);
                if (handle->regBuf[3] >= Cmt2300Timeout_Tick_sw)
                {
                    cmt2300SetChipFsm(handle, handle->targetChipFsm, handle->targetFsm);
                }
                else
                {
                    handle->fsm = Cmt2300Fsm_ChipFsmGetReg;
                    handle->cfg.api.usTimerCtrl(Cmt2300_TimerCtrlState_RstAndStart);
                }
            }
        }
        break;

    case Cmt2300Fsm_FifoWrEn:
        if (Cmt2300_SpiFsm_Idle == handle->spiFsm)
            // cmt2300SendRegBuf(handle, Cmt2300_RegAdd_Ctrl_Fifo, CMT2300A_MASK_SPI_FIFO_RD_WR_SEL, Cmt2300Fsm_FifoWriteData);
            cmt2300ModifyReg(handle, Cmt2300_RegAdd_Ctrl_Fifo,
                             CMT2300A_MASK_SPI_FIFO_RD_WR_SEL,
                             0xff, Cmt2300Fsm_FifoWriteData);
        break;

    case Cmt2300Fsm_FifoWriteData:
        handle->isOperateFifo = 1;
        handle->isRx = 0;
        handle->spiFsm = Cmt2300_SpiFsm_Required;
        handle->fsm = Cmt2300Fsm_TxData;
        break;
    case Cmt2300Fsm_TxData:
        if (Cmt2300_SpiFsm_Idle == handle->spiFsm)
            cmt2300SendRegBuf(handle, Cmt2300_RegAdd_Ctrl_ModeCtrl,
                              Cmt2300_ModeSwitchCmd_tx_____,
                              Cmt2300Fsm_TxStartTimer
                              //   Cmt2300Fsm_TxWaitModeChange
            );
        break;

    case Cmt2300Fsm_TxStartTimer:
        handle->cfg.api.usTimerCtrl(Cmt2300_TimerCtrlState_RstAndStart);
        handle->fsm = Cmt2300Fsm_TxWaitIntSignal;
        // break;
    case Cmt2300Fsm_TxWaitIntSignal:
        if (handle->cfg.api.usTimerGetCnt() >= Cmt2300Timeout_us_tx)
        {
            handle->errCode = Cmt2300_ErrCode_TxTimeOut;
            handle->fsm = Cmt2300Fsm_TxEndCheck2Stb;
        }
        break;

    case Cmt2300Fsm_TxEndCheck2Stb:
        cmt2300SetChipFsm(handle, Cmt2300_ChipFsm_Standby, Cmt2300Fsm_RunUsrCbk);
        break;

    // case Cmt2300Fsm_TxWaitModeChange:
    //     if (Cmt2300_SpiFsm_Idle == handle->spiFsm)
    //     {
    //         handle->cfg.api.usTimerCtrl(Cmt2300_TimerCtrlState_RstAndStart);
    //         handle->regBuf[3] = 0;
    //         handle->fsm = Cmt2300Fsm_TxWaitTimeout;
    //     }
    //     break;

    // case Cmt2300Fsm_TxWaitTimeout:
    //     if (handle->cfg.api.usTimerGetCnt() >= Cmt2300DelayTick_us_aftTx)
    //     {
    //         ++handle->regBuf[3];
    //         if ((handle->regBuf[3]) >= Cmt2300Timeout_Tick_tx)
    //         {
    //             handle->errCode = Cmt2300_ErrCode_TxTimeOut;
    //             handle->fsm = Cmt2300Fsm_RunUsrCbk;
    //             handle->cfg.api.usTimerCtrl(Cmt2300_TimerCtrlState_Stop);
    //             break;
    //         }
    //         cmt2300ReadRegBuf(handle, Cmt2300_RegAdd_Ctrl_Int1Clr, Cmt2300Fsm_TxCheckTxOk);
    //     }
    //     break;

    // case Cmt2300Fsm_TxCheckTxOk:
    //     if ((handle->regBuf[1]) & CMT2300A_MASK_TX_DONE_FLG)
    //     {
    //         handle->errCode = Cmt2300_ErrCode_Success;
    //         handle->fsm = Cmt2300Fsm_RunUsrCbk;
    //         break;
    //     }
    //     handle->cfg.api.usTimerCtrl(Cmt2300_TimerCtrlState_RstAndStart);
    //     handle->fsm = Cmt2300Fsm_TxWaitTimeout;
    //     break;
    case Cmt2300Fsm_ready2Rx:
        cmt2300SetChipFsm(handle, Cmt2300_ChipFsm_Rx, Cmt2300Fsm_UnderRx);
        break;

    case Cmt2300Fsm_ready2Tx:
        cmt2300SetChipFsm(handle, Cmt2300_ChipFsm_Standby, Cmt2300Fsm_RunUsrCbk);
        break;

    case Cmt2300Fsm_Rx1ByteOk:
        if (handle->rxLen)
            handle->fsm = Cmt2300Fsm_UnderRx;
        else
            // handle->fsm = Cmt2300Fsm_RunUsrCbk;
            cmt2300SetChipFsm(handle, Cmt2300_ChipFsm_Standby, Cmt2300Fsm_RunUsrCbk);

        break;
    case Cmt2300Fsm_RegMdf_read:
        if (Cmt2300_SpiFsm_Idle == handle->spiFsm)
        {
            handle->regBuf[0] &= 0x7f;
            handle->regBuf[1] |= handle->regBuf[2];
            handle->regBuf[1] &= handle->regBuf[3];
            handle->isRx = 0;
            handle->isOperateFifo = 0;
            handle->spiFsm = Cmt2300_SpiFsm_Required;
            handle->fsm = Cmt2300Fsm_RegMdf_write;
        }
        break;
    case Cmt2300Fsm_RegMdf_write:
        if (Cmt2300_SpiFsm_Idle == handle->spiFsm)
        {
            handle->fsm = handle->targetFsm;
        }
        break;

    case Cmt2300Fsm_RunUsrCbk:
        handle->fsm = Cmt2300Fsm_Idle;
        if (handle->cbk)
            handle->cbk(handle->arg);
        break;

    default:
        break;
    }
}

void cmt2300Boot(cmt2300Handle_t *handle)
{
    handle->fsm = Cmt2300Fsm_Boot;
}

void cmt2300InitAsync(cmt2300Handle_t *handle, cmt2300Cfg_t *cfg, void (*cbk)(void *), void *arg)
{
    handle->busBusy = 0;
    handle->spiFsm = Cmt2300_SpiFsm_Idle;
    handle->cbk = cbk;
    handle->arg = arg;

    handle->cfg.rxCbk = cfg->rxCbk;

    handle->cfg.api.gpioWrite = cfg->api.gpioWrite;
    handle->cfg.api.spiWriteReadAsync = cfg->api.spiWriteReadAsync;
    handle->cfg.api.usTimerCtrl = cfg->api.usTimerCtrl;
    handle->cfg.api.usTimerGetCnt = cfg->api.usTimerGetCnt;

    cmt2300SendRegBuf(handle, 0x7f, 0xff, Cmt2300Fsm_Rst);
}

/// @brief 重发fifo里的数据
// 1. fifo复位
// 3. 切到tx状态，完成发送
/// @param handle
/// @param cbk 发送完成之后的操作
/// @param arg cbk的参数
uint8_t cmt2300ResandAsync(cmt2300Handle_t *handle, void (*cbk)(void *), void *arg)
{
    if (handle->fsm != Cmt2300Fsm_Idle)
        return Cmt2300_ErrCode_BusBusy;

    if (handle->targetChipFsm != Cmt2300_ChipFsm_Standby)
        return Cmt2300_ErrCode_TxNotReady;
    handle->cbk = cbk;
    handle->arg = arg;

    cmt2300SendRegBuf(handle, Cmt2300_RegAdd_Ctrl_FifoClr, CMT2300A_MASK_FIFO_RESTORE, Cmt2300Fsm_TxData);
    return Cmt2300_ErrCode_Success;
}
/// @brief 发一段数据
// 1. 清空fifo
// 2. fifo写使能
// 3. 在standby状态下往fifo发数据，再切到tx状态，完成发送
/// @param handle
/// @param datPtr
/// @param datLen
/// @param cbk 发送完成之后的操作
/// @param arg cbk的参数
uint8_t cmt2300TransmitAsync(cmt2300Handle_t *handle, uint8_t *datPtr, uint8_t datLen, void (*cbk)(void *), void *arg)
{
    if (handle->fsm != Cmt2300Fsm_Idle)
        return Cmt2300_ErrCode_BusBusy;

    if (handle->targetChipFsm != Cmt2300_ChipFsm_Standby)
        return Cmt2300_ErrCode_TxNotReady;

    handle->txBuf = datPtr;
    handle->txLen = datLen;
    handle->cbk = cbk;
    handle->arg = arg;

    cmt2300SendRegBuf(handle, Cmt2300_RegAdd_Ctrl_FifoClr, CMT2300A_MASK_FIFO_CLR_TX, Cmt2300Fsm_FifoWrEn);
    return Cmt2300_ErrCode_Success;
}

void cmt2300OnIntCbk(cmt2300Handle_t *handle, uint8_t intId)
{
    (void)intId;
    if (Cmt2300Fsm_TxWaitIntSignal == (handle->fsm))
    {
        handle->errCode = Cmt2300_ErrCode_Success;
        // 发送完了，清除中断标志，再把状态切到standby
        cmt2300SendRegBuf(handle, Cmt2300_RegAdd_Ctrl_Int1Clr, CMT2300A_MASK_TX_DONE_CLR, Cmt2300Fsm_TxEndCheck2Stb);
    }
    else if (Cmt2300Fsm_UnderRx == (handle->fsm))
    {
        handle->isOperateFifo = 1;
        handle->isRx = 1;

        handle->fsm = Cmt2300Fsm_WaitSpi;
        handle->spiFsm = Cmt2300_SpiFsm_Required;
        handle->targetFsm = Cmt2300Fsm_Rx1ByteOk;
    }
}

uint8_t cmt2300TxPrepareAsync(cmt2300Handle_t *handle, void (*cbk)(void *), void *arg)
{
    // if (handle->targetChipFsm == Cmt2300_ChipFsm_Standby)
    // return Cmt2300_ErrCode_Success;
    if (handle->fsm != Cmt2300Fsm_Idle)
        return Cmt2300_ErrCode_BusBusy;

    handle->cbk = cbk;
    handle->arg = arg;
    cmt2300SendRegBuf(handle, Cmt2300_RegAdd_Ctrl_Int1, CMT2300A_INT_SEL_TX_DONE, Cmt2300Fsm_ready2Tx);
    return Cmt2300_ErrCode_Success;
}

uint8_t cmt2300SleepCtrlAsync(cmt2300Handle_t *handle, uint8_t isSleep, void (*cbk)(void *), void *arg)
{
    if (handle->fsm != Cmt2300Fsm_Idle)
        return Cmt2300_ErrCode_BusBusy;
    handle->cbk = cbk;
    handle->arg = arg;
    if (isSleep)
        cmt2300SetChipFsm(handle, Cmt2300_ChipFsm_Sleep, Cmt2300Fsm_RunUsrCbk);
    else
        cmt2300SetChipFsm(handle, Cmt2300_ChipFsm_Standby, Cmt2300Fsm_RunUsrCbk);
    return Cmt2300_ErrCode_Success;
}

uint8_t cmt2300StartRxAsync(cmt2300Handle_t *handle, uint8_t *buf, uint8_t bufLen, void (*cbk)(void *), void *arg)
{
    if (handle->targetChipFsm == Cmt2300_ChipFsm_Tx)
        return Cmt2300_ErrCode_TxNotReturn;
    if (handle->fsm != Cmt2300Fsm_Idle)
        return Cmt2300_ErrCode_BusBusy;

    handle->rxBuf = buf;
    handle->rxLen = bufLen;
    handle->cbk = cbk;
    handle->arg = arg;
    cmt2300SendRegBuf(handle, Cmt2300_RegAdd_Ctrl_Int1, CMT2300A_INT_SEL_RX_FIFO_NMTY, Cmt2300Fsm_ready2Rx);
    // cmt2300SetChipFsm(handle, Cmt2300_ChipFsm_Rx, Cmt2300Fsm_UnderRx);
    return Cmt2300_ErrCode_Success;
}
