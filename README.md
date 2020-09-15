# GlacionCoolingBackpack
The Slave and TP2 are the main files controlling the device. I had a more troubleshooting role but still contribute to portions of the code.
The testing portions are the Kalman DSB and the Live Import. 


The Slave_Wifi_Debug 
This side of the code takes in the temperature from the user or it takes in the High Medium or Low value and then transmits it to the TP2 to begin the product's operation. 
- There are buttons on the control box that are pressed to initialize High Medium, Low or automatic mode. 
  - High Medium and Low will autmoatically send in set conditions for the cooling device to operate at. 
  -Automatic Mode will take in the temperature of the user and then and set out variable commands to change the pump's speed and the cooling unit's power. This is designed to cool the person most efficiently. 
- The control box is also connected to  a screen so it will display the status of the cooling device whether it's high medium low or automatic. As well as whether the compressor for the cooling unit is running at normal operation. 
- This information is sent to another microcontroller via Wi-Fi.
 
 The TP2
 This file takes in the in
