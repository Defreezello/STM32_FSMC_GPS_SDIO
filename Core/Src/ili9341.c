
 // ili9341.c

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "ili9341.h"


static lcdPropertiesTypeDef  lcdProperties = { ILI9341_PIXEL_WIDTH, ILI9341_PIXEL_HEIGHT, 1, true, true };
static lcdFontPropTypeDef lcdFont = {COLOR_YELLOW, COLOR_BLACK, &Font12, 1};
static lcdCursorPosTypeDef cursorXY = {0, 0};

/*static unsigned char lcdPortraitConfig = 0;
static unsigned char lcdLandscapeConfig = 0;
static unsigned char lcdPortraitMirrorConfig = 0;
static unsigned char lcdLandscapeMirrorConfig = 0;*/

static void				lcdDrawPixels(uint16_t x, uint16_t y, uint16_t *data, uint32_t dataLength);
static void        		lcdReset(void);
void        		    lcdWriteCommand(unsigned char command);  //static
static void             lcdWriteData(unsigned short data);
static unsigned short	lcdReadData(void);


void lcdInit(void)
{

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_Delay(15);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_Delay(15);
  lcdReset();

  lcdWriteCommand(ILI9341_DISPLAYOFF);

  lcdWriteCommand(ILI9341_POWERCONTROL1);
  lcdWriteData(0x23); //(0x26);

  lcdWriteCommand(ILI9341_POWERCONTROL2);
  lcdWriteData(0x10); //(0x11);

  lcdWriteCommand(ILI9341_VCOMCONTROL1);
  lcdWriteData(0x3E); //(0x35);
  lcdWriteData(0x28); //(0x3E);

  lcdWriteCommand(ILI9341_VCOMCONTROL2);
  lcdWriteData(0x86); //(0xBE);

  lcdWriteCommand(ILI9341_PIXELFORMAT);	//16bits/pixel
  lcdWriteData(0x55);

  lcdWriteCommand(ILI9341_FRAMECONTROLNORMAL);
  lcdWriteData(0x00);
  lcdWriteData(0x1B); //Frame Rate 70Hz

  lcdWriteCommand(ILI9341_GAMMASET);
  lcdWriteData(0x01);

  lcdWriteCommand(ILI9341_POSITIVEGAMMCORR);
  lcdWriteData(0x1F);
  lcdWriteData(0x1A);
  lcdWriteData(0x18);
  lcdWriteData(0x0A);
  lcdWriteData(0x0F);
  lcdWriteData(0x06);
  lcdWriteData(0x45);
  lcdWriteData(0x87);
  lcdWriteData(0x32);
  lcdWriteData(0x0A);
  lcdWriteData(0x07);
  lcdWriteData(0x02);
  lcdWriteData(0x07);
  lcdWriteData(0x05);
  lcdWriteData(0x00);

  lcdWriteCommand(ILI9341_NEGATIVEGAMMCORR);
  lcdWriteData(0x00);
  lcdWriteData(0x25);
  lcdWriteData(0x27);
  lcdWriteData(0x05);
  lcdWriteData(0x10);
  lcdWriteData(0x09);
  lcdWriteData(0x3A);
  lcdWriteData(0x78);
  lcdWriteData(0x4D);
  lcdWriteData(0x05);
  lcdWriteData(0x18);
  lcdWriteData(0x0D);
  lcdWriteData(0x38);
  lcdWriteData(0x3A);
  lcdWriteData(0x1F);

  lcdWriteCommand(ILI9341_COLADDRSET);
  lcdWriteData(0x00);
  lcdWriteData(0x00);
  lcdWriteData(0x00);
  lcdWriteData(0xEF);

  lcdWriteCommand(ILI9341_PAGEADDRSET);
  lcdWriteData(0x00);
  lcdWriteData(0x00);
  lcdWriteData(0x01);
  lcdWriteData(0x3F);

  lcdWriteCommand(ILI9341_ENTRYMODE);	//Normal Display, LV detect disable
  lcdWriteData(0x07);

  lcdWriteCommand(ILI9341_DISPLAYFUNC);	//Default
  lcdWriteData(0x0A);
  lcdWriteData(0x82);
  lcdWriteData(0x27);
  lcdWriteData(0x00);

  lcdWriteCommand(ILI9341_SLEEPOUT);
  HAL_Delay(100);
  lcdWriteCommand(ILI9341_DISPLAYON);
  HAL_Delay(100);
  lcdWriteCommand(ILI9341_MEMORYWRITE);
  lcdWriteCommand(ILI9341_MEMCONTROL); //BGR format. SetOrientation LandscapeConfig // 0x20 for RGB
  lcdWriteData(0x28);
}


