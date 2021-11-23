//define the pin connections for the transmission and recieving pins 
//not usuing digital pins 1/0 made for tx/rx since they did not work
int tx = 3;
int rx = 2;

//initialize variables to hold sensor readings
int reading;

//boolean to hold state of whether someone is actually there
bool avail = true;

//make a counter to cout milliseconds that someone is detected
int timer = 0;

//make a timer to hold a students seat
int holdTime = 0;

//include the library for the sensor and the library to allow tx/rx of data from sensor
#include <SoftwareSerial.h>
#include "DFRobot_mmWave_Radar.h"


//need to allow for data transmission for the sensor using the library mmWave
SoftwareSerial data(tx, rx);

//to construct, need to have data stream going out of tx pin and into rx pin
DFRobot_mmWave_Radar hpSensor(&data);

void setup() {
  //start serial monitor with baud rate needed for the human presence sensor
  Serial.begin(115200);
  data.begin(115200);
  
  Serial.println("Loading...");

  //clear sensor to default settings
  hpSensor.factoryReset();

  //since reading a person at a desk, read up to 1m away <- can change
  hpSensor.DetRangeCfg(0, 1);  

  //look into this
  hpSensor.OutputLatency(0, 0);

  Serial.println("Starting up");
  
  

}

void loop() {
  //read the human presence sensor (1 -> detected, 0 -> nothing )
  reading = hpSensor.readPresenceDetection();
  
  // can be uncommented to see reading -> Serial.println(reading);

  //section to see if someone was walking by or actually there
  //if it reads someone for 5 straight seconds then they are there


  //if the seat was available before hand
  if(avail){
    
    //was someone detected
    if(reading == 1){
    
    //if timer is zero then its detecting someone for the first time
    if(timer == 0){
        Serial.println("Student Detected");
      }

    //if they are there for 5 seconds, change state to taken
    //should be 5000 but was moving really slow in serial monitor 
    //but it works like this for now
    if(timer > 500){
        
        Serial.println("Seat Taken");
        avail = false;
      }
    
    //if they read again add 100ms to the timer since that is the delay of the loop
    timer += 100;
    
    }else{
      //they are no longer detected so reset the timer
      timer = 0;

      //the seat is still available
      avail = true;
       
     
    }
  }
  //the case for if someone was actually there, we want to hold the seat for 15 seconds <- can change
  else{
     //if someone is there, hold the false value
     if(reading == 1){
        avail = false;

        //reset the time that it was being held
        holdTime = 0;
        
      }
      //the case where they are no longer detected
      else{

        //if they are gone for more than 15 seconds, seat is now free
        if(holdTime > 1500){
            Serial.println("Student Gone");
            avail = true;
        }
        //time hasnt reached 15 seconds, hold
        else{
          Serial.println("Currently Holding seat");
            avail = false;
          
        }
        //increase the time it has been held for
        holdTime += 100;
        
      }
      
  }
  
  
  //rerun every 100ms
  delay(100);

}
