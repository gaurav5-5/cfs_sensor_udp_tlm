#include "cfe.h"
#include "cfe_time.h"
#include "sns_rd_app_sensorlib.h"

#include <stdio.h>

static uint32 Sensorlib_RNG_Seed = 12345;

static void Sensorlib_SetSeed(void)
{
    CFE_TIME_SysTime_t now = CFE_TIME_GetTime();
    Sensorlib_RNG_Seed     = (uint32)(now.Seconds ^ now.Subseconds);
}

CFE_Status_t Sensorlib_Init(int sns_id)
{
    (void)printf("<<< Sensor %d Initialized >>>", sns_id);
    Sensorlib_SetSeed();

    return CFE_SUCCESS;
}

CFE_Status_t Sensorlib_Read(int sns_id, int *val)
{
    Sensorlib_SetSeed();
    (*val) = (1934700397 * Sensorlib_RNG_Seed + 12345) & 0x7fffffff;

    return CFE_SUCCESS;
}
