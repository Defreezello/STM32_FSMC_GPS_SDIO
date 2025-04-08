/*
 * map.h
 *
 *  Created on: Feb 13, 2025
 *      Author: Denis
 */

#ifndef INC_MAP_H_
#define INC_MAP_H_

#define MAP_LAT_MIN  51.3645  // Южная граница карты
#define MAP_LAT_MAX  51.4500  // Северная граница карты
#define MAP_LON_MIN  39.0200  // Западная граница карты
#define MAP_LON_MAX  39.2036  // Восточная граница карты

// Размеры изображения карты в пикселях
#define LCD_WIDTH   320  // Ширина карты в пикселях
#define LCD_HEIGHT  240  // Высота карты в пикселях

#include "fatfs.h"
#include "sdio.h"
#include "ili9341.h"

void  PrintBigMap(void);
void  PrintBigMapFirst(void);
void  PrintCoordinatesPoint(float latitude, float longitude);


#endif /* INC_MAP_H_ */
