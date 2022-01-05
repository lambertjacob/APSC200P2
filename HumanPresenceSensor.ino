

//initialize variables to hold sensor readings
int reading;

//boolean to hold state of whether someone is actually there
bool avail = true;

//make a counter to cout milliseconds that someone is detected
int timer = 0;

//make a timer to hold a students seat
int holdTime = 0;

//include the library for the sensor 

#include "DFRobot_mmWave_Radar.h"


//<-------------------------------WIFI CODE-------------------------------->
//include the library for the wifi arduino
#include <WiFiNINA.h>

char ssid[] = ""; //Network name SSID

char pass[] = ""; //Network password

int keyIndex = 0; //Only needed for WEP: Wired Equivalent Privacy is a security protocol,
//That standard is designed to provide a wireless local area network (WLAN) with a level 
//of security and privacy comparable to what is usually expected of a wired LAN.
int status = WL_IDLE_STATUS; //connection status 
WiFiServer server(80); //server socket

WiFiClient client = server.available(); 


void enable_WiFi(){
  //check for the WiFi module
  if(WiFi.status()== WL_NO_MODULE){
    Serial.println("Communication with WiFi module failed!");
  }
}
void connect_WiFi(){
  //attempt to connect to wifi network:
  while(status != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID:");
    Serial.println(ssid);
    //connect to WPA/WPA2 network. CHange this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    //wait 10 seconds for  connection

    delay(10000);
  }
}

void printWifiStatus(){
  //print the SSID of the network youre attached to:
  Serial.print("SSID:");
  Serial.println(WiFi.SSID());

  //print the board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address:");
  Serial.println(ip);

  //print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println("dBm");

  Serial.print("To see this page in action, open a browser to http://");
  Serial.print(ip);
}

//printWEB function declaration 

void printWEB(int avail){
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          
          //put in all the needed code
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          
          //logic for our project
          
          
          if(avail == 1){
            client.println("Available");
            
            }else{
              client.println("Not Available");
              }
          client.println("</html>");

          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
 }

//<-------------------------------WIFI CODE-------------------------------->


//to construct, need to have data stream going out of tx pin and into rx pin
DFRobot_mmWave_Radar hpSensor(&Serial1);

void setup() {
  
  
  //start serial monitor with baud rate needed for the human presence sensor
  Serial.begin(9600);
  Serial1.begin(115200);
  
  Serial.println("Loading...");

  //clear sensor to default settings
  hpSensor.factoryReset();

  //since reading a person at a desk, read up to 1m away <- can change
  hpSensor.DetRangeCfg(0, 1);  

  //look into this
  hpSensor.OutputLatency(0, 0);


  //<---Setup wifi----->
  enable_WiFi();
  connect_WiFi();
  server.begin();
  printWifiStatus();
  //<---Setup wifi----->

  Serial.println("\nStarting up");
  
  

}

void loop() {
  
  delay(1);
  
  
  
    //read the human presence sensor (1 -> detected, 0 -> nothing )
    reading = hpSensor.readPresenceDetection();
  
    // can be uncommented to see reading -> 
    Serial.println(reading);
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
        if(timer > 800){
        
         Serial.println("Seat Taken");
          avail = false;
          
          printWEB(0);
        }
    
        //if they read again add 100ms to the timer since that is the delay of the loop
        timer += 100;
    
        }else{
         //they are no longer detected so reset the timer
         timer = 0;

         //the seat is still available
         avail = true;
         printWEB(1);
       
     
        }
     }
    //the case for if someone was actually there, we want to hold the seat for 15 seconds <- can change
    else{
       //if someone is there, hold the false value
      if(reading == 1){
          avail = false;
          printWEB(0);

          //reset the time that it was being held
         holdTime = 0;
        
        }
      //the case where they are no longer detected
      else{

        //if they are gone for more than 15 seconds, seat is now free
        if(holdTime > 1500){
            Serial.println("Student Gone");
            avail = true;
            printWEB(1);
        }
        //time hasnt reached 15 seconds, hold
        else{
          Serial.println("Currently Holding seat");
          
            avail = false;
            printWEB(0);
          
        }
        //increase the time it has been held for
        holdTime += 100;
        
      }
      
    
  }
  
  
  
  
  //rerun every 100ms
  delay(100);

}
