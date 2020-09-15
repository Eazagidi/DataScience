//SENIOR DESIGN WIFI COMMUNICATION BETWEEN WRIST SENSOR AND MAIN CONTROLLER 

//Temperature Sensor Setup
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);
//_______________________________________________________________________________________________________
#include <Wire.h>;
#include "SSD1306.h"
SSD1306 display(0x3c, 23, 22);// Set the pins being used for SDA(23) and SCL(22)
float a=25;

//_______________________________________________________________________________________________________
//Wifi Initialization

#include <WiFi.h> //include the resources in the WiFi.h header file
#include <WiFiUdp.h> //include the resources in the WiFiUdp.h header file

const char* ssid = "Tristan"; //WiFi network name
const char* password = "00000000"; //WiFi network password

//const char* ssid = "Eiyob's iPhone"; //WiFi network name
//const char* password = "Berhanu123"; //WiFi network password

WiFiUDP TP1; //name my server
unsigned int UDPPort = 7373; //local port # for TP1 ESP
unsigned int UDPTargetPort = 6969; //target port # --> 6969 for TP2 ESP
const int UDP_PACKET_SIZE = 6; //packet size of 6 so can grab values from 0 to 50
byte udpBuffer[UDP_PACKET_SIZE + 1]; //initial send buffer of size packetsize + 1 for safety
byte udpBufferResponse [UDP_PACKET_SIZE + 1]; //response buffer of size packetsize + 1 for safety
byte udpBufferButton1[UDP_PACKET_SIZE + 1];
byte udpBufferButton2[UDP_PACKET_SIZE + 1];
byte udpBufferButton3[UDP_PACKET_SIZE + 1];
byte udpBufferTempSensor[UDP_PACKET_SIZE + 1];
byte udpBufferButton1Confirm[UDP_PACKET_SIZE + 1];
IPAddress ipTarget(192,168,1,157); //IP of who I want to communicate with --> 157 for TP2, 61 for Packetsender
IPAddress myIPAddress(192,168,1,173); //my IP address

//_________________________________________________________________________________________________
int Button1Pin = 15;
int Button1Val;
        
int Button2Pin = 27;
int Button2Val;

int Button3Pin = 33;
int Button3Val;

int ModeCheck;

float TP1input = 0; //initializes the variable that will hold the value being received from TP2 by TP1
float TP1output; //initializes the variable that will hold the value TP1 is sending to TP2

int CycleVal = 0;

String DisplayString;

int Error = 0;

int OnOff = 1;

int Stable;
//____________________________________________________________________________________________________________________________

void handleTP1(){ //function for handling when a packet comes in from TP2
  int tp1 = TP1.parsePacket();
  if (tp1){
    TP1.read(udpBuffer,UDP_PACKET_SIZE); //reads packets and puts contents into TP1input
    for(int i=0;i<UDP_PACKET_SIZE;i++){
      TP1input += (int)udpBuffer[i];
    }

    if(TP1input == 1){
      Serial.print("Compressor ERROR!");
      Serial.print("\n");
      Error = 1;
      goto Jump;
    }
 
  else if(TP1input == 2){
      Serial.print("Auto Mode Receipt");
      Serial.print("\n");
      ModeCheck = 1;
      goto Jump;
    }
  
  

 else if(TP1input == 3){
      Serial.print("Low Mode Receipt");
      Serial.print("\n");
      ModeCheck = 2;
      goto Jump;
    }
  
  

 else if(TP1input == 4){
      Serial.print("Medium Mode Receipt");
      Serial.print("\n");
      ModeCheck = 3;
      goto Jump;
    }
  
  

 else if(TP1input == 5){
      Serial.print("High Mode Receipt");
      Serial.print("\n");
      ModeCheck = 4;
      goto Jump;
    }
    Jump:;
  TP1input = 0;
//  Serial.print("ModeCheck: ");
//  Serial.print(ModeCheck);
//  Serial.print("\n");
  

}
}
//____________________________________________________________________________________________________________________________
void fncUdpSend(){ //function for sending TP1 values to TP2
  udpBufferButton1[0] = 1;
  Button1Val = digitalRead(Button1Pin);
  if(Button1Val == 1){
    TP1.beginPacket(ipTarget,UDPTargetPort);
    TP1.printf("%s",udpBufferButton1);
    TP1.endPacket();
//    Serial.print(CycleVal);
    if(CycleVal != 3){
      CycleVal = CycleVal + 1;
    }
    else if(CycleVal == 3){
      CycleVal = 0;
    }
  }
  

  udpBufferButton2[0] = 2;
  Button2Val = digitalRead(Button2Pin);
  if(Button2Val == 1){
    TP1.beginPacket(ipTarget,UDPTargetPort);
    TP1.printf("%s",udpBufferButton2);
    TP1.endPacket();
    
    OnOff = !OnOff;
    if(OnOff == 1){
      Serial.print("System ON");
      Serial.print("\t");
    }
    else if (OnOff == 0){
      Serial.print("System OFF");
      Serial.print("\t");
    }
  }

  udpBufferButton3[0] = 3;
  Button3Val = digitalRead(Button3Pin);
  if(Button3Val == 1){
    TP1.beginPacket(ipTarget,UDPTargetPort);
    TP1.printf("%s",udpBufferButton3);
    TP1.endPacket();
    Serial.print("Reset Temp Stable");
    Serial.print("\n");
  }

  udpBufferTempSensor[0] = sensors.getTempFByIndex(0);
  TP1.beginPacket(ipTarget,UDPTargetPort);
  TP1.printf("%s",udpBufferTempSensor);
  TP1.endPacket();
  
  String TempString = String(sensors.getTempFByIndex(0));
  DisplayString = TempString;
  String TempString2;
  long StringInt;
  long TempInt;
//  Serial.print("String Version: ");
  Serial.print(TempString);
//  Serial.print("\t");
  
  if(TempString.length() == 5){
    TempString.remove(0,3);
    TempString2 = "1" + TempString;
    StringInt = TempString.toInt();
    TempInt = TempString2.toInt();
  }
  else if(TempString.length() == 6){
    TempString.remove(0,4);
    TempString2 = "1" + TempString;
    StringInt = TempString.toInt();
    TempInt = TempString2.toInt();
  }

  if((StringInt/10) >= 1){
    udpBufferTempSensor[0] = TempInt;
    TP1.beginPacket(ipTarget,UDPTargetPort);
    TP1.printf("%s",udpBufferTempSensor);
    TP1.endPacket();
//    Serial.print(TempInt);
  }
  else{
    udpBufferTempSensor[0] = 110;
    TP1.beginPacket(ipTarget,UDPTargetPort);
    TP1.printf("%s",udpBufferTempSensor);
    TP1.endPacket();
//    Serial.print(udpBufferTempSensor[0]);
  }
  Serial.print("\n");
}

