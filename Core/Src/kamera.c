//////////////////////////////////////////////////////////////////////////////
//
// Moduł obsługi kamery OV5640
//
// (c) PitLab 2024
// http://www.pitlab.pl
//////////////////////////////////////////////////////////////////////////////
// KAMERA		Port uC				NUCLEO		PRZEWÓD
//  1 VCC		---					CN11-12		czerwony
//  2 GND		---					CN11-8		czarny
//  3 SCL		PF14, I2C4_SCL		CN12-50		żółty
//  4 SDA		PF15, I2C4_SDA		CN12-60		zielony
//  5 VSYNC		PB7, DCMI_VSYNC		CN11-21		brązowy
//  6 HREF		PA4, DCMI_HREF		CN11-32		niebieski
//  7 PCLK		PA6, DCMI_PIXCLK	CN12-13		niebieski
//  8 XCLK		PF9, TIM14_CH1		CN11-56		pomarańczowy
//  9 D9	 	PB9, DCMI_D7		CN11-62		fioletowy
// 10 D8		PE5, DCMI_D6		CN11-50		brązowy
// 11 D7		PD3, DCMI_D5		CN11-40		biały
// 12 D6		PC11, DCMI_D4		CN11-48		szary
// 13 D5		PE1, DCMI_D3		CN12-1		fioletowy
// 14 D4 		PC8, DCMI_D2		CN12-2		niebieski
// 15 D3		PC7, DCMI_D1		CN12-19		zielony
// 16 D2		PC6, DCMI_DO		CN12-4		żółty
// 17 RSV		---
// 18 PWDN		PF11				CN12-62

////////////////////////////////////////////////////////////////////////////////

#include "kamera.h"
#include "stm32f7xx_hal.h"

#include "..\..\..\..\include\PoleceniaKomunikacyjne.h"

////////////////////////////////////////////////////////////////////////////////
// Inicjalizacj pracy kamery
// Parametry: brak
// Zwraca: kod błędu HAL
////////////////////////////////////////////////////////////////////////////////
HAL_StatusTypeDef InitKamera(void)
{
	HAL_StatusTypeDef err = 0;
	GPIO_InitTypeDef gpio_init_structure;

	__HAL_RCC_DCMI_CLK_ENABLE();
	__HAL_RCC_DMA2_CLK_ENABLE();

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();


	// DCMI GPIO jako alternate function
	gpio_init_structure.Pin       = GPIO_PIN_4 | GPIO_PIN_6;	//PA4=HREF, PA6=PCLK
	gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
	gpio_init_structure.Pull      = GPIO_NOPULL;
	gpio_init_structure.Speed     = GPIO_SPEED_LOW;
	gpio_init_structure.Alternate = GPIO_AF13_DCMI;
	HAL_GPIO_Init(GPIOA, &gpio_init_structure);

	gpio_init_structure.Pin       = GPIO_PIN_7 | GPIO_PIN_9;	//PB7=VSYNC, PB9=DCMI_D7
	gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
	gpio_init_structure.Pull      = GPIO_NOPULL;
	gpio_init_structure.Speed     = GPIO_SPEED_LOW;
	gpio_init_structure.Alternate = GPIO_AF13_DCMI;
	HAL_GPIO_Init(GPIOB, &gpio_init_structure);

	gpio_init_structure.Pin       = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_11;	//PC6=DCMI_DO, PC7=DCMI_D1, PC8=DCMI_D2, PC11=DCMI_D4
	gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
	gpio_init_structure.Pull      = GPIO_NOPULL;
	gpio_init_structure.Speed     = GPIO_SPEED_LOW;
	gpio_init_structure.Alternate = GPIO_AF13_DCMI;
	HAL_GPIO_Init(GPIOC, &gpio_init_structure);

	gpio_init_structure.Pin       = GPIO_PIN_1 | GPIO_PIN_5;	//PE1=DCMI_D3, PE5=DCMI_D6
	gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
	gpio_init_structure.Pull      = GPIO_NOPULL;
	gpio_init_structure.Speed     = GPIO_SPEED_LOW;
	gpio_init_structure.Alternate = GPIO_AF13_DCMI;
	HAL_GPIO_Init(GPIOE, &gpio_init_structure);

	/*gpio_init_structure.Pin       = GPIO_PIN_14 | GPIO_PIN_15;	//PF14=I2C4_SCL, PF15=I2C4_SDA
	gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
	gpio_init_structure.Pull      = GPIO_PULLUP;
	gpio_init_structure.Speed     = GPIO_SPEED_MEDIUM;
	gpio_init_structure.Alternate = GPIO_AF4_I2C4;
	HAL_GPIO_Init(GPIOF, &gpio_init_structure);*/

	gpio_init_structure.Pin       = GPIO_PIN_9;					//PF9=TIM14_CH1
	gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
	gpio_init_structure.Pull      = GPIO_NOPULL;
	gpio_init_structure.Speed     = GPIO_SPEED_HIGH;
	gpio_init_structure.Alternate = GPIO_AF9_TIM14;
	HAL_GPIO_Init(GPIOF, &gpio_init_structure);

	HAL_NVIC_SetPriority(DCMI_IRQn, 0x0F, 0);
	HAL_NVIC_EnableIRQ(DCMI_IRQn);

	HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0x0F, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

	KameraPWDN_Pin(GPIO_PIN_RESET);		//wyłącz PWDN
	KameraRST_Pin(GPIO_PIN_RESET);		//włącz RST
	HAL_Delay(10);	//power on period
	KameraRST_Pin(GPIO_PIN_SET);		//wyłącz RST
	HAL_Delay(20);						//>20ms delay from RESETB pull high to SCCB initialisation

	//zegar timera taktowany jest z APB1 = 108MHz, prescaler = 0
	//kamera wymaga zegara 24MHz (6-27MHz), więc zegar trzeba podzielić na 5 => XCLK = 21,6MHz
	TIM14->CR1 |= TIM_CR1_CEN;		//włącz timer taktujacy kamerę
	//TIM14->ARR = 5-1;				//częstotliwość PWM
	TIM14->ARR = 6-1;				//częstotliwość PWM	12MHz
	TIM14->CCR1 = 2;				//wypełnienie PWM
	TIM14->CCER |= TIM_CCER_CC1E;	//włącz wyjście timera

	//sprawdź czy mamy kontakt z kamerą
	err = SprawdzKamere();
	if (err)
		return err;

	Wyslij_I2C_Kamera(0x3103, 0x11);	//PLL clock select: [1] select system input clock: 0=from pad clock, 1=from PLL
	Wyslij_I2C_Kamera(0x3008, 0x82);	//system control 00: [7] software reset mode, [6] software power down mode {def=0x02}
	HAL_Delay(10);

	OV5640_Flash_Lamp(1);
	err = Wyslij_Blok_Kamera(ov5640_init, sizeof(ov5640_init)/4);
	if (err)
		return err;

	err = Wyslij_Blok_Kamera(ov5640_rgb565, sizeof(ov5640_rgb565)/4);
		if (err)
			return err;

	/*/ustaw domyślne parametry pracy kamery
	KonfKam.sSzerWe = 1280;
	KonfKam.sWysWe = 960;
	KonfKam.sSzerWy = 320;
	KonfKam.sWysWy = 240;
	KonfKam.chTrybDiagn = 0;	//brak trybu diagnostycznego
	KonfKam.chFlagi = 0;

	err = UstawKamere(&KonfKam);
	if (err)
		return err;*/

	Wyslij_I2C_Kamera(0x4740, 0x23);	//Polarity CTRL00 {0x20}: [5] PCLK Polarity, [3] Gate PCLK under VSYNC, [2] Gate PCLK under HREF, [1] HREF polarity, [0]  VSYNC polarity

	OV5640_Flash_Lamp(0);
	return RozpocznijPraceDCMI(KonfKam.chFlagi & FUK1_ZDJ_FILM);	//1 = zdjecie, 0 = film (tylko ten jeden bit)
}



