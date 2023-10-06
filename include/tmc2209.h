#pragma once
#pragma pack(push, 1)

#include <stdint.h>

#include "softwareUart.h"

#define GCONF_ADDRESS 0x00
#define IOIN_ADDRESS 0x06
#define SLAVECONF_ADDRESS 0x03
#define FACTORYCONF_ADRESS 0x07
#define VACTUAL_ADDRESS 0x22
#define MSCURACT_ADDRESS 0x6B
#define CHOPCONF_ADDRESS 0x6C
#define PWMCONF_ADDRESS 0x70
#define DRVSTATUS_ADDRESS 0x6F
#define PWMSCALE_ADDRESS 0x71
#define IHOLDIRUN_ADDRESS 0x10
#define TPOWERDOWN_ADDRESS 0x11
#define TPWMTHRS_ADDRESS 0x13

#define TMC2208_SYNC 0x05
#define TMC_WRITE 0x80

#define DEFAULT_HOLD_MULT 0.7f

extern uint16_t bytesWritten;

typedef struct GCONF_t {
    union {
        uint16_t sr : 10;
        struct {
            bool i_scale_analog : 1, internal_rsense : 1, en_spreadcycle : 1,
                shaft : 1, index_otpw : 1, index_step : 1, pdn_disable : 1,
                mstep_reg_select : 1, multistep_filt : 1, test_mode : 1;
        };
    };
} gconf_t;

typedef struct IOIN_t {
    union {
        uint32_t sr;
        struct {
            bool enn : 1, : 1, ms1 : 1, ms2 : 1, diag : 1, : 1, pdn_uart : 1,
                step : 1, sel_a : 1, dir : 1;
            uint16_t : 14;
            uint8_t version : 8;
        };
    };
} ioin_t;

typedef struct SLAVECONF_t {
    union {
        uint16_t sr : 12;
        struct {
            uint8_t slaveaddr : 8;
            uint8_t senddelay : 4;
        };
    };
} slaveconf_t;

typedef struct FACTORY_CONF_t {
    union {
        uint16_t sr;
        struct {
            uint8_t fclktrim : 5, : 3, ottrim : 2;
        };
    };
} factory_conf_t;

typedef struct VACTUAL_t {
    uint32_t sr;
} vactual_t;

typedef struct MSCURACT_t {
    union {
        uint32_t sr : 25;
        struct {
            int16_t cur_a : 9, : 7, cur_b : 9;
        };
    };
} mscuract_t;

typedef struct CHOPCONF_t {
    union {
        uint32_t sr;
        struct {
            uint8_t toff : 4, hstrt : 3, hend : 4, : 4, tbl : 2;
            bool vsense : 1;
            uint8_t : 6, mres : 4;
            bool intpol : 1, dedge : 1, diss2g : 1, diss2vs : 1;
        };
    };
} chopconf_t;

typedef struct PWMCONF_t {
    union {
        uint32_t sr;
        struct {
            uint8_t pwm_ofs : 8, pwm_grad : 8, pwm_freq : 2;
            bool pwm_autoscale : 1, pwm_autograd : 1;
            uint8_t freewheel : 2, : 2, pwm_reg : 4, pwm_lim : 4;
        };
    };
} pwmconf_t;

typedef struct DRV_STATUS_t {
    union {
        uint32_t sr;
        struct {
            bool otpw : 1, ot : 1, s2ga : 1, s2gb : 1, s2vsa : 1, s2vsb : 1,
                ola : 1, olb : 1, t120 : 1, t143 : 1, t150 : 1, t157 : 1;
            uint8_t : 4, cs_actual : 5, : 3, : 6;
            bool stealth : 1, stst : 1;
        };
    };
} drv_status_t;

typedef struct PWM_SCALE_t {
    union {
        uint32_t sr;
        struct {
            uint8_t pwm_scale_sum : 8, : 8;
            int16_t pwm_scale_auto : 9;
        };
    };
} pwm_scale_t;

typedef struct IHOLD_IRUN_t {
    union {
        uint32_t sr : 20;
        struct {
        uint8_t ihold : 5,
                        : 3,
                irun : 5,
                    : 3,
                iholddelay : 4;
        };
    };
} ihold_irun_t;

typedef struct TPOWERDOWN_t {
    uint8_t sr : 8;
} tpowerdown_t;

typedef struct TPWMTHRS_t {
    uint32_t sr : 20;
} tpwmthrs_t;

typedef struct tmc2209_t{
    // Registers
    gconf_t gconf;
    ioin_t ioin;
    slaveconf_t slaveconf;
    factory_conf_t factory_conf;
    vactual_t vactual;
    mscuract_t mscuract;
    chopconf_t chopconf;
    pwmconf_t pwmconf;
    drv_status_t drv_status;
    pwm_scale_t pwm_scale;
    ihold_irun_t ihold_irun;
    tpowerdown_t tpowerdown;
    tpwmthrs_t tpwmthrs;

    // Uart
    softwareUart* uart;

    // Misc
    float rsense;
    float holdMultiplier;
    uint8_t tmc2208Sync;
    uint8_t tmcWrite;
    uint8_t address;
} tmc2209;

tmc2209* createDriver(float rSense, uint8_t uartAddress, softwareUart*  uart);

void defaultSettings(tmc2209* driver);

void removeDriver(tmc2209** driver);

void toff(tmc2209* driver, uint8_t B);

void tpwmthrs(tmc2209* driver, uint32_t B);

void multistepFilt(tmc2209* driver, uint8_t B);

void shaft(tmc2209* driver, uint8_t B);

void rms_current(tmc2209* driver, uint16_t mA);

// void mres(tmc2209 driver, uint8_t B);

void microsteps(tmc2209* driver, uint16_t ms);

void pwm_autoscale(tmc2209* driver, uint8_t B);

#pragma pack(pop)
