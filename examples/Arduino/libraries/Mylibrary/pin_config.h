#pragma once

#define XPOWERS_CHIP_AXP2101

#if defined(WS_LCD_DC) && defined(WS_LCD_CS) && defined(WS_LCD_SCL) && defined(WS_LCD_SDA) && defined(WS_LCD_RST) && defined(WS_LCD_BL)
#define LCD_DC WS_LCD_DC
#define LCD_CS WS_LCD_CS
#define LCD_SCK WS_LCD_SCL
#define LCD_MOSI WS_LCD_SDA
#define LCD_RST WS_LCD_RST
#define LCD_BL WS_LCD_BL
#define LCD_WIDTH 240
#define LCD_HEIGHT 280
#else
#define LCD_DC 4
#define LCD_CS 5
#define LCD_SCK 6
#define LCD_MOSI 7
#define LCD_RST 8
#define LCD_BL 15
#define LCD_WIDTH 240
#define LCD_HEIGHT 280
#endif

#if defined(WS_TP_SDA) && defined(WS_TP_SCL) && defined(WS_TP_RST) && defined(WS_TP_INT)
#define IIC_SDA WS_TP_SDA
#define IIC_SCL WS_TP_SCL
#define TP_RST WS_TP_RST
#define TP_INT WS_TP_INT
#else
#define IIC_SDA 11
#define IIC_SCL 10

#define TP_RST 13
#define TP_INT 14
#endif