////////////////////////////////////////////////////////////////////////////////
// konfiguruje wybrane parametry kamery
// Parametry: konf - struktura konfiguracji kamery
// Zwraca: kod błędu HAL
////////////////////////////////////////////////////////////////////////////////
HAL_StatusTypeDef UstawKamere(typKonfKam *konf)
{
	//uint8_t chReg;

	Wyslij_I2C_Kamera(0x5001, 0x7F);	//ISP control 01: [7] Special digital effects, [6] UV adjust enable, [5]1=Vertical scaling enable, [4]1=Horizontal scaling enable, [3] Line stretch enable, [2] UV average enable, [1] color matrix enable, [0] auto white balance AWB

	//ustaw rozdzielczość wejściową
	Wyslij_I2C_Kamera(0x3804, (uint8_t)(konf->sSzerWe>>8));		//Timing HW: [3:0] Horizontal width high byte 0x500=1280,  0x280=640, 0x140=320 (scale input}
	Wyslij_I2C_Kamera(0x3805, (uint8_t)(konf->sSzerWe & 0xFF));	//Timing HW: [7:0] Horizontal width low byte
	Wyslij_I2C_Kamera(0x3806, (uint8_t)(konf->sWysWe>>8));			//Timing VH: [3:0] HREF vertical height high byte 0x3C0=960, 0x1E0=480, 0x0F0=240
	Wyslij_I2C_Kamera(0x3807, (uint8_t)(konf->sWysWe & 0xFF));		//Timing VH: [7:0] HREF vertical height low byte

	//ustaw rozdzielczość wyjściową
	Wyslij_I2C_Kamera(0x3808, (uint8_t)(konf->sSzerWy>>8));		//Timing DVPHO: [3:0] output horizontal width high byte [11:8]
	Wyslij_I2C_Kamera(0x3809, (uint8_t)(konf->sSzerWy & 0xFF));	//Timing DVPHO: [7:0] output horizontal width low byte [7:0]
	Wyslij_I2C_Kamera(0x380a, (uint8_t)(konf->sWysWy>>8));			//Timing DVPVO: [3:0] output vertical height high byte [11:8]
	Wyslij_I2C_Kamera(0x380b, (uint8_t)(konf->sWysWy & 0xFF));		//Timing DVPVO: [7:0] output vertical height low byte [7:0]

	//wzór testowy
	switch(konf->chTrybDiagn)
	{
	case TDK_KRATA_CB:	//czarnobiała karata
		Wyslij_I2C_Kamera(0x503d , 0x85);	//test pattern: B/W square
		Wyslij_I2C_Kamera(0x503e, 0x1a);	//PRE ISP TEST SETTING2 [7] reserved, [6:4] 1=random data pattern seed enable, [3] 1=test pattern square b/w mode, [2] 1=add test pattern on image data, [1:0] 0=color bar, 1=random data, 2=square data, 3=black image
		break;

	case TDK_PASKI:		//7 pionowych pasków
		Wyslij_I2C_Kamera(0x503d , 0x80);	//test pattern: color bar
		Wyslij_I2C_Kamera(0x503e, 0x00);	//PRE ISP TEST SETTING2 [7] reserved, [6:4] 1=random data pattern seed enable, [3] 1=test pattern square b/w mode, [2] 1=add test pattern on image data, [1:0] 0=color bar, 1=random data, 2=square data, 3=black image
		break;

	case TDK_PRACA:		//normalna praca
	default:
	}

	return 0;
	//ustaw rotację w poziomie i pionie
	//chReg = 0x80 + ((konf->chFlagi && FUK1_OBR_PION) << 6) +  ((konf->chFlagi && FUK1_OBR_POZ) << 5);
	//return Wyslij_I2C_Kamera(0x3818, chReg);	//TIMING TC REG18: [6] mirror, [5] Vertial flip, [4] 1=thumbnail mode,  [3] 1=compression, [1] vertical subsample 1/4, [0] vertical subsample 1/2  <def:0x80>
	//for the mirror function it is necessary to set registers 0x3621 [5:4] and 0x3801
}



