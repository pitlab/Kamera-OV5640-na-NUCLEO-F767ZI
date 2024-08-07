/*
 * sys_def.h
 *
 *  Created on: 30 gru 2019
 *      Author: PitLab
 */

#ifndef SYS_DEF_H_
#define SYS_DEF_H_
#include <math.h>


#define WER_GLOWNA	0	//0=model, 1=prototyp
#define WER_PODRZ	1
#define WER_REPO	1	//numer wersji w repozytorium



#define DEG2RAD     (float)(M_PI/180)   //przeliczanie stopni na radiany
#define DEG2RAD2    (float)(M_PI/360)   //przeliczanie stopni na radiany - użyj całego kąta
#define RAD2DEG     (float)(180/M_PI)   //przeliczanie radianów na stopnie
#define PIERW2		(float)(1.414213562)
#define CLEAR		16	//przesunięcie do czyszczenia bitu poleceniem BSSR

#define LED_GREEN	1
#define LED_BLUE	2
#define LED_RED		3

#define COL_R	0
#define COL_G	1
#define COL_B	2

//rozmiary wyświetlacza
#define LCD_SZER	320
#define LCD_WYS		240

//rozmiary buforów wyrażone w słowach 16-bit
#define ROZM_BUF_CB		LCD_SZER * LCD_WYS
#define ROZM_BUF16_LCD	LCD_SZER * LCD_WYS
#define ROZM_BUF32_KAM	LCD_SZER * LCD_WYS / 2

//tryby pracy
#define TP_KAMERA_RGB	0
#define TP_HIST_BIT		1
#define TP_KAM_SET1		2
#define TP_KAM_SET2		3
#define TP_KAM_SET3		4
#define TP_KAM_SET4		5
#define TP_KAM_SET5		6
#define TP_KAM_CB		5
#define TP_DET_KRAW		6
#define TP_FRAKTAL		7
#define TP_POMOC		8
#define TP_MENU			10






#endif /* SYS_DEF_H_ */

