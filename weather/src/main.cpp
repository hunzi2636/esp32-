#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "OLED_font.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <complex.h>
#include <Page0.h>
#include "Page1.h"
#include <Page2.h>
#include <Page3.h>
#include <Page4.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include "DHT.h"   // 包含DHT库


//屏幕配置
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     4
#define OLED_SCL       17
#define OLED_SDA       5
uint8_t DHTPIN = 22; // 定义DHT11数据引脚
const int buttonPin = 23; // 按键连接的引脚
const int outputPin = 2; // 输出高低电平的引脚
#define DHTTYPE DHT11   // 定义传感器类型为DHT11
DHT dht(DHTPIN, DHTTYPE); // 创建DHT传感器对象

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Page0 page0;
static Page1 page1;
Page2 page2;
Page3 page3;
Page4 page4;
const char* ssid = "q";//你想要esp32的wifi名字
const char*password = "";//你想要的esp32的wifi密码
extern int httpcode1;
extern int httpcode2;
float h, t;
WebServer server(80);
bool timeQuest = false;
volatile uint8_t page = 1; //1是时钟页面
bool isConnectedDevice = false;
volatile bool isAuto = false; // 轮播是否开启
String time0 = String(5);
String time1 = String(5);
String time2 = String(5);
String time3 = String(5);
String time4 = String(5);

