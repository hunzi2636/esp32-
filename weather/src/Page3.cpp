//
// Created by JIAO REN TIAN on 24-9-27.
//
#include <Adafruit_SSD1306.h>
#include "Page3.h"
#include <OLED_font.h>
extern char secondDay_textDay[32], secondDay_tempMax[8], secondDay_tempMin[8], secondDay_sunrise[10], secondDay_sunset[
                10],
            secondDay_moonPhase[32], secondDay_textNight[32];
extern Adafruit_SSD1306 display;


Page3::Page3()
{
}


void Page3::PMainShow(int xOffset)
{
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.drawBitmap(112 - xOffset, 0, mingBitmap, 16, 16,WHITE);
    display.drawBitmap(112 - xOffset, 16, tianBitmap, 16, 16,WHITE);
    String textday = String(secondDay_textDay);
    String yuexiang = String(secondDay_moonPhase);
    String text_night = String(secondDay_textNight);
    display.drawBitmap(0 - xOffset, 0, tianBitmap, 16, 16,WHITE);
    display.drawBitmap(16 - xOffset, 0, TqiBitmap, 16, 16,WHITE);
    display.setCursor(32 - xOffset, 1);
    display.print(":");
    // 显示天气信息下方
    if (textday == "小雨")
    {
        display.drawBitmap(48 - xOffset, 0, xiaoBitmap, 16, 16,WHITE);
        display.drawBitmap(64 - xOffset, 0, yuBitmap, 16, 16,WHITE);
    }
    else if (textday == "中雨")
    {
        display.drawBitmap(48 - xOffset, 0, zhongBitmap, 16, 16,WHITE);
        display.drawBitmap(64 - xOffset, 0, yuBitmap, 16, 16,WHITE);
    }
    else if (textday == "大雨")
    {
        display.drawBitmap(48 - xOffset, 0, daBitmap, 16, 16,WHITE);
        display.drawBitmap(64 - xOffset, 0, yuBitmap, 16, 16,WHITE);
    }
    else if (textday == "晴")
    {
        display.drawBitmap(48 - xOffset, 0, sunBitmap, 16, 16,WHITE);
    }
    else if (textday == "多云")
    {
        display.drawBitmap(48 - xOffset, 0, duoBitmap, 16, 16,WHITE);
        display.drawBitmap(64 - xOffset, 0, yunBitmap, 16, 16,WHITE);
    }
    else
    {
        display.setCursor(48 - xOffset, 0);
        display.print("--");
    }

    display.drawBitmap(0 - xOffset, 16, wenBitmap, 16, 16,WHITE);
    display.drawBitmap(16 - xOffset, 16, duBitmap, 16, 16,WHITE);
    display.setCursor(32 - xOffset, 17);
    display.print(":");
    display.print(secondDay_tempMin);
    display.print("~");
    display.print(secondDay_tempMax);

    display.drawBitmap(0 - xOffset, 32, yueBitmap, 16, 16,WHITE);
    display.drawBitmap(16 - xOffset, 32, xiangBitmap, 16, 16,WHITE);
    display.setCursor(32 - xOffset, 33);
    display.print(":");
    if (yuexiang == "新月")
    {
        display.drawBitmap(44 - xOffset, 32, xinBitmap, 16, 16,WHITE);
        display.drawBitmap(60 - xOffset, 32, yueBitmap, 16, 16,WHITE);
    }
    else if (yuexiang == "蛾眉月")
    {
        display.drawBitmap(44 - xOffset, 32, eBitmap, 16, 16,WHITE);
        display.drawBitmap(60 - xOffset, 32, meiBitmap, 16, 16,WHITE);
        display.drawBitmap(76 - xOffset, 32, yueBitmap, 16, 16,WHITE);
    }
    else if (yuexiang == "上弦月")
    {
        display.drawBitmap(44 - xOffset, 32, shangBitmap, 16, 16,WHITE);
        display.drawBitmap(60 - xOffset, 32, xianBitmap, 16, 16,WHITE);
        display.drawBitmap(76 - xOffset, 32, yueBitmap, 16, 16,WHITE);
    }
    else if (yuexiang == "盈凸月")
    {
        display.drawBitmap(44 - xOffset, 32, yingBitmap, 16, 16,WHITE);
        display.drawBitmap(60 - xOffset, 32, tuBitmap, 16, 16,WHITE);
        display.drawBitmap(76 - xOffset, 32, yueBitmap, 16, 16,WHITE);
    }
    else if (yuexiang == "满月")
    {
        display.drawBitmap(44 - xOffset, 32, manBitmap, 16, 16,WHITE);
        display.drawBitmap(60 - xOffset, 32, yueBitmap, 16, 16,WHITE);
    }
    else if (yuexiang == "亏凸月")
    {
        display.drawBitmap(44 - xOffset, 32, kuiBitmap, 16, 16,WHITE);
        display.drawBitmap(60 - xOffset, 32, tuBitmap, 16, 16,WHITE);
        display.drawBitmap(76 - xOffset, 32, yueBitmap, 16, 16,WHITE);
    }
    else if (yuexiang == "下弦月")
    {
        display.drawBitmap(44 - xOffset, 32, xiaBitmap, 16, 16,WHITE);
        display.drawBitmap(60 - xOffset, 32, xianBitmap, 16, 16,WHITE);
        display.drawBitmap(76 - xOffset, 32, yueBitmap, 16, 16,WHITE);
    }
    else if (yuexiang == "残月")
    {
        display.drawBitmap(44 - xOffset, 32, canBitmap, 16, 16,WHITE);
        display.drawBitmap(60 - xOffset, 32, yueBitmap, 16, 16,WHITE);
    }


    display.drawBitmap(0 - xOffset, 48, yeBitmap, 16, 16,WHITE);
    display.drawBitmap(16 - xOffset, 48, wanBitmap, 16, 16,WHITE);
    display.drawBitmap(32 - xOffset, 48, tianBitmap, 16, 16,WHITE);
    display.drawBitmap(48 - xOffset, 48, TqiBitmap, 16, 16,WHITE);
    display.setCursor(64 - xOffset, 49);
    display.print(":");

    // 显示天气信息下方
    if (text_night == "小雨")
    {
        display.drawBitmap(76 - xOffset, 48, xiaoBitmap, 16, 16,WHITE);
        display.drawBitmap(92 - xOffset, 48, yuBitmap, 16, 16,WHITE);
    }
    else if (text_night == "中雨")
    {
        display.drawBitmap(76 - xOffset, 48, zhongBitmap, 16, 16,WHITE);
        display.drawBitmap(92 - xOffset, 48, yuBitmap, 16, 16,WHITE);
    }
    else if (text_night == "大雨")
    {
        display.drawBitmap(76 - xOffset, 48, daBitmap, 16, 16,WHITE);
        display.drawBitmap(92 - xOffset, 48, yuBitmap, 16, 16,WHITE);
    }
    else if (text_night == "晴")
    {
        display.drawBitmap(76 - xOffset, 48, sunBitmap, 16, 16,WHITE);
    }
    else if (text_night == "多云")
    {
        display.drawBitmap(76 - xOffset, 48, duoBitmap, 16, 16,WHITE);
        display.drawBitmap(92 - xOffset, 48, yunBitmap, 16, 16,WHITE);
    }
    else
    {
        display.setCursor(76 - xOffset, 48);
        display.print("--");
    }
    // display.display();
    // display.clearDisplay();
}
