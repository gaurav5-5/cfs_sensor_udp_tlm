#ifndef SNS_RD_APP_SENSORLIB_H
#define SNS_RD_APP_SENSORLIB_H

#include "cfe.h"

CFE_Status_t Sensorlib_Init(int);
CFE_Status_t Sensorlib_Read(int, int *);

#endif // SNS_RD_APP_SENSORLIB_H
