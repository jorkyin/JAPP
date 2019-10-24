
#ifndef __RT_P2P_API_DEFINE_H__
#define __RT_P2P_API_DEFINE_H__
#include <time.h>
#include "rt_p2p_types.h"


//Message type
#define MSG_TYPE_P2P_STATUS         0x0
#define MSG_TYPE_P2P_MODE           0x1
#define MSG_TYPE_GET_CAMERA_PARAMS  0x2
#define MSG_TYPE_DECODER_CONTROL    0x3
#define MSG_TYPE_GET_PARAMS         0x4
#define MSG_TYPE_SNAPSHOT           0x5
#define MSG_TYPE_CAMERA_CONTROL     0x6
#define MSG_TYPE_SET_NETWORK        0x7
#define MSG_TYPE_REBOOT_DEVICE      0x8
#define MSG_TYPE_RESTORE_FACTORY    0x9
#define MSG_TYPE_SET_USER           0xa
#define MSG_TYPE_SET_WIFI           0xb
#define MSG_TYPE_SET_DATETIME       0xc
#define MSG_TYPE_GET_STATUS         0xd
#define MSG_TYPE_GET_PTZ_PARAMS     0xe
#define MSG_TYPE_SET_DDNS           0xf
#define MSG_TYPE_SET_MAIL           0x10
#define MSG_TYPE_SET_FTP            0x11
#define MSG_TYPE_SET_ALARM          0x12
#define MSG_TYPE_SET_PTZ            0x13
#define MSG_TYPE_WIFI_SCAN          0x14
#define MSG_TYPE_GET_ALARM_LOG      0x15
#define MSG_TYPE_GET_RECORD         0x16
#define MSG_TYPE_GET_RECORD_FILE    0x17
#define MSG_TYPE_SET_PPPOE          0x18
#define MSG_TYPE_SET_UPNP           0x19
#define MSG_TYPE_DEL_RECORD_FILE    0x1a
#define MSG_TYPE_SET_MEDIA          0x1b
#define MSG_TYPE_SET_RECORD_SCH     0x1c
#define MSG_TYPE_CLEAR_ALARM_LOG    0x1d
#define MSG_TYPE_WIFI_PARAMS        0x1f
#define MSG_TYPE_MAIL_PARAMS        0x20
#define MSG_TYPE_FTP_PARAMS         0x21
#define MSG_TYPE_NETWORK_PARAMS     0x22
#define MSG_TYPE_USER_INFO          0x23
#define MSG_TYPE_DDNS_PARAMS        0x24
#define MSG_TYPE_DATETIME_PARAMS    0x25
#define MSG_TYPE_ALARM_PARAMS       0x26
#define MSG_TYPE_SET_DEVNAME        0x27
#define MSG_TYPE_GET_BILL        	0x32
#define MSG_TYPE_SET_BILL        	0x33
#define MSG_TYPE_SET_AP 			0x34
#define MSG_TYPE_GET_AP 			0x35
#define MSG_TYPE_GET_NEW_RECORD_FILE 0x40

#define MSG_TYPE_GET_NETWORK 	    0x41
#define MSG_TYPE_GET_USER_INFO 	    0x42
#define MSG_TYPE_GET_WIFI 	        0x43
#define MSG_TYPE_GET_FTP 	        0x44
#define MSG_TYPE_GET_EMAIL 	        0x45
#define MSG_TYPE_GET_DDNS 	        0x46
#define MSG_TYPE_GET_TIME 	        0x47
#define MSG_TYPE_GET_ALARM_PARAM 	0x48
#define MSG_TYPE_FORMAT_SD 	        0x49
#define MSG_TYPE_GET_P2P_CAPABILITY 0x4a
#define MSG_TYPE_GET_DOWNLOAD_FILE  0x4b
#define MSG_TYPE_GET_SNAP_UPLOAD_FTP  0x4c

#define MSG_TYPE_SET_REC_MODE  		0x50
#define MSG_TYPE_GET_REC_MODE  		0x51
#define MSG_TYPE_SET_MOBILE_TIME  	0x52
#define MSG_TYPE_GET_REC_CALENDAR   0x53

#define MSG_TYPE_UNMOUNT_SD         0x54
#define MSG_TYPE_GET_ALARM_EX       0x55
#define MSG_TYPE_SET_ALARM_EX       0x56
#define MSG_TYPE_GET_SD_REC_EX      0x57
#define MSG_TYPE_SET_SD_REC_EX      0x58
#define MSG_TYPE_FACTORY_RESET      0x59
#define MSG_TYPE_GET_OSD            0x5a
#define MSG_TYPE_SET_OSD            0x5b
#define MSG_TYPE_SET_GEOG           0x5c /*用于人图设置经纬度信息*/



//decoder control command
#define CMD_PTZ_UP                      0
#define CMD_PTZ_UP_STOP                 1
#define CMD_PTZ_DOWN                    2
#define CMD_PTZ_DOWN_STOP               3
#define CMD_PTZ_LEFT                    4
#define CMD_PTZ_LEFT_STOP               5
#define CMD_PTZ_RIGHT                   6
#define CMD_PTZ_RIGHT_STOP             	7

#define CMD_PTZ_CENTER                  25
#define CMD_PTZ_UP_DOWN                 26
#define CMD_PTZ_UP_DOWN_STOP            27
#define CMD_PTZ_LEFT_RIGHT              28
#define CMD_PTZ_LEFT_RIGHT_STOP         29


