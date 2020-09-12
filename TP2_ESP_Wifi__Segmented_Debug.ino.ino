//SENIOR DESIGN WIFI COMMUNICATION BETWEEN WRIST SENSOR AND MAIN CONTROLLER --> Tristan Peyton

//Wifi Initialization

#include <WiFi.h> //include the resources in the WiFi.h header file
#include <WiFiUdp.h> //include the resources in the WiFiUdp.h header file

//const char* ssid = "Bobby"; //WiFi network name
//const char* password = "mechatron"; //WiFi network password

//const char* ssid = "Tony's IPhone"; //WiFi network name
//const char* password = "seniordesign"; //WiFi network password

//const char* ssid = "Eiyob's iPhone"; //WiFi network name
//const char* password = "Berhanu123"; //WiFi network password

const char* ssid = "Tristan"; //WiFi network name
const char* password = "00000000"; //WiFi network password


WiFiUDP TP2; //name my server
unsigned int UDPPort = 6969; //local port # for TP2Server ESP
unsigned int UDPTargetPort = 7373; //target port # --> 7373 for TP1 ESP
const int UDP_PACKET_SIZE = 6; //packet size of 6 so can grab values from 0 to 50
byte udpBuffer[UDP_PACKET_SIZE + 1]; //initial send buffer of size packetsize + 10 for safety
byte udpBufferButton4 [UDP_PACKET_SIZE]; //response buffer of size packetsize + 1 for safety
byte udpBufferButton1_Receipt [UDP_PACKET_SIZE];

IPAddress ipTarget(192, 168, 1, 173); //IP of who I want to communicate with --> 173 for TP1, 61 for Packetsender
IPAddress myIPAddress(192, 168, 1, 157); //my IP address
//__________________________________________________________________________________________________________________________________________________________________________
// Analog Write Setup
// Pump
#define Pump_Output_Pin 25  // A1
#define Pump_Freq 50
#define Pump_Channel 0
#define Pump_Resolution 8

//#define LCM_Output_Pin 34   //A2
#define LCM_Output_Pin 26   //A0
#define LCM_Freq 50
#define LCM_Channel 1
#define LCM_Resolution 8


//__________________________________________________________________________________________________________________________________________________________________________
//Global Variables
int Button4Pin = 27;
int Button4Val;

int Button1Val;

float TP2input = 0; //initializes the variable that will hold the value being received from TP1 by TP2
float TP2output = 0; //initializes the variable that will hold the value TP2 is sending to TP1

int RecievedTemp = 1;
int FirstPart;
String FirstPartStr;
int SecondPart;
String SecondPartStr;
String FullString;
int Full;
float FullFloat;
int FullStringLength;
int SendMsg = 0;
float PassingTemp;
float T_error = 0;
float T_cStable;
float T_s;
int OnOff = 1;
int CycleVal = 0;
String Mode;

float lowADC = 0.0;
float highADC = 4095.0;

//initializing pump values
float Pump_ADC;
float Pump_speed;
float Pump_low_speed = 0;
float Pump_high_speed = 176.65;
float Pump_low_voltage = 0.0;
float Pump_high_voltage = 12.0;

//initializing LCM values
float LCM_ADC;
float LCM_speed;
float LCM_low_speed = 70.0;
float LCM_high_speed = 255.0;
float LCM_low_voltage = 0.0;
float LCM_high_voltage = 4.31;

// Demo Day Safety Voltage Reducer
float PumpVoltage_Red = 2.0;


