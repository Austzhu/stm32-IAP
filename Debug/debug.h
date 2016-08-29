#ifndef __DEBUG_H__
#define __DEBUG_H__

#define MY_DEBUG 	2
#define LEVEL 		5 	//设置调试打印等级

#if 	MY_DEBUG == 1
	#define debug(level,fmt,args...)  {if(level<=LEVEL)	printf(fmt,##args);}
#elif 	MY_DEBUG == 2
	#define debug(level,fmt,args...)  {if(level)Enable_485(Print_Port);printf(fmt,##args);}
#else
	#define debug(...)
#endif		//#ifdef DEBUG


/* 定义调试宏 */
#define DEBUG_All 		1
#define DEBUG_GetBaud 	1
#define DEBUG_Recvpkt 	1
#define DEBUG_Analysis 	1
#define DEBUG_Printpkg 	1
#define DEBUG_Update 	1
#define DEBUG_Bootload 	1
#define DEBUG_Program 	1
#define DEBUG_Init 		1
#define DEBUG_ImageInfo	1

#endif 		//#define __DEBUG_H__


			