#define CMD_PTZ_PREFAB_BIT_SET0         30
#define CMD_PTZ_PREFAB_BIT_SET1         32
#define CMD_PTZ_PREFAB_BIT_SET2         34
#define CMD_PTZ_PREFAB_BIT_SET3         36
#define CMD_PTZ_PREFAB_BIT_SET4         38
#define CMD_PTZ_PREFAB_BIT_SET5         40
#define CMD_PTZ_PREFAB_BIT_SET6         42
#define CMD_PTZ_PREFAB_BIT_SET7         44
#define CMD_PTZ_PREFAB_BIT_SET8         46
#define CMD_PTZ_PREFAB_BIT_SET9         48
#define CMD_PTZ_PREFAB_BIT_SETA         50
#define CMD_PTZ_PREFAB_BIT_SETB         52
#define CMD_PTZ_PREFAB_BIT_SETC         54
#define CMD_PTZ_PREFAB_BIT_SETD         56
#define CMD_PTZ_PREFAB_BIT_SETE         58
#define CMD_PTZ_PREFAB_BIT_SETF         60
#define CMD_PTZ_PREFAB_BIT_SET_EXT      70 //设置预置扩展,由用户自己输入设置值


#define CMD_PTZ_PREFAB_BIT_RUN0         31
#define CMD_PTZ_PREFAB_BIT_RUN1         33
#define CMD_PTZ_PREFAB_BIT_RUN2         35
#define CMD_PTZ_PREFAB_BIT_RUN3         37
#define CMD_PTZ_PREFAB_BIT_RUN4         39
#define CMD_PTZ_PREFAB_BIT_RUN5         41
#define CMD_PTZ_PREFAB_BIT_RUN6         43
#define CMD_PTZ_PREFAB_BIT_RUN7         45
#define CMD_PTZ_PREFAB_BIT_RUN8         47
#define CMD_PTZ_PREFAB_BIT_RUN9         49
#define CMD_PTZ_PREFAB_BIT_RUNA         51
#define CMD_PTZ_PREFAB_BIT_RUNB         53
#define CMD_PTZ_PREFAB_BIT_RUNC         55
#define CMD_PTZ_PREFAB_BIT_RUND         57
#define CMD_PTZ_PREFAB_BIT_RUNE         59
#define CMD_PTZ_PREFAB_BIT_RUNF         61
#define CMD_PTZ_PREFAB_BIT_RUN_EXT      80 //调用预置扩展,由用户自己输入调用值


#define CMD_PTZ_ZOOM_IN   99
#define CMD_PTZ_ZOOM_OUT  100
#define CMD_PTZ_FOCUS_IN  101
#define CMD_PTZ_FOCUS_OUT 102

/*msgtype*/
#define MSG_NOTIFY_TYPE_PPPP_STATUS (0)   /* pppp status */
#define MSG_NOTIFY_TYPE_PPPP_MODE   (1)   /* device type */
#define MSG_NOTIFY_TYPE_STREAM_TYPE (2)
#define MSG_NOTIFY_TYPE_TALK_STATUS (3)

/*pppp status*/
#define PPPP_STATUS_CONNECTING         (0) /* connecting */
#define PPPP_STATUS_INITIALING         (1) /* initialing */
#define PPPP_STATUS_ON_LINE            (2) /* on line */
#define PPPP_STATUS_CONNECT_FAILED     (3) /* connect failed */
#define PPPP_STATUS_DISCONNECT         (4) /*connect is off*/
#define PPPP_STATUS_INVALID_ID         (5) /* invalid id */
#define PPPP_STATUS_DEVICE_NOT_ON_LINE (6)
#define PPPP_STATUS_CONNECT_TIMEOUT    (7) /* connect timeout */
#define PPPP_STATUS_INVALID_USER_PWD   (8) /* user or pwd is invalid */
#define PPPP_STATUS_MAX_CONNECT_CNT    (9) /*超过设备端的最大连接数*/

/*pppp mode*/
#define PPPP_MODE_P2P   (1)
#define PPPP_MODE_RELAY (2)

/*talk status*/
#define PPPP_STATUS_SEND_TALK_DATA     (0) /* user or pwd is invalid */

#ifndef _DEF_STRU_AV_HEAD
#define _DEF_STRU_AV_HEAD
typedef struct tag_AV_HEAD
{
    RT_APP_UINT32   startcode;	//  0xa815aa55
    RT_APP_UCHAR	type;		//  0->264 idr frame 1->264 p frame
    RT_APP_UCHAR  	streamid;	
    RT_APP_UINT16  	militime;	//  diff time
    RT_APP_UINT32 	sectime;	//  diff time
    RT_APP_UINT32   frameno;	//  frameno
    RT_APP_UINT32	len;		//  data len
    RT_APP_UCHAR	version;	// version
    RT_APP_UCHAR	sessid;		//ssid
    RT_APP_UCHAR	other[2];
    //RT_APP_UCHAR	other1[8];
	RT_APP_ULONG64  timestamp;
}AV_HEAD,*PAV_HEAD;
#endif

