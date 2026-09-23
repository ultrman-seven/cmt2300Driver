#include "cmt2300.h"

#define Cmt2300DelayTime_us_SpiAftCsEn 2
#define Cmt2300DelayTime_us_SpiBfCsDis 2

#define Cmt2300DelayTime_us_FifoAftCsEn 2
#define Cmt2300DelayTime_us_FifoBfCsDis 3
#define Cmt2300DelayTime_us_FifoAftCsDis 5

#define Cmt2300DelayTime_us_swFromStb2RxTx 360

#define Cmt2300_RegAdd_Ctrl_ModeCtrl 0x60
#define Cmt2300_RegAdd_Ctrl_ModeState 0x61
#define Cmt2300_RegAdd_Ctrl_FreqChannel 0x63
#define Cmt2300_RegAdd_Ctrl_FreqOffset 0x64
#define Cmt2300_RegAdd_Ctrl_IO 0x65
#define Cmt2300_RegAdd_Ctrl_Int1 0x66
#define Cmt2300_RegAdd_Ctrl_Int2 0x67
#define Cmt2300_RegAdd_Ctrl_IntEn 0x68
#define Cmt2300_RegAdd_Ctrl_Fifo 0x69
#define Cmt2300_RegAdd_Ctrl_Int1Clr 0x6a
#define Cmt2300_RegAdd_Ctrl_Int2Clr 0x6b
#define Cmt2300_RegAdd_Ctrl_FifoClr 0x6c
#define Cmt2300_RegAdd_Ctrl_IntFlag 0x6d
#define Cmt2300_RegAdd_Ctrl_FifoFlag 0x6e
#define Cmt2300_RegAdd_Ctrl_RssiCode 0x6f
#define Cmt2300_RegAdd_Ctrl_RssiDbm 0x70
#define Cmt2300_RegAdd_Ctrl_Lbd 0x71

#define Cmt2300_ModeSwitchCmd_standby 0b00000010
#define Cmt2300_ModeSwitchCmd_rfs____ 0b00000100
#define Cmt2300_ModeSwitchCmd_rf_____ 0b00001000
#define Cmt2300_ModeSwitchCmd_sleep__ 0b00010000
#define Cmt2300_ModeSwitchCmd_tfs____ 0b00100000
#define Cmt2300_ModeSwitchCmd_tx_____ 0b01000000
#define Cmt2300_ModeSwitchCmd_swRxTx_ 0b10000000
#define Cmt2300_ModeSwitchCmd_

enum
{
    Cmt2300Fsm_Boot = 0,
    Cmt2300Fsm_Rst,
    Cmt2300Fsm_Init,
    Cmt2300Fsm_DelayAfterCsEn,
    Cmt2300Fsm_DelayBeforeCsDis,
    Cmt2300Fsm_DelayAfterFifo,
    Cmt2300Fsm_Wait4Spi,
    Cmt2300Fsm_FifoWrite,
    Cmt2300Fsm_,
    Cmt2300Fsm_Idle
};

// enum
// {
//     Cmt2300_SpiFsm_Idle = 0,
//     Cmt2300_SpiFsm_Required,
//     Cmt2300_SpiFsm_CsEnDelay,
//     Cmt2300_SpiFsm_SendRegAdd,
//     Cmt2300_SpiFsm_add3UsDelay,
//     Cmt2300_SpiFsm_DataTransmit,
//     Cmt2300_SpiFsm_DataWaitBus,
//     Cmt2300_SpiFsm_DataDelay,
//     Cmt2300_SpiFsm_CsDis,
//     Cmt2300_SpiFsm_complete3UsDelay,
//     Cmt2300_SpiFsm_,
// };

extern const uint16_t cmtRegCfgDataLen;
extern const uint16_t cmtRegCfgData[];

void cmt2300OnIntCbk(cmt2300Handle_t *handle, uint8_t intId)
{
}

void cmt2300TransmitAsync(cmt2300Handle_t *handle, uint8_t *datPtr, uint32_t datLen, void (*cbk)(void *), void *arg)
{
}

static void cmt2300OnSpiTxCpltCbk(void *handle)
{
    ((cmt2300Handle_t *)handle)->busBusy = 0;
}

#define cmt2300SpiTx(_handle, _buf, _len) (_handle)->cfg.api.spiWriteReadAsync(Cmt2300_SpiDir_Tx, (_buf), (_len), cmt2300OnSpiTxCpltCbk, (_handle))

void cmt2300Boot(cmt2300Handle_t *handle)
{
    handle->fsm = Cmt2300Fsm_Boot;
}

