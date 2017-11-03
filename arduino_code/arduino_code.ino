#include "mpr121.h"
#include <Wire.h>
//RPM VARIABLES:
volatile int count = 0; //variable declerations
volatile long cTime = 0; //volatile lets the variable be accessed in all functions
volatile long lastTime = 0;
volatile long period;
volatile double rpm = 0;
//SPEED VARIABLES:
const int goal = 2000;
const int interval = 100;
int speedTime = 0;
double speedStart = 0;
double speedStop; //rename speedspeedStop or speedStop
//STEERING VARIABLES:
double turnStart = 0;
double turnStop;
double factor;
int oldright=0;
int right;
int oldleft=0;
int left;
int same; //value of the last color that both detected
double total = 0;
//TOUCHPAD Variables:
int irqpin = 4;  // Digital 2
boolean touchStates[12]; //array of 12 booleans- keeps track of the previous touch states
boolean state=true;

int sumRPM=0;
int numTurns=0;


//:::SETUP:::
void setup(){
  Serial.begin(9600); //allows the serial monitor to display outputs
  Keyboard.begin();
  Wire.begin();
  attachInterrupt(4, rotationCounter, RISING); //rotationCounter function runs when LEONARDO board pin7 (the interrupt pin 4)
                                               //rises (goes from a low to high)
  pinMode(irqpin, INPUT);
  digitalWrite(irqpin, HIGH); //enable pullup resistor
  mpr121_setup();
  
  pinMode(8, INPUT); //switch pin
  pinMode(10, INPUT); //steering
  pinMode(11, INPUT); //steering

  //Keyboard.press(KEY_UP_ARROW);
}


//:::MAIN:::
void loop()
{

  while (digitalRead(8) == LOW) //stops the void loop here while the switch is making pin 8 high
    {
     Keyboard.releaseAll();
     if(state)
       {
       Serial.println("____PAUSED____");
       total=0; //steering must be centered at end of pause
       state=false;
       }
     }
      
 if(!state)
   {
   Serial.println("____GO____");
   state=true;
   //Keyboard.press(KEY_UP_ARROW);
   }
  

  //readTouchInputs();
  //controlSpeed();
  //controlSteering();
}


void controlSpeed()
{
  if( millis() >= speedStart && (millis() - lastTime) < 100 )  //presses up arrow for 500* rpm/goal (during which time this if shouldn't run again) 
    {
    //Serial.println("forward"); 
    Keyboard.press(KEY_UP_ARROW);
    speedStart = millis() + interval; //time for the NEXT speed cycle to begin at
    speedStop = millis() + interval * rpm/goal;
    
    if(rpm/goal > 1.0)
      speedStop = speedStart;
    }
    
  if(millis() >= speedStop) //release up arrow for rest time 
    Keyboard.release(KEY_UP_ARROW);
}


void controlSteering()
{
 left = digitalRead(10); //switched these two after printing the PCB
 right = digitalRead(11);  
 
 if((right != oldright) || (left != oldleft)) //a 1" turn occured
   {//Serial.print(left); Serial.print(", "); Serial.println(right);
   numTurns++;
   if(right == left)
     same = right; //save the value both sensors are on
   else
     same = -1;   
   
  if(right == oldleft)
    {total--; Serial.print("LEFT");}
  if(left == oldright)
    {total++; Serial.print("RIGHT");}
  Serial.print(" Total = "); Serial.print(total); Serial.print("  "); Serial.print(total/5 * 100); Serial.print("%"); Serial.print("   turned "); Serial.print(numTurns); Serial.println(" times");
   }
   
 oldright = right;
 oldleft = left;

 if(millis() >= turnStart)        //press up arrow for 500* rpm/goal (during which time this if shouldn't run again) 
  {
  if(total < -1)
    Keyboard.press(KEY_LEFT_ARROW);
  if(total > 1)
    Keyboard.press(KEY_RIGHT_ARROW);
         
  turnStop = millis() + interval * abs(total)/10; //max rotation when total=5
  turnStart = millis() + interval; //time for the NEXT speed cycle to begin at
  if(abs(total)/10 > 1)
    turnStop = turnStart;
  
   }
 
 if(millis() >= turnStop) //release arrow for rest time (remainder of interval)
   {
   Keyboard.release(KEY_LEFT_ARROW);
   Keyboard.release(KEY_RIGHT_ARROW);
   }
}


void rotationCounter() //variables modified within the attached function should be volatile
{
  lastTime = cTime;  //lastTime = time that the previous rotation occured at
  cTime = millis();   //time (in msec) since the arduino started
  count++;          //adds one to the number of revolutions that have occured
  period = cTime - lastTime; //time for the last rotation to occur
  rpm = 1.0 / (period / 1000.0 / 60.0);
  if(count%25==0)
    {Serial.print("***"); Serial.print(count); Serial.println("***");}
  Serial.print("rpm: "); Serial.println(rpm);
}

