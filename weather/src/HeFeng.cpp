//
// Created by JIAO REN TIAN on 24-9-27.
//
#include <HTTPClient.h>
#include "HeFeng.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <ArduinoUZlib.h>
HTTPClient http;
uint8_t cnt = 0;
uint8_t sec = 0;
StaticJsonDocument<2048> doc;
StaticJsonDocument<100> tim;
extern char Time[20]; // 为时间字符串分配足够的内存
const char* Timeurl = "https://quan.suning.com/getSysTime.do";
const char* url = "";//你的和风api

HeFeng::HeFeng()
{
}

//苏宁易购获取时间
int HeFeng::GetTime(char* time)
{
    http.begin(Timeurl);
    int httpCode = http.GET();
    if (httpCode == 200)
    {
        String payload = http.getString();
        // 使用StaticJsonDocument解析JSON数据
        DeserializationError error = deserializeJson(tim, payload);
        if (error)
        {
            Serial.print(F("解析JSON失败: "));
            Serial.println(error.c_str());
        }
        // 提取sysTime1字段
        if (tim.containsKey("sysTime2"))
        {
            strcpy(time, tim["sysTime2"]); // 将时间复制到Time数组
            // Serial.print("Time: ");
            // Serial.println(time);
        }
        else
        {
            Serial.println("没有找到 sysTime2 字段");
        }
    }
    else
    {
        Serial.printf("HTTP 请求失败，错误代码: %d\n", httpCode);
        if (httpCode ==403 && cnt < 3)
        {
            cnt++;
            delay(500);
            GetTime(Time);
        }
    }
    http.end();
    cnt = 0;
    return httpCode;
}


//返回true就是闰年
bool isrun(int year)
{
    bool is = false;
    if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))
    {
        is = true;
    }
    return is;
}


MyTim myTim;

MyTim* HeFeng::time(MyTim* my_tim)
{
    my_tim->sec++;
    if (my_tim->sec == 60)
    {
        my_tim->sec = 0;
        my_tim->minute++;
    }
    if (my_tim->minute == 60)
    {
        my_tim->minute = 0;
        my_tim->hour++;
    }
    if (my_tim->hour == 24)
    {
        my_tim->hour = 0;
        my_tim->day++;
    }
    if (isrun(my_tim->year))
    {
        if (my_tim->month == 2)
        {
            if (my_tim->day == 30)
            {
                my_tim->month++;
                my_tim->day = 1;
            }
        }
    }
    else
    {
        if (my_tim->month == 2)
        {
            if (my_tim->day == 29)
            {
                my_tim->month++;
                my_tim->day = 1;
            }
        }
    }
    if (my_tim->month == 1 || my_tim->month == 3 || my_tim->month == 5 || my_tim->month == 7 || my_tim->month == 8 ||
        my_tim->month == 10 || my_tim->month == 12)
    {
        if (my_tim->day == 32)
        {
            my_tim->month++;
            my_tim->day = 1;
        }
    }
    else
    {
        if (my_tim->day == 31)
        {
            my_tim->month++;
            my_tim->day = 1;
        }
    }
    if (my_tim->month == 13)
    {
        my_tim->year++;
        my_tim->month = 1;
    }
    return my_tim;
}

/*
daily.fxDate 预报日期
daily.sunrise 日出时间，在高纬度地区可能为空
daily.sunset 日落时间，在高纬度地区可能为空
daily.moonrise 当天月升时间，可能为空
daily.moonset 当天月落时间，可能为空
daily.moonPhase 月相名称
daily.tempMax 预报当天最高温度
daily.tempMin 预报当天最低温度
daily.textDay 预报白天天气状况文字描述，包括阴晴雨雪等天气状态的描述
daily.iconNight 预报夜间天气状况的图标代码，另请参考天气图标项目
daily.textNight 预报晚间天气状况文字描述，包括阴晴雨雪等天气状态的描述
daily.wind360Day 预报白天风向360角度
daily.windDirDay 预报白天风向
daily.windScaleDay 预报白天风力等级
daily.windSpeedDay 预报白天风速，公里/小时
daily.wind360Night 预报夜间风向360角度
daily.windDirNight 预报夜间当天风向
daily.windScaleNight 预报夜间风力等级
daily.windSpeedNight 预报夜间风速，公里/小时
daily.precip 预报当天总降水量，默认单位：毫米
daily.uvIndex 紫外线强度指数
daily.humidity 相对湿度，百分比数值
daily.pressure 大气压强，默认单位：百帕
daily.vis 能见度，默认单位：公里
daily.cloud 云量，百分比数值。可能为空
*/
//天气http请求函数
int HeFeng::GetHefeng(char *firstDay_humidity,char * firstDay_uvIndex,char* firstDay_textDay, char* firstDay_tempMax, char* firstDay_tempMin,
                      char* firstDay_sunrise, char* firstDay_sunset, char* firstDay_moonPhase,char* firstDay_textNight,
                      char* secondDay_textDay, char* secondDay_tempMax, char* secondDay_tempMin,
                      char* secondDay_sunrise, char* secondDay_sunset, char* secondDay_moonPhase, char* secondDay_textNight,
                      char* thirdDay_textDay, char* thirdDay_tempMax, char* thirdDay_tempMin,
                      char* thirdDay_sunrise, char* thirdDay_sunset, char* thirdDay_moonPhase ,char* thirdDay_textNight)

