#ifndef E7EDD314_CBB5_4135_8051_6E35E2102435
#define E7EDD314_CBB5_4135_8051_6E35E2102435

#ifdef __cplusplus
extern "C"
{
#endif

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


typedef void (*cmt2300UserOnRxCbk)(uint8_t *, uint32_t);

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

    uint8_t cfgRegIdx;
    
    uint8_t fsm;
    uint8_t targetFsm;

    cmt2300Cfg_t cfg;
    void (*cbk)(void *);
    void *arg;
    uint8_t regBuf[4];
    struct
    {
        uint8_t *buf;
        uint32_t len;
    } rx, tx;

} cmt2300Handle_t;


#ifdef __cplusplus
}
#endif

#endif
