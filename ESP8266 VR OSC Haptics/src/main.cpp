#include <Arduino.h>
#include <ArduinoOSCWiFi.h>
#include <ESP8266WiFi.h>
#include <Adafruit_DRV2605.h>

//declare functions
float velocity(double time); 
void thermalRead();
void haptics();
void drvHaptics();
void toggleSend();

const char* ssid     = "";         // The SSID (name) of the Wi-Fi network you want to connect to.
const char* password = "";     // The password of the Wi-Fi network.
const char* hostname = "ESP_OSC_VR";       // The name for the ESP.

//Physical IO
int thermal = A0;       //thermal sensor
int buttonPin = D5;     //button
int outPin = D6;        //vibration motor

//OSC config
int OSCportR = 9002; //port where this device will be listening
int OSCportS = 9000; //port where this device will be sending
const char* OSCip = "192.168.77.105"; //device to send OSC commands TO

const char* OSCpath1 = "/avatar/parameters/HapticIntensity"; //set max haptic intensity, (float 0-255).
const char* OSCpathHap0Pos = "/avatar/parameters/HapticsHead0Position"; //OSC path for interaction location/distance in the trigger/contact (float). default:/Hap0Pos

const char* OSCpath2 = "/avatar/parameters/Bread"; //Used to send toggle boolean state on avatar, (bool). default:/Toggle0
const char* OSCpath3 = "/avatar/parameters/blush"; //Used to send temperature values.

//global variables
bool toggle = 0; //OSC pram. toggle
float inten = 200; //max duty cycle, (0-255)
float Hap0Pos = 0.0; //Position of the game interaction.
int velCalc = 0;
int gain = 3;
int tempRaw = 0; //raw value of temp
float temp = 0.0; //scaled temp. 0-1

unsigned long startMillis;  //used to compare elapsed time.
unsigned long currentMillis;

//drv
Adafruit_DRV2605 drv;

void setup() {
  Serial.begin(115200); //serial mon speed.
  //IO
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(outPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(thermal, INPUT);
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
  Serial.println(OSCpath2);
  Serial.println(OSCpath3);
  Serial.print("OSC receive Port: ");
  Serial.println(OSCportR); 
  Serial.print("OSC send Port: ");
  Serial.println(OSCportS);
  Serial.print("OSC send IP: ");
  Serial.println(OSCip);  
  Serial.println("");

  //osc
  OscWiFi.subscribe(OSCportR, OSCpath1, inten); //intensity set value
  OscWiFi.subscribe(OSCportR, OSCpathHap0Pos, Hap0Pos); //Hatpic position

  //DRV
  drv.begin();
  //Run a boot buzz/tone
  drv.selectLibrary(1);
  drv.setMode(DRV2605_MODE_INTTRIG);
  drv.setWaveform(0, 12);  // play effect 
  drv.setWaveform(1, 0);   // end waveform
  drv.go();                // play the effect!

  delay(1000); //pause before running OSC commands

  drv.setMode(DRV2605_MODE_REALTIME); //set it to realtime mode

}

void loop() {
  OscWiFi.update(); //Update any variables specified with subscribeand publish.

  //---choose between using the PWM or the DRV over I2C---
  //haptics(); //for normal PWM output
  drvHaptics(); //for DRV
  toggleSend(); //reads button input 

  //---Uncomment to read an analog value via the ADC on the ESP8266, sends it over OSC---
  //currentMillis = millis(); //store elapsed time (for temp sens)
  //thermalRead();

}

float velocity(double time) { //General Use for CVR and VRC
  float pos1 = Hap0Pos; 
  delay(time); 
  OscWiFi.parse();
  float pos2 = Hap0Pos;
  float Vel = abs(pos1-pos2)/(time/1000); //Gives us a velocity value
  Vel *= 100; //Moving the decimal right 2 places to preseve the smaller values

  return Vel;
}

void thermalRead(){
//Thermal Sensor
  if(currentMillis - startMillis >= 1000){
    tempRaw = constrain(analogRead(thermal),400, 450); //limits the raw ADC value read to be between 400(warmer) - 500(colder)
    Serial.print(analogRead(thermal));
    Serial.print(",");

    temp = (map(tempRaw,400,450,100,0)*0.01); //inverts/maps the raw value
    OscWiFi.send(OSCip, OSCportS, OSCpath3, temp); //Send val over OSC
    
    Serial.print(temp);
    Serial.print("\n");

    startMillis = currentMillis;
  }
}

void haptics(){
  //Haptics
  velCalc = velocity(20); //sample time in ms (20 default)
  if(velCalc > 0){ 
    Serial.print("Raw: ");
    Serial.println(velCalc);
    velCalc = map(velCalc,0,500,30,inten) * gain;
    
    Serial.print("Map: ");
    Serial.println(velCalc);

    analogWrite(outPin, velCalc);
    digitalWrite(LED_BUILTIN, LOW);

  }
  else{
    digitalWrite(outPin, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void drvHaptics(){
  //Haptics
  velCalc = velocity(20); //sample time in ms (20 default)

  if(velCalc > 0){ 
    Serial.print("Raw: ");
    Serial.println(velCalc);
    velCalc = map(velCalc,0,500,10,inten);
    
    Serial.print("Map: ");
    Serial.println(velCalc);

    drv.setRealtimeValue(velCalc);
    digitalWrite(LED_BUILTIN, LOW);

  }
  else{
    drv.setRealtimeValue(0x00);
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void toggleSend(){
  //Toggle state send
  if(digitalRead(buttonPin) == LOW){
    Serial.print("Toggle: ");
    Serial.println(toggle);
    OscWiFi.send(OSCip, OSCportS, OSCpath2, toggle); //Send toggle state
    toggle = !toggle;
    delay(500);
  }
}