typedef struct tag_STRU_CAMERA_PARAMS
{
    RT_APP_INT32 resolution;  /*分辨率*/
    RT_APP_INT32 brightness;  /*亮度 1~255*/
    RT_APP_INT32 contrast;    /*对比度 1~255*/
    RT_APP_INT32 hue;         /*色度，不支持*/
    RT_APP_INT32 saturation;  /*模式 0：50hz	1：60hz*/
    RT_APP_INT32 flip;        /*视频旋转方式 0：原始   1：垂直翻转   2：水平镜像  3：垂直翻转 + 水平镜像*/
	RT_APP_INT32 mainFram;    /*主码流帧率*/
	RT_APP_INT32 subFram;     /*副码流帧率*/
	RT_APP_INT32 mode;        /*模式 0：50hz	1：60hz*/
	RT_APP_INT32 ircut;	/* ircut的状态 0:close   1:auto   2:open*/
	RT_APP_INT32 led; /*led状态 0:close 1:open*/
}STRU_CAMERA_PARAMS,*PSTRU_CAMERA_PARAMS;

/*
param	            value
0：分辨率	    0:VGA 1:QVGA
1：亮度	            1~255
2：对比度	        1~255
3：模式	        0：50hz	1：60hz
5：旋转	        0：原始   1：垂直翻转   2：水平镜像  3：垂直翻转 + 水平镜像
14:红外灯       0:close   1:auto   2:open
15:led灯        0:close   1:open
*/
typedef struct tag_STRU_CAMERA_CONTROL
{
    RT_APP_INT32 param;
    RT_APP_INT32 value;
}STRU_CAMERA_CONTROL,*PSTRU_CAMERA_CONTROL;


typedef struct tag_STRU_NETWORK_PARAMS
{
    RT_APP_CHAR ipaddr[64];  /*ip地址*/
    RT_APP_CHAR netmask[64]; /*子网掩码*/
    RT_APP_CHAR gateway[64]; /*网关*/
    RT_APP_CHAR dns1[64];    /*dns服务器1，默认8.8.8.8 暂不支持修改*/
    RT_APP_CHAR dns2[64];    /*dns服务器2*/
    RT_APP_INT32 dhcp;         /*是否开启dhcp 1: 开启 0: 不开启*/
    RT_APP_INT32 port;         /*http端口*/
    RT_APP_INT32 rtspport;     /*rtsp端口，不支持*/
}STRU_NETWORK_PARAMS,*PSTRU_NETWORK_PARAMS;

typedef struct tag_STRU_USER_INFO
{
    RT_APP_CHAR user1[64];  /*参观者用户名*/
    RT_APP_CHAR pwd1[64];   /*参观者密码*/
    RT_APP_CHAR user2[64];  /*操作者用户名*/
    RT_APP_CHAR pwd2[64];   /*操作者密码*/
    RT_APP_CHAR user3[64];  /*管理员用户名*/
    RT_APP_CHAR pwd3[64];   /*管理员密码*/
}STRU_USER_INFO,*PSTRU_USER_INFO;


typedef struct tag_STRU_WIFI_PARAMS
{
    RT_APP_INT32 enable;     /*是否启用wifi，1: 启用 0: 不启用*/
    RT_APP_CHAR ssid[128]; /*ssid*/
    RT_APP_INT32 channel;    /*通道号*/
    RT_APP_INT32 mode;       /*wifi模式，填0*/
    RT_APP_INT32 authtype;   /*wifi认证类型 0: 无 1: WEP 2: WPA-PSK(AES) 2: WPA-PSK(TKIP) 3: WPA2-PSK(AES) 4: WPA2-PSK(TKIP)*/
    RT_APP_INT32 encrypt;    /*安全模式 0: 开放系统 1: 共享密钥*/
    RT_APP_INT32 keyformat;  /*密钥格式 0: 16进制数 1: ASCII码*/
    RT_APP_INT32 defkey;     /*缺省使用密钥 0~3*/
    RT_APP_CHAR key1[128]; /*密钥1*/
    RT_APP_CHAR key2[128]; /*密钥2*/
    RT_APP_CHAR key3[128]; /*密钥3*/
    RT_APP_CHAR key4[128]; /*密钥4*/
    RT_APP_INT32 key1_bits;  /*密钥1的长度 0: 64bits 1: 128bits*/
    RT_APP_INT32 key2_bits;  /*密钥2的长度 0: 64bits 1: 128bits*/
    RT_APP_INT32 key3_bits;  /*密钥3的长度 0: 64bits 1: 128bits*/
    RT_APP_INT32 key4_bits;  /*密钥4的长度 0: 64bits 1: 128bits*/
    RT_APP_CHAR wpa_psk[128]; /*wpa密码*/
}STRU_WIFI_PARAMS,*PSTRU_WIFI_PARAMS;

typedef struct tag_STRU_AP_PARAMS
{
    RT_APP_CHAR ssid[128];
    RT_APP_CHAR pwd[128];
}STRU_AP_PARAMS,*PSTRU_AP_PARAMS;

