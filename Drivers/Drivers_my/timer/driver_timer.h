#ifndef _DRIVER_TIMER_H
#define _DRIVER_TIMER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int driver_timer_init(void);
void udelay(int us);
void mdelay(int ms);
uint64_t system_get_ns(void);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_TIMER_H */
