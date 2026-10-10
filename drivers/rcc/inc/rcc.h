#ifndef RCC_H
#define RCC_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    RCC_OK = 0,
    RCC_INVALID_PTR,
    RCC_INVALID_PARAMS,
    RCC_PLL_BUSY
} rcc_status_e;

typedef enum {
    RCC_PLLSRC_HSI = 0,
    RCC_PLLSRC_HSE,
    RCC_PLLSRC_COUNT
} rcc_pll_src_e;

/* HSI functions */
void rcc_enable_hsi(bool enable);
bool rcc_is_hsi_rdy(void);

/* HSE functions */
void rcc_enable_hse(bool enable);
bool rcc_is_hse_rdy(void);

/* PLL functions*/
typedef struct {
    uint16_t pllm;
    uint16_t plln;
    uint16_t pllp;
    uint16_t pllq;
    rcc_pll_src_e pll_src;
} pll_config_t;

void rcc_enable_pll(bool enable);
bool rcc_is_pll_rdy(void);
bool rcc_is_pll_on(void);
rcc_status_e rcc_config_pll(const pll_config_t *pll_config);
rcc_status_e rcc_read_pll_config(pll_config_t *pll_config);

#endif /* RCC_H */