void lcdFillRGB(uint16_t color)
{
  lcdSetWindow(0, 0, lcdProperties.width - 1, lcdProperties.height - 1);
  int dimensions = lcdProperties.width * lcdProperties.height;
  while(dimensions--)
  {
	  if (dimensions % 240)
		  lcdWriteData(color);
	  else {
		  lcdWriteData(color);
	  	  //HAL_Delay(5);
	  }
  }
}

void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    // Clip
    if ((x < 0) || (y < 0) || (x >= lcdProperties.width) || (y >= lcdProperties.height))
        return;

    lcdSetWindow(x, y, x, y);
    lcdWriteData(color);

}

void lcdDrawPosition(uint16_t x, uint16_t y, uint16_t color)
{
    if ((x < 0) || (y < 0) || (x >= lcdProperties.width) || (y >= lcdProperties.height))
        return;

    lcdSetWindow((x-3), (y-3), (x+3), (y+3));
    for (int i = 49; i > 0; i--)
    {
    lcdWriteData(color);
    }
}

void lcdDrawHLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color)
{
  // Allows for slightly better performance than setting individual pixels

	if (x1 < x0)
	{
		// Switch x1 and x0
		swap(x0, x1);
	}

	// Check limits
	if (x1 >= lcdProperties.width)
	{
		x1 = lcdProperties.width - 1;
	}

	if (x0 >= lcdProperties.width)
	{
		x0 = lcdProperties.width - 1;
	}

	lcdSetWindow(x0, y, x1, y);

	for (int line = x0; line <= x1; line++)
	{
		lcdWriteData(color);
	}
}

void lcdDrawVLine(uint16_t x, uint16_t y0, uint16_t y1, uint16_t color)
{
  if (y1 < y0)
  {
	  swap(y0, y1);
  }

  if (x >= lcdProperties.width)
  {
    x = lcdProperties.width - 1;
  }

  if (y0 >= lcdProperties.height)
  {
    y0 = lcdProperties.height - 1;
  }

  if (y1 >= lcdProperties.height)
  {
    y1 = lcdProperties.height - 1;
  }

  lcdSetWindow(x, y0, x, y1);

  for(int line = y0; line <= y1; line++)
  {
	  lcdWriteData(color);
  }
}

void lcdDrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{

	int16_t steep = abs(y2 - y1) > abs(x2 - x1);
	if (steep)
	{
		swap(x1, y1);
		swap(x2, y2);
	}

	if (x1 > x2)
	{
		swap(x1, x2);
		swap(y1, y2);
	}

	int16_t dx, dy;
	dx = x2 - x1;
	dy = abs(y2 - y1);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y1 < y2)
	{
		ystep = 1;
	}
	else
	{
		ystep = -1;
	}

	for (; x1 <= x2; x1++)
	{
		if (steep)
		{
			lcdDrawPixel(y1, x1, color);
		}
		else
		{
			lcdDrawPixel(x1, y1, color);
		}
		err -= dy;
		if (err < 0)
		{
			y1 += ystep;
			err += dx;
		}
	}
}

