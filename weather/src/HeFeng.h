//
// Created by JIAO REN TIAN on 24-9-27.
//

#ifndef HEFENG_H
#define HEFENG_H

struct MyTim
{
    int year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t sec;
};

class HeFeng
{
public:
    HeFeng();
    int GetTime(char* time);
    int GetHefeng(char *firstDay_humidity,char * firstDay_uvIndex,char* firstDay_textDay, char* firstDay_tempMax, char* firstDay_tempMin,
                       char* firstDay_sunrise, char* firstDay_sunset, char* firstDay_moonPhase,char* firstDay_textNight,
                       char* secondDay_textDay, char* secondDay_tempMax, char* secondDay_tempMin,
                       char* secondDay_sunrise, char* secondDay_sunset, char* secondDay_moonPhase, char* secondDay_textNight,
                       char* thirdDay_textDay, char* thirdDay_tempMax, char* thirdDay_tempMin,
                       char* thirdDay_sunrise, char* thirdDay_sunset, char* thirdDay_moonPhase ,char* thirdDay_textNight);

    MyTim* time(MyTim* my_tim);
};


#endif //HEFENG_H
