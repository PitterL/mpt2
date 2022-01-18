/*
 * t109.h
 *
 * Created: 6/16/2019 2:48:23 AM
 *  Author: A41450
 */ 


#ifndef T109_H_
#define T109_H_

typedef struct object_t109 {
	u8 ctrl;
	u8 rsv0;
	u8 cmdonreset;
	u8 cmd;
	u8 rsv1[4];
	u8 tunecfg;
} object_t109_t;

#define MXT_SPT_SELFCAPGLOBALCONFIG_T109_CTRL 0
#define MXT_T109_CMD_SNGLENDEN BIT(2)
#define MXT_T109_CMD_RPTEN BIT(1)

#define MXT_SPT_SELFCAPGLOBALCONFIG_T109_CMD 1

#define CMD_NONE 0
#define CMD_TUNE 1
#define CMD_STORE_TO_NVM 2
#define CMD_APPLY_FROM_NVM 3
#define CMD_STORE_TO_CONFIG 4
#define CMD_APPLY_FROM_CONFIG 5
#define CMD_APPLY_FROM_NVM_AND_STORE_TO CONFIG 6
#define CMD_REFRESH_CONFIG_RAM 7

#define MSG_ERRCODE_SUCCESSFUL 0x00
#define MSG_ERRCODE_WRITE_ERR 0x20
#define MSG_ERRCODE_INVALID_CRC 0x08
#define MSG_ERRCODE_READ_ERROR 0x10

typedef struct object_t109_status {
	struct {
		u8 cmd;
		u8 errorcode;
	}__attribute__ ((packed)) result;
	
#define T109_DEF_NUM_SENSORS 14	// Need to be check whether the actual sensor channel (DEF_NUM_CHANNEL_NODES in touch.h) is more than this value
	u16 node_comp_caps[T109_DEF_NUM_SENSORS];
	u8 counter;
} object_t109_Status_t;

typedef struct t109_data {
	txx_data_t common;
	object_t109_Status_t status;
} t109_data_t;

#define MXT_SPT_SELFCAPGLOBALCONFIG_T109_RIDS 1

ssint object_t109_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);
void object_t109_start(u8 unused);
ssint object_api_t109_set_sensor_data(u8 channel, /*const cap_sample_value_t * const*/ const void * cv);
u8 object_t109_single_end_mode(void);
void object_t109_param_sync(void);
void object_t109_data_sync(u8 rw);

#endif /* T109_H_ */