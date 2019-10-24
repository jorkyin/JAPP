#ifndef _RT_MAP_H_
#define _RT_MAP_H_


#include "rt_p2p_types.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

typedef struct _RT_DEV_HANDLE_MAP_
{
	char key[64];
	int value;
}Rt_dev_handle_map;


/**
*
*/
void RT_InitMap();

void RT_FreeMap();

/**
* 初始化map,用于存储设备句柄
*/
int RT_AddHandleToMap(const RT_APP_CHAR *did,RT_APP_UPTR u32Handle);

/**
* 删除handle
*/
int RT_DeleteHanleFromMap(const RT_APP_CHAR *did);

/**
* 通过did,获取handle
*/
int RT_GetHandleFromMap(const RT_APP_CHAR *did, RT_APP_UPTR* handle);

void RT_PrintMap();


#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */


#endif /*_RT_MAP_H_*/