//__________________________________________________________________________________________________________________________________________________________________________
void Execute(float PumpVoltage, float CompressorVoltage) {
  // Analog write to p+ump pin
//  Pump_ADC = map(PumpVoltage, Pump_low_voltage, Pump_high_voltage, lowADC , highADC);
//  Serial.print("Pump_ADC:  ");
//  Serial.print(Pump_ADC);
//  Serial.print("\t");  
  
  Pump_speed = map((PumpVoltage), Pump_low_voltage, Pump_high_voltage, Pump_low_speed, Pump_high_speed);
  Serial.print("P_S:  ");
  Serial.print(Pump_speed);
  Serial.print("\t");

  ledcWrite(Pump_Channel, Pump_speed); 
  //dacWrite(Pump_Output_Pin, Pump_speed);

  // Analog write to LCM pin
//   LCM_ADC = map(CompressorVoltage, LCM_low_voltage, LCM_high_voltage, lowADC , highADC);
//  Serial.print("LCM_ADC:  ");
//  Serial.print(LCM_ADC);
//  Serial.print("\t");  
  
  LCM_speed = map(CompressorVoltage, LCM_low_voltage, LCM_high_voltage, LCM_low_speed, LCM_high_speed);
  Serial.print("LCM_S:  ");
  Serial.print(LCM_speed);
  Serial.print("\t");

  ledcWrite(LCM_Channel, LCM_speed);
  //dacWrite(LCM_Channel, LCM_speed);
}
//__________________________________________________________________________________________________________________________________________________________________________
float handleTP2() { //function for handling when a packet comes in from TP1
  int tp2 = TP2.parsePacket();
  int i = 0;
  if (tp2) {

    TP2.read(udpBuffer, UDP_PACKET_SIZE); //reads packets and puts contents into TP2input
    for (i = 0; i < UDP_PACKET_SIZE; i++) {
      TP2input += (int)udpBuffer[i];
    }
    
    if(TP2input == 1){
      if(CycleVal != 3){
        CycleVal = CycleVal + 1;
      }
      else if(CycleVal == 3){
        CycleVal = 0;
      }
    }

    else if(TP2input == 2){
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

    else if(TP2input == 3){
      T_cStable = FullFloat;
      TempStable(T_cStable);
      Serial.print("Stabilize");
      Serial.print("\t");
    }
    
    else{
      String TP2inputStr = String(TP2input);
      String Char1TP2inputStr = TP2inputStr.substring(0,1);
      String Char2TP2inputStr = TP2inputStr.substring(1,2);
      
      if((RecievedTemp == 1)&&(Char1TP2inputStr == "1")&&(Char2TP2inputStr != 0)){
       SendMsg = 0;
       SecondPart = TP2input;
//       Serial.print(FirstPart);
//       Serial.print("\n");
       SecondPartStr = String(SecondPart);
       SecondPartStr.remove(0,1);
       RecievedTemp = 2;
       goto Bailout;
      }

      if(RecievedTemp == 2){
        FirstPart = TP2input;
        FirstPartStr = String(FirstPart);
//        Serial.print(SecondPart);
        RecievedTemp = 1;
        SendMsg = 1;
        goto Bailout;
      }

      Bailout:;
    }
    
    TP2input = 0;
  }

    if(SendMsg == 1){
      FullString = FirstPartStr + SecondPartStr;
      Full = FullString.toInt();
      FullFloat = Full / 100.0;
//      Serial.print(FullFloat);
//      Serial.print("\n");
   }
  return FullFloat;
}
//___________________________________________________________________________________________________________________________________________________________________________
void FindOptimalVals(float T_c, float T_error, float arr[]){
  T_s = T_c + T_error;
  
  if(T_s < 99){
    arr[0] = 0;
    arr[1] = 0;
    arr[2] = 0;
  }
  else if((T_s >= 99) && (T_s <= 100.91)){
    arr[0] = -.0077;
    arr[1] = 8.3125;
    arr[2] = 1.3750;
  }
  else if ((T_s >= 100.92) && (T_s <= 101.01)){
    arr[0] = -.0078;
    arr[1] = 8.1250;
    arr[2] = 1.3750; 
  }
   else if ((T_s >= 101.02) && (T_s <= 101.13)){
    arr[0] = -.0080;
    arr[1] = 7.9375;
    arr[2] = 1.3750; 
  }
  else if ((T_s >= 101.14) && (T_s <= 101.27)){
    arr[0] = -.0084;
    arr[1] = 7.7500;
    arr[2] = 1.3750;
  }
   else if ((T_s >= 101.28) && (T_s <= 101.42)){
    arr[0] = -.0087;
    arr[1] = 7.5625;
    arr[2] = 1.3750;
  }
  else if ((T_s >= 101.43) && (T_s <= 101.55)){
    arr[0] = -.0091;
    arr[1] = 7.3750;
    arr[2] = 1.3750;
  }
  else if ((T_s >= 101.56) && (T_s <= 101.65)){
    arr[0] = -.0095;
    arr[1] = 7.1875;
    arr[2] = 1.3750;
  }
  else if ((T_s >= 101.66) && (T_s <= 101.67)){
    arr[0] = -.0097;;
    arr[1] = 5.3125;
    arr[2] = 1.3750; 
  }
  else if ((T_s >= 101.68) && (T_s <= 101.71)){
    arr[0] = -.0097;
    arr[1] = 5.5000;
    arr[2] = 1.3750;
  }
  else if ((T_s >= 101.72) && (T_s <= 101.74)){
    arr[0] = -.0099;
    arr[1] = 5.6875;
    arr[2] = 1.3750; 
  }
  else if (T_s >= 101.75){
    arr[0] = -.0100;
    arr[1] = 5.8750;
    arr[2] = 1.3750;
  } 

  if (T_s < 99){
    arr[3] = 0;
  }
  else if ((T_s >= 99)&&(T_s <= 100.88)){
    arr[3] = 7.7114;
  }
  else if ((T_s >= 100.89)&&(T_s <= 100.92)){
    arr[3] = 9.0180;
  }
  else if ((T_s >= 100.93)&&(T_s <= 101.01)){
    arr[3] = 8.669;
  }
  else if ((T_s >= 101.02)&&(T_s <= 101.13)){
    arr[3] = 8.7052;
  }
  else if ((T_s >= 101.14)&&(T_s <= 101.27)){
    arr[3] = 8.5414;
  }
  else if ((T_s >= 101.28)&&(T_s <= 101.42)){
    arr[3] = 8.3848;
  }
  else if ((T_s >= 101.43)&&(T_s <= 101.55)){
    arr[3] = 8.2431;
  }
  else if ((T_s >= 101.56)&&(T_s <= 101.65)){
    arr[3] = 8.1253;
  }
  else if ((T_s >= 101.66)&&(T_s <= 101.67)){
    arr[3] = 7.9041;
  }
  else if ((T_s >= 101.68)&&(T_s <= 101.70)){
    arr[3] = 7.800;
  }
  else if ((T_s >= 101.71)&&(T_s <= 100.74)){
    arr[3] = 7.7342;
  }
  else if (T_s >= 101.75){
    arr[3] = 7.7114;
  }

  arr[4] = T_s;
  
}
//___________________________________________________________________________________________________________________________________________________________________________
//Temperature Stabilization

float TempStable(float T_cStable){
  T_error = 98 - T_cStable;
  return T_error;
  Serial.print("\t");
  Serial.print(T_error);
}
//___________________________________________________________________________________________________________________________________________________________________________
// Mode Selector
String ModeSelector(int CycleVal, float T_c, float T_error, float arr[]){
  if(CycleVal == 0){
    Mode = "A";
    FindOptimalVals(T_c,T_error,arr);
    udpBufferButton1_Receipt[0] = 2;
    TP2.beginPacket(ipTarget, UDPTargetPort);
    TP2.printf("%s", udpBufferButton1_Receipt);
    TP2.endPacket();
  }
  else if (CycleVal == 1){
    Mode = "L";
    arr[0] = -.007692;
    arr[2] = 1.375;
    arr[1] = 8.313;
    arr[3] = NULL;
    udpBufferButton1_Receipt[0] = 3;
    TP2.beginPacket(ipTarget, UDPTargetPort);
    TP2.printf("%s", udpBufferButton1_Receipt);
    TP2.endPacket();
  }
  else if (CycleVal == 2){
    Mode = "M";
    arr[0] = -.01431;
    arr[2] = 3.026;
    arr[1] = 7;
    arr[3] = NULL;
    udpBufferButton1_Receipt[0] = 4;
    TP2.beginPacket(ipTarget, UDPTargetPort);
    TP2.printf("%s", udpBufferButton1_Receipt);
    TP2.endPacket();
  }
  else if (CycleVal == 3){
    Mode = "H";
    arr[0] = -.02063;
    arr[2] = 4.31;
    arr[1] = 6.813;
    arr[3] = NULL;
    udpBufferButton1_Receipt[0] = 5;
    TP2.beginPacket(ipTarget, UDPTargetPort);
    TP2.printf("%s", udpBufferButton1_Receipt);
    TP2.endPacket();
  }
  return Mode;
}
//___________________________________________________________________________________________________________________________________________________________________________

void fncUdpSend(){
  udpBufferButton4[0] = 1;
  Button4Val = digitalRead(Button4Pin);
  if(Button4Val == 1){
    TP2.beginPacket(ipTarget, UDPTargetPort);
    TP2.printf("%s", udpBufferButton4);
    TP2.endPacket();
    //Serial.print("Compressor ERROR!");
    //Serial.print("\n");
  }
}

//_________________________________________________________________________________________________________________________________________________________________________

void setup() { //WiFi Setup
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(Button4Pin, INPUT);

  // Analog Write Setup
  ledcSetup(Pump_Channel, Pump_Freq, Pump_Resolution);
  ledcAttachPin(Pump_Output_Pin, Pump_Channel);
 

   // Analog Write Setup
  ledcSetup(LCM_Channel, LCM_Freq, LCM_Resolution);
  ledcAttachPin(LCM_Output_Pin, LCM_Channel);
 

  Serial.begin(115200); //baud rate
  Serial.println();
  Serial.print("Connecting TP2 to ");
  Serial.print(ssid);

  WiFi.begin(ssid, password);
  WiFi.config(myIPAddress, IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));

  TP2.begin(UDPPort);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Wifi connected");
//signal there is a connection to the Wifi Network
}
//_________________________________________________________________________________________________________________________________________________________________________//

