#ifndef __ESP_AT_H__
#define __ESP_AT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "image.h"
typedef enum
{
    ESPAT_RST = 0,
    ESPAT_AT,
    ESPAT_ATE0,
    ESPAT_CWMODE,
    ESPAT_CWJAP
} ESPAT_State_t;

#define KEY "SjpsqGQzzLXIxkWfs"
#define WIFI_SSID "hhh"
#define WIFI_PASS "12345678"
#define WEATHER_LOCATION_ID "WSKMS3KBE2JM"



void ESPAT_Init(UART_HandleTypeDef *huart);
int  ESPAT_SendCmdWait(const char *cmd, const char *expect, uint32_t timeout_ms);
/* return: 0=connected, 1=step done continue, -1=failed */
int  ESPAT_ConnectWiFi(const char *ssid, const char *password, uint32_t timeout_ms);
int  ESPAT_GetWeatherNow(const char *location_id, char *weather_text, uint16_t text_size, int *temperature, int *weather_code);
int  ESPAT_GetWeatherNowEx(const char *location_id,
                           char *city, uint16_t city_size,
                           char *update_time, uint16_t time_size,
                           char *weather_text, uint16_t text_size,
                           int *temperature, int *weather_code);
void ESPAT_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void UI_FormatTimeHM(const char *src, char *dst, uint16_t dst_size);
const image_t *GetWeatherIconByCode(int code);
#ifdef __cplusplus
}
#endif

#endif
