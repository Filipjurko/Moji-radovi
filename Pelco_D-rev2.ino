
/*SP485EE
         ____
nc --- 1|    |8 --- +5
+5 --- 2|    |7 --- RS-485 - black wire to cam
+5 --- 3|    |6 --- RS-485 + white wire to cam
pin8 - 4|____|5 --- GND
*/

#include <SoftwareSerial.h>

const byte recPin = 7; //not receiving anything back, any unused pin will do
const byte transMitPin = 8;
const int xAxis = A2;  //Up Down
const int yAxis = A3;  //Left Right
const int zAxis = A4;
const byte stopHex = 0x00;
const byte tiltDown = 0x10;
const byte tiltUp = 0x08;
const byte panLeft = 0x04;
const byte panRight = 0x02;
const byte zoom_in = 0x20;
const byte zoom_out = 0x40;
int xVal = 512;
int yVal = 512;
int zVal = 512;

SoftwareSerial Pelco_D =  SoftwareSerial(recPin, transMitPin);

void setup() {
  // put your setup code here, to run once:
 pinMode(recPin, INPUT);
 pinMode(transMitPin, OUTPUT);
 Pelco_D.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
 yVal = analogRead(yAxis);
 xVal = analogRead(xAxis);
 zVal = analogRead(zAxis);
 
  if(xVal >= 450 && xVal <= 550 && yVal >= 450 && yVal <= 550){
   functionXmit(0x01, stopHex, 0x00, 0x00);
     while(xVal >= 450 && xVal <= 550 && yVal >= 450 && yVal <= 550){
       xVal = analogRead(xAxis);
     }
  }

     if(xVal < 450 && xVal >= 250){
   functionXmit(0x01, panLeft, 0x1F, 0x00);
     while(xVal < 450 && xVal >= 250){
       xVal = analogRead(xAxis);
      }
     }
     if(xVal > 250 && xVal >= 0){
   functionXmit(0x01, panLeft, 0x3F, 0x00);
     while(xVal > 250 && xVal >= 0){
       xVal = analogRead(xAxis);
   }
  }
   if(xVal > 550 && xVal <= 750){
    functionXmit(0x01, panRight, 0x1F, 0x00);
     while(xVal > 550 && xVal <= 750){
       xVal = analogRead(xAxis);
   }
 }
 if(xVal > 750 && xVal <= 1023){
    functionXmit(0x01, panRight, 0x3F, 0x00);
     while(xVal > 750 && xVal <= 1023){
       xVal = analogRead(xAxis);
   }
 }

 if(yVal < 450 && yVal >= 250){
   functionXmit(0x01, tiltUp, 0x00, 0x1F);
     while(yVal < 450 && yVal >= 250){
       yVal = analogRead(yAxis);
     }
 }
 if(yVal > 250 && yVal >= 0){
   functionXmit(0x01, tiltUp, 0x00, 0x3F);
     while(yVal > 250 && yVal >= 0){
       yVal = analogRead(yAxis);
     }
 }
 if(yVal > 550 && yVal <= 750){
   functionXmit(0x01, tiltDown, 0x00, 0x1F);
     while(yVal > 550 && yVal <= 750){
       yVal = analogRead(yAxis);
     }
 }
 if(yVal > 750 && yVal <= 1023){
   functionXmit(0x01, tiltDown, 0x00, 0x3F);
     while(yVal > 750 && yVal <= 1023){
       yVal = analogRead(yAxis);
     }
 }

 if (zVal > 512){
  functionXmit(0x01, zoom_in, 0x00, 0x00);
  while(zVal > 512){
       zVal = analogRead(zAxis);
     }
 }
 if (zVal < 512){
  functionXmit(0x01, zoom_out, 0x00, 0x00);
  while(zVal < 512){
       zVal = analogRead(zAxis);
     }
 }
 halt();
}
void functionXmit(byte camNum, byte funcVal, byte speedVal, byte speedVal1){
 int modSum = 0;
 byte dataVal[6] ={0xFF, camNum, 0x1A, funcVal, speedVal, speedVal1};
     for(int i=0; i<6; i++){
       Pelco_D.print(dataVal[i]);
       if(i > 0)      //dont add in the SYNC byte to calculate the check sum
         modSum += dataVal[i];
     }
     modSum %= 100;      //PelcoD calls for the check sum to to be the sum of bits 2 - 6 modulo 256. 256 decimal, or 100 hex
     Pelco_D.print(modSum);
}
void halt(){
           //sync  cam#  Command1  Command2  Data1  Data2  CheckSum
 byte dataVal[7] ={0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01};   //This is the most basic instruction. Camera 1, four zeros and a check sum of one
     for(int i=0; i<7; i++){
       Pelco_D.print(dataVal[i]);
     }
}
