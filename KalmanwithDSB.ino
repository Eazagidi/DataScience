#include <OneWire.h> 
#include <DallasTemperature.h>
/********************************************************************/
// Data wire is conntec to the Arduino digital pin 2
#define ONE_WIRE_BUS 2


// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

const int sensorPin = A0;  // named constant for the pin the sensor is connected to
float current;
// kalman variables
float varVolt = 1.12184278324081E-05;  // variance determined using excel and reading samples of raw sensor data
float varProcess = 1e-8;
float Pc = 0.0;
float G = 0.0;
float P = 1.0;
float Xp = 0.0;
float Zp = 0.0;
float Xe = 0.0;

float varVolt1 = 1.12184278324081E-05;  // variance determined using excel and reading samples of raw sensor data
float varProcess1 = 1e-8;
float Pc1 = 0.0;
float G1 = 0.0;
float P1 = 1.0;
float Xp1 = 0.0;
float Zp1 = 0.0;
float Xe1= 0.0;


float varVolt2 = 1.12184278324081E-05;  // variance determined using excel and reading samples of raw sensor data
float varProcess2 = 1e-8;
float Pc2 = 0.0;
float G2 = 0.0;
float P2 = 1.0;
float Xp2 = 0.0;
float Zp2 = 0.0;
float Xe2 = 0.0;

float varVolt3 = 1.12184278324081E-05;  // variance determined using excel and reading samples of raw sensor data
float varProcess3 = 1e-8;
float Pc3 = 0.0;
float G3 = 0.0;
float P3 = 1.0;
float Xp3 = 0.0;
float Zp3 = 0.0;
float Xe3 = 0.0;

float varVolt4 = 1.12184278324081E-05;  // variance determined using excel and reading samples of raw sensor data
float varProcess4 = 1e-8;
float Pc4 = 0.0;
float G4 = 0.0;
float P4 = 1.0;
float Xp4 = 0.0;
float Zp4 = 0.0;
float Xe4 = 0.0;

void setup() {
  // open a serial connection to display values
  Serial.begin(9600);
   // Start up the library
  sensors.begin();
}

void loop() {
  
  current=millis()/1000.0;
  int sensorVal = analogRead(A0);     // read the value on AnalogIn pin 0 and store it in a variable
  int sensorVal1 = analogRead(A1); 
  int sensorVal2 = analogRead(A2); 
  int sensorVal3 = analogRead(A3); sensors.requestTemperatures(); 
  int sensorVal4 = analogRead(A4); 
  



  
  float volts = sensorVal * 5.0 / 1023.0;  // convert the ADC reading to voltage
  float volts1 = sensorVal1 * 5.0 / 1023.0;
  float volts2 = sensorVal2 * 5.0 / 1023.0;
  float volts3 = sensorVal3 * 5.0 / 1023.0;
  float volts4 = sensorVal4 * 5.0 / 1023.0;
  
  //float celsiustemp= 100.0 * volts - 50;
  //float fahrenheittemp= celsiustemp * 9.0/5.0 + 32.0;
  
  // kalman process helps live plot data clear and clenaly
  Pc = P + varProcess;
  G = Pc/(Pc + varVolt);    // kalman gain
  P = (1-G)*Pc;
  Xp = Xe;
  Zp = Xp;

  Pc1 = P1 + varProcess1;
  G1= Pc1/(Pc1 + varVolt1);    // kalman gain
  P1 = (1-G1)*Pc1;
  Xp1 = Xe1;
  Zp1= Xp1;

  Pc2 = P2 + varProcess2;
  G2 = Pc2/(Pc2 + varVolt2);    // kalman gain
  P2 = (1-G2)*Pc2;
  Xp2 = Xe2;
  Zp2 = Xp2;

   Pc3 = P3 + varProcess3;
  G3 = Pc3/(Pc3 + varVolt3);    // kalman gain
  P3 = (1-G3)*Pc3;
  Xp3 = Xe3;
  Zp3 = Xp3;

   Pc4 = P4 + varProcess4;
  G4 = Pc4/(Pc4 + varVolt4);    // kalman gain
  P4 = (1-G4)*Pc4;
  Xp4 = Xe4;
  Zp4 = Xp4;

  
 Xe = G*(volts-Zp)+Xp;
 Xe1 = G1*(volts1-Zp1)+Xp1;
 Xe2 = G2*(volts2-Zp2)+Xp2;
 Xe3 = G3*(volts3-Zp3)+Xp3;
 Xe4 = G4*(volts4-Zp4)+Xp4;

  // the kalman estimate of the sensor voltage
  float celsiustempK= 100.0 * Xe - 50;
  float celsiustempK1= 100.0 * Xe1 - 50;
  float celsiustempK2= 100.0 * Xe2 - 50;
  float celsiustempK3= 100.0 * Xe3 - 50;
  float celsiustempK4= 100.0 * Xe4 - 50;
  
  float fahrenheittempK= celsiustempK * 9.0/5.0 + 32.0;
  float fahrenheittempK1= celsiustempK1 * 9.0/5.0 + 32.0;
  float fahrenheittempK2= celsiustempK2 * 9.0/5.0 + 32.0;
  float fahrenheittempK3= celsiustempK3 * 9.0/5.0 + 32.0;
  float fahrenheittempK4= celsiustempK4 * 9.0/5.0 + 32.0;

    
  Serial.print(current);
  Serial.print(" , ");
  Serial.print(fahrenheittempK);
  Serial.print(" , ");
  Serial.print(fahrenheittempK1);
  Serial.print(" , ");
  Serial.print(fahrenheittempK2);
  Serial.print(" , ");
  Serial.print(sensors.getTempFByIndex(0));
  Serial.print(" , ");
  Serial.println(sensors.getTempFByIndex(1));
  
  delay(200);
}
