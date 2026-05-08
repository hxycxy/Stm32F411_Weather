#include "driver_timer.h"

#include "stm32f4xx_hal.h"
#include "tim.h"

static uint8_t s_timer_inited = 0U;

static uint32_t TIM4_GetClockHz(void)
{
    RCC_ClkInitTypeDef clk_cfg;
    uint32_t flash_latency;
    uint32_t pclk1;

    HAL_RCC_GetClockConfig(&clk_cfg, &flash_latency);
    pclk1 = HAL_RCC_GetPCLK1Freq();

    if (clk_cfg.APB1CLKDivider == RCC_HCLK_DIV1)
    {
        return pclk1;
    }

    return pclk1 * 2U;
}

int driver_timer_init(void)
{
    uint32_t tim_clk;

    if (s_timer_inited != 0U)
    {
        return 0;
    }

    /* TIM4 由 MX_TIM4_Init() 创建句柄，这里只做 1us 计数配置并启动 */
    if (htim4.Instance != TIM4)
    {
        return -1;
    }

    tim_clk = TIM4_GetClockHz();
    htim4.Init.Prescaler = (tim_clk / 1000000U) - 1U;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 0xFFFFU;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
    {
        return -1;
    }

    __HAL_TIM_SET_COUNTER(&htim4, 0U);
    if (HAL_TIM_Base_Start(&htim4) != HAL_OK)
    {
        return -1;
    }

    s_timer_inited = 1U;
    return 0;
}

void udelay(int us)
{
    uint32_t remaining;
    uint32_t wait_us;
    uint16_t start;

    if (us <= 0)
    {
        return;
    }

    if (driver_timer_init() != 0)
    {
        return;
    }

    remaining = (uint32_t)us;
    while (remaining != 0U)
    {
        wait_us = (remaining > 0xFFFFU) ? 0xFFFFU : remaining;
        start = (uint16_t)__HAL_TIM_GET_COUNTER(&htim4);
        while ((uint16_t)(__HAL_TIM_GET_COUNTER(&htim4) - start) < (uint16_t)wait_us)
        {
        }
        remaining -= wait_us;
    }
}

void mdelay(int ms)
{
    int i;

    if (ms <= 0)
    {
        return;
    }

    for (i = 0; i < ms; i++)
    {
        udelay(1000);
    }
}

uint64_t system_get_ns(void)
{
    if (driver_timer_init() != 0)
    {
        return 0ULL;
    }

    return (uint64_t)HAL_GetTick() * 1000000ULL;
}