////////////////////////////////////////////////////////////////////////////////
// uruchamia DCMI w trybie pojedyńczego zdjęcia jako aparat lub ciagłej pracy jako kamera
// Parametry: chAparat - 1 = tryb pojedyńczego zdjęcia, 0 = tryb filmu
// Zwraca: kod błędu HAL
////////////////////////////////////////////////////////////////////////////////
HAL_StatusTypeDef RozpocznijPraceDCMI(uint8_t chAparat)
{
	HAL_StatusTypeDef err;

	hdcmi.Instance = DCMI;
	hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
	hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_FALLING;
	hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;
	hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_HIGH;
	hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
	hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
	hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;
	hdcmi.Init.ByteSelectMode = DCMI_BSM_ALL;
	hdcmi.Init.ByteSelectStart = DCMI_OEBS_ODD;
	hdcmi.Init.LineSelectMode = DCMI_LSM_ALL;
	hdcmi.Init.LineSelectStart = DCMI_OELS_ODD;
	hdcmi.Instance->IER = DCMI_IT_FRAME | DCMI_IT_OVR | DCMI_IT_ERR | DCMI_IT_VSYNC | DCMI_IT_LINE;
	err = HAL_DCMI_Init(&hdcmi);
	if (err)
		return err;

	//konfiguracja DMA do DCMI
	hdma_dcmi.Instance = DMA2_Stream1;
	//hdma_dcmi.Init.Request = DMA_REQUEST_DCMI;
	hdma_dcmi.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_dcmi.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_dcmi.Init.MemInc = DMA_MINC_ENABLE;
	hdma_dcmi.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma_dcmi.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	if (chAparat)		//1 = zdjecie, 0 = film
		hdma_dcmi.Init.Mode = DMA_NORMAL;
	else
		hdma_dcmi.Init.Mode = DMA_CIRCULAR;
	hdma_dcmi.Init.Priority = DMA_PRIORITY_HIGH;
	hdma_dcmi.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	err = HAL_DMA_Init(&hdma_dcmi);
	if (err)
		return err;

	//Konfiguracja transferu DMA z DCMI do pamięci
	if (chAparat)		//1 = zdjecie, 0 = film
		return HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT, (uint32_t)nBuforKamery, ROZM_BUF32_KAM);
	else
		return HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)nBuforKamery, ROZM_BUF32_KAM);
}



