#include "stdint.h"

#define Cmt2300_IntId_INT1 0
#define Cmt2300_IntId_INT2 0

#define Cmt2300_PinId_SpiCs 0
#define Cmt2300_PinId_FifoCs 1

#define Cmt2300_PinLevel_Low 0
#define Cmt2300_PinLevel_High 1

#define Cmt2300_SpiDir_Tx 0
#define Cmt2300_SpiDir_Rx 1

#define Cmt2300_TimerCtrlState_Stop 0
#define Cmt2300_TimerCtrlState_RstAndStart 1

struct __cmt2300ExternalApi_t
{
    void (*gpioWrite)(uint8_t id, uint8_t level);
    // void (*spiWriteReadAsync)(uint8_t *txBuf, uint8_t *rxBuf, uint32_t len, void (*cbk)(void *), void *arg);
    void (*spiWriteReadAsync)(uint8_t dir, uint8_t *buf, uint32_t len, void (*cbk)(void *), void *arg);
    void (*usTimerCtrl)(uint8_t ctrlState);
    uint32_t (*usTimerGetCnt)(void);
};

typedef (*cmt2300UserOnRxCbk)(uint8_t *, uint32_t);

typedef struct
{
    struct __cmt2300ExternalApi_t api;
    cmt2300UserOnRxCbk rxCbk;
} cmt2300Cfg_t;

typedef struct
{
    uint8_t busBusy;
    uint8_t isOperateFifo;
    uint8_t isRx;

    uint8_t fsm;

    cmt2300Cfg_t cfg;
    void (*cbk)(void *);
    void *arg;
    uint8_t regBuf[4];
} cmt2300Handle_t;

#include "cmt2300.h"

#define Cmt2300DelayTime_us_SpiAftCsEn 2
#define Cmt2300DelayTime_us_SpiBfCsDis 2

#define Cmt2300DelayTime_us_FifoAftCsEn 2
#define Cmt2300DelayTime_us_FifoBfCsDis 3
#define Cmt2300DelayTime_us_FifoAftCsDis 5

#define Cmt2300DelayTime_us_swFromStb2RxTx 360

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
    Cmt2300Fsm_DelayAfterCsEn,
    Cmt2300Fsm_DelayBeforeCsDis,
    Cmt2300Fsm_DelayAfterFifo,
    Cmt2300Fsm_,
    Cmt2300Fsm_,
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

void cmt2300InitAsync(cmt2300Handle_t *handle, cmt2300Cfg_t *cfg, void (*cbk)(void *), void *arg)
{
    handle->busBusy = 0;
}

void cmt2300ConfigAsync(cmt2300Handle_t *handle, cmt2300Cfg_t *cfg, void (*cbk)(void *), void *arg)
{
}

void cmt2300Loop(cmt2300Handle_t *handle)
{
    switch (handle->fsm)
    {
    case Cmt2300Fsm_Boot:
        break;
    case Cmt2300Fsm_:
        break;

    default:
        break;
    }
}
