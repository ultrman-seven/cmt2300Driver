#ifndef B3C03B3B_4D17_41E7_8ECE_CF53136F0D29
#define B3C03B3B_4D17_41E7_8ECE_CF53136F0D29

#define Cmt2300_RegAdd_Sys1 0x0c
#define Cmt2300_RegAdd_Sys2 0x0d

#define Cmt2300_RegAdd_Ctrl_ModeCtrl 0x60
#define Cmt2300_RegAdd_Ctrl_ModeState 0x61
#define Cmt2300_RegAdd_Ctrl_EnCtrl 0x62
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

/* ********** CMT2300A_CUS_SYS1 registers ********** */
#define CMT2300A_MASK_LMT_VTR 0xC0
#define CMT2300A_MASK_MIXER_BIAS 0x30
#define CMT2300A_MASK_LNA_MODE 0x0C
#define CMT2300A_MASK_LNA_BIAS 0x03

/* ********** CMT2300A_CUS_SYS2 registers ********** */
#define CMT2300A_MASK_LFOSC_RECAL_EN 0x80
#define CMT2300A_MASK_LFOSC_CAL1_EN 0x40
#define CMT2300A_MASK_LFOSC_CAL2_EN 0x20
#define CMT2300A_MASK_RX_TIMER_EN 0x10
#define CMT2300A_MASK_SLEEP_TIMER_EN 0x08
#define CMT2300A_MASK_TX_DC_EN 0x04
#define CMT2300A_MASK_RX_DC_EN 0x02
#define CMT2300A_MASK_DC_PAUSE 0x01

/* ********** CMT2300A_CUS_SYS3 registers ********** */
#define CMT2300A_MASK_SLEEP_BYPASS_EN 0x80
#define CMT2300A_MASK_XTAL_STB_TIME 0x70
#define CMT2300A_MASK_TX_EXIT_STATE 0x0C
#define CMT2300A_MASK_RX_EXIT_STATE 0x03

/* ********** CMT2300A_CUS_SYS4 registers ********** */
#define CMT2300A_MASK_SLEEP_TIMER_M_7_0 0xFF

/* ********** CMT2300A_CUS_SYS5 registers ********** */
#define CMT2300A_MASK_SLEEP_TIMER_M_10_8 0x70
#define CMT2300A_MASK_SLEEP_TIMER_R 0x0F

/* ********** CMT2300A_CUS_SYS6 registers ********** */
#define CMT2300A_MASK_RX_TIMER_T1_M_7_0 0xFF

/* ********** CMT2300A_CUS_SYS7 registers ********** */
#define CMT2300A_MASK_RX_TIMER_T1_M_10_8 0x70
#define CMT2300A_MASK_RX_TIMER_T1_R 0x0F

/* ********** CMT2300A_CUS_SYS8 registers ********** */
#define CMT2300A_MASK_RX_TIMER_T2_M_7_0 0xFF

/* ********** CMT2300A_CUS_SYS9 registers ********** */
#define CMT2300A_MASK_RX_TIMER_T2_M_10_8 0x70
#define CMT2300A_MASK_RX_TIMER_T2_R 0x0F

/* ********** CMT2300A_CUS_SYS10 registers ********** */
#define CMT2300A_MASK_COL_DET_EN 0x80
#define CMT2300A_MASK_COL_OFS_SEL 0x40
#define CMT2300A_MASK_RX_AUTO_EXIT_DIS 0x20
#define CMT2300A_MASK_DOUT_MUTE 0x10
#define CMT2300A_MASK_RX_EXTEND_MODE 0x0F

/* ********** CMT2300A_CUS_SYS11 registers ********** */
#define CMT2300A_MASK_PJD_TH_SEL 0x80
#define CMT2300A_MASK_CCA_INT_SEL 0x60
#define CMT2300A_MASK_RSSI_DET_SEL 0x18
#define CMT2300A_MASK_RSSI_AVG_MODE 0x07

/* ********** CMT2300A_CUS_SYS12 registers ********** */
#define CMT2300A_MASK_PJD_WIN_SEL 0xC0
#define CMT2300A_MASK_CLKOUT_EN 0x20
#define CMT2300A_MASK_CLKOUT_DIV 0x1F

/* ********** CMT2300A_CUS_MODE_STA registers ********** */
#define CMT2300A_MASK_RSTN_IN_EN 0x20
#define CMT2300A_MASK_CFG_RETAIN 0x10

/* ********** CMT2300A_CUS_EN_CTL registers ********** */
#define CMT2300A_MASK_LOCKING_EN 0x20


/* ********** CMT2300A_CUS_INT_CLR1 registers ********** */
#define CMT2300A_MASK_SL_TMO_FLG 0x20
#define CMT2300A_MASK_RX_TMO_FLG 0x10
#define CMT2300A_MASK_TX_DONE_FLG 0x08
#define CMT2300A_MASK_TX_DONE_CLR 0x04
#define CMT2300A_MASK_SL_TMO_CLR 0x02
#define CMT2300A_MASK_RX_TMO_CLR 0x01

/* ********** CMT2300A_CUS_FIFO_CLR registers ********** */
#define CMT2300A_MASK_FIFO_RESTORE 0x04
#define CMT2300A_MASK_FIFO_CLR_RX 0x02
#define CMT2300A_MASK_FIFO_CLR_TX 0x01

/* ********** CMT2300A_CUS_FIFO_CTL registers ********** */
#define CMT2300A_MASK_TX_DIN_EN 0x80
#define CMT2300A_MASK_TX_DIN_SEL 0x60
#define CMT2300A_MASK_FIFO_AUTO_CLR_DIS 0x10
#define CMT2300A_MASK_FIFO_TX_RD_EN 0x08
#define CMT2300A_MASK_FIFO_RX_TX_SEL 0x04
#define CMT2300A_MASK_FIFO_MERGE_EN 0x02
#define CMT2300A_MASK_SPI_FIFO_RD_WR_SEL 0x01