////////////////////////////////////////////////////////////////////////////////
// wykonuje jedno zdjęcie kamerą i trzyma je w buforze kamery
// Parametry:
// [i] - sSzerokosc - szerokość zdjecia w pikselach
// [i] - sWysokosc - wysokość zdjęcia w pikselach
// Zwraca: kod błędu
////////////////////////////////////////////////////////////////////////////////
HAL_StatusTypeDef ZrobZdjecie(int16_t sSzerokosc, uint16_t sWysokosc)
{
	HAL_StatusTypeDef err;

	err = HAL_DCMI_Stop(&hdcmi);
	if (err)
		return err;

	//skalowanie obrazu
	Wyslij_I2C_Kamera(0x5001, 0x7F);	//ISP control 01: [7] Special digital effects, [6] UV adjust enable, [5]1=Vertical scaling enable, [4]1=Horizontal scaling enable, [3] Line stretch enable, [2] UV average enable, [1] color matrix enable, [0] auto white balance AWB
	Wyslij_I2C_Kamera(0x3804, 0x05);	//Timing HW: [3:0] Horizontal width high byte 0x500=1280,  0x280=640, 0x140=320 (scale input}
	Wyslij_I2C_Kamera(0x3805, 0x00);	//Timing HW: [7:0] Horizontal width low byte
	Wyslij_I2C_Kamera(0x3806, 0x03);	//Timing VH: [3:0] HREF vertical height high byte 0x3C0=960, 0x1E0=480, 0x0F0=240
	Wyslij_I2C_Kamera(0x3807, 0xC0);	//Timing VH: [7:0] HREF vertical height low byte

	//ustaw rozmiar obrazu
	Wyslij_I2C_Kamera(0x3808, (sSzerokosc & 0xFF00)>>8);	//Timing DVPHO: [3:0] output horizontal width high byte [11:8]
	Wyslij_I2C_Kamera(0x3809, (sSzerokosc & 0x00FF));		//Timing DVPHO: [7:0] output horizontal width low byte [7:0]
	Wyslij_I2C_Kamera(0x380a, (sWysokosc & 0xFF00)>>8);		//Timing DVPVO: [3:0] output vertical height high byte [11:8]
	Wyslij_I2C_Kamera(0x380b, (sWysokosc & 0x00FF));		//Timing DVPVO: [7:0] output vertical height low byte [7:0]

	return RozpocznijPraceDCMI(1);	//Konfiguracja transferu DMA z DCMI do pamięci
	//return HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT, (uint32_t)nBuforKamery, ROZM_BUF32_KAM);
}


HAL_StatusTypeDef InitKamera1(void)
{
	HAL_StatusTypeDef err = 0;

	KameraRST_Pin(GPIO_PIN_RESET);		//włącz RST
	HAL_Delay(10);	//power on period
	KameraRST_Pin(GPIO_PIN_SET);		//wyłącz RST
	HAL_Delay(10);

	//zegar timera taktowany jest z APB1 = 108MHz, prescaler = 0
	//kamera wymaga zegara 24MHz (6-27MHz), więc zegar trzeba podzielić na 5 => XCLK = 21,6MHz
	TIM14->CR1 |= TIM_CR1_CEN;		//włącz timer taktujacy kamerę
	TIM14->ARR = 6-1;				//częstotliwość PWM
	TIM14->CCR1 = 2;				//wypełnienie PWM
	TIM14->CCER |= TIM_CCER_CC1E;	//włącz wyjście timera

	Wyslij_I2C_Kamera(0x3103, 0x11);	//PLL clock select: [1] select system input clock: 0=from pad clock, 1=from PLL
	Wyslij_I2C_Kamera(0x3008, 0x82);	//system control 00: [7] software reset mode, [6] software power down mode {def=0x02}
	HAL_Delay(10);
	err = Wyslij_Blok_Kamera(ov5640_init, sizeof(ov5640_init)/4);
	if (err)
		return err;

	err = Wyslij_Blok_Kamera(ov5640_rgb565, sizeof(ov5640_rgb565)/4);
		if (err)
			return err;

	//Wyslij_I2C_Kamera(0x4300, 0x61);	// Format control: [7:4]3=YUV 422, YUYV, 6=RGB565, [3:0]

	//konfiguracja DCMI
	hdcmi.Instance = DCMI;
	hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
	hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_FALLING;
	hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;
	hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_HIGH;
	hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
	hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
	hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;
	hdcmi.Init.ByteSelectMode = DCMI_BSM_ALL;
	hdcmi.Init.ByteSelectStart = DCMI_OEBS_ODD;
	hdcmi.Init.LineSelectMode = DCMI_LSM_ALL;
	hdcmi.Init.LineSelectStart = DCMI_OELS_ODD;
	hdcmi.Instance->IER = DCMI_IT_FRAME | DCMI_IT_OVR | DCMI_IT_ERR | DCMI_IT_VSYNC | DCMI_IT_LINE;
	HAL_DCMI_Init(&hdcmi);

	//HAL_DCMI_ConfigCrop(&hdcmi, (2592-640)/2, (1944-480)/2, 640-1, 480-1);
	//HAL_DCMI_EnableCrop(&hdcmi);

	Wyslij_I2C_Kamera(0x4740, 0x03);	//Polarity CTRL00 {0x20}: [5] PCLK Polarity, [3] Gate PCLK under VSYNC, [2] Gate PCLK under HREF, [1] HREF polarity, [0]  VSYNC polarity

	//Konfiguracja transferu DMA z DCMI do pamięci
	//return HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)nBuforKamery, ROZM_BUF32_KAM);
	return RozpocznijPraceDCMI(KonfKam.chFlagi & FUK1_ZDJ_FILM);	//1 = zdjecie, 0 = film (tylko ten jeden bit)
}



