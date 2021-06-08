
/**** LIBRERIAS  ****/
#include "heltec.h" 
#include "images.h"
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "WiFi.h"
#include <WiFiManager.h> 
#include <Wire.h>
#include <ESP32Ping.h>
#include <ping.h>
 



/****DEFINICIONES****/
#define BUFFER_LEN  256
#define AWS_IOT_PUBLISH_TOPIC   "data/sensors"
#define AWS_IOT_SUBSCRIBE_TOPIC "data/recibo"
#define BAND    915E6  //you can set band here directly,e.g. 868E6,915E6

/****VARIABLES****/
char msg[BUFFER_LEN];
WiFiManager wifiManager;
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);
String packSize = "--";
String packet;
int pos1,pos2,pos3;
String temp,spo2,bpm,ID;

void messageHandler(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void logo(){
  Heltec.display->clear();
  Heltec.display->drawXbm(0,5,logo_width,logo_height,logo_bits);
  Heltec.display->display();
}
void connectAWS()
{
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  // Create a message handler
  client.setCallback(messageHandler);

  Serial.print("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if(!client.connected()){

    Heltec.display->drawString(0, 30, "Error con la red, reinicie el dispositivo y pruebe con otra red");
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void publishMessage()
{
  Serial.print("ESTOY ENVIANDO POR AWS"); 
  snprintf (msg, BUFFER_LEN,"{ \"ID\": \"%s\" , \"Temprature\" : %s , \"Pulsaciones\": %s , \"Oxigenación\": %s }",ID.c_str(),temp.c_str(),bpm.c_str(),spo2.c_str());
  Serial.println(msg);
  client.publish(AWS_IOT_PUBLISH_TOPIC,msg);
  Serial.print("Mensaje enviado");
}
void LoRaData(){
  temp="";
  bpm="";
  spo2="";
  ID="";
  pos1=packet.indexOf("#");
  pos2=packet.indexOf("$");
  pos3=packet.indexOf("/");
  temp=packet.substring(0,pos1);
  bpm=packet.substring(pos1+1,pos2);
  spo2=packet.substring(pos2+1,pos3);
  ID=packet.substring(pos3+1,packet.length());
 
}

void cbk(int packetSize) {
  packet ="";
  for (int i = 0; i < packetSize; i++) { packet += (char) LoRa.read(); }
  LoRaData();
}

void setup() { 
   //WIFI Kit series V1 not support Vext control
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
 
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_10);
  logo();
  delay(1500);
  Heltec.display->clear();
  Heltec.display->drawString(0, 10, "Si es primera vez");
  Heltec.display->drawString(0, 10, "Para Configurar el Wi-fi: Ingrese a la Red:");
  Heltec.display->drawString(0, 20, "Covid-Monitor-Server");
  Heltec.display->display();
  wifiManager.autoConnect("Covid-Monitor-Server");
 while(!Ping.ping("www.google.com",3))
  {
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "NO HAY CONEXION DE RED");
    Heltec.display->drawString(0, 10, "PRUEBE OTRA RED");
    wifiManager.resetSettings();
    Heltec.display->display();
    WiFi.disconnect();
    wifiManager.autoConnect("Covid-Monitor-Server");
  }
  connectAWS();
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "LoRa Iniciado y Wi-fi Exitosamente!");
  Heltec.display->display();
  delay(2000);
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "ESTADO LORA: OK");
  Heltec.display->drawString(0, 10, "ESTADO WIFI: OK");
  
   
  Heltec.display->display();
  
  //LoRa.onReceive(cbk);
  LoRa.receive();
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) { 
    cbk(packetSize); 
    publishMessage();
    }
  delay(10);
}