/*
　　now: 当前时间（从1970-1-1 0:0:0到指定的时间所逝去的秒数），如果不设置当前时间值，则填0
　  tz: 时区，可包含以下取值：
　　39600：(GMT -11:00) 中途岛, 萨摩亚群岛
　　36000：(GMT -10:00) 夏威夷
　　32400：(GMT -09:00) 阿拉斯加
　　28800：(GMT -08:00) 太平洋时间(美国和加拿大)
　　25200：(GMT -07:00) 山地时间(美国和加拿大)
　　21600：(GMT -06:00) 中部时间(美国和加拿大), 墨西哥城
　　18000：(GMT -05:00) 东部时间(美国和加拿大), 利马, 波哥大
　  14400：(GMT -04:00) 大西洋时间(加拿大), 圣地亚哥, 拉巴斯
　　12600：(GMT -03:30) 纽芬兰
　　10800>(GMT -03:00) 巴西利亚, 布宜诺斯艾丽斯, 乔治敦
　　7200>(GMT -02:00) 中大西洋
　　3600>(GMT -01:00) 佛得角群岛
　　0>(GMT) 格林威治平时; 伦敦, 里斯本, 卡萨布兰卡
　　-3600：(GMT +01:00) 布鲁赛尔, 巴黎, 柏林, 罗马, 马德里, 斯多哥尔摩, 贝尔格莱德, 布拉格
　　-7200：GMT +02:00) 雅典, 耶路撒冷, 开罗, 赫尔辛基
　　-10800：(GMT +03:00) 内罗毕, 利雅得, 莫斯科
　　-12600：(GMT +03:30) 德黑兰
　　-14400：(GMT +04:00) 巴库, 第比利斯, 阿布扎比, 马斯科特
　　-16200：(GMT +04:30) 科布尔
　　-18000：(GMT +05:00) 伊斯兰堡, 卡拉奇, 塔森干
　　-19800：(GMT +05:30) 加尔各答, 孟买, 马德拉斯, 新德里
　　-21600：(GMT +06:00) 阿拉木图, 新西伯利亚, 阿斯塔南, 达尔
　　-25200：(GMT +07:00) 曼谷, 河内, 雅加达
　　-28800：(GMT +08:00) 北京, 新加坡, 台北
　　-32400：(GMT +09:00) 首尔, 雅库茨克, 东京
　　-34200：(GMT +09:30) 达尔文
　　-36000：(GMT +10:00) 关岛, , 墨尔本, 悉尼, 莫尔兹比港, 符拉迪沃斯托克
　　-39600：(GMT +11:00) 马加丹, 所罗门群岛, 新喀里多尼亚
　　-43200：(GMT +12:00) 奥克兰, 惠灵顿, 斐济
　　ntp_enable: 是否启用ntp服务 1：启用 0：不启用
　　ntp_svr: ntp服务器地址
*/
typedef struct tag_STRU_DATETIME_PARAMS
{
    RT_APP_INT32 now;
    RT_APP_INT32 tz;
    RT_APP_INT32 ntp_enable;
	RT_APP_INT32 timeMode;  /*时间制式 0:24 1:12*/
    RT_APP_CHAR ntp_svr[64];
}STRU_DATETIME_PARAMS,*PSTRU_DATETIME_PARAMS;


typedef struct tag_STRU_DDNS_PARAMS
{
    RT_APP_INT32 service;
    RT_APP_CHAR user[64];
    RT_APP_CHAR pwd[64];
    RT_APP_CHAR host[64];
    RT_APP_CHAR proxy_svr[64];
    RT_APP_INT32 ddns_mode;
    RT_APP_INT32 proxy_port;
    RT_APP_INT32 ddns_status;
}STRU_DDNS_PARAMS,*PSTRU_DDNS_PARAMS;


typedef struct tag_STRU_FTP_PARAMS
{
    RT_APP_CHAR svr_ftp[64];   /*ftp服务器的地址*/
    RT_APP_CHAR user[64];      /*ftp登录用户名*/
    RT_APP_CHAR pwd[64];       /*ftp登录密码*/
    RT_APP_CHAR dir[128];      /*ftp上传目录，填 / (即根目录)*/
    RT_APP_INT32 port;           /*ftp服务器端口（默认21）*/
    RT_APP_INT32 mode;           /*上传模式 0: PORT 1: PASV*/
    RT_APP_INT32 upload_interval;/*ftp定时上传的时间间隔（单位秒） 0表示不定时上传*/
}STRU_FTP_PARAMS,*PSTRU_FTP_PARAMS;

typedef struct tag_STRU_MAIL_PARAMS
{
    RT_APP_CHAR svr[64];       /*SMTP服务器地址*/ 
    RT_APP_CHAR user[64];      /*邮件用户名*/
    RT_APP_CHAR pwd[64];       /*邮件密码*/
    RT_APP_CHAR sender[64];    /*发送者邮箱地址*/
    RT_APP_CHAR receiver1[64]; /*接收者1的邮箱地址*/
    RT_APP_CHAR receiver2[64]; /*接收者2的邮箱地址*/
    RT_APP_CHAR receiver3[64]; /*接收者3的邮箱地址*/
    RT_APP_CHAR receiver4[64]; /*接收者4的邮箱地址*/
    RT_APP_INT32 port;           /*SMTP服务器的端口*/
    RT_APP_INT32 ssl;            /*邮件加密方式 0: 不加密 1: SSL 2: TLS*/
}STRU_MAIL_PARAMS,*PSTRU_MAIL_PARAMS;