HAL_StatusTypeDef InitKamera2(void)
{
	HAL_StatusTypeDef err = 0;

	KameraRST_Pin(GPIO_PIN_RESET);		//włącz RST
	HAL_Delay(10);						//power on period
	KameraRST_Pin(GPIO_PIN_SET);		//wyłącz RST

	//zegar timera taktowany jest z APB1 = 108MHz, prescaler = 0
	//kamera wymaga zegara 24MHz (6-27MHz), więc zegar trzeba podzielić na 5 => XCLK = 21,6MHz
	TIM14->CR1 |= TIM_CR1_CEN;		//włącz timer taktujacy kamerę
	TIM14->ARR = 6-1;				//częstotliwość PWM
	TIM14->CCR1 = 2;				//wypełnienie PWM
	TIM14->CCER |= TIM_CCER_CC1E;	//włącz wyjście timera

	Wyslij_I2C_Kamera(0x3103, 0x11);	//PLL clock select: [1] select system input clock: 0=from pad clock, 1=from PLL
	Wyslij_I2C_Kamera(0x3008, 0x82);	//system control 00: [7] software reset mode, [6] software power down mode {def=0x02}
	HAL_Delay(10);
	err = Wyslij_Blok_Kamera(ov5640_init, sizeof(ov5640_init)/4);
	if (err)
		return err;

	err = Wyslij_Blok_Kamera(ov5640_rgb565, sizeof(ov5640_rgb565)/4);
	if (err)
		return err;

	//Wyslij_I2C_Kamera(0x503D, 0x80);	//Pre ISP Test Setting 1: [7] Color bar enable; [3:2] color bar style: 0=standard 8 col,1=gradual vert1, 2=gradual hor, 3=gradual vert2
	//Wyslij_I2C_Kamera(0x4300, 0x62);	// Format control: [7:4]3=YUV 422, YUYV, 6=RGB565, [3:0]

	//konfiguracja DCMI
	hdcmi.Instance = DCMI;
	hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
	hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
	hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;
	hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_HIGH;
	hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
	hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
	hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;
	hdcmi.Init.ByteSelectMode = DCMI_BSM_ALL;
	hdcmi.Init.ByteSelectStart = DCMI_OEBS_ODD;
	hdcmi.Init.LineSelectMode = DCMI_LSM_ALL;
	hdcmi.Init.LineSelectStart = DCMI_OELS_ODD;
	HAL_DCMI_Init(&hdcmi);

	Wyslij_I2C_Kamera(0x4740, 0x03);	//Polarity CTRL00 {0x20}: [5] PCLK Polarity, [3] Gate PCLK under VSYNC, [2] Gate PCLK under HREF, [1] HREF polarity, [0]  VSYNC polarity

	//Konfiguracja transferu DMA z DCMI do pamięci
	return HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)nBuforKamery, ROZM_BUF32_KAM);
	//return RozpocznijPraceDCMI(KonfKam.chFlagi & FUK1_ZDJ_FILM);	//1 = zdjecie, 0 = film (tylko ten jeden bit)
}



HAL_StatusTypeDef InitKamera3(void)
{
	HAL_StatusTypeDef err = 0;

	KameraRST_Pin(GPIO_PIN_RESET);		//włącz RST
	HAL_Delay(10);						//power on period
	KameraRST_Pin(GPIO_PIN_SET);		//wyłącz RST

	//zegar timera taktowany jest z APB1 = 108MHz, prescaler = 0
	//kamera wymaga zegara 24MHz (6-27MHz), więc zegar trzeba podzielić na 5 => XCLK = 21,6MHz
	TIM14->CR1 |= TIM_CR1_CEN;		//włącz timer taktujacy kamerę
	TIM14->ARR = 6-1;				//częstotliwość PWM
	TIM14->CCR1 = 2;				//wypełnienie PWM
	TIM14->CCER |= TIM_CCER_CC1E;	//włącz wyjście timera

	Wyslij_I2C_Kamera(0x3103, 0x11);	//PLL clock select: [1] select system input clock: 0=from pad clock, 1=from PLL
	Wyslij_I2C_Kamera(0x3008, 0x82);	//system control 00: [7] software reset mode, [6] software power down mode {def=0x02}
	HAL_Delay(10);
	err = Wyslij_Blok_Kamera(ov5640_init, sizeof(ov5640_init)/4);
	if (err)
		return err;

	err = Wyslij_Blok_Kamera(ov5640_rgb565, sizeof(ov5640_rgb565)/4);
	if (err)
		return err;

	//Wyslij_I2C_Kamera(0x503D, 0x84);	//Pre ISP Test Setting 1: [7] Color bar enable; [3:2] color bar style: 0=standard 8 col,1=gradual vert1, 2=gradual hor, 3=gradual vert2
	//Wyslij_I2C_Kamera(0x4300, 0x63);	// Format control: [7:4]3=YUV 422, YUYV, 6=RGB565, [3:0]

	//konfiguracja DCMI
	hdcmi.Instance = DCMI;
	hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
	hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
	hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;
	hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_HIGH;
	hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
	hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
	hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;
	hdcmi.Init.ByteSelectMode = DCMI_BSM_ALL;
	hdcmi.Init.ByteSelectStart = DCMI_OEBS_ODD;
	hdcmi.Init.LineSelectMode = DCMI_LSM_ALL;
	hdcmi.Init.LineSelectStart = DCMI_OELS_ODD;
	HAL_DCMI_Init(&hdcmi);

	Wyslij_I2C_Kamera(0x4740, 0x23);	//Polarity CTRL00 {0x20}: [5] PCLK Polarity, [3] Gate PCLK under VSYNC, [2] Gate PCLK under HREF, [1] HREF polarity, [0]  VSYNC polarity

	//Konfiguracja transferu DMA z DCMI do pamięci
	return HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)nBuforKamery, ROZM_BUF32_KAM);
	//return HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT, (uint32_t)nBuforKamery, ROZM_BUF32_KAM);
}



