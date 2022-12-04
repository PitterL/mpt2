/*
 * t18.h
 *
 * Created: 6/9/2019 10:44:33 AM
 *  Author: A41450
 */ 


#ifndef T18_H_
#define T18_H_

#ifdef OBJECT_T18

/* MXT_SPT_COMMSCONFIG_T18 */
#define MXT_COMMS_CTRL		0
#define MXT_COMMS_CMD		1

#define MXT_COMMS_CTRL_RETRIGEN      BIT(6)
#define MXT_COMMS_CTRL_DISMNTR		 BIT(7)

/* T18 Memory Space */
typedef struct object_t18 {
	u8 ctrl;
	u8 cmd;
} __attribute__ ((packed)) object_t18_t;

typedef struct txx_data t18_data_t;

ssint object_t18_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);
void object_t18_start(u8 loaded);
void object_t18_data_sync(u8 rw);
bool object_t18_check_retrigger(void);
bool object_t18_check_dismntr(void);

#endif /* OBJECT_T18 */

#endif /* T18_H_ */