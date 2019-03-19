# AirDog

AirDog is an Air Quality Detector Project.

It can detect carbon dioxide(CO2) , formaldehyde, VOCs/Volatile Organic Compounds or 

TVOC/Total Volatile Organic Compound, PM2.5/PM10/PM0.3, Temperature, humidity in the air.


sensors: 

CO2  :  Sweden SenseAir LTD, S8 0053 CO2 Sensor

Formaldehyde: England DART SENSORS LTD, WZ-S Module

PM2.5/PM10:  China PlantPower LTD, PMS7003, PMS7003M

Temperature & Humidity: Switzerland SENSIRION LTD, SHT20 Sensor

VOCs: China Winsensor LTD, ZM01 IIC BUS VOCs MEMS Sensor


Others:

Photoresistance,  

PCF8563 RTC Clock, 

MCU: STM32F103RCT6(64K RAM, 256K ROM), 

ETA9742 3A Switching Charger

WIFI Module: ESP-8266


Features/Functions:

1    High-precision imported sensors: PM2.5, PM10, CO2, HCHO, VOC, temperature and humidity, etc.

2    The system is configured with multiple sensors, including illumination, vibration, and PCB temperature detection.

3    2.8-inch color TFT display with color screen display.

4    Touch sensing, vibration sensing, light sensing, intelligent adjustment of screen brightness at night.

5    In addition to the 5V USB power supply, it is equipped with a large-capacity imported lithium battery to provide air movement for air quality detection.

6    Built-in SD card, standard configuration 2G SD card, can store 5 years of sensor data, support up to 32GB SD card.

7    can connect to the computer to view sensor data, virtual U disk supports FAT, FAT32, exFAT file system.

8    Support online upgrade firmware, support remote firmware upgrade.

9    equipped with WIFI module, support network control function

10   Optional GPRS module, can be placed outdoors, remote detection in the deep forest.

11   Configure the mobile app, including IOS and Android versions.

12   Supports up to 2A fast charging function to quickly fill the battery.

13   Reserved external interface to connect sensors such as oxygen, carbon monoxide, etc.

14   Scrubed acrylic shell, so addictive ^-^.

15   black gold plated PCB, noble and generous, lead-free solder, components are RoHS compliant.


resources:

1 WIFI: 

my ESP8266 sdk in: https://github.com/SaberOnGo/ESP8266-AIRDOG

ESP8266(ie: ai-thinker ESP-12F, esp8266 with 32Mbit SPI FLASH)

http://wiki.ai-thinker.com/esp8266/docs

ESP8266 SDK and resources on Espressif:

https://www.espressif.com/en/support/download/sdks-demos?keys=&field_type_tid%5B%5D=14







/---------------------------------------------------------------------------/

AirDog 是一个空气质量检测仪项目，可以检测CO2,甲醛，PM2.5/PM10,VOCs，温湿度等，

具有WIFI/GPRS模块，可使用iOS or Android APP 显示/控制。

传感器:

CO2: 瑞典 SenseAir, S8 0053 二氧化碳传感器

甲醛:英国DART WZ-S甲醛模块

PM2.5/PM10: 攀藤PMS7003/PMS7003M

温湿度: 瑞士 SENSIRION SHT20温湿度传感器

VOCs: 炜盛ZM01 IIC 总线 MEMS 超低功耗VOCs传感器


其他:

光敏电阻

PCF8563 RTC 时钟芯片

MCU: STM32F103RCT6

充电升压芯片: ETA9742

WIFI模块: ESP-8266

功能:
1     高精度进口传感器：PM2.5, PM10, CO2, HCHO, VOC, 温湿度等。

2     系统配置多传感器，包括光照，震动，PCB温度检测等。

3     2.8寸彩色TFT显示屏，彩色屏幕显示。

4     触摸感应，震动感应，光照感应，夜晚智能调整屏幕亮度。

5     除5V USB供电外，配置大容量进口锂电池供电，可移动检测空气质量。

6     内置SD卡，标准配置2G SD卡，可存储5年传感器数据，支持高达32GB SD卡.

7     可连接电脑查看传感器数据，虚拟U盘支持FAT,FAT32,exFAT文件系统

8     支持联机升级固件，支持远程升级固件。

9     配备WIFI模块，支持联网控制功能

10    可选配GPRS模块，可放置在室外，深山老林中远程检测。

11    配置手机端APP, 包括IOS 和 Android版本。

12    支持高达2A快速充电功能，可快速充满电池。

13    保留外置接口，可连接其他传感器，如氧气，一氧化碳等。

14    磨砂亚克力外壳，令人爱不释手^-^。
15    黑色镀金版PCB，高贵大方，无铅焊锡，元器件符合RoHS要求。


资源:
1 WIFI: 

笔者编译的SDK: 

https://github.com/SaberOnGo/ESP8266-AIRDOG



ESP8266(ie: ai-thinker ESP-12F, esp8266 with 32Mbit SPI FLASH)

安信可文档中心

http://wiki.ai-thinker.com/esp8266/docs


ESP8266 SDK and resources on Espressif:

乐鑫原厂SDK:

https://www.espressif.com/en/support/download/sdks-demos?keys=&field_type_tid%5B%5D=14