HAL_StatusTypeDef InitKamera4(void)
{
	HAL_StatusTypeDef err = 0;

		KameraRST_Pin(GPIO_PIN_RESET);		//włącz RST
		HAL_Delay(10);						//power on period
		KameraRST_Pin(GPIO_PIN_SET);		//wyłącz RST

		//zegar timera taktowany jest z APB1 = 108MHz, prescaler = 0
		//kamera wymaga zegara 24MHz (6-27MHz), więc zegar trzeba podzielić na 5 => XCLK = 21,6MHz
		TIM14->CR1 |= TIM_CR1_CEN;		//włącz timer taktujacy kamerę
		TIM14->ARR = 6-1;				//częstotliwość PWM
		TIM14->CCR1 = 2;				//wypełnienie PWM
		TIM14->CCER |= TIM_CCER_CC1E;	//włącz wyjście timera

		Wyslij_I2C_Kamera(0x3103, 0x11);	//PLL clock select: [1] select system input clock: 0=from pad clock, 1=from PLL
		Wyslij_I2C_Kamera(0x3008, 0x82);	//system control 00: [7] software reset mode, [6] software power down mode {def=0x02}
		HAL_Delay(10);
		err = Wyslij_Blok_Kamera(ov5640_init, sizeof(ov5640_init)/4);
		if (err)
			return err;

		err = Wyslij_Blok_Kamera(ov5640_rgb565, sizeof(ov5640_rgb565)/4);
		if (err)
			return err;

		//Wyslij_I2C_Kamera(0x503D, 0x88);	//Pre ISP Test Setting 1: [7] Color bar enable; [3:2] color bar style: 0=standard 8 col,1=gradual vert1, 2=gradual hor, 3=gradual vert2
		Wyslij_I2C_Kamera(0x4300, 0x64);	// Format control: [7:4]3=YUV 422, YUYV, 6=RGB565, [3:0]

		//konfiguracja DCMI
		hdcmi.Instance = DCMI;
		hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
		hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
		//hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;
		hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_HIGH;
		//hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_HIGH;
		hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_LOW;
		hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
		hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
		hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;
		hdcmi.Init.ByteSelectMode = DCMI_BSM_ALL;
		hdcmi.Init.ByteSelectStart = DCMI_OEBS_ODD;
		hdcmi.Init.LineSelectMode = DCMI_LSM_ALL;
		hdcmi.Init.LineSelectStart = DCMI_OELS_ODD;
		HAL_DCMI_Init(&hdcmi);

		//Konfiguracja transferu DMA z DCMI do pamięci
		return HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)nBuforKamery, ROZM_BUF32_KAM);
}



HAL_StatusTypeDef InitKamera5(void)
{
	HAL_StatusTypeDef err = 0;

		KameraRST_Pin(GPIO_PIN_RESET);		//włącz RST
		HAL_Delay(10);						//power on period
		KameraRST_Pin(GPIO_PIN_SET);		//wyłącz RST

		//zegar timera taktowany jest z APB1 = 108MHz, prescaler = 0
		//kamera wymaga zegara 24MHz (6-27MHz), więc zegar trzeba podzielić na 5 => XCLK = 21,6MHz
		TIM14->CR1 |= TIM_CR1_CEN;		//włącz timer taktujacy kamerę
		TIM14->ARR = 6-1;				//częstotliwość PWM
		TIM14->CCR1 = 2;				//wypełnienie PWM
		TIM14->CCER |= TIM_CCER_CC1E;	//włącz wyjście timera

		Wyslij_I2C_Kamera(0x3103, 0x11);	//PLL clock select: [1] select system input clock: 0=from pad clock, 1=from PLL
		Wyslij_I2C_Kamera(0x3008, 0x82);	//system control 00: [7] software reset mode, [6] software power down mode {def=0x02}
		HAL_Delay(10);
		err = Wyslij_Blok_Kamera(ov5640_init, sizeof(ov5640_init)/4);
		if (err)
			return err;

		err = Wyslij_Blok_Kamera(ov5640_rgb565, sizeof(ov5640_rgb565)/4);
		if (err)
			return err;

		Wyslij_I2C_Kamera(0x503D, 0x8C);	//Pre ISP Test Setting 1: [7] Color bar enable; [3:2] color bar style: 0=standard 8 col,1=gradual vert1, 2=gradual hor, 3=gradual vert2
		Wyslij_I2C_Kamera(0x4741, 0x00);	//[2] test pattern enable, [1] pattern select 0/1, [0] 0=10-bit, 1=8-bit
		//Wyslij_I2C_Kamera(0x4300, 0x65);	// Format control: [7:4]3=YUV 422, YUYV, 6=RGB565, [3:0]

		//konfiguracja DCMI
		hdcmi.Instance = DCMI;
		hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
		hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
		hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;
		hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_HIGH;
		hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
		hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
		hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;
		hdcmi.Init.ByteSelectMode = DCMI_BSM_ALL;
		hdcmi.Init.ByteSelectStart = DCMI_OEBS_ODD;
		hdcmi.Init.LineSelectMode = DCMI_LSM_ALL;
		hdcmi.Init.LineSelectStart = DCMI_OELS_ODD;
		HAL_DCMI_Init(&hdcmi);

		//Konfiguracja transferu DMA z DCMI do pamięci
		return HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)nBuforKamery, ROZM_BUF32_KAM);
}



