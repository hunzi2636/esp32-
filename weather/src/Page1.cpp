//
// Created by JIAO REN TIAN on 24-9-27.
//
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "OLED_font.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "HeFeng.h"
#include <Digital_7_V720pt7b.h>
#include <esp32-hal-timer.h>
#include "Page1.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     4
HeFeng heFeng;
extern Adafruit_SSD1306 display;
char Time[20]; // 为时间字符串分配足够的内存
char firstDay_humidity[4] = "--", firstDay_uvIndex[4] = "--", firstDay_textDay[32] = "--", firstDay_tempMax[8] = "--",
     firstDay_tempMin[8] = "--", firstDay_sunrise[10] = "--", firstDay_sunset[10] = "--",
     firstDay_moonPhase[32] = "--", firstDay_textNight[32] = "--";
char secondDay_textDay[32] = "--", secondDay_tempMax[8] = "--", secondDay_tempMin[8] = "--", secondDay_sunrise[10] =
         "--", secondDay_sunset[10] = "--",
     secondDay_moonPhase[32] = "--", secondDay_textNight[32] = "--";
char thirdDay_textDay[32] = "--", thirdDay_tempMax[8] = "--", thirdDay_tempMin[8] = "--", thirdDay_sunrise[10] = "--",
     thirdDay_sunset[10] = "--",
     thirdDay_moonPhase[32] = "--", thirdDay_textNight[32] = "--";
