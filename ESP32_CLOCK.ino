#include <HTTPClient.h>

#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "imagedata.h"
#include <stdlib.h>
#include "EPD.h"

#include "epd_init.h"
#include "wifiConnect.h"

#include "ArduinoJson.h"

// #include <cstring>
#include <string>

DynamicJsonDocument doc_weather(512);
DynamicJsonDocument doc_time(512);

int timestamp = 10;
UBYTE *BlackImage;
UWORD Imagesize = ((EPD_2IN9_WIDTH % 8 == 0)? (EPD_2IN9_WIDTH / 8 ): (EPD_2IN9_WIDTH / 8 + 1)) * EPD_2IN9_HEIGHT;

const char* url_time = "http://quan.suning.com/getSysTime.do";

// add your own seniverse key!
const char* url_weather = "https://api.seniverse.com/v3/weather/now.json?key=XXX&location=wuxi&language=en&unit=c";

// QWeather API, can't resolve, don't know why
const char* url_QW = "https://devapi.qweather.com/v7/weather/now?location=101010100&key=XXX";

bool wifi_status = 0;

void setup() {
  // put your setup code here, to run once:
    // Serial port initialization
    Serial.begin(115200);
    delay(10);

    // Server initialization
    wifi_setup();

    /*
    // supposed to work, not recommended, edit wifiConnect.h first
    // seu-wlan login, add ID & IP address
    const char* url_seuwlan = "http://10.9.10.100:801/eportal/?c=Portal&a=login&callback=dr1003&login_method=1&user_account=%2C0%2CXXXXXXXXX&user_password=123456&wlan_user_ip=10.11.XXX.XXX&wlan_user_ipv6=&wlan_user_mac=000000000000&wlan_ac_ip=10.1.1.1&wlan_ac_name=&jsVersion=3.3.2&v=6886"
    // seu login
    HTTPClient http_seu;
    http_seu.begin(url_seuwlan);
    http_seu.GET();
    */

    printf("EPD_2IN9_test Demo\r\n");

    EPD_initSPI_1();

    //DEV_Module_Init();

    EPD_2IN9_Init(EPD_2IN9_FULL);
    EPD_2IN9_Clear();
    DEV_Delay_ms(500);

    //Create a new image cache
    
    // you have to edit the startup_stm32fxxx.s file and set a big enough heap size 
    
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        while(1);
    }
    printf("Start\r\n");
    Paint_NewImage(BlackImage, EPD_2IN9_WIDTH, EPD_2IN9_HEIGHT, 270, WHITE);

    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);

    Paint_DrawString_CN(180, 0,"你好", &Font24CN, BLACK, WHITE);
    Paint_DrawString_EN(10, 0, "InTereSTingHE", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(10, 20, "WIFI Connected", &Font16, BLACK, WHITE);

    Paint_DrawString_EN(10, 65, "Weather", &Font12, WHITE, BLACK);       
    Paint_DrawString_EN(10, 85, "Temperature", &Font12, WHITE, BLACK);
    Paint_DrawString_EN(140, 80, "C", &Font24, WHITE, BLACK);
    Paint_DrawString_EN(10, 100, "Last Update", &Font12, WHITE, BLACK);
    EPD_2IN9_Display(BlackImage);

}

void loop() {
  // put your main code here, to run repeatedly:


    if ((WiFi.status() == WL_CONNECTED)){
        HTTPClient http_weather;
        HTTPClient http_time;

        http_weather.begin(url_weather);
        http_time.begin(url_time);
        http_time.GET(); 

        int httpCodeWeather = http_weather.GET();  

        if (httpCodeWeather > 0) { //检查返回的代码

                String payload_weather = http_weather.getString();
                String payload_time = http_time.getString();

                deserializeJson(doc_weather, payload_weather);
                String weather = doc_weather["results"][0]["now"]["text"].as<String>();
                String temperature = doc_weather["results"][0]["now"]["temperature"].as<String>();
                String path = doc_weather["results"][0]["location"]["path"].as<String>();
                String last = doc_weather["results"][0]["last_update"].as<String>();

                const char* wchar = weather.c_str();
                const char* tchar = temperature.c_str();
                const char* pchar = path.c_str();
                const char* lchar = last.c_str();

                Serial.println(payload_time);
                deserializeJson(doc_time, payload_time);
                String time1 = doc_time["sysTime1"].as<String>();

                // hour[0] = time1[8];//20201124191911
                Serial.println(time1);
                //Serial.println(hour);

                Serial.println(weather);
                Serial.println(payload_weather);

                // if(time1[11] != timestamp || flag_first){                   
                    
                    EPD_2IN9_Init(EPD_2IN9_PART);
                    Paint_SelectImage(BlackImage);
                    if (wifi_status == false){
                        Paint_Clear(WHITE);
                        Paint_DrawString_CN(180, 0,"你好", &Font24CN, BLACK, WHITE);
                        Paint_DrawString_EN(10, 0, "InTereSTingHE", &Font16, WHITE, BLACK);
                        Paint_DrawString_EN(10, 20, "WIFI Connected", &Font16, BLACK, WHITE);

                        Paint_DrawString_EN(10, 65, "Weather", &Font12, WHITE, BLACK);       
                        Paint_DrawString_EN(10, 85, "Temperature", &Font12, WHITE, BLACK);
                        Paint_DrawString_EN(140, 80, "C", &Font24, WHITE, BLACK);
                        Paint_DrawString_EN(10, 100, "Last Update", &Font12, WHITE, BLACK);
                    }

                    Paint_ClearWindows(10, 40, 10 + Font16.Width * 23, 40 + Font16.Height, WHITE);
                    Paint_DrawString_EN(10, 40, pchar, &Font16, WHITE, BLACK);

                    Paint_ClearWindows(100, 60, 100 + Font24.Width * 10, 60 + Font24.Height, WHITE);
                    Paint_DrawString_EN(100, 60, wchar, &Font24, WHITE, BLACK);

                    Paint_ClearWindows(100, 80, 100 + Font24.Width * 2, 80 + Font24.Height, WHITE);
                    Paint_DrawString_EN(100, 80, tchar, &Font24, WHITE, BLACK);

                    Paint_ClearWindows(10, 115, 10 + Font12.Width * 27, 115 + Font12.Height, WHITE);
                    Paint_DrawString_EN(10, 115, lchar, &Font12, WHITE, BLACK);

                    EPD_2IN9_Display(BlackImage);

                    // timestamp = time1[11];
                // }

                wifi_status = true;

        }else {

                wifi_status = false;
                Paint_SelectImage(BlackImage);
                Paint_Clear(WHITE);

                Paint_DrawString_EN(10, 0, "InTereSTingHE", &Font16, WHITE, BLACK);

                Paint_DrawString_EN(10, 40, "WIFI LOSE !!!", &Font24, BLACK, WHITE);

                EPD_2IN9_Display(BlackImage);
                Serial.println("Error on HTTP request");

        }

        http_weather.end(); //释放资源

        delay(10000);

    }
}