void sendFan(String cmd, int val)
{
  if(cmd == "power")
    digitalWrite(5, HIGH);
  else
    digitalWrite(5, LOW);
  
  if(cmd == "speed")
  {
    if(val==1)
    {
      digitalWrite(6, LOW);
      digitalWrite(9, HIGH);
    }
    if(val==2)
    {
     digitalWrite(6, HIGH);
     digitalWrite(9, LOW); 
    }
    
    if(val == 3)
    {
     digitalWrite(6, HIGH);
     digitalWrite(9, HIGH); 
    }
  }
  
  digitalWrite(12, HIGH);
  delay(10);
  digitalWrite(12, LOW);  
}


void readTouchInputs(){
  if(!checkInterrupt()){
    //read the touch state from the MPR121
    Wire.requestFrom(0x5A,2); 
    
    byte LSB = Wire.read();
    byte MSB = Wire.read();
    
    uint16_t touched = ((MSB << 8) | LSB); //16bits that make up the touch states

    
    for (int i=0; i < 12; i++){  // Check what electrodes were pressed
      if(touched & (1<<i))
      {
        if(touchStates[i] == 0){
          //pin i was just touched
          Serial.print("pin ");
          Serial.print(i);
          Serial.println(" touched");
        
        }else if(touchStates[i] == 1){
          //pin i is still being touched
        }  
      
        touchStates[i] = 1;      
      }else{
        if(touchStates[i] == 1){
          Serial.print("pin ");
          Serial.print(i);
          Serial.println(" released");
          
          //pin i is no longer being touched
       }
        //if(touchStates[0] == 1) //activate nitro
          //Keyboard.write('n');
        //touchStates[i] = 0;
      }
    
    }
    
  }
}


void mpr121_setup(void){

  set_register(0x5A, ELE_CFG, 0x00); 
  
  // Section A - Controls filtering when data is > baseline.
  set_register(0x5A, MHD_R, 0x01);
  set_register(0x5A, NHD_R, 0x01);
  set_register(0x5A, NCL_R, 0x00);
  set_register(0x5A, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(0x5A, MHD_F, 0x01);
  set_register(0x5A, NHD_F, 0x01);
  set_register(0x5A, NCL_F, 0xFF);
  set_register(0x5A, FDL_F, 0x02);
  
  // Section C - Sets touch and release thresholds for each electrode
  set_register(0x5A, ELE0_T, TOU_THRESH);
  set_register(0x5A, ELE0_R, REL_THRESH);
 
  set_register(0x5A, ELE1_T, TOU_THRESH);
  set_register(0x5A, ELE1_R, REL_THRESH);
  
  set_register(0x5A, ELE2_T, TOU_THRESH);
  set_register(0x5A, ELE2_R, REL_THRESH);
  
  set_register(0x5A, ELE3_T, TOU_THRESH);
  set_register(0x5A, ELE3_R, REL_THRESH);
  
  set_register(0x5A, ELE4_T, TOU_THRESH);
  set_register(0x5A, ELE4_R, REL_THRESH);
  
  set_register(0x5A, ELE5_T, TOU_THRESH);
  set_register(0x5A, ELE5_R, REL_THRESH);
  
  set_register(0x5A, ELE6_T, TOU_THRESH);
  set_register(0x5A, ELE6_R, REL_THRESH);
  
  set_register(0x5A, ELE7_T, TOU_THRESH);
  set_register(0x5A, ELE7_R, REL_THRESH);
  
  set_register(0x5A, ELE8_T, TOU_THRESH);
  set_register(0x5A, ELE8_R, REL_THRESH);
  
  set_register(0x5A, ELE9_T, TOU_THRESH);
  set_register(0x5A, ELE9_R, REL_THRESH);
  
  set_register(0x5A, ELE10_T, TOU_THRESH);
  set_register(0x5A, ELE10_R, REL_THRESH);
  
  set_register(0x5A, ELE11_T, TOU_THRESH);
  set_register(0x5A, ELE11_R, REL_THRESH);
  
  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(0x5A, FIL_CFG, 0x04);
  
  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(0x5A, ELE_CFG, 0x0C);  // Enables all 12 Electrodes
  
  
  // Section F
  // Enable Auto Config and auto Reconfig
  /*set_register(0x5A, ATO_CFG0, 0x0B);
  set_register(0x5A, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(0x5A, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
  set_register(0x5A, ATO_CFGT, 0xB5);*/  // Target = 0.9*USL = 0xB5 @3.3V
  
  set_register(0x5A, ELE_CFG, 0x0C);
  
}


boolean checkInterrupt(void){
  return digitalRead(irqpin);
}


void set_register(int address, unsigned char r, unsigned char v){
    Wire.beginTransmission(address);
    Wire.write(r);
    Wire.write(v);
    Wire.endTransmission();
}
