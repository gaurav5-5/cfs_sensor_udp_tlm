/**
 * @file
 *
 * Define sns_rd app table
 */

#ifndef SNS_RD_APP_TABLE_H
#define SNS_RD_APP_TABLE_H

#include "common_types.h"

/*
** Table structure
*/
typedef struct
{
    uint32 ReadIntervalMs;
    uint32 TimClkAccuracy;
} SNS_RD_APP_Table_t;

#endif /* SNS_RD_APP_TABLE_H */
