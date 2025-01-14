#include <ESP8266WiFi.h>   //安装esp8266arduino开发环境
#include <PubSubClient.h>  //安装PubSubClient库
#include <ArduinoJson.h>   //json  V5版本
#include <Servo.h>
#include "aliyun_mqtt.h"

#define WIFI_SSID        ""//替换自己的WIFI
#define WIFI_PASSWD      ""//替换自己的WIFI

#define PRODUCT_KEY      "" //替换自己的PRODUCT_KEY
#define DEVICE_NAME      "" //替换自己的DEVICE_NAME
#define DEVICE_SECRET    ""//替换自己的DEVICE_SECRET

/*-----------------------------订阅消息的TOPIC -------------------------------------------*/
#define ALINK_TOPIC_PROP_SET      "" //这个就是我们接收消息的TOPIC        

/***************************这个是发布数据的TOPIC******************************************/
#define ALINK_TOPIC_PROP_POST      "" //这个是上传数据的TOPIC

unsigned long lastMs = 0;

WiFiClient   espClient;
PubSubClient mqttClient(espClient);

void setup()
{ 
    pinMode(D4, OUTPUT); 
    Serial.begin(115200);
    Serial.println("Demo Start");
    init_wifi(WIFI_SSID, WIFI_PASSWD);

    mqttClient.setCallback(mqtt_callback);
}

void loop()
{
   if (millis() - lastMs >= 5000){
    lastMs = millis();
    mqtt_check_connect();
    mqtt_interval_post(0);
   }
    mqttClient.loop();  
}

//回调函数-阿里云端数据发送过来之后，此函数进行解析
void mqtt_callback(char *topic, byte *payload, unsigned int length) //mqtt回调函数“byte *payload”这东西是个指针
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    payload[length] = '\0';
    
    Serial.println((char *)payload);
      
    if (strstr(topic, ALINK_TOPIC_PROP_SET))
    {
        StaticJsonBuffer<100> jsonBuffer;
        JsonObject &root = jsonBuffer.parseObject(payload);
        
        /**解析JSON数据**/
        int WeChat_data = root["data"];

        Serial.print("WeChat_data:");
        Serial.println(WeChat_data);
        Serial.print("root.success():");
        Serial.println(root.success());
        
        if(WeChat_data == 1){
          Serial.println("led on");
          digitalWrite(D4, LOW);
         }else if(WeChat_data == 0){
         Serial.println("led off");
         digitalWrite(D4, HIGH); 
         }     
    }
}

//订阅topic
void mqtt_check_connect(){                                        
    while (!mqttClient.connected())
    {
        while (connect_aliyun_mqtt(mqttClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET))
        {
            Serial.println("MQTT connect succeed!");
            mqttClient.subscribe(ALINK_TOPIC_PROP_SET);//这个就是引用开始定义的topic 订阅topic
            Serial.println("subscribe done");
           
        }
    }   
}

//上传数据到云端
void mqtt_interval_post(int a)
{
    char param[512];
    char jsonBuf[1024];
    
    sprintf(param, "{\"a\":%d,\"read\":%d,\"currentTemperature\":%d}",a,analogRead(A0),random(0,55));
    Serial.println(param);
    mqttClient.publish(ALINK_TOPIC_PROP_POST, param); //这个是上传数据的topic,jsonBuf这个是上传的数据
}

//连接WiFi
void init_wifi(const char *ssid, const char *password)      
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi does not connect, try again ...");
        delay(500);
    }
    Serial.println("Wifi is connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
