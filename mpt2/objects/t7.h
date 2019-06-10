/*
 * t7.h
 *
 * Created: 6/9/2019 10:30:19 AM
 *  Author: A41450
 */ 


#ifndef T7_H_
#define T7_H_

/* MXT_GEN_POWER_T7 field */
struct t7_config {
	u8 idle;
	u8 active;
}  __attribute__ ((packed));

#define MXT_POWER_CFG_RUN		0
#define MXT_POWER_CFG_DEEPSLEEP		1

/* T7 Memory space */
typedef struct object_t7 {
	u8 idleacqint;
	u8 actvaqint;
	u8 actv2idleto;
} __attribute__ ((packed)) object_t7_t;

#endif /* T7_H_ */