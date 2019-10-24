
#include <memory.h>
#include <log.h>
#include "RTMap.h"


//map的最大尺度
#define MAP_MAX_SIZE 20

//存储键值对 handle did
Rt_dev_handle_map g_map[MAP_MAX_SIZE];


/**
* 
*/
void RT_InitMap()
{
	int i=0;
	for(i=0 ; i<MAP_MAX_SIZE ;i++)
	{
		g_map[i].value=0;
		memset(&g_map[i].key,0,sizeof(g_map[i].key));	
	}	
}

void RT_FreeMap()
{
	int i=0;
	for(i=0 ; i<MAP_MAX_SIZE ;i++)
	{
		g_map[i].value=0;
		memset(&g_map[i].key,0,sizeof(g_map[i].key));	
	}
}

/**
* 初始化map,用于存储设备句柄
*/
int RT_AddHandleToMap(const RT_APP_CHAR *did,RT_APP_UPTR u32Handle)
{
	int i=0;
	for(i=0 ; i<MAP_MAX_SIZE ;i++)
	{
		if(g_map[i].value==0)
		{
			g_map[i].value=u32Handle;
			strcpy(g_map[i].key,did);
			return i;
		}
	}

	return -1;
}


/**
* 删除handle
*/
int RT_DeleteHanleFromMap(const RT_APP_CHAR *did)
{
	int i=0;
	for(i=0 ; i<MAP_MAX_SIZE ;i++)
	{
		if(strcmp(g_map[i].key,did)==0)
		{
			g_map[i].value=0;
			memset(&g_map[i].key,0,sizeof(g_map[i].key));
			return i;
		}
	}
	return -1;
}


/**
* 通过did,获取handle
*/
int RT_GetHandleFromMap(const RT_APP_CHAR *did, RT_APP_UPTR* handle)
{
	int i=0;
	for(i=0 ; i<MAP_MAX_SIZE ;i++)
	{
		if(strcmp(g_map[i].key,did)==0 && g_map[i].value!=0)
		{
			*handle=g_map[i].value;
			return i;
		}
	}
	return -1;
}


void RT_PrintMap()
{
	int i=0;
	for(i=0 ; i<MAP_MAX_SIZE ;i++)
	{
		if(g_map[i].value != 0)
		{
			LOGD("key=%s,value=%d\n",g_map[i].key,g_map[i].value);
		}
	}	
}