static inline void cmt2300SendRegBuf(cmt2300Handle_t *handle, uint8_t target)
{
    handle->cfg.api.gpioWrite(Cmt2300_PinId_SpiCs, Cmt2300_PinLevel_Low);
    handle->cfg.api.usTimerCtrl(Cmt2300_TimerCtrlState_RstAndStart);
    handle->fsm = Cmt2300Fsm_DelayAfterCsEn;
    handle->targetFsm = target;
}

void cmt2300InitAsync(cmt2300Handle_t *handle, cmt2300Cfg_t *cfg, void (*cbk)(void *), void *arg)
{
    handle->busBusy = 0;
    handle->cbk = cbk;
    handle->arg = arg;
    handle->cfg.api.usTimerCtrl(Cmt2300_TimerCtrlState_RstAndStart);
    handle->regBuf[0] = 0x7f;
    handle->regBuf[1] = 0xff;
    cmt2300SendRegBuf(handle, Cmt2300Fsm_Init);
}

void cmt2300ConfigAsync(cmt2300Handle_t *handle, cmt2300Cfg_t *cfg, void (*cbk)(void *), void *arg)
{
}

#define Cmt2300Mode_Rx 1
#define Cmt2300Mode_Tx 0

void cmt2300SetModeAsync(cmt2300Handle_t *handle, uint8_t mode, void (*cbk)(void *), void *arg)
{
}

// void cmt2300TransmitAsync(cmt2300Handle_t *handle, uint8_t *buf, uint32_t len, void (*cbk)(void *), void *arg)
// {
// }

void cmt2300Loop(cmt2300Handle_t *handle)
{
    switch (handle->fsm)
    {
    case Cmt2300Fsm_Boot:
        break;
    case Cmt2300Fsm_Rst:
        if (handle->cfg.api.usTimerGetCnt() <= 30000)
            break;
        break;
    case Cmt2300Fsm_Init:
        if (handle->busBusy)
            break;
        handle->cfg.api.gpioWrite(Cmt2300_PinId_SpiCs, Cmt2300_PinLevel_Low);
        handle->cfg.api.usTimerCtrl(Cmt2300_TimerCtrlState_RstAndStart);
        handle->fsm = Cmt2300Fsm_DelayAfterCsEn;
        break;
    case Cmt2300Fsm_DelayAfterCsEn:
        if (Cmt2300Fsm_FifoWrite == handle->targetFsm)
        {
            if (handle->cfg.api.usTimerGetCnt() <= Cmt2300DelayTime_us_FifoAftCsEn)
                break;
            handle->cfg.api.spiWriteReadAsync(Cmt2300_SpiDir_Tx, handle->tx.buf, 1, cmt2300OnSpiTxCpltCbk, handle);
            ++(handle->tx.buf);
            --(handle->tx.len);
        }
        else
        {
            if (handle->cfg.api.usTimerGetCnt() <= Cmt2300DelayTime_us_SpiAftCsEn)
                break;
            handle->cfg.api.spiWriteReadAsync(Cmt2300_SpiDir_Tx, handle->regBuf, 2, cmt2300OnSpiTxCpltCbk, handle);
        }
        handle->fsm = Cmt2300Fsm_Wait4Spi;
        break;
    case Cmt2300Fsm_DelayBeforeCsDis:
        if (Cmt2300Fsm_FifoWrite == handle->targetFsm)
        {
            if (handle->cfg.api.usTimerGetCnt() <= Cmt2300DelayTime_us_FifoBfCsDis)
                break;
            handle->cfg.api.gpioWrite(Cmt2300_PinId_FifoCs, Cmt2300_PinLevel_High);
            handle->fsm = Cmt2300Fsm_DelayAfterFifo;
            handle->cfg.api.usTimerCtrl(Cmt2300_TimerCtrlState_RstAndStart);
        }
        else
        {
        }
        break;
    case Cmt2300Fsm_DelayAfterFifo:
        if (handle->cfg.api.usTimerGetCnt() <= Cmt2300DelayTime_us_FifoAftCsDis)
            break;
        if (handle->tx.len)
        {
            handle->cfg.api.gpioWrite(Cmt2300_PinId_FifoCs, Cmt2300_PinLevel_Low);
            handle->fsm = Cmt2300Fsm_DelayBeforeCsDis;
            handle->cfg.api.usTimerCtrl(Cmt2300_TimerCtrlState_RstAndStart);
        }
        break;
    case Cmt2300Fsm_Wait4Spi:
        if (handle->busBusy)
            break;
        handle->fsm = Cmt2300Fsm_DelayBeforeCsDis;
        handle->cfg.api.usTimerCtrl(Cmt2300_TimerCtrlState_RstAndStart);
        break;
    case Cmt2300Fsm_:
        break;

    default:
        break;
    }
}
