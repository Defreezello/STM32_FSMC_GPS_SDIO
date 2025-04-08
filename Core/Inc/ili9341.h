/*
 * ili9341.h
 */

#ifndef ILI9341_H_
#define ILI9341_H_

#include "main.h"
#include "colors.h"
#include "registers.h"
#include "fonts.h"
#include "image.h"
#include <stdbool.h>

#define LCD_BL_ON() HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET)  //GPIO_PIN_RESET for some Displays
#define LCD_BL_OFF() HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET)

#define LCD_BASE0        		((uint32_t)0x60000000)
#define LCD_BASE1        		((uint32_t)0x60080000)

#define LCD_CmdWrite(command)	*(volatile uint16_t *) (LCD_BASE0) = (command)
#define LCD_DataWrite(data)		*(volatile uint16_t *) (LCD_BASE1) = (data)
#define	LCD_StatusRead()		*(volatile uint16_t *) (LCD_BASE0) //if use read  Mcu interface DB0~DB15 needs increase pull high
#define	LCD_DataRead()			*(volatile uint16_t *) (LCD_BASE1) //if use read  Mcu interface DB0~DB15 needs increase pull high

#define swap(a, b) { int16_t t = a; a = b; b = t; }

#define ILI9341_PIXEL_WIDTH		320	//240
#define ILI9341_PIXEL_HEIGHT 	240	//320
#define ILI9341_PIXEL_COUNT		ILI9341_PIXEL_WIDTH * ILI9341_PIXEL_HEIGHT

// Any LCD needs to implement these common methods, which allow the low-level
// initialisation and pixel-setting details to be abstracted away from the
// higher level drawing and graphics code.
/**
  * @brief  Draw Properties structures definition
  */
typedef struct
{
	uint32_t 	TextColor;
	uint32_t 	BackColor;
	sFONT*    	pFont;
	uint8_t		TextWrap;
}lcdFontPropTypeDef;

typedef struct
{
	unsigned short	x;
	unsigned short	y;
}lcdCursorPosTypeDef;

// This struct is used to indicate the capabilities of different LCDs
typedef struct
{
  uint16_t				width;         // LCD width in pixels (default orientation)
  uint16_t				height;        // LCD height in pixels (default orientation)
  bool	orientation;   // Whether the LCD orientation can be modified lcdOrientationTypeDef
  bool					touchscreen;   // Whether the LCD has a touch screen
  bool					hwscrolling;   // Whether the LCD support HW scrolling
} lcdPropertiesTypeDef;

void					lcdInit(void);
void					lcdFillRGB(uint16_t color);
void					lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color);
void              		lcdDrawHLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color);
void              		lcdDrawVLine(uint16_t x, uint16_t y0, uint16_t y1, uint16_t color);
void 					lcdDrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void					lcdDrawImage(uint16_t x, uint16_t y, GUI_CONST_STORAGE GUI_BITMAP* pBitmap);
void              		lcdHome(void);
void 					lcdDrawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg);
void					lcdPrintf(const char *fmt, ...);
void					lcdSetTextFont(sFONT* font);
void					lcdSetTextColor(uint16_t c, uint16_t b);
void					lcdSetTextWrap(uint8_t w);
void					lcdSetCursor(unsigned short x, unsigned short y);
void              		lcdSetWindow(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1);
void					lcdBacklightOff(void);
void					lcdBacklightOn(void);
void					lcdInversionOff(void);
void					lcdInversionOn(void);
void					lcdDisplayOff(void);
void					lcdDisplayOn(void);
void					lcdTearingOff(void);
void					lcdTearingOn(bool m);
uint16_t          		lcdGetWidth(void);
uint16_t          		lcdGetHeight(void);
uint32_t          		lcdGetControllerID(void);
sFONT*					lcdGetTextFont(void);
uint16_t				lcdReadPixel(uint16_t x, uint16_t y);
uint16_t 				lcdColor565(uint8_t r, uint8_t g, uint8_t b);
//Need enable use float with 'printf' and 'scanf' in CubeIDE(Project->Properties->C/C++ Build->Settings->Tool Settings->MCU Settings)
void					LCD_PrintVbat(float Text);
void 					LCD_PrintCPU_t(float Text);
void 					LCD_PrintTime(RTC_TimeTypeDef Time);
void 					LCD_PrintDate(RTC_DateTypeDef Date);
void 					LCD_PrintSat(char sat1, char sat2);
void 					lcdDrawPosition(uint16_t x, uint16_t y, uint16_t color);
void					LCD_GPS_lost();

#endif /* ILI9341_H_ */