typedef struct tag_STRU_ALARM_PARAMS
{
    RT_APP_INT32 motion_armed;       /*移动侦测开关 0: 关闭 1: 打开*/
    RT_APP_INT32 motion_sensitivity; /*移动侦测灵敏度 取值1~10 （取值越小越灵敏）*/
    RT_APP_INT32 input_armed;        /*输入报警开关 0: 关闭 1: 打开*/
    RT_APP_INT32 ioin_level;         /*输入报警触发电平 0: 低电平 1: 高电平*/
    RT_APP_INT32 iolinkage;          /*报警IO输出联动开关 0: 关闭 1: 打开*/
    RT_APP_INT32 ioout_level;        /*报警输出电平 0: 低电平 1: 高电平*/
    RT_APP_INT32 alarmpresetsit;     /*报警调用预置位 取值 0~16 0表示不调用预置位*/
    RT_APP_INT32 mail;               /*报警发送邮件 0: 关闭 1: 打开*/
    RT_APP_INT32 snapshot;           /*报警抓图 0: 关闭 1: 打开  （目前不支持）*/
    RT_APP_INT32 record;             /*报警录像 0: 关闭 1: 打开*/
    RT_APP_INT32 upload_interval;    /*报警ftp上传时间间隔(单位秒) 0表示不上传*/
    RT_APP_INT32 schedule_enable;    /*报警布防开关 0: 关闭 1: 打开 （注意：关闭布防，将不会触发任何报警）*/
    RT_APP_INT32 schedule_sun_0;     /*每天按24小时，每小时按15分钟划分为96个布防时段，使用3个整形表示，每个整形32位分别表示1个时段，如果该位为0：该时段不布防；1：该时段布防*/
    RT_APP_INT32 schedule_sun_1;
    RT_APP_INT32 schedule_sun_2;
    RT_APP_INT32 schedule_mon_0;
    RT_APP_INT32 schedule_mon_1;
    RT_APP_INT32 schedule_mon_2;
    RT_APP_INT32 schedule_tue_0;
    RT_APP_INT32 schedule_tue_1;
    RT_APP_INT32 schedule_tue_2;
    RT_APP_INT32 schedule_wed_0;
    RT_APP_INT32 schedule_wed_1;
    RT_APP_INT32 schedule_wed_2;
    RT_APP_INT32 schedule_thu_0;
    RT_APP_INT32 schedule_thu_1;
    RT_APP_INT32 schedule_thu_2;
    RT_APP_INT32 schedule_fri_0;
    RT_APP_INT32 schedule_fri_1;
    RT_APP_INT32 schedule_fri_2;
    RT_APP_INT32 schedule_sat_0;
    RT_APP_INT32 schedule_sat_1;
    RT_APP_INT32 schedule_sat_2;
}STRU_ALARM_PARAMS,*PSTRU_ALARM_PARAMS;

typedef struct tag_STRU_PTZ_PARAMS
{
    RT_APP_INT32 led_mode;             /*led指示灯状态 0：关闭 1：打开 （某些版本可用）*/
    RT_APP_INT32 ptz_center_onstart;   /*启动时云台自动居中 0：不居中 1：居中*/
    RT_APP_INT32 ptz_run_times;        /*云台在上下巡航，左右巡航时的巡航圈数 0：表示无限制（但是系统会限制最长时间为1小时）*/
    RT_APP_INT32 ptz_patrol_rate;      /*云台上下巡航，左右巡航的速度，取值1~10*/
    RT_APP_INT32 ptz_patrol_up_rate;   /*云台向上转动速度，取值1~10*/
    RT_APP_INT32 ptz_patrol_down_rate; /*云台向下转动速度，取值1~10*/
    RT_APP_INT32 ptz_patrol_left_rate; /*云台向左转动速度，取值1~10*/
    RT_APP_INT32 ptz_patrol_right_rate;/*云台向右转动速度，取值1~10*/
    RT_APP_INT32 disable_preset;       /*禁用预置位 0：不禁用 1： 禁用*/
    RT_APP_INT32 ptz_preset;           /*启动时，对准预置位 取值0~16，0表示不对准预置位 1~16分别对应预置位1至预置位16*/
}STRU_PTZ_PARAMS,*PSTRU_PTZ_PARAMS;

typedef struct tag_STRU_CAMERA_STATUS
{
    RT_APP_CHAR sysver[32];     /*系统固件的版本号*/
    RT_APP_CHAR devname[96];    /*设备名称，使用utf-8编码*/
    RT_APP_CHAR devid[32];      /*设备ID*/
    RT_APP_INT32 alarmstatus;     /*报警状态*/
    RT_APP_INT32 sdcardstatus;    /*SD卡插入状态 0: 未插入 1: 插入*/
    RT_APP_INT32 sdcardtotalsize; /*SD总容量*/
    RT_APP_INT32 sdcardremainsize;/*SD剩余容量*/
    RT_APP_CHAR mac[32];        /*设备mac*/
    RT_APP_CHAR wifimac[32];    /*wifi mac*/
    RT_APP_INT32 dns_status;      /*dns 状态 */
    RT_APP_INT32 upnp_status;     /*upnp 状态*/
	RT_APP_INT32 onlineNum;     /*在线人数*/
}STRU_CAMERA_STATUS,*PSTRU_CAMERA_STATUS;

