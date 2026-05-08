#include "espAT.h"

#include "usart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ESPAT_RX_BUF_SIZE 256U
#define ESPAT_RX_FIFO_SIZE 2048U
#define ESPAT_HTTP_BUF_SIZE 1024U

static UART_HandleTypeDef *s_esp_huart = NULL;
static ESPAT_State_t s_wifi_state = ESPAT_RST;
static uint8_t s_wifi_connected = 0;
static uint8_t s_rx_it_byte = 0;
static volatile uint16_t s_rx_head = 0;
static volatile uint16_t s_rx_tail = 0;
static uint8_t s_rx_fifo[ESPAT_RX_FIFO_SIZE];

static void ESPAT_RxFifoClear(void)
{
    s_rx_head = 0;
    s_rx_tail = 0;
}

static void ESPAT_RxFifoPush(uint8_t ch)
{
    uint16_t next = (uint16_t)((s_rx_head + 1U) % ESPAT_RX_FIFO_SIZE);

    if (next == s_rx_tail)
    {
        s_rx_tail = (uint16_t)((s_rx_tail + 1U) % ESPAT_RX_FIFO_SIZE);
    }

    s_rx_fifo[s_rx_head] = ch;
    s_rx_head = next;
}

static int ESPAT_RxFifoPop(uint8_t *ch)
{
    if ((ch == NULL) || (s_rx_head == s_rx_tail))
    {
        return 0;
    }

    *ch = s_rx_fifo[s_rx_tail];
    s_rx_tail = (uint16_t)((s_rx_tail + 1U) % ESPAT_RX_FIFO_SIZE);
    return 1;
}

static void ESPAT_StartRxIT(void)
{
    if (s_esp_huart != NULL)
    {
        HAL_UART_Receive_IT(s_esp_huart, &s_rx_it_byte, 1);
    }
}

static UART_HandleTypeDef *ESPAT_GetUart(void)
{
    if (s_esp_huart == NULL)
    {
        s_esp_huart = &huart2;
    }
    return s_esp_huart;
}

static void ESPAT_ClearRx(UART_HandleTypeDef *huart)
{
    (void)huart;
    ESPAT_RxFifoClear();
}

static int ESPAT_SendCmdCollect(const char *cmd, char *rx_buf, uint16_t rx_size, uint32_t timeout_ms)
{
    UART_HandleTypeDef *huart;
    uint32_t start_tick;
    uint8_t ch;
    uint16_t len = 0;

    if ((cmd == NULL) || (rx_buf == NULL) || (rx_size < 2U))
    {
        return -1;
    }

    huart = ESPAT_GetUart();
    memset(rx_buf, 0, rx_size);
    ESPAT_ClearRx(huart);

    if (HAL_UART_Transmit(huart, (uint8_t *)cmd, (uint16_t)strlen(cmd), 1000) != HAL_OK)
    {
        return -1;
    }
    if (HAL_UART_Transmit(huart, (uint8_t *)"\r\n", 2, 1000) != HAL_OK)
    {
        return -1;
    }

    start_tick = HAL_GetTick();
    while ((HAL_GetTick() - start_tick) < timeout_ms)
    {
        if (!ESPAT_RxFifoPop(&ch))
        {
            HAL_Delay(1);
            continue;
        }

        if (len < (rx_size - 1U))
        {
            rx_buf[len++] = (char)ch;
            rx_buf[len] = '\0';
        }
        else
        {
            memmove(rx_buf, rx_buf + 1, rx_size - 2U);
            rx_buf[rx_size - 2U] = (char)ch;
            rx_buf[rx_size - 1U] = '\0';
            len = rx_size - 1U;
        }

        if ((strstr(rx_buf, "+HTTPCLIENT:") != NULL) &&
            ((strstr(rx_buf, "\r\nOK\r\n") != NULL) || (strstr(rx_buf, "\nOK\r\n") != NULL) || (strstr(rx_buf, "\nOK\n") != NULL)))
        {
            return 0;
        }

        if ((strstr(rx_buf, "ERROR") != NULL) || (strstr(rx_buf, "FAIL") != NULL))
        {
            return -1;
        }
    }

    return -1;
}

