/**
 * @file
 *
 * Define UDP Telemetry App Message IDs
 *
 * \note The Sensor Read App assumes default configuration which uses V1 of message id implementation
 */

#ifndef UDP_TLM_MSGIDS_H
#define UDP_TLM_MSGIDS_H

/* V1 Command Message IDs must be 0x18xx */
#define UDP_TLM_CMD_MID     0x1894
#define UDP_TLM_SEND_HK_MID 0x1895

/* V1 Telemetry Message IDs must be 0x08xx */
#define UDP_TLM_HK_TLM_MID 0x0896

#endif /* UDP_TLM_MSGIDS_H */