typedef struct tag_STRU_WIFI_SEARCH_RESULT
{
    RT_APP_CHAR ssid[64]; /*网络的ssid */
    RT_APP_CHAR mac[64];  /*wifi的mac地址 */
    RT_APP_INT32 security;  /*加密方式 0: 不加密 1: WEP 2: WPA-PSK(AES) 3: WPA-PSK(TKIP) 4: WPA2-PSK(AES) 5: WPA2-PSK(TKIP) */
    RT_APP_CHAR dbm0[32]; /*WIFI信号强度 以100为最强 */
    RT_APP_CHAR  dbm1[32];/*WIFI基准值 ，暂时忽略该参数 */
    RT_APP_INT32 mode;      /*wifi模式 */
    RT_APP_INT32 channel;   /*wifi通道号 */

}STRU_WIFI_SEARCH_RESULT,*PSTRU_WIFI_SEARCH_RESULT;

typedef struct tag_STRU_WIFI_SEARCH_RESULT_LIST
{
    RT_APP_INT32 nResultCount;                 /*nResultCount: WIFI扫描结果个数 */
    STRU_WIFI_SEARCH_RESULT wifi[50]; /*存储扫描结果*/
}STRU_WIFI_SEARCH_RESULT_LIST,*PSTRU_WIFI_SEARCH_RESULT_LIST;

typedef struct tag_STRU_SEARCH_SDCARD_RECORD_FILE
{
    RT_APP_CHAR starttime[16]; /*搜索开始时间(20170818000000)*/
    RT_APP_CHAR endtime[16];   /*搜索结束时间(20170818235959)*/
	RT_APP_INT32 page;         /*当前翻到第几页(备注:如果为0表示将当前天所有录像返回)*/
	RT_APP_INT32 pageSize;     /*每页显示的条数*/
	RT_APP_INT32 fileType;     /*文件类型 :1-常规录像,2-报警录像,3-所有录像*/
	RT_APP_INT32 showType;     /*文件显示类型 0:文件夹方式显示 1:时间轴方式显示(设备只传文件开始和结束时间)*/
}STRU_SEARCH_SDCARD_RECORD_FILE, *PSTRU_SEARCH_SDCARD_RECORD_FILE;

typedef struct tag_STRU_SDCARD_RECORD_FILE
{
    RT_APP_CHAR szFileName[128];   /*录像文件名*/
    RT_APP_INT32 nFileSize;        /*录像文件大小*/
	RT_APP_INT32 fileDayStartTime; /*录像的开始时间 HH:MM:SS的时间秒的总和,ex: 18:23:56 ---> 18*3600+23*60+56*/
	RT_APP_INT32 fileDayEndTime;   /*录像的结束时间 HH:MM:SS的时间秒的总和,ex: 18:23:56 ---> 18*3600+23*60+56*/
	RT_APP_INT32 fileTotalTime;    /*录像文件单个时长*/
	RT_APP_INT32 currRecordCount;  /*当前页的录像文件个数*/
	RT_APP_INT32 fileTotalCount;   /*当前设备搜索时间中SD总文件个数*/
}STRU_SDCARD_RECORD_FILE,*PSTRU_SDCARD_RECORD_FILE;

typedef struct  tag_STRU_SD_RECORD_PARAM
{
    RT_APP_INT32 record_cover_enable;  /*是否录像覆盖*/
    RT_APP_INT32 record_timer;         /*录像的时间*/
    RT_APP_INT32 record_size;          /*录像文件大小*/
    RT_APP_INT32 record_time_enable;   /*定时录像是否开启*/
	RT_APP_INT32 record_sd_status;    /*SD状态 0:未插入  1:插入  2:正在录像*/
    RT_APP_INT32 sdtotal;              /*SD总容量*/
    RT_APP_INT32 sdfree;               /*SD剩余大小*/
    RT_APP_INT32 record_schedule_sun_0;
    RT_APP_INT32 record_schedule_sun_1; 
    RT_APP_INT32 record_schedule_sun_2; 
    RT_APP_INT32 record_schedule_mon_0; 
    RT_APP_INT32 record_schedule_mon_1; 
    RT_APP_INT32 record_schedule_mon_2; 
    RT_APP_INT32 record_schedule_tue_0; 
    RT_APP_INT32 record_schedule_tue_1; 
    RT_APP_INT32 record_schedule_tue_2; 
    RT_APP_INT32 record_schedule_wed_0;
    RT_APP_INT32 record_schedule_wed_1; 
    RT_APP_INT32 record_schedule_wed_2; 
    RT_APP_INT32 record_schedule_thu_0; 
    RT_APP_INT32 record_schedule_thu_1; 
    RT_APP_INT32 record_schedule_thu_2; 
    RT_APP_INT32 record_schedule_fri_0; 
    RT_APP_INT32 record_schedule_fri_1; 
    RT_APP_INT32 record_schedule_fri_2; 
    RT_APP_INT32 record_schedule_sat_0; 
    RT_APP_INT32 record_schedule_sat_1; 
    RT_APP_INT32 record_schedule_sat_2;  
}STRU_SD_RECORD_PARAM, *PSTRU_SD_RECORD_PARAM;

typedef struct tag_STRU_BILL_PARAMS
{
    RT_APP_CHAR svr[64];
    RT_APP_CHAR user[64];
    RT_APP_CHAR pwd[64];
    RT_APP_INT32 port;
    RT_APP_INT32 reboottime;
}STRU_BILL_PARAMS,*PSTRU_BILL_PARAMS;