//_________________________________________________________________________________________________
//Screen Activate

void ScreenActivate(String DisplayString, int CycleVal, int ModeCheck){
  display.flipScreenVertically();
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 1, "TEMP: "); 
  display.drawString(33, 1, DisplayString+" "+"F");// this is to put in a constantly changing parameter must be in a string

  display.drawString(0, 18, "MODE:" );
  display.drawString(40, 18,"H ");
  display.drawString(57, 18,"M ");
  display.drawString(74, 18,"L ");
  display.drawString(91, 18,"Auto");
  display.drawString(0, 31, "LCM: ");
  display.drawString(64, 31,"PUMP: ");
  display.drawString(0, 44,"COMP. STATUS: ");

  if(OnOff == 1){
    display.drawString(30, 31,"ON");
    display.drawString(100, 31,"ON");
  }
  else if(OnOff == 0){
    display.drawString(30, 31,"OFF");
    display.drawString(100, 31,"OFF");
  }
  
  if(Error == 0){
  display.drawString(85, 44,"NORMAL");
  }
  else if(Error == 1){
    display.drawString(85,44, "ERROR");
  }
  
  if ((ModeCheck == 1)){
    display.drawRect(89, 18, 26, 13); // display.drawrect(Xposition,yposition,x_width,y width)
  }
  else if ((ModeCheck == 2)){
    display.drawRect(72, 18, 11, 13); // display.drawrect(Xposition,yposition,x_width,y width)
  }
  else if ((ModeCheck == 3)){
    display.drawRect(55, 18, 13, 13); // display.drawrect(Xposition,yposition,x_width,y width)
  }
  else if ((ModeCheck == 4)){
    display.drawRect(38, 18, 11, 13); // display.drawrect(Xposition,yposition,x_width,y width)  
  }

  if(Button3Val == 1){
    display.drawString(80,1,"Set Point");
  }
  
  display.display();
}
//_________________________________________________________________________________________________
void setup() { //WiFi Setup
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);

  pinMode(Button1Pin, INPUT);
  pinMode(Button2Pin,INPUT);
  pinMode(Button3Pin,INPUT);
  
  Serial.begin(115200); //baud rate
  Serial.println();
  Serial.print("Connecting TP1 to ");
  Serial.print(ssid);

  WiFi.begin(ssid,password);
  WiFi.config(myIPAddress,IPAddress(192,168,1,1),IPAddress(255,255,255,0));

  TP1.begin(UDPPort);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Wifi connected");

  digitalWrite(LED_BUILTIN,HIGH); //signal there is a connection to a Wifi Network

  sensors.begin();
  display.init();
}
//_________________________________________________________________________________________________
void loop() {
  handleTP1();
  sensors.requestTemperatures();
  fncUdpSend();
  ScreenActivate(DisplayString,CycleVal,ModeCheck);
  
  if(TP1input == 0){
    Error = 0;
  }
  
  delay(10);
  }