/* ********** CMT2300A_CUS_INT2_CTL registers ********** */
#define CMT2300A_MASK_LFOSC_OUT_EN       0x40
#define CMT2300A_MASK_TX_DIN_INV         0x20
#define CMT2300A_MASK_INT2_SEL           0x1F

/* ********** CMT2300A_CUS_INT1_CTL registers ********** */
#define CMT2300A_MASK_RF_SWT1_EN         0x80
#define CMT2300A_MASK_RF_SWT2_EN         0x40
#define CMT2300A_MASK_INT_POLAR          0x20
#define CMT2300A_MASK_INT1_SEL           0x1F
/* CMT2300A_MASK_INT_POLAR options */
#define CMT2300A_INT_POLAR_SEL_0         0x00
#define CMT2300A_INT_POLAR_SEL_1         0x20
/* CMT2300A_MASK_INT1_SEL options */
#define CMT2300A_INT_SEL_RX_ACTIVE       0x00
#define CMT2300A_INT_SEL_TX_ACTIVE       0x01
#define CMT2300A_INT_SEL_RSSI_VLD        0x02
#define CMT2300A_INT_SEL_PREAM_OK        0x03
#define CMT2300A_INT_SEL_SYNC_OK         0x04
#define CMT2300A_INT_SEL_NODE_OK         0x05
#define CMT2300A_INT_SEL_CRC_OK          0x06
#define CMT2300A_INT_SEL_PKT_OK          0x07
#define CMT2300A_INT_SEL_SL_TMO          0x08
#define CMT2300A_INT_SEL_RX_TMO          0x09
#define CMT2300A_INT_SEL_TX_DONE         0x0A
#define CMT2300A_INT_SEL_RX_FIFO_NMTY    0x0B
#define CMT2300A_INT_SEL_RX_FIFO_TH      0x0C
#define CMT2300A_INT_SEL_RX_FIFO_FULL    0x0D
#define CMT2300A_INT_SEL_RX_FIFO_WBYTE   0x0E
#define CMT2300A_INT_SEL_RX_FIFO_OVF     0x0F
#define CMT2300A_INT_SEL_TX_FIFO_NMTY    0x10
#define CMT2300A_INT_SEL_TX_FIFO_TH      0x11
#define CMT2300A_INT_SEL_TX_FIFO_FULL    0x12
#define CMT2300A_INT_SEL_STATE_IS_STBY   0x13
#define CMT2300A_INT_SEL_STATE_IS_FS     0x14
#define CMT2300A_INT_SEL_STATE_IS_RX     0x15
#define CMT2300A_INT_SEL_STATE_IS_TX     0x16
#define CMT2300A_INT_SEL_LED             0x17
#define CMT2300A_INT_SEL_TRX_ACTIVE      0x18
#define CMT2300A_INT_SEL_PKT_DONE        0x19
/* ********** CMT2300A_CUS_IO_SEL registers ********** */
#define CMT2300A_MASK_GPIO4_SEL          0xC0
#define CMT2300A_MASK_GPIO3_SEL          0x30
#define CMT2300A_MASK_GPIO2_SEL          0x0C
#define CMT2300A_MASK_GPIO1_SEL          0x03
/* CMT2300A_MASK_GPIO4_SEL options */
#define CMT2300A_GPIO4_SEL_RSTIN         0x00
#define CMT2300A_GPIO4_SEL_INT1          0x40
#define CMT2300A_GPIO4_SEL_DOUT          0x80
#define CMT2300A_GPIO4_SEL_DCLK          0xC0
/* CMT2300A_MASK_GPIO3_SEL options */
#define CMT2300A_GPIO3_SEL_CLKO          0x00
#define CMT2300A_GPIO3_SEL_DOUT          0x10
#define CMT2300A_GPIO3_SEL_DIN           0x10
#define CMT2300A_GPIO3_SEL_INT2          0x20
#define CMT2300A_GPIO3_SEL_DCLK          0x30
/* CMT2300A_MASK_GPIO2_SEL options */
#define CMT2300A_GPIO2_SEL_INT1          0x00
#define CMT2300A_GPIO2_SEL_INT2          0x04
#define CMT2300A_GPIO2_SEL_DOUT          0x08
#define CMT2300A_GPIO2_SEL_DIN           0x08
#define CMT2300A_GPIO2_SEL_DCLK          0x0C
/* CMT2300A_MASK_GPIO1_SEL options */
#define CMT2300A_GPIO1_SEL_DOUT          0x00
#define CMT2300A_GPIO1_SEL_DIN           0x00
#define CMT2300A_GPIO1_SEL_INT1          0x01
#define CMT2300A_GPIO1_SEL_INT2          0x02
#define CMT2300A_GPIO1_SEL_DCLK          0x03

/* ********** CMT2300A_CUS_INT_EN registers ********** */
#define CMT2300A_MASK_SL_TMO_EN          0x80
#define CMT2300A_MASK_RX_TMO_EN          0x40
#define CMT2300A_MASK_TX_DONE_EN         0x20
#define CMT2300A_MASK_PREAM_OK_EN        0x10
#define CMT2300A_MASK_SYNC_OK_EN         0x08
#define CMT2300A_MASK_NODE_OK_EN         0x04
#define CMT2300A_MASK_CRC_OK_EN          0x02
#define CMT2300A_MASK_PKT_DONE_EN        0x01

#endif /* B3C03B3B_4D17_41E7_8ECE_CF53136F0D29 */
