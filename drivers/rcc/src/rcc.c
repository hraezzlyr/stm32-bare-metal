#include "rcc.h"
#include "stm32f439xx.h"

#define MIN_PLLM_VALUE      2
#define MAX_PLLM_VALUE      63
#define MIN_PLLN_VALUE      50
#define MAX_PLLN_VALUE      432
#define MIN_PLLQ_VALUE      2
#define MAX_PLLQ_VALUE      15

static const uint8_t REAL_PLLP_DIVIDERS[] = {2, 4, 6, 8};
static const uint8_t FIELD_PLLP_DIVIDERS[] = {0, 1, 2, 3};

/* HSI functions */
void rcc_enable_hsi(bool enable) {
    if (enable) {
        RCC->CR |= RCC_CR_HSION_Msk; 
    } else {
        RCC->CR &= ~RCC_CR_HSION_Msk; 
    }
}

bool rcc_is_hsi_rdy(void) {
    return (RCC->CR & RCC_CR_HSIRDY_Msk) != 0;
}

/* HSE functions */
void rcc_enable_hse(bool enable) {
    if (enable) {
        RCC->CR |= RCC_CR_HSEON_Msk;
    } else {
        RCC->CR &= ~RCC_CR_HSEON_Msk;
    }
}

bool rcc_is_hse_rdy(void) {
    return (RCC->CR & RCC_CR_HSERDY_Msk) != 0;
}

/* PLL functions */
rcc_status_e rcc_config_pll(const pll_config_t *pll_config) {
    rcc_status_e status = RCC_OK;

    /* Validate pointer */
    if (!pll_config) {
        status = RCC_INVALID_PTR;
        goto exit;
    }

    /* Check if PLL is enabled - if so, then raise an error */
    if ((RCC->CR & RCC_CR_PLLON_Msk) != 0) {
        status = RCC_PLL_BUSY;
        goto exit;
    }

    rcc_pll_src_e pll_src = pll_config->pll_src;
    uint8_t pllm = pll_config->pllm;
    uint16_t plln = pll_config->plln;
    uint8_t pllp = pll_config->pllp;
    uint8_t pllq = pll_config->pllq;

    /* Check if the PLLP factor is correct and convert it to a field value */
    bool is_pllp_correct = false;
    for (uint8_t i = 0;
        i < sizeof(REAL_PLLP_DIVIDERS) / sizeof(REAL_PLLP_DIVIDERS[0]);
        i++) {
            if (pllp == REAL_PLLP_DIVIDERS[i]) {
                pllp = FIELD_PLLP_DIVIDERS[i];
                is_pllp_correct = true;
                break;
            }
    }

    /* Validate PLLM, PLLN, PLLP values and the given PLL source - */
    /* if they are incorrect, rise an error */
    if ((pllm > MAX_PLLM_VALUE) || (pllm < MIN_PLLM_VALUE) ||
        (plln > MAX_PLLN_VALUE) || (plln < MIN_PLLN_VALUE) ||
        (pllq > MAX_PLLQ_VALUE) || (pllq < MIN_PLLQ_VALUE) ||
        (is_pllp_correct == false) ||
        ((uint32_t)pll_src >= RCC_PLLSRC_COUNT)) {
            status = RCC_INVALID_PARAMS;
            goto exit;
    }

    /* Clear bitfields of factors and the PLL source and set */
    /* their configurations */
    uint32_t clear_mask = (uint32_t)~(RCC_PLLCFGR_PLLM_Msk |
                            RCC_PLLCFGR_PLLN_Msk | RCC_PLLCFGR_PLLP_Msk |
                            RCC_PLLCFGR_PLLQ_Msk | RCC_PLLCFGR_PLLSRC_Msk);
    uint32_t new_value = (uint32_t)((pllm << RCC_PLLCFGR_PLLM_Pos) |
                            (plln << RCC_PLLCFGR_PLLN_Pos) |
                            (pllp << RCC_PLLCFGR_PLLP_Pos) |
                            (pllq << RCC_PLLCFGR_PLLQ_Pos) |
                            (pll_src << RCC_PLLCFGR_PLLSRC_Pos));
    RCC->PLLCFGR = (RCC->PLLCFGR & clear_mask) | new_value;

exit:
    return status;
}

bool rcc_is_pll_rdy(void) {
    return (RCC->CR & RCC_CR_PLLRDY_Msk) != 0;
}