void loop() {
  
  fncUdpSend();
  
  handleTP2(); //see if TP1 has received anything and take action if TP1 did

  volatile float T_c = FullFloat;
  float arr[5] = {0};

  ModeSelector(CycleVal,T_c,T_error,arr);
  Serial.print("Mode: ");
  Serial.print(Mode);
  Serial.print("\t");
  
  Serial.print("A Temp: ");
  Serial.print(T_c);
  Serial.print("\t");
  
  Serial.print("Mod Temp: ");
  Serial.print(arr[4]);
  Serial.print("\t");
  
  float HeatingRate = arr[0];
  Serial.print("HR: ");
  Serial.print(HeatingRate,5);
  Serial.print("\t");
  float PumpVoltage = arr[1];
  Serial.print("PV: ");
  Serial.print(PumpVoltage,5);
  Serial.print("\t");
  float CompressorVoltage = arr[2];
  Serial.print("CV: ");
  Serial.print(CompressorVoltage,5);
  Serial.print("\t");
  float MinimumAmperage = arr[3];
  Serial.print("MA: ");
  Serial.print(MinimumAmperage,5);
  Serial.print("\n");
  //delay(10);

  if(OnOff == 1){
    Execute(PumpVoltage,CompressorVoltage);
  }
  else if(OnOff == 0){
    Pump_speed = 0;
    ledcWrite(Pump_Channel, Pump_speed);
    LCM_speed = 0;
    ledcWrite(LCM_Channel, LCM_speed);
  }
}