static int ESPAT_ParseWeather(const char *rx,
                              char *weather_text,
                              uint16_t text_size,
                              int *temperature,
                              int *weather_code)
{
    const char *p;
    const char *q;
    uint16_t copy_len;
    char temp_buf[8];
    char code_buf[8];

    if ((rx == NULL) || (weather_text == NULL) || (temperature == NULL) || (weather_code == NULL) || (text_size < 2U))
    {
        return -1;
    }

    p = strstr(rx, "\"text\":\"");
    if (p == NULL)
    {
        return -1;
    }
    p += strlen("\"text\":\"");
    q = strchr(p, '"');
    if (q == NULL)
    {
        return -1;
    }

    copy_len = (uint16_t)(q - p);
    if (copy_len >= text_size)
    {
        copy_len = text_size - 1U;
    }
    memcpy(weather_text, p, copy_len);
    weather_text[copy_len] = '\0';

    p = strstr(q, "\"temperature\":\"");
    if (p == NULL)
    {
        p = strstr(rx, "\"temperature\":\"");
        if (p == NULL)
        {
            return -1;
        }
    }
    p += strlen("\"temperature\":\"");
    q = strchr(p, '"');
    if ((q == NULL) || (q <= p))
    {
        return -1;
    }

    copy_len = (uint16_t)(q - p);
    if (copy_len >= sizeof(temp_buf))
    {
        copy_len = sizeof(temp_buf) - 1U;
    }
    memcpy(temp_buf, p, copy_len);
    temp_buf[copy_len] = '\0';
    *temperature = atoi(temp_buf);

    p = strstr(rx, "\"code\":\"");
    if (p == NULL)
    {
        return -1;
    }
    p += strlen("\"code\":\"");
    q = strchr(p, '"');
    if ((q == NULL) || (q <= p))
    {
        return -1;
    }
    copy_len = (uint16_t)(q - p);
    if (copy_len >= sizeof(code_buf))
    {
        copy_len = sizeof(code_buf) - 1U;
    }
    memcpy(code_buf, p, copy_len);
    code_buf[copy_len] = '\0';
    *weather_code = atoi(code_buf);

    return 0;
}

static int ESPAT_ParseCity(const char *rx, char *city, uint16_t city_size)
{
    const char *p;
    const char *q;
    uint16_t copy_len;

    if ((rx == NULL) || (city == NULL) || (city_size < 2U))
    {
        return -1;
    }

    p = strstr(rx, "\"name\":\"");
    if (p == NULL)
    {
        return -1;
    }

    p += strlen("\"name\":\"");
    q = strchr(p, '"');
    if ((q == NULL) || (q <= p))
    {
        return -1;
    }

    copy_len = (uint16_t)(q - p);
    if (copy_len >= city_size)
    {
        copy_len = city_size - 1U;
    }
    memcpy(city, p, copy_len);
    city[copy_len] = '\0';

    return 0;
}

static int ESPAT_ParseUpdateTime(const char *rx, char *update_time, uint16_t time_size)
{
    const char *p;
    const char *q;
    uint16_t copy_len;
    const char *time_part;

    if ((rx == NULL) || (update_time == NULL) || (time_size < 2U))
    {
        return -1;
    }

    p = strstr(rx, "\"last_update\":\"");
    if (p == NULL)
    {
        return -1;
    }
    p += strlen("\"last_update\":\"");
    q = strchr(p, '"');
    if ((q == NULL) || (q <= p))
    {
        return -1;
    }

    time_part = strchr(p, 'T');
    if ((time_part != NULL) && (time_part < q) && ((q - time_part) >= 9))
    {
        time_part += 1; /* skip 'T' */
        copy_len = 8;   /* HH:MM:SS */
        if (copy_len >= time_size)
        {
            copy_len = time_size - 1U;
        }
        memcpy(update_time, time_part, copy_len);
        update_time[copy_len] = '\0';
        return 0;
    }

    copy_len = (uint16_t)(q - p);
    if (copy_len >= time_size)
    {
        copy_len = time_size - 1U;
    }
    memcpy(update_time, p, copy_len);
    update_time[copy_len] = '\0';
    return 0;
}

void ESPAT_Init(UART_HandleTypeDef *huart)
{
    s_esp_huart = (huart != NULL) ? huart : &huart2;
    s_wifi_state = ESPAT_RST;
    s_wifi_connected = 0;
    ESPAT_RxFifoClear();
    ESPAT_StartRxIT();
}