void lcdDrawImage(uint16_t x, uint16_t y, GUI_CONST_STORAGE GUI_BITMAP* pBitmap)
{
	if((x >= lcdProperties.width) || (y >= lcdProperties.height)) return;
	if((x + pBitmap->xSize - 1) >= lcdProperties.width) return;
	if((y + pBitmap->ySize - 1) >= lcdProperties.height) return;

	for (int i = 0; i < pBitmap->ySize; ++i)
	{
		lcdDrawPixels(x, y + i, (uint16_t*)(pBitmap->pData + i * pBitmap->bytesPerLine), pBitmap->bytesPerLine / (pBitmap->bitsPerPixel / 8));
	}
}

void lcdHome(void)
{
	cursorXY.x = 0;
	cursorXY.y = 0;
	lcdSetWindow(0, 0, lcdProperties.width - 1, lcdProperties.height - 1);
}

/**
 * \brief Draws a character at the specified coordinates
 *
 * \param x			The x-coordinate
 * \param y			The y-coordinate
 * \param c			Character
 * \param color		Character color
 * \param bg		Background color
 * \param size		Character Size
 *
 * \return void
 */
void lcdDrawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg)
{
	if ((x >= lcdProperties.width) || 			// Clip right
			(y >= lcdProperties.height) || 		// Clip bottom
			((x + lcdFont.pFont->Width) < 0) || // Clip left
			((y + lcdFont.pFont->Height) < 0))  // Clip top
		return;

	uint8_t fontCoeff = lcdFont.pFont->Height / 8;
	uint8_t xP = 0;

	for(uint8_t i = 0; i < lcdFont.pFont->Height; i++)
	{
		uint8_t line;

		for(uint8_t k = 0; k < fontCoeff; k++)
		{
			line = lcdFont.pFont->table[((c - 0x20) * lcdFont.pFont->Height * fontCoeff) + (i * fontCoeff) + k];

			for(uint8_t j = 0; j < 8; j++)
			{
				if((line & 0x80) == 0x80)
				{
					lcdDrawPixel(x + j + xP, y + i, color);
				}
				else if (bg != color)
				{
					lcdDrawPixel(x + j + xP, y + i, bg);
				}
				line <<= 1;
			}

			xP += 8;
		}

		xP = 0;
	}
}

/**
 * \brief Print the specified Text
 *
 * \param fmt	Format text
 * \param
 *
 * \return void
 */
void lcdPrintf(const char *fmt, ...)
{
	static char buf[256];
	char *p;
	va_list lst;

	va_start(lst, fmt);
	vsprintf(buf, fmt, lst);
	va_end(lst);

	//sprintf(buf, fmt, lst);
	p = buf;
	while (*p)
	{
		if (*p == '\n')
		{
			cursorXY.y += lcdFont.pFont->Height;
			cursorXY.x = 0;
		}
		else if (*p == '\r')
		{
			// skip em
		}
		else if (*p == '\t')
		{
			cursorXY.x += lcdFont.pFont->Width * 4;
		}
		else
		{
			lcdDrawChar(cursorXY.x, cursorXY.y, *p, lcdFont.TextColor, lcdFont.BackColor);
			cursorXY.x += lcdFont.pFont->Width;
			if (lcdFont.TextWrap && (cursorXY.x > (lcdProperties.width - lcdFont.pFont->Width)))
			{
				cursorXY.y += lcdFont.pFont->Height;
				cursorXY.x = 0;
			}
		}
		p++;

		if (cursorXY.y >= lcdProperties.height)
		{
			cursorXY.y = 0;
		}
	}
}

/**
 * \brief Sets the font
 *
 * \param font pointer font
 *
 * \return void
 */
void lcdSetTextFont(sFONT* font)
{
	lcdFont.pFont = font;
}

/**
 * \brief Sets the text color
 *
 * \param c		Text color
 * \param b		Background color
 *
 * \return void
 */
