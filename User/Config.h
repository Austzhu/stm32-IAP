#ifndef __Config_h__
#define __Config_h__

//#define Config_IWDG
//#define Application
//#define Coordi_App
#define Single_App

//#define Uart1_Print
//#define Uart2_Print


#ifdef Coordi_App
#ifdef Single_App
	#error Can't define Coordi_App and Single_App at the same time.
#endif
#endif 





#endif
