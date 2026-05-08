#ifndef _DRIVER_DHT11_H
#define _DRIVER_DHT11_H

#ifdef __cplusplus
extern "C" {
#endif

void DHT11_Init(void);
int DHT11_Read(int *hum, int *temp);
void DHT11_Test(void);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_DHT11_H */