void lcdSetTextColor(uint16_t c, uint16_t b)
{
	lcdFont.TextColor = c;
	lcdFont.BackColor = b;
}

/**
 * \brief Set Text wrap
 *
 * \param w
 *
 * \return void
 */
void lcdSetTextWrap(uint8_t w)
{
	lcdFont.TextWrap = w;
}


void lcdSetCursor(unsigned short x, unsigned short y)
{
	cursorXY.x = x;
	cursorXY.y = y;
	lcdSetWindow(x, y, x, y);
}

/**
 * \brief Sets window address
 *
 * \param x0         Left top window x-coordinate
 * \param y0         Left top window y-coordinate
 * \param x1         Rigth bottom window x-coordinate
 * \param y1         Rigth bottom window y-coordinate
 *
 * \return void
 */
void lcdSetWindow(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1)
{
  lcdWriteCommand(ILI9341_COLADDRSET);
  lcdWriteData((x0 >> 8) & 0xFF);
  lcdWriteData(x0 & 0xFF);
  lcdWriteData((x1 >> 8) & 0xFF);
  lcdWriteData(x1 & 0xFF);
  lcdWriteCommand(ILI9341_PAGEADDRSET);
  lcdWriteData((y0 >> 8) & 0xFF);
  lcdWriteData(y0 & 0xFF);
  lcdWriteData((y1 >> 8) & 0xFF);
  lcdWriteData(y1 & 0xFF);
  lcdWriteCommand(ILI9341_MEMORYWRITE);
}

void lcdBacklightOff(void)
{
	LCD_BL_OFF();
}

void lcdBacklightOn(void)
{
	LCD_BL_ON();
}

void lcdInversionOff(void)
{
	lcdWriteCommand(ILI9341_INVERTOFF);
}

void lcdInversionOn(void)
{
	lcdWriteCommand(ILI9341_INVERTON);
}

void lcdDisplayOff(void)
{
	lcdWriteCommand(ILI9341_DISPLAYOFF);
	LCD_BL_OFF();
}

void lcdDisplayOn(void)
{
	lcdWriteCommand(ILI9341_DISPLAYON);
	LCD_BL_ON();
}

void lcdTearingOff(void)
{
	lcdWriteCommand(ILI9341_TEARINGEFFECTOFF);
}

void lcdTearingOn(bool m)
{
	lcdWriteCommand(ILI9341_TEARINGEFFECTON);
	lcdWriteData(m);
}

uint16_t lcdGetWidth(void)
{
  return lcdProperties.width;
}

uint16_t lcdGetHeight(void)
{
  return lcdProperties.height;
}

uint32_t lcdGetControllerID(void)
{
	uint32_t id;
	lcdWriteCommand(ILI9341_READID4);
//	id = lcdReadData();
//	id = lcdReadData();
	id = ((uint32_t) lcdReadData() << 24);
	id |= ((uint32_t) lcdReadData() << 16);
	id |= ((uint32_t) lcdReadData() << 8);
	id |= lcdReadData();
	return id;
}


sFONT* lcdGetTextFont(void)
{
	return lcdFont.pFont;
}

lcdPropertiesTypeDef lcdGetProperties(void)
{
  return lcdProperties;
}

/**
 * \brief Reads a point from the specified coordinates
 *
 * \param x        x-Coordinate
 * \param y        y-Coordinate
 *
 * \return uint16_t     Color
 */
uint16_t lcdReadPixel(uint16_t x, uint16_t y)
{
    uint16_t temp[3];
    // Clip
    if ((x < 0) || (y < 0) || (x >= lcdProperties.width) || (y >= lcdProperties.height))
        return 0;

    lcdWriteCommand(ILI9341_COLADDRSET);
    lcdWriteData((x >> 8) & 0xFF);
    lcdWriteData(x & 0xFF);

    lcdWriteCommand(ILI9341_PAGEADDRSET);
    lcdWriteData((y >> 8) & 0xFF);
    lcdWriteData(y & 0xFF);

    lcdWriteCommand(ILI9341_MEMORYREAD);

    temp[0] = lcdReadData(); // dummy read
    temp[1] = lcdReadData();
    temp[2] = lcdReadData();

    return lcdColor565((temp[1] >> 8) & 0xFF, temp[1] & 0xFF, (temp[2] >> 8) & 0xFF);
}

