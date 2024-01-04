#include <Arduino.h>
#include <ArduinoOSCWiFi.h>
#include <ESP8266WiFi.h>

float velocityCVR(); //declare functions
float velocity(double time); 

const char* ssid     = "";      // The SSID (name) of the Wi-Fi network you want to connect to.
const char* password = "";     // The password of the Wi-Fi network.
const char* hostname = "ESP_OSC_VR";

//Physical IO
int outPin = D2;        //vibration motor

//OSC config
int OSCportR = 9002; //port where this device will be listening

const char* OSCpath1 = "/device/intensity"; //set max haptic intensity, (float 0-255).
const char* OSCpathHap0Pos = "/avatar/parameters/HapticsHead0Position"; //OSC path for interaction location/distance in the trigger/contact (float). default:/Hap0Pos


//osc variables
float inten = 200; //Max haptic strength (duty cycle is from 0-255)
float Hap0Pos = 0.0; //Position of the game interaction.
int gain = 2.7; //Tweak this for haptic gain (Note: Too much will result in less variation)
int velCalc = 0; //creates variable (ignore)

void setup() {
  Serial.begin(115200); //serial mon speed.
  //IO
  pinMode(outPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  //WiFi Stuff
  Serial.println('\n');
  WiFi.hostname(hostname); //set hostname.
  WiFi.begin(ssid, password); // Connect to the network.
  Serial.print("Connecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect.
    delay(1500);
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
  Serial.println(OSCpathHap0Pos);
  Serial.print("OSC receive Port: ");
  Serial.println(OSCportR); 
  Serial.println("");

  //osc
  OscWiFi.subscribe(OSCportR, OSCpath1, inten); //receive intensity set value
  OscWiFi.subscribe(OSCportR, OSCpathHap0Pos, Hap0Pos); //Hatpic position
  
  
}

void loop() {
  OscWiFi.update(); //Update any variables specified with OscWiFi.subscribe(); in setup.
  
  velCalc = velocity(20); //sample time in ms (20 default)

  //Haptics
  if(velCalc > 0){ 
    //uncomment for debug
    //Serial.print("Raw: ");
    //Serial.println(velCalc);

    velCalc = map(velCalc,0,500,30,inten) * gain;
    
    //Serial.print("Map: ");
    //Serial.println(velCalc);

    analogWrite(outPin, velCalc);
    digitalWrite(LED_BUILTIN, LOW);

  }
  else{
    digitalWrite(outPin, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
  }

  delay(5);

}

float velocityCVR() { //CVR values are absolue postion. (testing)
  float pos1 = Hap0Pos*0.2; //*0.2 Assuming OSC position value received is 0.000-1.000 and IRL trigger size is ~200mm (converting to meters)
  delay(20); //you can increase sample rate (ms) by lowering it AND changing "t" (in seconds) in the equation below respectively 
  OscWiFi.parse();
  float pos2 = Hap0Pos*0.2;
/*
  Assuming the size of the trigger is ~200mm IRL or 0.2 meters
  Note: velocity = The change in distance (m) / change in time. (s)
*/
  float Vel = abs(pos1-pos2)/0.02; //Gives us value in m/s
  Vel *= 100; //Moving the decimal right 2 places to preseve the smaller values
  Vel = map(Vel,0,125,0,inten); //map value to maximum intensity set. map(value, fromLow, fromHigh, toLow, toHigh) (headpats should be around 1.23 M/S at most)

  return Vel;
}

float velocity(double time) { //General Use for CVR and VRC
  float pos1 = Hap0Pos;
  delay(time); 
  OscWiFi.parse();
  float pos2 = Hap0Pos;
/*
  Note: velocity = The change in distance (m) / change in time. (s)
*/
  float Vel = abs(pos1-pos2)/(time/1000); //Gives us a velocity value
  Vel *= 100; //Moving the decimal right 2 places to preseve the smaller values

  return Vel;
}
