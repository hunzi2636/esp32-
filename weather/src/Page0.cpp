#include <Adafruit_Sensor.h>
#include "Page0.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "OLED_font.h"

extern Adafruit_SSD1306 display;
extern char firstDay_uvIndex[4], firstDay_humidity[4];

extern float h, t;
Page0::Page0()
{
}

void Page0::PMainShow(int xOffset)
{
    // 串口打印温湿度信息
    // Serial.printf("湿度: %.1f%% 温度: %.1f°C\n", h, t);  // 格式化输出湿度和温度
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.drawBitmap(0 - xOffset, 0, shijBitmap, 16, 16,WHITE);
    display.drawBitmap(16 - xOffset, 0, jiBitmap, 16, 16,WHITE);
    display.drawBitmap(32 - xOffset, 0, wenBitmap, 16, 16,WHITE);
    display.drawBitmap(48 - xOffset, 0, duBitmap, 16, 16,WHITE);
    display.setCursor(64 - xOffset, 1);
    display.print(":");
    display.print(String(t).substring(0, 4));

    display.drawBitmap(0 - xOffset, 16, shijBitmap, 16, 16,WHITE);
    display.drawBitmap(16 - xOffset, 16, jiBitmap, 16, 16,WHITE);
    display.drawBitmap(32 - xOffset, 16, shiqqBitmap, 16, 16,WHITE);
    display.drawBitmap(48 - xOffset, 16, duBitmap, 16, 16,WHITE);
    display.setCursor(64 - xOffset, 17);
    display.print(":");
    display.print(String(h).substring(0, 2));
    display.print("%");


    display.drawBitmap(0 - xOffset, 32, yu_siBitmap, 16, 16,WHITE);
    display.drawBitmap(16 - xOffset, 32, baoBitmap, 16, 16,WHITE);
    display.drawBitmap(32 - xOffset, 32, shiqqBitmap, 16, 16,WHITE);
    display.drawBitmap(48 - xOffset, 32, duBitmap, 16, 16,WHITE);
    display.setCursor(64 - xOffset, 33);
    display.print(":");
    display.print(String(firstDay_humidity));
    display.print("%");

    display.drawBitmap(0 - xOffset, 48, ziBitmap, 16, 16,WHITE);
    display.drawBitmap(16 - xOffset, 48, waiBitmap, 16, 16,WHITE);
    display.drawBitmap(32 - xOffset, 48, xian_siBitmap, 16, 16,WHITE);
    display.setCursor(48 - xOffset, 49);
    display.print(":");
    display.print(String(firstDay_uvIndex));
    // display.display();
    // display.clearDisplay();
}