void DHTGet(void* pvParameters)
{
    while (true)
    {
        // 读取湿度和温度值
        h = dht.readHumidity(); // 读取湿度
        t = dht.readTemperature(); // 读取温度
        // 检查读取是否成功
        if (isnan(h) || isnan(t))
        {
            Serial.println("读取DHT11失败！请检查连接"); // 如果读取失败，在串口监视器打印失败信息
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// Web服务器任务
void webServerTask(void* pvParameters)
{
    // 设置HTTP服务器路由
    server.on("/", HTTP_GET, []()
    {
        String html = "<h1 style='text-align:center; font-size:50px'>设备管理</h1>";

        // 读取保存的WiFi配置
        String savedSSID;
        String savedPassword;
        File file = SPIFFS.open("/config.txt", "r");
        if (file)
        {
            savedSSID = file.readStringUntil('\n'); // 读取SSID
            savedPassword = file.readStringUntil('\n'); // 读取密码
            file.close();
            html += "<p style='font-size:50px; display:block; margin:auto;text-align:center;'>当前保存的WiFi配置:</p>";
            html += "<p style='font-size:50px; display:block; margin:auto;text-align:center;'>WiFi名称: " + savedSSID +
                "</p>";
            html += "<p style='font-size:50px; display:block; margin:auto;text-align:center;'>密码:" + savedPassword +
                "</p>";
        }
        else
        {
            html += "<p>未找到保存的WiFi配置。</p>";
        }

        html +=
            "<button style='font-size:50px; display:block; margin:auto;' onclick=\"location.href='/add'\">添加WiFi</button><br>";
        html +=
            "<button style='font-size:50px; display:block; margin:auto;' onclick=\"location.href='/delete'\">删除WiFi</button><br>";
        html +=
            "<button style='font-size:50px; display:block; margin:auto;' onclick=\"location.href='/update_time'\">更新时间天气</button><br>";
        html += "<div style='text-align:center; font-size:50px'>当前温度: <span id='temp'>-</span>°C</div>";
        html += "<div style='text-align:center; font-size:50px'>当前湿度: <span id='humidity'>-</span>%</div>";

        // 读取单片机的 isAuto 变量
        html += "<p style='font-size:50px; text-align:center;'>轮播状态: " + String(isAuto ? "启用" : "禁用") + "</p>";

        // 轮播设置表单
        html += "<form action='/submit_settings' method='POST' style='text-align:center;'>";
        html += "<label style='font-size:50px;'>是否轮播:</label>";
        html +=
            " <input type='checkbox' id='carousel' name='carousel' style='transform: scale(2); display:inline-block; vertical-align:middle;' onchange='toggleTimeFields()'><br><br>";

        // 添加四个输入框，用于轮播时间设置，默认隐藏
        html += "<div id='timeFields' style='display:none;'>";
        html += "<label style='font-size:40px;'>页面0停留时间 (秒):</label>";
        html += "<input type='number' name='time0' value='5' style='width:100px; font-size:40px;'><br>";

        html += "<label style='font-size:40px;'>页面1停留时间 (秒):</label>";
        html += "<input type='number' name='time1' value='5' style='width:100px; font-size:40px;'><br>";
        html += "<label style='font-size:40px;'>页面2停留时间 (秒):</label>";
        html += "<input type='number' name='time2' value='5' style='width:100px; font-size:40px;'><br>";
        html += "<label style='font-size:40px;'>页面3停留时间 (秒):</label>";
        html += "<input type='number' name='time3' value='5' style='width:100px; font-size:40px;'><br>";
        html += "<label style='font-size:40px;'>页面4停留时间 (秒):</label>";
        html += "<input type='number' name='time4' value='5' style='width:100px; font-size:40px;'><br><br>";
        html += "</div>";

        html += "<input type='submit' value='更新设置' style='font-size:50px;'>";
        html += "</form>";


        html += "<script>";
        html += "function toggleTimeFields() {";
        html += "  var checkbox = document.getElementById('carousel');";
        html += "  var timeFields = document.getElementById('timeFields');";
        html += "  if (checkbox.checked) {";
        html += "    timeFields.style.display = 'block';";
        html += "  } else {";
        html += "    timeFields.style.display = 'none';";
        html += "  }";
        html += "}";

        // 获取 isAuto 状态并更新复选框
        html += "function updateisAuto() {";
        html += "  fetch('/get_isAuto').then(response => response.json()).then(data => {";
        html += "    var checkbox = document.getElementById('carousel');";
        html += "    checkbox.checked = data.isAuto;";
        html += "    document.getElementsByName('time0')[0].value = data.time0;";
        html += "    document.getElementsByName('time1')[0].value = data.time1;";
        html += "    document.getElementsByName('time2')[0].value = data.time2;";
        html += "    document.getElementsByName('time3')[0].value = data.time3;";
        html += "    document.getElementsByName('time4')[0].value = data.time4;";
        html += "    toggleTimeFields();"; // 根据最新的 isAuto 状态显示或隐藏时间输入框
        html += "  });";
        html += "}";
        html += "updateisAuto();";
        // html += "setInterval(updateisAuto, 1000);";

        html += "function updateSensorData() {";
        html += "  fetch('/sensor_data').then(response => response.json()).then(data => {";
        html += "    document.getElementById('temp').innerText = data.temp;";
        html += "    document.getElementById('humidity').innerText = data.humidity;";
        html += "  });";
        html += "}";
        html += "setInterval(updateSensorData, 1000);"; // 每1秒更新一次温湿度
        html += "</script>";
        server.send(200, "text/html; charset=utf-8", html);
    });
    // 路由: 返回 isAuto 变量的状态
    server.on("/get_isAuto", HTTP_GET, []()
    {
        String json = "{\"isAuto\": " + String(isAuto ? "true" : "false") +
            ",\"time0\":\"" + time0 + "\"" +
            ",\"time1\":\"" + time1 + "\"" +
            ",\"time2\":\"" + time2 + "\"" +
            ",\"time3\":\"" + time3 + "\"" +
            ",\"time4\":\"" + time4 + "\"}";

        server.send(200, "application/json", json); // 返回 JSON 数据
    });

    server.on("/sensor_data", HTTP_GET, []()
    {
        String jsonData = "{\"temp\":\"" + String(t) + "\",\"humidity\":\"" + String(h) + "\"}";
        server.send(200, "application/json", jsonData);
    });

    server.on("/add", HTTP_GET, []()
    {
        String html = "<h1 style='text-align:center; font-size:50px'>设置WiFi</h1>";
        html += "<form action='/submit' method='POST' style='text-align:center;'>";
        html += "WiFi名称: <input type='text' name='ssid' style='font-size:50px;'><br>";
        html += "密码: <input type='text' name='password' style='font-size:50px;'><br>";
        html += "<input type='submit' value='提交' style='font-size:50px;'>";
        html += "</form>";
        server.send(200, "text/html; charset=utf-8", html);
    });
    server.on("/submit_settings", HTTP_POST, []()
    {
        // 读取表单数据
        isAuto = server.hasArg("carousel");
        time0 = server.arg("time0");
        time1 = server.arg("time1");
        time2 = server.arg("time2");
        time3 = server.arg("time3");
        time4 = server.arg("time4");
        // isAuto = isCarouselEnabled;
        // 将设置发送到串口（单片机）
        // Serial.println(isAuto ? "轮播已启用" : "轮播未启用");
        // Serial.println("页面1停留时间: " + time1 + "秒");
        // Serial.println("页面2停留时间: " + time2 + "秒");
        // Serial.println("页面3停留时间: " + time3 + "秒");
        // Serial.println("页面4停留时间: " + time4 + "秒");

        // 返回确认消息
        server.send(200, "text/html; charset=utf-8",
                    "<p style='text-align:center; font-size:50px'>轮播设置已更新</p> "
                    "<meta http-equiv='refresh' content='2; URL=/' />");
    });

    server.on("/submit", HTTP_POST, []()
    {
        String ssid = server.arg("ssid");
        String password = server.arg("password");

        // 保存SSID和密码到SPIFFS
        File file = SPIFFS.open("/config.txt", "w");
        if (file)
        {
            file.println(ssid);
            file.println(password);
            file.close();
        }

        // 串口输出SSID和密码
        Serial.println("SSID: " + ssid);
        Serial.println("Password: " + password);

        server.send(200, "text/html; charset=utf-8",
                    "<p style='text-align:center; font-size:50px'>已添加" + ssid + "到配置</p> "
                    "<meta http-equiv='refresh' content='2; URL=/' />");
        WiFi.begin(ssid.c_str(), password.c_str());
    });

    server.on("/delete", HTTP_GET, []()
    {
        // 删除WiFi配置逻辑
        SPIFFS.remove("/config.txt");
        server.send(200, "text/html; charset=utf-8",
                    "<p style='text-align:center; font-size:50px'>WiFi配置已删除</p> "
                    "<meta http-equiv='refresh' content='2; URL=/' />");
    });

    server.on("/update_time", HTTP_GET, []()
    {
        // 发送更新时间信号
        timeQuest = true;
        Serial.println("收到更新时间请求"); // 可以在这里处理更新时间逻辑
        server.send(200, "text/html; charset=utf-8",
                    "<p style='text-align:center; font-size:50px'>更新请求已发送"
                    "</br>天气返回状态：" + String(httpcode1) +
                    "</br>时间请求返回状态：" + String(httpcode2) +
                    "</p>" "<meta http-equiv='refresh' content='2; URL=/' />");
    });

    server.begin();
    Serial.println("HTTP server started");

    // 循环处理客户端请求
    while (true)
    {
        server.handleClient();
        vTaskDelay(pdMS_TO_TICKS(1)); // 让出 CPU 给其他任务
        // 获取连接到热点的设备数量
        if (WiFi.softAPgetStationNum())
        {
            isConnectedDevice = true;
        }
        else
        {
            isConnectedDevice = false;
        }
    }
}

void buttonTask(void* pvParameters)
{
    while (true)
    {
        // Serial.print("剩余栈空间: ");
        // Serial.println(uxTaskGetStackHighWaterMark(NULL)); // 打印剩余的栈空间
        if (digitalRead(buttonPin) == HIGH)
        {
            vTaskDelay(pdMS_TO_TICKS(20));
            while (digitalRead(buttonPin) == HIGH)
            {
            }
            vTaskDelay(pdMS_TO_TICKS(20));
            digitalWrite(outputPin, !digitalRead(outputPin));
            page++;
        }

        if (isAuto)
        {
            switch (page)
            {
            case 0:
                vTaskDelay(time0.toInt() * 1000);
                page++;
                break;
            case 1:
                vTaskDelay(time1.toInt() * 1000);
                page++;
                break;
            case 2:
                vTaskDelay(time2.toInt() * 1000);
                page++;
                break;
            case 3:
                vTaskDelay(time3.toInt() * 1000);
                page++;
                break;
            case 4:
                vTaskDelay(time4.toInt() * 1000);
                page++;
                break;
            default:
                break;
            }
        }

        if (page >= 5)
        {
            page = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void setup()
{
    // 进入热点模式
    WiFi.softAP(ssid, password);
    dht.begin(); // 初始化DHT11传感器
    // 初始化SPIFFS
    if (!SPIFFS.begin())
    {
        Serial.println("SPIFFS-An error occurred while mounting SPIFFS");
        // 格式化SPIFFS分区
        if (SPIFFS.format())
        {
            Serial.println("SPIFFS partition formatted successfully");
            ESP.restart(); // 重启ESP32
        }
        else
        {
            Serial.println("SPIFFS partition format failed");
        }
        return;
    }

    pinMode(buttonPin,INPUT_PULLDOWN);
    pinMode(outputPin,OUTPUT);
    Wire.begin(OLED_SDA, OLED_SCL); // sda, scl
    Serial.begin(115200);
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        return;
    }

    // 旋转屏幕180度
    display.setRotation(2); // 0 为默认，1 为 90 度，2 为 180 度，3 为 270 度
    // 读取保存的WiFi配置
    File file = SPIFFS.open("/config.txt", "r");
    if (file)
    {
        String savedSSID = file.readStringUntil('\n'); // 读取SSID
        String savedPassword = file.readStringUntil('\n'); // 读取密码
        file.close();
        // 去除空格和换行符
        savedSSID.trim();
        savedPassword.trim();
        // 尝试连接保存的WiFi
        display.setTextColor(WHITE);
        display.setTextSize(1);
        if (savedSSID.isEmpty())
        {
            display.clearDisplay();
            display.println("please connect the wifi was called:" + String(ssid) + ",password:" + password);
            display.display();
            display.clearDisplay();
        }
        else
        {
            display.clearDisplay();
            display.println("try to connect " + savedSSID + " " + savedPassword);
            // Serial.println("尝试连接到保存的WiFi..." + savedSSID + " " + savedPassword);
            WiFi.begin(savedSSID.c_str(), savedPassword.c_str());
            display.display();
        }

        // 等待连接
        unsigned long startAttemptTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000)
        {
            delay(1000); // 等待10秒以内
            Serial.print(".");
        }
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("连接成功");
            //wifi成功连接，展示wifi图标
            display.clearDisplay();
            display.drawBitmap(118, 0, signBitmap, 10, 6,WHITE);
            display.display();
            delay(100);
        }
        else
        {
            Serial.println("无法连接到WiFi，使用热点模式");
        }
    }
    else
    {
        Serial.println("未找到保存的WiFi配置，使用热点模式");
        display.clearDisplay();
        display.println("please connect the wifi was called:" + String(ssid) + ",password:" + password);
        display.display();
        display.clearDisplay();
    }
    xTaskCreate(webServerTask,
                "WebServerTask",
                7168,
                NULL,
                1,
                NULL);
    xTaskCreate(buttonTask
                , "ButtonTask" //任务名
                , 2048 // 栈大小
                , NULL
                , 1 // 任务优先级
                , NULL);
    xTaskCreate(DHTGet,
                "DHTGet",
                2048,
                NULL,
                1,
                NULL);
}

void ShowPage(int pageIndex, int xOffset)
{
    switch (pageIndex)
    {
    case 0:
        page0.PMainShow(xOffset);
        break;
    case 1:
        page1.PMainShow(xOffset);
        break;
    case 2:
        page2.PMainShow(xOffset);
        break;
    case 3:
        page3.PMainShow(xOffset);
        break;
    case 4:
        page4.PMainShow(xOffset);
        break;
    default:
        break;
    }
}

/*
 * @param right为ture时,->滑动
 *        right为false时,<-滑动
 */
void Slide(int fromPage, int toPage)
{
    // bool right = fromPage - toPage > 0;
    bool right = false;
    int start = right ? SCREEN_WIDTH : 0;
    int end = right ? 0 : SCREEN_WIDTH;

    if (right)
    {
        // for (int xOffset = start; xOffset >= end; xOffset -= 16)
        // {
        //     ShowPage(fromPage, xOffset - SCREEN_WIDTH);
        //     ShowPage(toPage, xOffset);
        // }
    }
    else
    {
        for (int xOffset = start; xOffset <= end; xOffset += 16)
        {
            // 显示滑出页面
            ShowPage(fromPage, xOffset);
            // 显示滑入页面
            ShowPage(toPage, xOffset - SCREEN_WIDTH);
            display.display();
            display.clearDisplay();
        }
    }
}

int cruPage = 1;

void loop()
{
    if (page - cruPage > 0)
    {
        Slide(cruPage, page);
    }
    else if (cruPage - page == 4)
    {
        Slide(4, 0);
    }
    cruPage = page;
    switch (cruPage)
    {
    case 0:
        page0.PMainShow(0);
        break;
    case 1:
        page1.PMainShow(0);
        break;
    case 2:
        page2.PMainShow(0);
        break;
    case 3:
        page3.PMainShow(0);
        break;
    case 4:
        page4.PMainShow(0);
        break;
    default:
        break;
    }
    display.display();
    display.clearDisplay();
    // Serial.println(page);
}
