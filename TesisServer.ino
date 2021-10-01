
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
int pos1,pos2,pos3,pos4;
String temp,spo2,hr,ID,nodo,flag;
float tempF,spo2F,hrF;
bool flagB;
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
  json.clear();
  String nodo = path + "/"+ID+"/";
  String nodo1 = "/Historicos/"+ID+"/"+String(epochTime)+"/";

  if(!flagB)
  {
    json.add("flag",flagB);
    Firebase.updateNode(firebaseData,nodo,json);
  }
  else{
    
  json.add("spo2", spo2F);
  json.add("hr", hrF);
  json.add("temp", tempF);
  Firebase.updateNode(firebaseData,nodo1,json);
  json.add("flag",flagB);   
  Firebase.updateNode(firebaseData,nodo,json);
  }
              
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
  flag="";
  if(packet.indexOf("!") == 0)
  {
    int posID = packet.indexOf("$");
    flag = packet.substring(0,posID);
    ID = packet.substring(posID+1,packet.length());
    flagB = bool(flag);
    Serial.println(flag + " " + ID);
    
  }
  else{
    
  
  pos1=packet.indexOf("#");
  pos2=packet.indexOf("$");
  pos3=packet.indexOf("/");
  pos4=packet.indexOf("!");
  temp=packet.substring(0,pos1);
  hr=packet.substring(pos1+1,pos2);
  spo2=packet.substring(pos2+1,pos3);
  ID=packet.substring(pos3+1,pos4);
  flag =packet.substring(pos4+1,packet.length());
  // Convertir String a Float
  temp.toCharArray(tempC,temp.length()+1);
  spo2.toCharArray(spo2C,spo2.length()+1);
  hr.toCharArray(hrC,hr.length()+1);
  tempF=atof(tempC);
  spo2F=atof(spo2C);
  hrF=atof(hrC);
  flagB = bool(flag);
String a= "info: "+ String(tempF) + " " + String(spo2F) + " " +String(hrF);
Serial.println(a);
  }
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
  Heltec.display->drawString(0, 20, "Para Configurar el Wi-fi:");
  Heltec.display->drawString(0, 30, "Ingrese a la Red:");
  Heltec.display->drawString(0, 40, "Covid-Monitor-Server");
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
  Heltec.display->drawString(0, 0, "LoRa Iniciado");
  Heltec.display->drawString(0,10, "Wi-fi Exitosamente!");
  Heltec.display->display();
  delay(2000);
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "ESTADO LoRa: OK");
  Heltec.display->drawString(0, 10, "ESTADO WI-FI: OK");
  
   
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
