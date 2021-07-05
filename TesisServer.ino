
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
#include <FirebaseESP32.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
 

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

/****DEFINICIONES****/
#define BAND    915E6  //you can set band here directly,e.g. 868E6,915E6

/****VARIABLES****/
// Firebase Data object
FirebaseData firebaseData;
FirebaseJson json;
FirebaseAuth auth;
FirebaseConfig config;

// WifiManager Variables
WiFiManager wifiManager;
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

// Data Variables
String packSize = "--";
String packet;
String path = "/Sensores";
int pos1,pos2,pos3;
String temp,spo2,hr,ID,nodo;
float tempF,spo2F,hrF;
char tempC[8],spo2C[8],hrC[8];

// NTP Variables
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "south-america.pool.ntp.org");
unsigned long epochTime; 

// GET EPOCH
unsigned long get_Time() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}


void connectFirebase(){
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = FIREBASE_HOST;
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
}


void sendFirebase() {
  epochTime = get_Time();
  String nodo = path + "/"+ID+"/";
  String nodo1 = "/Historicos/"+ID+"/"+String(epochTime)+"/";
  json.add("spo2", spo2F);
  json.add("hr", hrF);
  json.add("temp", tempF);   
  Firebase.updateNode(firebaseData,nodo,json);
  Firebase.updateNode(firebaseData,nodo1,json);            
}

void logo(){
  Heltec.display->clear();
  Heltec.display->drawXbm(0,5,logo_width,logo_height,logo_bits);
  Heltec.display->display();
}

void LoRaData(){
  temp="";
  hr="";
  spo2="";
  ID="";
  pos1=packet.indexOf("#");
  pos2=packet.indexOf("$");
  pos3=packet.indexOf("/");
  temp=packet.substring(0,pos1);
  hr=packet.substring(pos1+1,pos2);
  spo2=packet.substring(pos2+1,pos3);
  ID=packet.substring(pos3+1,packet.length());


  // Convertir String a Float
  temp.toCharArray(tempC,temp.length()+1);
  spo2.toCharArray(spo2C,spo2.length()+1);
  hr.toCharArray(hrC,hr.length()+1);
  tempF=atof(tempC);
  spo2F=atof(spo2C);
  spo2F=atof(hrC);
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
    connectFirebase();
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
    sendFirebase();
    }
  delay(10);
}