/* app内网搜索结构体*/
typedef struct tag_STRU_SEARCH_PARAMS
{
	RT_APP_CHAR   szIpAddr[16];  /*设备IP*/
	RT_APP_CHAR   szMacAddr[32]; /*设备MAC*/
	RT_APP_CHAR   szDevUID[32];  /*设备UID*/
	RT_APP_CHAR   szDevName[32]; /*设备名称*/
	RT_APP_UINT16 nPort;         /*设备端口*/
}STRU_SEARCH_PARAMS,*PSTRU_SEARCH_PARAMS;

typedef enum tag_STRU_ALARM_TYPE
{
    MOTION_ALARM     = 0x01,/* 移动侦测报警 */
    GPIO_ALARM       = 0x02,/* GPIO报警 */
    SCHDEULE_ALARM   = 0x03,/* 布防报警*/
    CONTROL_ALARM    = 0x04,/* 遥控器报警*/ 
    HUMI_ALARM       = 0x05,/* 温湿度报警*/
    PM25_ALARM       = 0x06,/* PM25报警*/
    SMOKE_ALARM      = 0x07,/* 烟感报警*/
    SOUND_ALARM      = 0x08,/* 声音报警*/
	MOTION_ALARM_EX  = 0x11,/* 移动侦测报警扩展*/
    GPIO_ALARM_EX    = 0x12,/* GPIO报警扩展*/
}STRU_ALARM_TYPE;

typedef struct tag_STRU_ALARM_EX_INFO
{
	RT_APP_CHAR alarmInfo[128]; //目前将其用来存储文件名,也可用作其他存储
}STRU_ALARM_EX_INFO,*PSTRU_ALARM_EX_INFO;

/*媒体数据状态*/
typedef enum tag_STRU_MEDIA_DATA_STATE
{
	ENUM_MEDIA_DATA_EXIST = 0,  // 媒体数据存在TF中
	ENUM_MEDIA_DATA_NONENTITY,  // 媒体数据不存在TF中
	ENUM_MEDIA_DATA_BUTT
}STRU_MEDIA_DATA_STATE;

/*媒体数据类型*/
typedef enum tag_STRU_MEDIA_DATA_TYPE
{
	ENUM_MEDIA_TYPE_VIDEO = 0,
	ENUM_MEDIA_TYPE_PICTURE = 1,
	ENUM_MEDIA_TYPE_BUTT
}STRU_MEDIA_DATA_TYPE;

/* 媒体文件*/
typedef struct tag_STRU_MEDIA_DATA
{
	RT_APP_INT32 nHandle;
	RT_APP_ULONG64 fileSize;
	RT_APP_CHAR szFileName[128];
	STRU_MEDIA_DATA_STATE dataState;
	STRU_MEDIA_DATA_TYPE mediaType;
}STRU_MEDIA_DATA,*PSTRU_MEDIA_DATA;

/*抓拍流的分辨率*/
typedef enum _STRU_SNAP_STREAM_VAL
{
	ENUM_SNAP_STREAM_720 = 1,
	ENUM_SNAP_STREAM_VGA,
	ENUM_SNAP_STREAM_BUTT
}STRU_SNAP_STREAM_VAL;

/*音频数据类型*/
typedef enum _STRU_AUDIO_TYPE
{
	ENUM_AUDIO_TYPE_PCM = 0,
	ENUM_AUDIO_TYPE_AAC = 1,
	ENUM_AUDIO_TYPE_BUTT
}STRU_AUDIO_TYPE;

/* 抓拍上传FTP */
typedef struct _STRU_SNAP_UPLOAD_FTP
{
    STRU_SNAP_STREAM_VAL snapStreamVal;      //需要抓拍的码流值
    RT_APP_CHAR ftpPath[128];//ftp上传的路径及文件名
}STRU_SNAP_UPLOAD_FTP,*PSTRU_SNAP_UPLOAD_FTP;

/* 录像模式选择 */
typedef struct tag_STRU_REC_MODE
{
    RT_APP_INT32 timeRecEnable;    /* 0:关定时录像 1:开定时录像 */
    RT_APP_INT32 motionRecEnable;  /* 0:关移动侦测录像 1:开移动侦测录像 */
}STRU_REC_MODE,*PSTRU_REC_MODE;

