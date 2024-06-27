/* USER CODE BEGIN Header */
//////////////////////////////////////////////////////////////////////////////
//
// Kamera OV5640
// Moduł pętli głównej
//
// (c) PitLab 2024
// http://www.pitlab.pl
//////////////////////////////////////////////////////////////////////////////
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sys_def.h"
#include "konfiguracja.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define BUTTON_RELEASED                    0U
#define BUTTON_PRESSED                     1U
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define USER_Btn_Pin GPIO_PIN_13
#define USER_Btn_GPIO_Port GPIOC
#define USER_Btn_EXTI_IRQn EXTI15_10_IRQn
#define LCD_CS_Pin GPIO_PIN_3
#define LCD_CS_GPIO_Port GPIOF
#define LCD_RST_Pin GPIO_PIN_5
#define LCD_RST_GPIO_Port GPIOF
#define XCLK_Pin GPIO_PIN_9
#define XCLK_GPIO_Port GPIOF
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOH
#define LCD_WR_Pin GPIO_PIN_0
#define LCD_WR_GPIO_Port GPIOC
#define RMII_MDC_Pin GPIO_PIN_1
#define RMII_MDC_GPIO_Port GPIOC
#define LCD_RS_Pin GPIO_PIN_3
#define LCD_RS_GPIO_Port GPIOC
#define RMII_REF_CLK_Pin GPIO_PIN_1
#define RMII_REF_CLK_GPIO_Port GPIOA
#define RMII_MDIO_Pin GPIO_PIN_2
#define RMII_MDIO_GPIO_Port GPIOA
#define LCD_RD_Pin GPIO_PIN_3
#define LCD_RD_GPIO_Port GPIOA
#define RMII_CRS_DV_Pin GPIO_PIN_7
#define RMII_CRS_DV_GPIO_Port GPIOA
#define RMII_RXD0_Pin GPIO_PIN_4
#define RMII_RXD0_GPIO_Port GPIOC
#define RMII_RXD1_Pin GPIO_PIN_5
#define RMII_RXD1_GPIO_Port GPIOC
#define LD1_Pin GPIO_PIN_0
#define LD1_GPIO_Port GPIOB
#define LCD_D0_Pin GPIO_PIN_12
#define LCD_D0_GPIO_Port GPIOF
#define LCD_D7_Pin GPIO_PIN_13
#define LCD_D7_GPIO_Port GPIOF
#define LCD_D4_Pin GPIO_PIN_14
#define LCD_D4_GPIO_Port GPIOF
#define LCD_D2_Pin GPIO_PIN_15
#define LCD_D2_GPIO_Port GPIOF
#define LCD_D6_Pin GPIO_PIN_9
#define LCD_D6_GPIO_Port GPIOE
#define LCD_D5_Pin GPIO_PIN_11
#define LCD_D5_GPIO_Port GPIOE
#define LCD_D3_Pin GPIO_PIN_13
#define LCD_D3_GPIO_Port GPIOE
#define RMII_TXD1_Pin GPIO_PIN_13
#define RMII_TXD1_GPIO_Port GPIOB
#define LD3_Pin GPIO_PIN_14
#define LD3_GPIO_Port GPIOB
#define STLK_RX_Pin GPIO_PIN_8
#define STLK_RX_GPIO_Port GPIOD
#define STLK_TX_Pin GPIO_PIN_9
#define STLK_TX_GPIO_Port GPIOD
#define LCD_D1_Pin GPIO_PIN_15
#define LCD_D1_GPIO_Port GPIOD
#define USB_PowerSwitchOn_Pin GPIO_PIN_6
#define USB_PowerSwitchOn_GPIO_Port GPIOG
#define USB_OverCurrent_Pin GPIO_PIN_7
#define USB_OverCurrent_GPIO_Port GPIOG
#define USB_SOF_Pin GPIO_PIN_8
#define USB_SOF_GPIO_Port GPIOA
#define USB_VBUS_Pin GPIO_PIN_9
#define USB_VBUS_GPIO_Port GPIOA
#define USB_ID_Pin GPIO_PIN_10
#define USB_ID_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define CAM_PWDN_Pin GPIO_PIN_9
#define CAM_PWDN_GPIO_Port GPIOG
#define RMII_TX_EN_Pin GPIO_PIN_11
#define RMII_TX_EN_GPIO_Port GPIOG
#define CAM_RST_Pin GPIO_PIN_12
#define CAM_RST_GPIO_Port GPIOG
#define RMII_TXD0_Pin GPIO_PIN_13
#define RMII_TXD0_GPIO_Port GPIOG
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
void KameraPWDN_Pin(uint32_t SetReset);
void KameraRST_Pin(uint32_t SetReset);
void LED_Toggle(uint8_t LED);
extern uint16_t sBuforLCD[];
extern uint32_t nBuforKamery[];
extern unsigned char chRysujRaz;	//flaga informująca o konieczności jednorazowego narysowania statycznych elementów ekranu
extern void InitDisplay(void);
extern void LCD_clear(void);
extern void drawBitmap(int x, int y, int sx, int sy, const unsigned short* data);
//extern void drawBitmapYUV(int x, int y, int sx, int sy, const unsigned short* data);
extern void fillRect(int x1, int y1, int x2, int y2);
extern void setColor(unsigned short color);
extern void RysujDane(char *str, uint16_t dane, uint8_t pozY);
extern unsigned char Menu(unsigned char chPozycja);
extern void FraktalDemo(void);
extern void RysujMenuTimer(unsigned short sCzas);
extern void WyswietlPomoc(void);
extern HAL_StatusTypeDef InitKamera(void);
extern HAL_StatusTypeDef InitKamera1(void);
extern HAL_StatusTypeDef InitKamera2(void);
extern HAL_StatusTypeDef InitKamera3(void);
extern HAL_StatusTypeDef InitKamera4(void);
extern HAL_StatusTypeDef InitKamera5(void);
extern uint8_t	SprawdzKamere(void);
extern HAL_StatusTypeDef UstawKamere(typKonfKam *konf);
extern HAL_StatusTypeDef RozpocznijPraceDCMI(uint8_t chAparat);
extern HAL_StatusTypeDef ZrobZdjecie(int16_t sSzerokosc, uint16_t sWysokosc);
extern  uint8_t chNowyObrazKamery;
extern uint8_t CzytajKamInit(void);
extern void RAW2RGB(uint32_t *nBufKamery, uint16_t *sBufLCD);
extern uint16_t sMenuTimer;
extern void OV5640_OutSize_Set(uint16_t offX, uint16_t offY, uint16_t width, uint16_t height);
extern volatile uint8_t chObrazGotowy;
extern void WyswietlKodBledu(uint8_t blad, uint8_t pozX, uint8_t pozY);
extern uint8_t CzekajNaBit(volatile uint8_t *chBit, uint16_t sTimeout);

//obróbka obrazu
extern void KonwersjaRGB565doCB7(uint16_t *obrazRGB565, uint8_t *obrazCB, uint32_t rozmiar);
extern void KonwersjaCB7doRGB565(uint8_t *obrazCB, uint16_t *obrazCB565, uint32_t rozmiar);

//Protokół komunikacyjny
extern uint8_t InitProtokol(void);
extern uint8_t OdbierzDaneKom(uint8_t chWe, uint8_t chInterfejs);
extern uint8_t Wyslij_OK(uint8_t chInterfejs);
extern int16_t sSzerZdjecia, sWysZdjecia;
extern uint8_t chStatusZdjecia;		//status gotowości wykonania zdjęcia


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
