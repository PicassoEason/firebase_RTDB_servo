#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#include <Ultrasonic.h>
#include <ESP32Servo.h>

#define WIFI_SSID "YunTech_5G"
#define WIFI_PASSWORD "12341234"
#define API_KEY "AIzaSyDhxS8c718wJtTyPzSTkupguVo3FP8dT40"
#define DATABASE_URL "https://shgo-64872-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrewMills=0;
bool signupOK=false;
bool data=false; // upload data
float voltage=0.0; 

Servo servoN1;
Ultrasonic ultrasonic(32,33);
int distance;

void setup() {
  // put your setup code here, to run once:
  servoN1.attach(25);
  servoN1.write(90);

  Serial.begin(19200);
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while(WiFi.status()!=WL_CONNECTED){
    Serial.print(".");delay(300);
  }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());

    config.api_key=API_KEY;
    config.database_url=DATABASE_URL;

    if(Firebase.signUp(&config,&auth,"","")){
      Serial.print("SignUp ok");
      signupOK=true;
    }else{
      Serial.printf("%s\n",config.signer.signupError.message.c_str());
    }
    config.token_status_callback=tokenStatusCallback;
    Firebase.begin(&config,&auth);
    Firebase.reconnectWiFi(true);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(Firebase.ready()&&signupOK&&(millis()-sendDataPrewMills>5000||sendDataPrewMills==0)){
    sendDataPrewMills-millis();
    //----------------STORE sensor data to a RTDB
    distance = ultrasonic.read();
    Serial.print("Distance in CM: ");
    Serial.println(distance);
    if(distance<=35)
    {
      data=false;
      servoN1.write(90);
      Serial.println("there has something...");
      delay(1000);
    }
    else
    {
      data=true;
      servoN1.write(0);
      Serial.println("nothing is here...");
      delay(1000);
      
    }
    delay(1000); //每次間格0.5秒
    String dataStr = data ? "true" : "false";
    if (Firebase.RTDB.set(&fbdo, "Sensor/data", dataStr)) {
      Serial.println("Data stored in RTDB");
    } else {
      Serial.println("Error storing data in RTDB");
    }
    
    
  }
}