uint8_t CzytajKamInit(void)
{
	uint8_t dane;
	//Czytaj_I2C_Kamera(0x4740, &dane);	//Polarity CTRL00
	Czytaj_I2C_Kamera(0x4300, &dane);	//format control
	return dane;
}



// Flash Lamp
//  sw:  0: off
//       1:  on
void OV5640_Flash_Lamp(uint8_t lampa)
{
	Wyslij_I2C_Kamera(0x3016, 0x02);
	Wyslij_I2C_Kamera(0x301C, 0x02);

	if(lampa)
		Wyslij_I2C_Kamera(0x3019, 0x02);
	else
		Wyslij_I2C_Kamera(0x3019, 0x00);
}



////////////////////////////////////////////////////////////////////////////////
// Wyślij polecenie konfiguracyjne do kamery przez I2C2. Funkcja wysyłajaca jest blokująca, kończy się po wysłaniu danych.
// Parametry:
//  rejestr - 16 bitowy adres rejestru kamery
//  dane - dane zapisywane do rejestru
// Zwraca: kod błędu HAL
////////////////////////////////////////////////////////////////////////////////
HAL_StatusTypeDef Wyslij_I2C_Kamera(uint16_t rejestr, uint8_t dane)
{
	uint8_t dane_wy[3];

	dane_wy[0] = (rejestr & 0xFF00) >> 8;
	dane_wy[1] = (rejestr & 0x00FF);
	dane_wy[2] = dane;
	return HAL_I2C_Master_Transmit(&hi2c2, OV5640_I2C_ADR, dane_wy, 3, KAMERA_TIMEOUT);
}



////////////////////////////////////////////////////////////////////////////////
// Odczytaj dane z rejestru kamery
// Parametry:
//  rejestr - 16 bitowy adres rejestru kamery
//  dane - dane zapisywane do rejestru
// Zwraca: kod błędu HAL
////////////////////////////////////////////////////////////////////////////////
HAL_StatusTypeDef Czytaj_I2C_Kamera(uint16_t rejestr, uint8_t *dane)
{
	uint8_t dane_wy[2];
	HAL_StatusTypeDef err;

	dane_wy[0] = (rejestr & 0xFF00) >> 8;
	dane_wy[1] = (rejestr & 0x00FF);
	err = HAL_I2C_Master_Transmit(&hi2c2, OV5640_I2C_ADR, dane_wy, 2, KAMERA_TIMEOUT);
	if (err == 0)
		err = HAL_I2C_Master_Receive(&hi2c2, OV5640_I2C_ADR, dane, 1, KAMERA_TIMEOUT);
	return err;
}



////////////////////////////////////////////////////////////////////////////////
// Wyślij blok poleceń konfiguracyjnych do kamery
// Parametry:
//  reglist - struktura zawierajaca adresy rejestrów i ich zawartość
//  rozmiar - rozmiar struktury
// Zwraca: kod błędu HAL
////////////////////////////////////////////////////////////////////////////////
HAL_StatusTypeDef Wyslij_Blok_Kamera(const struct sensor_reg reglist[], uint16_t rozmiar)
{
	HAL_StatusTypeDef err;

	for (uint16_t x=0; x<rozmiar; x++)
	{
		uint8_t liczba_powtorzen = 5;			//liczba powtórzeń w przypadku błędu
		do
		{
			err = Wyslij_I2C_Kamera(reglist[x].reg, reglist[x].val);
			liczba_powtorzen--;
		}
		while (err && liczba_powtorzen);
	}
	return err;
}



////////////////////////////////////////////////////////////////////////////////
// Callback od końca wiersza kamery
// Parametry:
//  hdcmi - wskaźnik na interfejs DCMI
// Zwraca: nic
////////////////////////////////////////////////////////////////////////////////
void HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	LED_Toggle(LED_RED);
}



////////////////////////////////////////////////////////////////////////////////
// Callback od końca wiersza kamery
// Parametry:
//  hdcmi - wskaźnik na interfejs DCMI
// Zwraca: nic
////////////////////////////////////////////////////////////////////////////////
void HAL_DCMI_VsyncEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	LED_Toggle(LED_BLUE);
}



////////////////////////////////////////////////////////////////////////////////
// Callback od końca wiersza kamery
// Parametry:
//  hdcmi - wskaźnik na interfejs DCMI
// Zwraca: nic
////////////////////////////////////////////////////////////////////////////////
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	chNowyObrazKamery = 1;
}


void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef *hdcmi)
{

}