String Day_txt = "--";
bool isData = false;
hw_timer_t* timer = NULL; // 定时器句柄
volatile bool showBitmap = true; // 需要使用volatile以便在中断中使用
extern MyTim myTim;
uint8_t currentData = 0;
int httpcode1 = 0;
int httpcode2 = 0;
//通过日期换算周几
int calculateDayOfWeek(int year, int month, int day)
{
    if (month < 3)
    {
        month += 12;
        year--;
    }
    int k = year % 100;
    int j = year / 100;
    int h = (day + (13 * (month + 1)) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;
    return h;
}

// int p = 21600;
int p = 0;
extern bool timeQuest;
extern bool isConnectedDevice;

void IRAM_ATTR onTimer()
{
    heFeng.time(&myTim);
    p++;
    showBitmap = !showBitmap; // 切换显示状态
}

Page1::Page1()
{
    // 初始化定时器，设置1秒的定时中断
    timer = timerBegin(0, 80, true); // 0号定时器，80分频（80 MHz -> 1 MHz），递增计数模式
    timerAttachInterrupt(timer, &onTimer, true); // 绑定中断处理函数
    timerAlarmWrite(timer, 1000000, true); // 1秒 = 1000000 微秒
    timerAlarmEnable(timer); // 启用定时器
    myTim.year = 2024;
    myTim.month = 6;
    myTim.day = 11;
    myTim.hour = 5;
    myTim.minute = 20;
    myTim.sec = 1;
}


void Page1::PMainShow(int xOffset)
{
    /* 避免时间过了一天却不更新天气的问题,过了一天后下面还会请求天气及时更新,会覆盖这个，这个就是防止下面获取天气失败的备用措施*/
    if (currentData != myTim.day)
    {
        currentData = myTim.day;
        isData = true;
        strcpy(firstDay_humidity, "--");
        strcpy(firstDay_uvIndex, "--");
        strcpy(firstDay_textDay, secondDay_textDay);
        strcpy(firstDay_tempMax, secondDay_tempMax);
        strcpy(firstDay_tempMin, secondDay_tempMin);
        strcpy(firstDay_sunrise, secondDay_sunrise);
        strcpy(firstDay_sunset, secondDay_sunset);
        strcpy(firstDay_moonPhase, secondDay_moonPhase);
        strcpy(firstDay_textNight, secondDay_textNight);

        strcpy(secondDay_textDay, thirdDay_textDay);
        strcpy(secondDay_tempMax, thirdDay_tempMax);
        strcpy(secondDay_tempMin, thirdDay_tempMin);
        strcpy(secondDay_sunrise, thirdDay_sunrise);
        strcpy(secondDay_sunset, thirdDay_sunset);
        strcpy(secondDay_moonPhase, thirdDay_moonPhase);
        strcpy(secondDay_textNight, thirdDay_textNight);

        strcpy(thirdDay_textDay, "--");
        strcpy(thirdDay_tempMax, "--");
        strcpy(thirdDay_tempMin, "--");
        strcpy(thirdDay_sunrise, "--");
        strcpy(thirdDay_sunset, "--");
        strcpy(thirdDay_moonPhase, "--");
        strcpy(thirdDay_textNight, "--");
    }
    if (isConnectedDevice)
    {
        display.drawBitmap(106 - xOffset, 0, deviceBitmap, 7, 8,WHITE);
    }
    else
    {
        display.fillRect(106 - xOffset, 0, 7, 8, SSD1306_BLACK);
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        display.drawBitmap(118 - xOffset, 1, signBitmap, 10, 6,WHITE);
        if ((p >= 21600) || timeQuest || isData)
        {
            p = 0;
            timeQuest = false;
            isData = false;
            httpcode1 = heFeng.GetHefeng(firstDay_humidity, firstDay_uvIndex, firstDay_textDay, firstDay_tempMax,
                                         firstDay_tempMin, firstDay_sunrise,
                                         firstDay_sunset, firstDay_moonPhase, firstDay_textNight,
                                         secondDay_textDay, secondDay_tempMax, secondDay_tempMin, secondDay_sunrise,
                                         secondDay_sunset, secondDay_moonPhase, secondDay_textNight,
                                         thirdDay_textDay, thirdDay_tempMax, thirdDay_tempMin, thirdDay_sunrise,
                                         thirdDay_sunset, thirdDay_moonPhase, thirdDay_textNight);
            httpcode2 = heFeng.GetTime(Time);
            Serial.print(httpcode1);
            Serial.print(httpcode2);
            // 将 char 数组转换为 String
            if (httpcode1 == 200)
            {
                Day_txt = String(firstDay_textDay);
            }
            if (httpcode2 == 200)
            {
                String Timed = String(Time);
                myTim.hour = Timed.substring(11, 13).toInt(); // 从第11个字符开始，取出时间
                myTim.minute = Timed.substring(14, 16).toInt();
                myTim.sec = Timed.substring(17, 19).toInt();
                myTim.year = Timed.substring(0, 4).toInt();
                myTim.month = Timed.substring(5, 7).toInt();
                myTim.day = Timed.substring(8, 10).toInt();
            }
            // Serial.printf("%04d/%02d/%02d %02d:%02d:%02d\n", myTim.year, myTim.month, myTim.day, myTim.hour,
            //               myTim.minute,
            //               myTim.sec);
        }
    }
    else
    {
        display.fillRect(118 - xOffset, 1, 10, 6, SSD1306_BLACK);
    }


    if (showBitmap)
    {
        display.drawBitmap(64 - xOffset, 22, TimeBitmap, 4, 20, SSD1306_WHITE);
    }
    else
    {
        display.fillRect(64 - xOffset, 22, 4, 20, SSD1306_BLACK);
    }
    display.setTextColor(WHITE); // 设置白色文字
    display.setTextWrap(false); // 不自动换行
    display.setFont(&Digital_7_V720pt7b);
    display.setTextSize(1);
    display.setCursor(16 - xOffset, 45); // 设置时间的光标位置
    display.printf("%02d", myTim.hour);
    display.setCursor(80 - xOffset, 45); // 设置时间的光标位置
    display.printf("%02d", myTim.minute);
    // // 设置日期字体大小并获取文本宽度和高度
    display.setFont();
    display.setCursor(34 - xOffset, 1);
    display.printf("%04d-%02d-%02d", myTim.year, myTim.month, myTim.day); // 打印日期
    // 显示天气信息下方
    if (Day_txt == "小雨")
    {
        display.drawBitmap(48 - xOffset, 48, xiaoBitmap, 16, 16,WHITE);
        display.drawBitmap(64 - xOffset, 48, yuBitmap, 16, 16,WHITE);
    }
    else if (Day_txt == "中雨")
    {
        display.drawBitmap(48 - xOffset, 48, zhongBitmap, 16, 16,WHITE);
        display.drawBitmap(64 - xOffset, 48, yuBitmap, 16, 16,WHITE);
    }
    else if (Day_txt == "大雨")
    {
        display.drawBitmap(48 - xOffset, 48, daBitmap, 16, 16,WHITE);
        display.drawBitmap(64 - xOffset, 48, yuBitmap, 16, 16,WHITE);
    }
    else if (Day_txt == "晴")
    {
        display.drawBitmap(56 - xOffset, 48, sunBitmap, 16, 16,WHITE);
    }
    else if (Day_txt == "多云")
    {
        display.drawBitmap(48 - xOffset, 48, duoBitmap, 16, 16,WHITE);
        display.drawBitmap(64 - xOffset, 48, yunBitmap, 16, 16,WHITE);
    }
    else
    {
        display.setCursor(60 - xOffset, 56);
        display.print(Day_txt);
    }
    display.setCursor(0 - xOffset, 56);
    display.print(firstDay_tempMin);
    display.setCursor(106 - xOffset, 56);
    display.print(firstDay_tempMax);
    display.drawBitmap(13 - xOffset, 56, tempBitmap, 10, 8,WHITE);
    display.drawBitmap(118 - xOffset, 56, tempBitmap, 10, 8,WHITE);
    //获取星期几并显示
    int dayOfWeek = calculateDayOfWeek(myTim.year, myTim.month, myTim.day);

    switch (dayOfWeek)
    {
    case 0: display.drawBitmap(0 - xOffset, 0, liuBitmap, 16, 16,WHITE);
        break;
    case 1: display.drawBitmap(0 - xOffset, 0, riBitmap, 16, 16,WHITE);
        break;
    case 2: display.drawBitmap(0 - xOffset, 0, yiBitmap, 16, 16,WHITE);
        break;
    case 3: display.drawBitmap(0 - xOffset, 0, erBitmap, 16, 16,WHITE);
        break;
    case 4: display.drawBitmap(0 - xOffset, 0, sanBitmap, 16, 16,WHITE);
        break;
    case 5: display.drawBitmap(0 - xOffset, 0, siBitmap, 16, 16,WHITE);
        break;
    case 6: display.drawBitmap(0 - xOffset, 0, wuBitmap, 16, 16,WHITE);
        break;
    default: display.setCursor(0 - xOffset, 0);
        display.print("err");
        break;
    }
    // Serial.printf("%04d/%02d/%02d %02d:%02d:%02d\n", myTim.year, myTim.month, myTim.day, myTim.hour, myTim.minute,
    //               myTim.sec);
    // 显示到屏幕上
    // display.display();
    // display.clearDisplay();
}
