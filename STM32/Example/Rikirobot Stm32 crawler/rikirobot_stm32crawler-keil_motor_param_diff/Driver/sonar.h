#ifdef __cplusplus
extern "C" {
#endif
#ifndef _SONAR_H_
#define _SONAR_H_

#include "config.h"

void sonar_init(uint32_t _arr, uint32_t _psc);
float get_distance(void);

#endif //_SONAR_H_

#ifdef __cplusplus
}
#endif
