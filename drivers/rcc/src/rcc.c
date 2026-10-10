#include "rcc.h"
#include "stm32f439xx.h"

#define MIN_PLLM_VALUE      2
#define MAX_PLLM_VALUE      63
#define MIN_PLLN_VALUE      50
#define MAX_PLLN_VALUE      432
#define MIN_PLLQ_VALUE      2
#define MAX_PLLQ_VALUE      15

static const uint16_t REAL_PLLP_DIVIDERS[] = {2, 4, 6, 8};
static const uint16_t FIELD_PLLP_DIVIDERS[] = {0, 1, 2, 3};

#define PLLP_DIVIDERS_NUMBER sizeof(REAL_PLLP_DIVIDERS) / \
    sizeof(REAL_PLLP_DIVIDERS[0])

static uint8_t s_rcc_find_pllp_table_index(
    uint16_t pllp,
    const uint16_t *pllp_dividers_table) {

    uint8_t idx = UINT8_MAX;
    for (uint8_t i = 0; i < PLLP_DIVIDERS_NUMBER; i++) {
        if (pllp == pllp_dividers_table[i]) {
            idx = i;
            break;
        }
    }

    return idx;
}

static uint16_t s_rcc_convert_pllp_value(
    uint16_t pllp,
    const uint16_t *pllp_dividers_table_src,
    const uint16_t *pllp_dividers_table_sink) {

    /* Validate the given PLLP value */
    uint8_t idx = s_rcc_find_pllp_table_index(pllp, pllp_dividers_table_src);
    if (idx == UINT8_MAX) {
        return UINT16_MAX;
    }

    return pllp_dividers_table_sink[idx];
}

static rcc_status_e s_rcc_pll_config_valid(const pll_config_t *pll_config) {
    if (!pll_config) {
        return RCC_INVALID_PTR;
    }

    /* Find out that the given real value of the PLLP exists */
    uint8_t pllp_idx_in_tab = s_rcc_find_pllp_table_index(
        pll_config->pllp,
        REAL_PLLP_DIVIDERS);

    /* Validate PLLM, PLLN, PLLP values and the given PLL source. */
    /* if they are incorrect, raise an error */
    if ((pll_config->pllm > MAX_PLLM_VALUE) ||
        (pll_config->pllm < MIN_PLLM_VALUE) ||
        (pll_config->plln > MAX_PLLN_VALUE) ||
        (pll_config->plln < MIN_PLLN_VALUE) ||
        (pll_config->pllq > MAX_PLLQ_VALUE) ||
        (pll_config->pllq < MIN_PLLQ_VALUE) ||
        (pllp_idx_in_tab == UINT8_MAX) ||
        ((uint32_t)pll_config->pll_src >= RCC_PLLSRC_COUNT)) {

        return RCC_INVALID_PARAMS;
    }

    return RCC_OK;
}

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
void rcc_enable_pll(bool enable) {
    uint32_t pll_en_value = enable << RCC_CR_PLLON_Pos;
    RCC->CR = (RCC->CR & (~RCC_CR_PLLON_Msk)) | pll_en_value;
}

bool rcc_is_pll_rdy(void) {
    return (RCC->CR & RCC_CR_PLLRDY_Msk) != 0;
}

bool rcc_is_pll_on(void) {
    return (RCC->CR & RCC_CR_PLLON_Msk) != 0;
}

rcc_status_e rcc_config_pll(const pll_config_t *pll_config) {
    /* If PLL configuration parameters are incorrect, raise an error */
    rcc_status_e param_status = s_rcc_pll_config_valid(pll_config);
    if (param_status != RCC_OK) {
        return param_status;
    }

    /* If PLL is enabled, raise an error */
    if (rcc_is_pll_on()) {
        return RCC_PLL_BUSY;
    }

    rcc_pll_src_e pll_src = pll_config->pll_src;
    uint16_t pllm = pll_config->pllm;
    uint16_t plln = pll_config->plln;
    uint16_t pllq = pll_config->pllq;
    uint16_t pllp = s_rcc_convert_pllp_value(
        pll_config->pllp,
        REAL_PLLP_DIVIDERS,
        FIELD_PLLP_DIVIDERS);

    /* Clear bitfields of factors and the PLL source and set */
    /* their configurations */
    uint32_t clear_mask = ~(
        RCC_PLLCFGR_PLLM_Msk |
        RCC_PLLCFGR_PLLN_Msk |
        RCC_PLLCFGR_PLLP_Msk |
        RCC_PLLCFGR_PLLQ_Msk |
        RCC_PLLCFGR_PLLSRC_Msk);

    uint32_t new_value =
        ((uint32_t)pllm << RCC_PLLCFGR_PLLM_Pos) |
        ((uint32_t)plln << RCC_PLLCFGR_PLLN_Pos) |
        ((uint32_t)pllp << RCC_PLLCFGR_PLLP_Pos) |
        ((uint32_t)pllq << RCC_PLLCFGR_PLLQ_Pos) |
        ((uint32_t)pll_src << RCC_PLLCFGR_PLLSRC_Pos);

    RCC->PLLCFGR = (RCC->PLLCFGR & clear_mask) | new_value;

    return RCC_OK;
}

rcc_status_e rcc_read_pll_config(pll_config_t *pll_config) {
    if (!pll_config) {
        return RCC_INVALID_PTR;
    }

    uint32_t rcc_pllcfgr = RCC->PLLCFGR;
    pll_config->pllm =
        (uint16_t)((rcc_pllcfgr & RCC_PLLCFGR_PLLM_Msk) >>
                    RCC_PLLCFGR_PLLM_Pos);

    pll_config->plln =
        (uint16_t)((rcc_pllcfgr & RCC_PLLCFGR_PLLN_Msk) >>
                    RCC_PLLCFGR_PLLN_Pos);

    pll_config->pllq =
        (uint16_t)((rcc_pllcfgr & RCC_PLLCFGR_PLLQ_Msk) >>
                    RCC_PLLCFGR_PLLQ_Pos);

    pll_config->pll_src =
        (rcc_pll_src_e)((rcc_pllcfgr & RCC_PLLCFGR_PLLSRC_Msk) >>
                        RCC_PLLCFGR_PLLSRC_Pos);

    uint32_t pllp_bitfield =
        (rcc_pllcfgr & RCC_PLLCFGR_PLLP_Msk) >> RCC_PLLCFGR_PLLP_Pos;

    uint16_t pllp = s_rcc_convert_pllp_value(
        pllp_bitfield,
        FIELD_PLLP_DIVIDERS,
        REAL_PLLP_DIVIDERS);

    pll_config->pllp = pllp;

    return RCC_OK;
}