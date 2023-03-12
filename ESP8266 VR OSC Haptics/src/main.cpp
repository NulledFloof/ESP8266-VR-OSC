#include <Arduino.h>
#include <ArduinoOSCWiFi.h>
#include <ESP8266WiFi.h>

const char* ssid     = "YourSSID";         // The SSID (name) of the Wi-Fi network you want to connect to.
const char* password = "YourPassword";     // The password of the Wi-Fi network.
const char* hostname = "ESP_OSC_VR";       // The name for the ESP.

//Physical IO
int buttonPin = D1;     //button
int outPin = D2;        //vibration motor

//OSC config variables
int OSCportR = 9002; //port where this device will be listening
int OSCportS = 9000; //port where this device will be sending
const char* OSCip = "YourComputer's IP"; //device to send OSC commands TO
const char* OSCpath = "/avatar/parameters/NoseBoop"; //OSC path for boolean haptics trigger. (Feel free to change "NoseBoop" to reflect your avatar settings).
const char* OSCpath1 = "/device/intensity";
const char* OSCpath2 = "/avatar/parameters/RaveGlasses"; //Used to toggle boolean state on avatar. (Feel free to change "RaveGlasses" to reflect your avatar settings).

//osc variables
float hap0 = 0; //haptics toggle state 
float inten = 100; //duty cycle, anything between 40-255 seems to work (@5v).
bool toggle = 0;

void setup() {
  Serial.begin(115200); //serial mon speed.
  delay(10);
  Serial.println('\n');
  //WiFi Stuff
  WiFi.hostname(hostname); //set hostname.
  WiFi.begin(ssid, password); // Connect to the network.
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");
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
  Serial.print("OSCPath(s):\n");
  Serial.println(OSCpath);
  Serial.println(OSCpath1);
  Serial.println(OSCpath2); //Feel free to expand this for easier reference through the serial monitor.   
  Serial.print("OSC receive Port: ");
  Serial.println(OSCportR); 
  Serial.print("OSC send Port: ");
  Serial.println(OSCportS);
  Serial.print("OSC send IP: ");
  Serial.println(OSCip);  
  Serial.println("");

  //Physical IO
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(outPin, OUTPUT);

  //osc stuff
  OscWiFi.subscribe(OSCportR, OSCpath, hap0); //receive haptic state
  OscWiFi.subscribe(OSCportR, OSCpath1, inten); //receive intensity state 
  OscWiFi.publish(OSCip, OSCportS, OSCpath2, toggle); //send toggle state
  
}

void loop() {
  OscWiFi.parse(); //Update any variables specified with OscWiFi.subscribe(); in setup.
  
  //Haptics receive
  if(hap0 == 1){
    analogWrite(outPin, inten);
    Serial.println("Trigger received!");
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

  delay(20);

}