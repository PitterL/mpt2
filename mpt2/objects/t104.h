/*
 * t104.h
 *
 * Created: 6/12/2019 2:17:06 PM
 *  Author: A41450
 */ 


#ifndef T104_H_
#define T104_H_

# define MXT_T104_CTRL 0

#define MXT_T104_CTRL_ENABLE BIT(0)

typedef struct object_t104 {
	u8 ctrl;
	u8 xgain;
	u8 xtchthr;
	u8 xtchhyst;
	u8 xintthr;
	u8 xinthyst;
	u8 ygain;
	u8 ytchthr;
	u8 ytchhyst;
	u8 yintthr;
	u8 yinthyst;
} object_t104_t;

typedef struct t104_data {
	txx_data_t common;
	/*nodes_desc_t*/void *ns;
} t104_data_t;

#define MXT_SPT_AUXTOUCHCONFIG_T104_INST /*MXT_TOUCH_MULTI_T9_INST*/1

ssint object_t104_init(u8 rid,  const /*qtouch_config_t*/void *def, void *mem, const /*mpt_api_callback_t*/void *cb);
void object_t104_start(u8 loaded);
void object_t104_data_sync(u8 rw);

#endif /* T104_H_ */