uint16_t lcdColor565(uint8_t r, uint8_t g, uint8_t b)
{
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

/*---------Static functions--------------------------*/

static void lcdDrawPixels(uint16_t x, uint16_t y, uint16_t *data, uint32_t dataLength)
{
  uint32_t i = 0;

  lcdSetWindow(x, y, lcdProperties.width - 1, lcdProperties.height - 1);

  do
  {
	  lcdWriteData(data[i++]);
  }
  while (i < dataLength);
}

static void lcdReset(void)
{
	lcdWriteCommand(ILI9341_SOFTRESET);
	HAL_Delay(50);
}

// Write an 8 bit command to the IC driver
void lcdWriteCommand(unsigned char command)
{
	LCD_CmdWrite(command);
}

// Write an 16 bit data word to the IC driver
static void lcdWriteData(unsigned short data)
{
	LCD_DataWrite(data);
}

static unsigned short lcdReadData(void)
{
	return LCD_DataRead();
}

void LCD_PrintVbat(float Text)
{
	lcdSetTextFont(&Font16);
	lcdSetCursor(10, lcdGetHeight() - lcdGetTextFont()->Height - 220);
	lcdSetTextColor(COLOR_BLACK, COLOR_WHITE);
	lcdPrintf("Vbat: %.2f B.", Text);
}

void LCD_PrintCPU_t(float Text)
{
	lcdSetTextFont(&Font16);
	lcdSetCursor(10, lcdGetHeight() - lcdGetTextFont()->Height - 200);
	lcdSetTextColor(COLOR_BLACK, COLOR_WHITE);
	lcdPrintf("CPU Temp.: %.1f c.", Text);
}

void LCD_PrintTime(RTC_TimeTypeDef Time)
{
	lcdSetTextFont(&Font24);
	lcdSetCursor(10, lcdGetHeight() - lcdGetTextFont()->Height - 60);
	lcdSetTextColor(COLOR_BLACK, COLOR_WHITE);
	lcdPrintf("Time: %02d:%02d:%02d", Time.Hours, Time.Minutes, Time.Seconds);
}

void LCD_PrintDate(RTC_DateTypeDef Date)
{
	char i[7][10] = {"MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY", "SUNDAY"};
	lcdSetTextFont(&Font24);
	lcdSetCursor(10, lcdGetHeight() - lcdGetTextFont()->Height - 5);
	lcdSetTextColor(COLOR_BLACK, COLOR_WHITE);
	lcdPrintf("Date: %02d.%02d.20%02d", Date.Date, Date.Month, Date.Year);
	lcdSetTextFont(&Font20);
	lcdSetCursor(10, lcdGetHeight() - lcdGetTextFont()->Height - 35);
	lcdPrintf("Day: %s", i[Date.WeekDay -1]);
}

void LCD_PrintSat(char sat1, char sat2)
{
	lcdSetTextFont(&Font16);
	lcdSetCursor(10, lcdGetHeight() - lcdGetTextFont()->Height - 180);
	lcdSetTextColor(COLOR_BLACK, COLOR_WHITE);
	lcdPrintf("Satellites in view: %c%c", sat1, sat2);
}

void LCD_GPS_lost()
{
	lcdSetTextFont(&Font12);
	lcdSetCursor(0, lcdGetHeight() - lcdGetTextFont()->Height - 0);
	lcdSetTextColor(COLOR_RED, COLOR_BLACK);
	lcdPrintf("GPS Lost");
}
