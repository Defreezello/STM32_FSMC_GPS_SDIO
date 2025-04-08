// gps.c
#include "map.h"



#define BUFFER_SIZE     512    // Sector Size SD card
#define IMAGE_SECTORS   300    // 240*320*2=153600/512=300 sectors;  115200 byte / 512 byte = 225 sectors for 240x240 RGB565
#define min(a,b) ((a < b) ? a : b)

#define FLASH_BASE_PICTURE      	((uint32_t)0x08005A70)

FATFS fs;
FIL file;
DWORD firstCluster;
DWORD firstSector;

void  PrintBigMapFirst(void)
{

		HAL_SD_CardInfoTypeDef cardInfo;
		FRESULT fr;

		HAL_SD_GetCardInfo(&hsd, &cardInfo);
		if (cardInfo.BlockSize != 512)
			Error_Handler();

	    // Mount File System
	    fr = f_mount(&fs, "", 1);
	    if(fr != FR_OK)
	        Error_Handler();

	    // Open File
	    fr = f_open(&file, "small.hex", FA_READ);
	    if(fr != FR_OK)
	        Error_Handler();


	    // Get first claster of file
	    firstCluster = file.obj.sclust;
	    // Math claster to sector
	    firstSector = ((firstCluster - 2) * fs.csize) + fs.database;
	    PrintBigMap();
}

void  PrintBigMap(void)
{
	    // Set LCD
	    lcdSetWindow(0, 0, 319, 239);

	    // Reed and send sectors
	    if(HAL_SD_ReadBlocks_DMA(&hsd, (uint8_t*)LCD_BASE1, firstSector, IMAGE_SECTORS) != HAL_OK) {
            Error_Handler();
        }

	        // Wait complete of transfer
	        while(HAL_SD_GetState(&hsd) != HAL_SD_STATE_READY) {}

	    // Clearing
	    f_close(&file);
	    f_mount(NULL, "", 0);
}

void PrintCoordinatesPoint(float lat, float lon)
{
	uint16_t x, y;

    // Position and map coordinates check
    if (lat < MAP_LAT_MIN || lat > MAP_LAT_MAX ||
        lon < MAP_LON_MIN || lon > MAP_LON_MAX) {
        x = 0;
        y = 0;
        return;
    }

    // Coordinates to pixel transformation
    x = (uint16_t)((lon - MAP_LON_MIN) * LCD_WIDTH / (MAP_LON_MAX - MAP_LON_MIN));
    y = (uint16_t)(LCD_HEIGHT - ((lat - MAP_LAT_MIN) * LCD_HEIGHT / (MAP_LAT_MAX - MAP_LAT_MIN)));

    //lcdFillRGB(COLOR_WHITE);
    lcdDrawPosition(x, y, COLOR_RED); // Pixel print if Touch pressed


}