////////////////////////////////////////////////////////////////////////////////
// konwersja bufora w formacie RAW na RGB656
// Parametry:
//  nBuforKamery - wskaźnik na bufor danych do konwersji
// Zwraca: nic
////////////////////////////////////////////////////////////////////////////////
void RAW2RGB(uint32_t *nBufKamery, uint16_t *sBufLCD)
{
	uint8_t pixel1[3], pixel2[3];
	uint16_t x, y;
	uint32_t dx, dy0, dy1;
	uint32_t pk1, pk2;

	for (y=0; y<LCD_WYS; y++)
		for (x=0; x<LCD_SZER; x++)
			sBufLCD[y * LCD_SZER + x] = 0;

	for (y=0; y<LCD_WYS; y++)
	{
		dy0 = (2*y + 0) * LCD_SZER/2;
		dy1 = (2*y + 1) * LCD_SZER/2;
		for (x=0; x<LCD_SZER/2; x++)
		{
			dx = 2*x;
			pk1 = nBufKamery[dy0 + x];
			pk2 = nBufKamery[dy1 + x];

			/*pixel1[COL_B] = (pk1 & 0x000000FF);
			pixel2[COL_B] = (pk1 & 0x00FF0000) >> 16;

			pixel1[COL_R] = (pk2 & 0x00FF0000) >> 16;
			pixel2[COL_R] = (pk2 & 0x000000FF);

			pixel1[COL_G] = (((pk1 & 0xFF000000) >> 24) + ((pk2 & 0x00FF0000) >> 16)) / 2;
			pixel2[COL_G] = (((pk1 & 0x0000FF00) >> 8)  + (pk2 & 0x000000FF)) / 2; */

			pixel1[COL_B] = (pk1 & 0x0000FF00) >> 8;
			pixel2[COL_B] = (pk1 & 0xFF000000) >> 24;

			pixel1[COL_R] = (pk2 & 0x000000FF);
			pixel2[COL_R] = (pk2 & 0x00FF0000) >> 16;

			pixel1[COL_G] = (((pk2 & 0x0000FF00) >> 8)  + (pk1 & 0x000000FF)) / 2;
			pixel2[COL_G] = (((pk2 & 0xFF000000) >> 24) + ((pk1 & 0x00FF0000) >> 16)) / 2;

			//formatowanie danych wyjściowych
			sBufLCD[y * LCD_SZER + dx + 0] = ((pixel1[COL_R] & 0xF8) << 8) | ((pixel1[COL_G] & 0xFC) << 3) | ((pixel1[COL_B] & 0xF8) >> 3);
			sBufLCD[y * LCD_SZER + dx + 1] = ((pixel2[COL_R] & 0xF8) << 8) | ((pixel2[COL_G] & 0xFC) << 3) | ((pixel2[COL_B] & 0xF8) >> 3);
		}
	}
}



// set the output size
void OV5640_OutSize_Set(uint16_t offX, uint16_t offY, uint16_t width, uint16_t height)
{
	Wyslij_I2C_Kamera(0x3212,0x03);

	Wyslij_I2C_Kamera(0x3808, width >> 8);
	Wyslij_I2C_Kamera(0x3809, width & 0xFF);
	Wyslij_I2C_Kamera(0x380a, height >> 8);
	Wyslij_I2C_Kamera(0x380b, height & 0xFF);

	Wyslij_I2C_Kamera(0x3810, offX >> 8);
	Wyslij_I2C_Kamera(0x3811, offX & 0xFF);

	Wyslij_I2C_Kamera(0x3812, offY >> 8);
	Wyslij_I2C_Kamera(0x3813, offY & 0xFF);

	Wyslij_I2C_Kamera(0x3212, 0x13);
	Wyslij_I2C_Kamera(0x3212, 0xa3);
}



////////////////////////////////////////////////////////////////////////////////
// Sprawdź czy mamy kontakt z kamerą
// Parametry: brak
// Zwraca: systemowy kod błędu
////////////////////////////////////////////////////////////////////////////////
uint8_t	SprawdzKamere(void)
{
	uint16_t DaneH;
	uint8_t daneL, powtorz = 10;

	do
	{
		Czytaj_I2C_Kamera(OV5640_CHIPIDH, (uint8_t*)&DaneH);	//Chip ID High Byte = 0x56
		Czytaj_I2C_Kamera(OV5640_CHIPIDL, &daneL);	//Chip ID Low Byte = 0x42
		powtorz--;
		HAL_Delay(1);
		DaneH <<= 8;
		DaneH |= daneL;
	}
	while ((DaneH != OV5640_ID) && powtorz);
	if (powtorz == 0)
		return ERR_BRAK_KAMERY;
	else
		return ERR_OK;
}



////////////////////////////////////////////////////////////////////////////////
// Aktywne oczekiwanie na zmianę stanu zmiennej z timeoutem
// Parametry:
// [we] *chBit - wskaźnik na zmienną typu volatile, która ma zmienić swój stan w przerwaniu
// [we] sTimeout - czas oczekiwania liczony w obiegach pętli
// Zwraca: systemowy kod błędu
////////////////////////////////////////////////////////////////////////////////
uint8_t CzekajNaBit(volatile uint8_t *chBit, uint16_t sTimeout)
{
	do
	{
		sTimeout--;
	}
	while (sTimeout && (*chBit == 0));

	if (sTimeout)
		return ERR_OK;
	else
		return ERR_TIMEOUT;
}

