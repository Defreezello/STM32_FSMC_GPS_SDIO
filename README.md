# STM32_FSMC_GPS_SDIO
STM32F407 GPS navigation device using ili9341 display and loading maps from a microSD card

This project include: FSMC for ili9341 display, USART for GPS module, SDIO for microSD with map(s), SPI for touch XPT2046,
ADC connection to Vbat on the board.
RTC is used to display time and date. The time is adjusted every time after switching on, when a valid GPS signal is found.
Key1 can be used for test output of coordinates.

Input frequency is 8MHz - HSE and 32.768KHz - LSE(to RTC)
MicroSD works at maximum speed 48MHz! 

Connections:
FSMC:
FSMC_D0 - PD14
FSMC_D1 - PD15
FSMC_D2 - PD0
FSMC_D3 - PD1
FSMC_D4 - PE7
FSMC_D5 - PE8
FSMC_D6 - PE9
FSMC_D7 - PE10
FSMC_D8 - PE11
FSMC_D9 - PE12
FSMC_D10 - PE13
FSMC_D11 - PE14
FSMC_D12 - PE15
FSMC_D13 - PD8
FSMC_D14 - PD9
FSMC_D15 - PD10
FSMC_A18 - PD13
FSMC_NWE - PD5
FSMC_NOE - PD4
FSMC_NE1 - PD7
LCD_Hard_Reset - PC6
LCD_BL - PB1

LED:
LED1 - PA6
LED2 - PA7

Touch_SPI:
IRQ_Touch - PC5
CS_Touch - PB12
SPI_SCK - PB13
SPI_MISO - PB14
SPI_MOSI - PB15

GPS_UART:
TX - PA2
RX - PA3

Vbat:
ADC_IN - PA0

MicroSD:
SDIO_D0 - PC8
SDIO_D1 - PC9
SDIO_D2 - PC10
SDIO_D3 - PC11
SDIO_CK - PC12
SDIO_CMD - PD2

Button:
Key0 - PE2
Key1 - PE3