/* 时间设置 */
typedef struct tag_STRU_TIME_ATTR
{
    RT_APP_INT32 year;  /* 年 */
    RT_APP_INT32 month; /* 月 */
	RT_APP_INT32 day;   /* 日 */
    RT_APP_INT32 hour;  /* 时 */
	RT_APP_INT32 min;   /* 分 */
    RT_APP_INT32 sec;   /* 秒 */
	RT_APP_CHAR  timezone[64]; /*时区的名字,按照给定名字下发*/
	                   /*		
						 时区名字              时区值
						"Etc/GMT-12"            -12
						"Pacific/Apia"          -11
						"Pacific/Honolulu"      -10
						"Pacific/Marquesas"     -9:30
						"America/Anchorage"     -9
						"America/Los_Angeles"   -8
						"America/Denver"        -7
						"America/Mexico_City"   -6
						"America/Bogota"        -5
						"America/Caracas"       -4:30
						"America/Montreal"      -4
						"America/St_Johns"      -3:30
						"America/Sao_Paulo"     -3
						"Atlantic/South_Georgia" -2
						"Atlantic/Cape_Verde"   -1
						"Europe/Dublin"         0
						"Europe/Brussels"       1
						"Europe/Athens"         2
						"Europe/Moscow"         3
						"Asia/Tehran"           3:30
						"Asia/Baku"             4
						"Asia/Kabul"            4:30
						"Asia/Karachi"          5
						"Asia/Calcutta"         5:30
						"Asia/Katmandu"         5:45
						"Asia/Rangoon"          6:30
						"Asia/Almaty"           6
						"Asia/Bangkok"          7
						"Asia/Hong_Kong"        8
						"Asia/Tokyo"            9
						"Australia/Adelaide"    9:30
						"Pacific/Guam"          10
						"Asia/Magadan"          11
						"Pacific/Auckland"      12
						"Pacific/Tongatapu"     13
						"Pacific/Kiritimati"    14
	                   */
}STRU_TIME_ATTR,*PSTRU_TIME_ATTR;

/*月中有多少天有录像*/
typedef struct tag_STRU_REC_CALENDAR
{
	RT_APP_UINT32 calendar; /*导入导出参数 导入:要查那个月 导出:每bit表示一天,1:有 0:无*/
	RT_APP_UINT32 count; /*返回这个月有多少天有录像文件*/
}STRU_REC_CALENDAR,*PSTRU_REC_CALENDAR;

typedef struct tag_STRU_ALARM_PARAMS_EX
{
	RT_APP_INT32 mdSensitive; /*移动侦测灵敏度 0:关闭 1:高 2:中 3:低 4:最低*/
	RT_APP_INT32 mdInterval;  /*移动侦测报警间隔时间(单位s)*/
	RT_APP_INT32 mdEmailSnap; /*移动侦测邮件抓拍*/
	RT_APP_INT32 mdFtpSnap;   /*移动侦测ftp抓拍*/
	RT_APP_INT32 mdFtpRec;    /*移动侦测ftp录像*/
	RT_APP_INT32 ioEnable;    /*io告警开关 0-常开，1-常关，2-关闭*/
	RT_APP_INT32 ioInterval;  /*io告警报警间隔时间(单位s)*/
	RT_APP_INT32 ioEmailSnap; /*io告警邮件抓拍*/
	RT_APP_INT32 ioFtpSnap;   /*io告警ftp抓拍*/
	RT_APP_INT32 ioFtpRec;    /*io告警ftp录像*/
}STRU_ALARM_PARAMS_EX,*PSTRU_ALARM_PARAMS_EX;

typedef struct tag_STRU_SD_RECORD_EX
{
	RT_APP_INT32 sdRecMode;     /*sd卡录像模式 0:不录像 1:全时录像 2:报警录像*/
	RT_APP_INT32 fullRecTime;   /*全时录像打包时间(单位秒)*/
	RT_APP_INT32 alarmRecTime;  /*告警录像打包时间(单位秒)*/
	RT_APP_INT32 timelapseRecEnable;/*缩时录像开关 0:关 1:开*/
	RT_APP_INT32 timelapseRecTime;  /*缩时录像打包时间(单位小时)*/
	RT_APP_INT32 timePlanEnable;    /*定时录像计划开关 1:打开 0:关闭 (备注关闭的话默认为走7*24小时录像)*/
	RT_APP_INT32 timePlanStartTime; /*定时录像计划开始时间 (时间精度为15分钟 例如03:30  3*(60/15)+30/15=14) 备注:如果*/
	RT_APP_INT32 timePlanEndTime;   /*定时录像计划结束时间 (时间精度为15分钟 例如20:45  20*(60/15)+45/15=83)*/
	RT_APP_INT32 loopRecEnable;     /*循环录像 1:循环录像 0:不循环录像*/
	RT_APP_INT32 audioRecEnable;    /*录像是否录制声音 1:录制声音 0:不录像声音*/
	RT_APP_INT32 resolutionRec;     /*录像的分辨率 0:主码流 1:副码流*/
	
}STRU_SD_RECORD_EX,*PSTRU_SD_RECORD_EX;

typedef struct tag_STRU_OSD_ATTRS
{
	RT_APP_INT32 osdEnable; /*osd 0:关 1:开*/ 
	RT_APP_CHAR  osdName[64]; /*osd的名字*/
}STRU_OSD_ATTRS,*PSTRU_OSD_ATTRS;

/*目前主要是用于人图的天气设备,设计成buf是因为底层数据解析不支持浮点型数据*/
typedef struct tag_STRU_GEOG_ATTRS
{
	RT_APP_CHAR longitude[16]; /*经度*/
	RT_APP_CHAR latitude[16];  /*纬度*/
	RT_APP_INT32 isRefresh;    /*是否需要立即刷新天气信息 0:不刷新(默认) 1:刷新*/
}STRU_GEOG_ATTRS,*PSTRU_GEOG_ATTRS;

/*云台控制结构体*/
typedef struct tag_STRU_PTZ_CONTROL
{
	RT_APP_UINT32 command;  /*命令值*/
	RT_APP_UINT32 extParam; /*扩展参数*/
}STRU_PTZ_CONTROL,*PSTRU_PTZ_CONTROL;

#endif //__RT_P2P_API_DEFINE_H__