int ESPAT_SendCmdWait(const char *cmd, const char *expect, uint32_t timeout_ms)
{
    UART_HandleTypeDef *huart;
    uint32_t start_tick;
    uint8_t ch;
    char rx_buf[ESPAT_RX_BUF_SIZE];
    uint16_t len = 0;

    if ((cmd == NULL) || (expect == NULL))
    {
        return -1;
    }

    huart = ESPAT_GetUart();
    memset(rx_buf, 0, sizeof(rx_buf));

    ESPAT_ClearRx(huart);

    if (HAL_UART_Transmit(huart, (uint8_t *)cmd, (uint16_t)strlen(cmd), 1000) != HAL_OK)
    {
        return -1;
    }

    if (HAL_UART_Transmit(huart, (uint8_t *)"\r\n", 2, 1000) != HAL_OK)
    {
        return -1;
    }

    start_tick = HAL_GetTick();
    while ((HAL_GetTick() - start_tick) < timeout_ms)
    {
        if (!ESPAT_RxFifoPop(&ch))
        {
            HAL_Delay(1);
            continue;
        }

        if (len < (ESPAT_RX_BUF_SIZE - 1U))
        {
            rx_buf[len++] = (char)ch;
            rx_buf[len] = '\0';
        }
        else
        {
            memmove(rx_buf, rx_buf + 1, ESPAT_RX_BUF_SIZE - 2U);
            rx_buf[ESPAT_RX_BUF_SIZE - 2U] = (char)ch;
            rx_buf[ESPAT_RX_BUF_SIZE - 1U] = '\0';
        }

        if (strstr(rx_buf, expect) != NULL)
        {
            return 0;
        }

        if ((strstr(rx_buf, "ERROR") != NULL) || (strstr(rx_buf, "FAIL") != NULL))
        {
            return -1;
        }
    }

    return -1;
}

int ESPAT_ConnectWiFi(const char *ssid, const char *password, uint32_t timeout_ms)
{
    int ret;
    char cmd[160];

    if ((ssid == NULL) || (password == NULL))
    {
        return -1;
    }

    if (s_wifi_connected)
    {
        printf("WiFi already connected\r\n");
        return 0;
    }

    switch (s_wifi_state)
    {
    case ESPAT_RST:
        ret = ESPAT_SendCmdWait("AT+RST", "OK", 5000);
        printf("Resetting ESP32...\r\n");
        if (ret == 0)
        {
            HAL_Delay(1200);
        }
        break;

    case ESPAT_AT:
        ret = ESPAT_SendCmdWait("AT", "OK", 1000);
        printf("Sending AT command...\r\n");
        break;

    case ESPAT_ATE0:
        ret = ESPAT_SendCmdWait("ATE0", "OK", 1000);
        printf("Disabling echo...\r\n");
        break;

    case ESPAT_CWMODE:
        ret = ESPAT_SendCmdWait("AT+CWMODE=1", "OK", 1000);
        printf("Setting WiFi mode...\r\n");
        break;

    case ESPAT_CWJAP:
        snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"", ssid, password);
        ret = ESPAT_SendCmdWait(cmd, "OK", timeout_ms);
        printf("Connecting to WiFi...\r\n");
        break;

    default:
        return -1;
    }

    if (ret != 0)
    {
        return -1;
    }

    if (s_wifi_state == ESPAT_CWJAP)
    {
        s_wifi_connected = 1;
        return 0;
    }

    s_wifi_state = (ESPAT_State_t)(s_wifi_state + 1);
    return 1;
}

int ESPAT_GetWeatherNow(const char *location_id, char *weather_text, uint16_t text_size, int *temperature, int *weather_code)
{
    char cmd[360];
    char rx_buf[ESPAT_HTTP_BUF_SIZE];
    int cmd_len;

    if ((location_id == NULL) || (weather_text == NULL) || (temperature == NULL) || (weather_code == NULL))
    {
        return -1;
    }

    if (!s_wifi_connected)
    {
        return -1;
    }

    cmd_len = snprintf(cmd, sizeof(cmd),
                       "AT+HTTPCLIENT=2,0,"
                       "\"https://api.seniverse.com/v3/weather/now.json?key=%s&location=%s&language=zh-Hans&unit=c\",,,2",
                       KEY, location_id);
    if ((cmd_len <= 0) || (cmd_len >= (int)sizeof(cmd)))
    {
        return -1;
    }

    if (ESPAT_SendCmdCollect(cmd, rx_buf, sizeof(rx_buf), 20000) != 0)
    {
        printf("[HTTP] REQ FAIL: %s\r\n", rx_buf);
        return -1;
    }

    if (strstr(rx_buf, "\"results\"") == NULL)
    {
        printf("[HTTP] BAD JSON: %s\r\n", rx_buf);
        return -1;
    }

    if (ESPAT_ParseWeather(rx_buf, weather_text, text_size, temperature, weather_code) != 0)
    {
        printf("[HTTP] PARSE FAIL: %s\r\n", rx_buf);
        return -1;
    }

    return 0;
}

