
#ifndef __RT_LOG_H__
#define __RT_LOG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "rt_p2p_types.h"

#define MAX_LOG_SIZE (256)

typedef enum tag_LOG_LEVEl
{
	ENUM_LOGI=0,
	ENUM_LOGD,
	ENUM_LOGE	
}LOG_LEVEl;

typedef RT_APP_VOID (*RT_LOG_CALL_Back)(RT_APP_INT32 logLevel,const RT_APP_CHAR *format);

RT_APP_VOID RT_SHOW_Log(RT_APP_INT32 logLevel,const RT_APP_CHAR *format,...);

RT_APP_VOID RT_LOG_REGISTER_CALL_Back(RT_LOG_CALL_Back logCallBack);
								  
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif //__RT_LOG_H__

