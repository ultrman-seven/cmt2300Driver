#ifndef E7EDD314_CBB5_4135_8051_6E35E2102435
#define E7EDD314_CBB5_4135_8051_6E35E2102435

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"

#define Cmt2300_IntId_INT1 0
#define Cmt2300_IntId_INT2 1

#define Cmt2300_PinId_SpiCs 0
#define Cmt2300_PinId_FifoCs 1

#define Cmt2300_PinLevel_Low 0
#define Cmt2300_PinLevel_High 1

#define Cmt2300_SpiDir_Tx 0
#define Cmt2300_SpiDir_Rx 1

#define Cmt2300_TimerCtrlState_Stop 0
#define Cmt2300_TimerCtrlState_RstAndStart 1

#define Cmt2300_ErrCode_Success 0
#define Cmt2300_ErrCode_TxTimeOut 1
#define Cmt2300_ErrCode_CrcErr 2
#define Cmt2300_ErrCode_BusBusy 3
#define Cmt2300_ErrCode_TxNotReady 4
#define Cmt2300_ErrCode_TxNotReturn 5

// #define Cmt2300Mode_Rx 1
// #define Cmt2300Mode_TxPrepare 0

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

/**
 * 使用流程
 *
 * boot -> init -> sleep(false) -> tx prepare -> tx data -> rx -> sleep(true)
 *
 */
typedef struct
{
    struct
    {
        uint8_t busBusy : 1;
        uint8_t isOperateFifo : 1;
        uint8_t isRx : 1;
        uint8_t isSpiDoingRegRx : 1;
        uint8_t targetChipFsm : 4;
    };

    // uint8_t cfgRegIdx;

    uint8_t fsm;
    uint8_t spiFsm;
    uint8_t targetFsm;
    uint8_t errCode;
    union
    {
        uint8_t txLen;
        uint8_t rxLen;
    };

    uint8_t regBuf[4];
    cmt2300Cfg_t cfg;
    union
    {
        uint8_t *txBuf;
        uint8_t *rxBuf;
    };
    void (*cbk)(void *);
    void *arg;
    // struct
    // {
    //     uint8_t *buf;
    //     uint32_t len;
    // } rx, tx;

} cmt2300Handle_t;


void cmt2300Boot(cmt2300Handle_t *handle);
void cmt2300InitAsync(cmt2300Handle_t *handle, cmt2300Cfg_t *cfg, void (*cbk)(void *), void *arg);
void cmt2300Loop(cmt2300Handle_t *handle);
void cmt2300OnIntCbk(cmt2300Handle_t *handle, uint8_t intId);

uint8_t cmt2300SleepCtrlAsync(cmt2300Handle_t *handle, uint8_t isSleep, void (*cbk)(void *), void *arg);

uint8_t cmt2300TxPrepareAsync(cmt2300Handle_t *handle, void (*cbk)(void *), void *arg);
uint8_t cmt2300ResandAsync(cmt2300Handle_t *handle, void (*cbk)(void *), void *arg);
uint8_t cmt2300TransmitAsync(cmt2300Handle_t *handle, uint8_t *datPtr, uint8_t datLen, void (*cbk)(void *), void *arg);

uint8_t cmt2300StartRxAsync(cmt2300Handle_t *handle, uint8_t *buf, uint8_t bufLen, void (*cbk)(void *), void *arg);

#ifdef __cplusplus
}
#endif

#endif
