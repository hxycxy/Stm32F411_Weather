#include "driver_dht11.h"

#include "driver_timer.h"
#include "main.h"

#define DHT11_START_LOW_MS        20
#define DHT11_RELEASE_DELAY_US    30
#define DHT11_ACK_TIMEOUT_US      120
#define DHT11_BIT_TIMEOUT_US      100
#define DHT11_SAMPLE_DELAY_US     40

typedef struct
{
    uint8_t humidity_int;
    uint8_t humidity_dec;
    uint8_t temperature_int;
    uint8_t temperature_dec;
    uint8_t checksum;
} DHT11_Frame_t;

static void DHT11_PinSet(int val)
{
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, val ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static int DHT11_PinRead(void)
{
    return (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET) ? 1 : 0;
}

static int DHT11_WaitForLevel(int level, int timeout_us)
{
    while (timeout_us-- > 0)
    {
        if (DHT11_PinRead() == level)
        {
            return 0;
        }

        udelay(1);
    }

    return -1;
}

static void DHT11_Start(void)
{
    DHT11_PinSet(0);
    mdelay(DHT11_START_LOW_MS);
    DHT11_PinSet(1);
    udelay(DHT11_RELEASE_DELAY_US);
}

static int DHT11_WaitAck(void)
{
    if (DHT11_WaitForLevel(0, DHT11_ACK_TIMEOUT_US) != 0)
    {
        return -1;
    }

    if (DHT11_WaitForLevel(1, DHT11_ACK_TIMEOUT_US) != 0)
    {
        return -1;
    }

    if (DHT11_WaitForLevel(0, DHT11_ACK_TIMEOUT_US) != 0)
    {
        return -1;
    }

    return 0;
}

static int DHT11_ReadByte(void)
{
    int bit_index;
    int data = 0;

    for (bit_index = 0; bit_index < 8; bit_index++)
    {
        if (DHT11_WaitForLevel(1, DHT11_BIT_TIMEOUT_US) != 0)
        {
            return -1;
        }

        udelay(DHT11_SAMPLE_DELAY_US);

        data <<= 1;
        if (DHT11_PinRead() == 1)
        {
            data |= 1;
        }

        if (DHT11_WaitForLevel(0, DHT11_BIT_TIMEOUT_US) != 0)
        {
            return -1;
        }
    }

    return data;
}

static int DHT11_ReadFrame(DHT11_Frame_t *frame)
{
    int byte_value;

    if (frame == NULL)
    {
        return -1;
    }

    byte_value = DHT11_ReadByte();
    if (byte_value < 0)
    {
        return -1;
    }
    frame->humidity_int = (uint8_t)byte_value;

    byte_value = DHT11_ReadByte();
    if (byte_value < 0)
    {
        return -1;
    }
    frame->humidity_dec = (uint8_t)byte_value;

    byte_value = DHT11_ReadByte();
    if (byte_value < 0)
    {
        return -1;
    }
    frame->temperature_int = (uint8_t)byte_value;

    byte_value = DHT11_ReadByte();
    if (byte_value < 0)
    {
        return -1;
    }
    frame->temperature_dec = (uint8_t)byte_value;

    byte_value = DHT11_ReadByte();
    if (byte_value < 0)
    {
        return -1;
    }
    frame->checksum = (uint8_t)byte_value;

    return 0;
}

void DHT11_Init(void)
{
    driver_timer_init();
    DHT11_PinSet(1);
}

int DHT11_Read(int *hum, int *temp)
{
    DHT11_Frame_t frame;
    uint32_t primask;
    int ret = -1;

    if ((hum == NULL) || (temp == NULL))
    {
        return -1;
    }

    /* 1. Send the start pulse. */
    DHT11_Start();

    /* 2. Disable interrupts during the critical timing window. */
    primask = __get_PRIMASK();
    __disable_irq();

    /* 3. Wait for DHT11 response: low -> high -> low. */
    if (DHT11_WaitAck() != 0)
    {
        goto out;
    }

    /* 4. Read the 5-byte frame. */
    if (DHT11_ReadFrame(&frame) != 0)
    {
        goto out;
    }

    /* 5. Verify checksum. */
    if ((uint8_t)(frame.humidity_int + frame.humidity_dec +
                  frame.temperature_int + frame.temperature_dec) != frame.checksum)
    {
        goto out;
    }

    *hum = frame.humidity_int;
    *temp = frame.temperature_int;
    ret = 0;

out:
    if (primask == 0U)
    {
        __enable_irq();
    }

    DHT11_PinSet(1);
    return ret;
}

void DHT11_Test(void)
{
}
