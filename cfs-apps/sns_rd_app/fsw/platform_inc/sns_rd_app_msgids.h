/**
 * @file
 *
 * Define Sensor Read App Message IDs
 *
 * \note The Sensor Read App assumes default configuration which uses V1 of message id implementation
 */

#ifndef SNS_RD_APP_MSGIDS_H
#define SNS_RD_APP_MSGIDS_H

/* V1 Command Message IDs must be 0x18xx */
#define SNS_RD_APP_CMD_MID     0x1890
#define SNS_RD_APP_SEND_HK_MID 0x1891
/* V1 Telemetry Message IDs must be 0x08xx */
#define SNS_RD_APP_HK_TLM_MID     0x0892
#define SNS_RD_APP_SENSOR_TLM_MID 0x0893

#endif /* SNS_RD_APP_MSGIDS_H */