{
    //请求天气api
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
        // 获取压缩后的数据
        WiFiClient* stream = http.getStreamPtr();
        size_t size = stream->available();
        uint8_t inbuff[size] = {0};
        // Serial.print("size:");
        // Serial.println(size);
        // 将http流数据写入inbuff中
        stream->readBytes(inbuff, size);
        // for (int i = 0; i < size; ++i) {
        //     Serial.print(inbuff[i],HEX);
        //     Serial.print(" ");
        // }
        // Serial.println();
        uint8_t* outbuf = NULL; //解压后的输出流
        uint32_t outsize = 0; // 解压后多大？在调用解压方法后会被赋值。
        // 调用解压函数
        int result = ArduinoUZlib::decompress(inbuff, size, outbuf, outsize);
        if (result == outsize)
        {
            // 解压成功，打印解压后的数据
            // Serial.println("解压后的数据:");
            // Serial.write(outbuf, outsize);
            // Serial.println();
            // 将解压后的数据转换为字符串
            char jsonData[outsize + 1];
            strncpy(jsonData, (char*)outbuf, outsize);
            jsonData[outsize] = '\0'; // 确保字符串以空字符结尾
            // 解析JSON数据
            DeserializationError error = deserializeJson(doc, jsonData);
            if (error)
            {
                Serial.print(F("解析JSON失败: "));
                Serial.println(error.c_str());
                return 1;
            }
            // 提取updateTime字段
            if (doc.containsKey("updateTime"))
            {
                const char* updateTime = doc["updateTime"];
                Serial.print("updateTime: ");
                Serial.println(updateTime);
            }
            else
            {
                Serial.println("没有找到 updateTime 字段");
            }
            // 提取daily数组的第一个元素中的fxDate和sunrise字段
            if (doc.containsKey("daily"))
            {
                JsonArray dailyArray = doc["daily"].as<JsonArray>();
                if (dailyArray.size() > 0)
                {
                    //第一天
                    JsonObject firstDay = dailyArray[0];
                    strcpy(firstDay_textDay, firstDay["textDay"]);
                    strcpy(firstDay_tempMax, firstDay["tempMax"]);
                    strcpy(firstDay_tempMin, firstDay["tempMin"]);
                    strcpy(firstDay_sunrise, firstDay["sunrise"]);
                    strcpy(firstDay_sunset, firstDay["sunset"]);
                    strcpy(firstDay_moonPhase, firstDay["moonPhase"]);
                    strcpy(firstDay_uvIndex, firstDay["uvIndex"]);
                    strcpy(firstDay_humidity, firstDay["humidity"]);
                    strcpy(firstDay_textNight, firstDay["textNight"]);
                    //第二天
                    JsonObject secondDay = dailyArray[1];
                    strcpy(secondDay_textDay, secondDay["textDay"]); //天气
                    strcpy(secondDay_tempMax, secondDay["tempMax"]); //最高温度
                    strcpy(secondDay_tempMin, secondDay["tempMin"]); //最低温度
                    strcpy(secondDay_sunrise, secondDay["sunrise"]);
                    strcpy(secondDay_sunset, secondDay["sunset"]);
                    strcpy(secondDay_moonPhase, secondDay["moonPhase"]);
                    strcpy(secondDay_textNight, secondDay["textNight"]);
                    //第三天
                    JsonObject thirdDay = dailyArray[2];
                    strcpy(thirdDay_textDay, thirdDay["textDay"]); //天气
                    strcpy(thirdDay_tempMax, thirdDay["tempMax"]); //最高温度
                    strcpy(thirdDay_tempMin, thirdDay["tempMin"]); // 最低温度
                    strcpy(thirdDay_sunrise, thirdDay["sunrise"]);
                    strcpy(thirdDay_sunset, thirdDay["sunset"]);
                    strcpy(thirdDay_moonPhase, thirdDay["moonPhase"]);
                    strcpy(thirdDay_textNight, secondDay["textNight"]);
                }
                else
                {
                    Serial.println("daily数组为空");
                }
            }
            else
            {
                Serial.println("没有找到 daily 字段");
            }
        }
        else
        {
            Serial.printf("解压失败，错误代码: %d\n", result);
        }
    }
    else
    {
        Serial.printf("HTTP 请求失败, 错误代码: %d\n", httpCode);
    }
    http.end();
    return httpCode;
}
