/*
 * t18.h
 *
 * Created: 6/9/2019 10:44:33 AM
 *  Author: A41450
 */ 


#ifndef T18_H_
#define T18_H_

/* MXT_SPT_COMMSCONFIG_T18 */
#define MXT_COMMS_CTRL		0
#define MXT_COMMS_CMD		1
#define MXT_COMMS_RETRIGEN      BIT(6)

/* T18 Memory Space */
typedef struct object_t18 {
	u8 ctrl;
	u8 cmd;
} __attribute__ ((packed)) object_t18_t;

#endif /* T18_H_ */