int ESPAT_GetWeatherNowEx(const char *location_id,
                          char *city, uint16_t city_size,
                          char *update_time, uint16_t time_size,
                          char *weather_text, uint16_t text_size,
                          int *temperature, int *weather_code)
{
    char cmd[360];
    char rx_buf[ESPAT_HTTP_BUF_SIZE];
    int cmd_len;

    if ((location_id == NULL) || (city == NULL) || (update_time == NULL) || (weather_text == NULL) || (temperature == NULL) || (weather_code == NULL))
    {
        return -1;
    }

    if (!s_wifi_connected)
    {
        return -1;
    }

    cmd_len = snprintf(cmd, sizeof(cmd),
                       "AT+HTTPCLIENT=2,0,"
                       "\"https://api.seniverse.com/v3/weather/now.json?key=%s&location=%s&language=zh-Hans&unit=c\",,,2",
                       KEY, location_id);
    if ((cmd_len <= 0) || (cmd_len >= (int)sizeof(cmd)))
    {
        return -1;
    }

    if (ESPAT_SendCmdCollect(cmd, rx_buf, sizeof(rx_buf), 20000) != 0)
    {
        printf("[HTTP] REQ FAIL: %s\r\n", rx_buf);
        return -1;
    }

    if (strstr(rx_buf, "\"results\"") == NULL)
    {
        printf("[HTTP] BAD JSON: %s\r\n", rx_buf);
        return -1;
    }

    if (ESPAT_ParseCity(rx_buf, city, city_size) != 0)
    {
        printf("[HTTP] CITY PARSE FAIL: %s\r\n", rx_buf);
        return -1;
    }

    if (ESPAT_ParseUpdateTime(rx_buf, update_time, time_size) != 0)
    {
        printf("[HTTP] TIME PARSE FAIL: %s\r\n", rx_buf);
        return -1;
    }

    if (ESPAT_ParseWeather(rx_buf, weather_text, text_size, temperature, weather_code) != 0)
    {
        printf("[HTTP] PARSE FAIL: %s\r\n", rx_buf);
        return -1;
    }

    return 0;
}

void ESPAT_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if ((s_esp_huart != NULL) && (huart->Instance == s_esp_huart->Instance))
    {
        ESPAT_RxFifoPush(s_rx_it_byte);
        HAL_UART_Receive_IT(s_esp_huart, &s_rx_it_byte, 1);
    }
}
/*对时间格式进行格式化，将"HH:MM"格式转换为"HH MM"格式*/
void UI_FormatTimeHM(const char *src, char *dst, uint16_t dst_size)
{
  if ((dst == NULL) || (dst_size < 6)) {
    return;
  }

  if ((src != NULL) &&
      (src[0] != '\0') && (src[1] != '\0') &&
      (src[2] == ':') &&
      (src[3] != '\0') && (src[4] != '\0')) {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = ' ';
    dst[3] = src[3];
    dst[4] = src[4];
    dst[5] = '\0';
  } else {
    snprintf(dst, dst_size, "-- --");
  }
}

const image_t *GetWeatherIconByCode(int code)
{
  if ((code == 0) || (code == 1) || (code == 2) || (code == 3))
  {
    return &icon_qing;
  }
  if ((code == 4) || (code == 5) || (code == 6) || (code == 7) || (code == 8) || (code == 9))
  {
    return &icon_duoyun;
  }
  if ((code >= 10) && (code <= 19))
  {
    return &icon_zhongyu;
  }
  if ((code >= 20) && (code <= 25))
  {
    return &icon_na;
  }
  if ((code >= 26) && (code <= 29))
  {
    return &icon_zhongxue;
  }
  if ((code == 30) || (code == 31))
  {
    return &icon_yueliang;
  }
  if ((code == 37) || (code == 38))
  {
    return &icon_leizhenyu;
  }
  return &icon_yintian;
}
