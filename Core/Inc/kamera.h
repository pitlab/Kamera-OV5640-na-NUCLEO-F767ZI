/*
 * kamera.h
 *
 *  Created on: Apr 7, 2024
 *      Author: PitLab
 */

#ifndef INC_KAMERA_H_
#define INC_KAMERA_H_

#include "stm32f7xx_hal.h"
#include "ov5640_regs.h"
#include "sys_def.h"
#include "errcode.h"
#include "konfiguracja.h"

#define OV5640_ID           0x5640
#define OV5640_I2C_ADR    	0x78
#define OV5640_CHIPIDH      0x300A
#define OV5640_CHIPIDL      0x300B

#define KAMERA_TIMEOUT	1	//czas w milisekundach na wysłanie jednego polecenia do kamery. Nominalnie jest to ok 400us na adres i 3 bajty danych

//sekcja .BuforObrazu ma 368k
uint32_t nBuforKamery[ROZM_BUF32_KAM] __attribute__((section(".BuforObrazu")));		//153,6k
uint8_t chBuforCB[ROZM_BUF_CB]	__attribute__((section(".BuforObrazu")));			//76,8k
uint8_t chBuforCKraw[ROZM_BUF_CB] __attribute__((section(".BuforObrazu")));			//76,8k
///////////////////////////////////////////////////////////////////////// Lącznie:	307,2k

struct st_KonfKam KonfKam;


//volatile uint16_t sLicznikLiniiKamery;
extern I2C_HandleTypeDef hi2c2;
extern DMA_HandleTypeDef hdma_dcmi;
extern DCMI_HandleTypeDef hdcmi;
uint8_t chNowyObrazKamery;
extern void KameraPWDN_Pin(uint32_t SetReset);
extern void KameraRST_Pin(uint32_t SetReset);
extern void LED_Toggle(uint8_t LED);

HAL_StatusTypeDef InitKamera(void);
HAL_StatusTypeDef InitKamera1(void);
HAL_StatusTypeDef InitKamera2(void);
HAL_StatusTypeDef InitKamera3(void);
HAL_StatusTypeDef InitKamera4(void);
HAL_StatusTypeDef InitKamera5(void);
uint8_t CzytajKamInit(void);
HAL_StatusTypeDef Wyslij_I2C_Kamera(uint16_t rejestr, uint8_t dane);
HAL_StatusTypeDef Czytaj_I2C_Kamera(uint16_t rejestr, uint8_t *dane);
HAL_StatusTypeDef Wyslij_Blok_Kamera(const struct sensor_reg reglist[], uint16_t rozmiar);
void OV5640_OutSize_Set(uint16_t offX, uint16_t offY, uint16_t width, uint16_t height);
void OV5640_Flash_Lamp(uint8_t lampa);
void RAW2RGB(uint32_t *nBufKamery, uint16_t *sBufLCD);
uint8_t	SprawdzKamere(void);
HAL_StatusTypeDef UstawKamere(typKonfKam *konf);
HAL_StatusTypeDef RozpocznijPraceDCMI(uint8_t chAparat);
HAL_StatusTypeDef ZrobZdjecie(int16_t sSzerokosc, uint16_t sWysokosc);
uint8_t CzekajNaBit(volatile uint8_t *chBit, uint16_t sTimeout);


extern void Error_Handler(void);

#endif /* INC_KAMERA_H_ */
