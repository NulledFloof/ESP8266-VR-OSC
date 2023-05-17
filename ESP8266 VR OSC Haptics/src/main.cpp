#include <Arduino.h>
#include <ArduinoOSCWiFi.h>
#include <ESP8266WiFi.h>

float velocity(); //declare function

const char* ssid     = "SSID";         // The SSID (name) of the Wi-Fi network you want to connect to.
const char* password = "PASSWORD";     // The password of the Wi-Fi network.
const char* hostname = "ESP_OSC_VR";       // The name for the ESP.

//Physical IO
int buttonPin = D1;     //button
int outPin = D2;        //vibration motor

//OSC config
int OSCportR = 9002; //port where this device will be listening
int OSCportS = 9003; //port where this device will be sending
const char* OSCip = "192.168.77.105"; //device to send OSC commands TO

const char* OSCpath1 = "/device/intensity"; //set max haptic intensity, (float 0-255).
const char* OSCpathVelEn = "/device/HapVelEn"; //Enable or disable Variation of haptics, (bool)

const char* OSCpathHap0 = "/Hap0"; //OSC path to trigger haptics, (bool). default:/Hap0
const char* OSCpathHap0Pos = "/Hap0Pos"; //OSC path for CVR pointer location in the trigger (float). default:/Hap0Pos

const char* OSCpath2 = "/Toggle0"; //Used to toggle boolean state on avatar, (bool). default:/Toggle0

//osc variables
bool hap0 = 0; //OSC haptics enter or exit state
bool toggle = 0; //OSC pram. toggle
float inten = 120; //max duty cycle, (0-255)
bool VelEn = 1; //Enable or disable velocity of the haptics strength (For use with CVR and VOR; leave this true)
float Hap0Pos = 0.0; //CVRpointer postion of Haptic0, used to calc velocity.
float velocityCalc = 0.0;

void setup() {
  Serial.begin(115200); //serial mon speed.
  //IO
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(outPin, OUTPUT);
  //WiFi Stuff
  Serial.println('\n');
  WiFi.hostname(hostname); //set hostname.
  WiFi.begin(ssid, password); // Connect to the network.
  Serial.print("Connecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect.
    delay(1000);
    Serial.print('.');
  }
  //Wifi/Device info:
  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); 
  Serial.print("Hostname:\t");
  Serial.println(hostname); 
  //OSC info:
  Serial.print("OSCPath(s):\n");//Feel free to expand this for easier reference through the serial monitor.
  Serial.println(OSCpath1);
  Serial.println(OSCpathVelEn);
  Serial.println(OSCpathHap0);
  Serial.println(OSCpathHap0Pos);
  Serial.println(OSCpath2);
  Serial.print("OSC receive Port: ");
  Serial.println(OSCportR); 
  Serial.print("OSC send Port: ");
  Serial.println(OSCportS);
  Serial.print("OSC send IP: ");
  Serial.println(OSCip);  
  Serial.println("");

  //osc
  OscWiFi.subscribe(OSCportR, OSCpath1, inten); //receive intensity value
  OscWiFi.subscribe(OSCportR, OSCpathVelEn, VelEn); //enable or disable haptic velocity
  
  OscWiFi.subscribe(OSCportR, OSCpathHap0, hap0); //receive haptic state  
  OscWiFi.subscribe(OSCportR, OSCpathHap0Pos, Hap0Pos);
  
  OscWiFi.publish(OSCip, OSCportS, OSCpath2, toggle); //send toggle state
  
}

void loop() {
  OscWiFi.parse(); //Update any variables specified with OscWiFi.subscribe(); in setup.
  
  //Haptics
  if(hap0 == 1){ //When pointer enters trigger.
    
    if(VelEn == 1){ //Velocity enable or disable
      velocityCalc = velocity(); //sore calculated velocity to prevent calling function twice for serial.print
      Serial.print("Velocity: ");
      Serial.println(velocityCalc);

      analogWrite(outPin, velocityCalc);
    }
    else{
      analogWrite(outPin, inten);
    }

  }
  else{
    digitalWrite(outPin, LOW);
  }

  //Toggle state send
  if(digitalRead(buttonPin) == LOW){
    Serial.print("Toggle: ");
    Serial.println(toggle);
    OscWiFi.post(); //Send any variables specified with OscWiFi.publish(); in setup.
    toggle = !toggle;
    delay(500);
  }

  delay(5);
}

float velocity() {
  float pos1 = Hap0Pos;
  delay(20); //you can increase sample rate (ms) by lowering it and changing "t" in the equation respectively 
  OscWiFi.parse();
  float pos2 = Hap0Pos;

  //velocity = absolute value of the change in distance / time.
  float Vel = abs((pos2-pos1)/0.02)*100; //*100 to move the decimal. assuming OSC position value received is 0.000-1.000
  Vel = map(Vel,0,200,0,inten); //map value to maximum intensity set. map(value, fromLow, fromHigh, toLow, toHigh)

  return Vel;
}