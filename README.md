# ESP8266 OSC Haptics
A quick and cheap way to add haptics to your VR headset or body for social VR apps. I've designed this to work with ChilloutVR (with [kafeijao’s](https://github.com/kafeijao/Kafe_CVR_Mods/tree/master/OSC) OSC mod) and VRChat. It's more or less a "native" approach, utilizing OSC integration in these social VR apps.   

----

## Hardware
**ESP8266:** You can use any ESP8266-based dev board, I've used this with a NodeMCU v2 (ESP-12E module).

**Vibration Motor:** The ESP simply sends out a PWM signal, so you have the option to use a “Vibration motor module” **or** save a few bucks and make your own.

- Module: Simply connect its power (VCC), ground (GND), and input (SIG) respectively to your ESP dev board.

![Haptics Module](https://github.com/NulledFloof/ESP8266-VR-OSC/blob/main/Images/Vibration%20Motor%20Module.jpg)

- DIY: All you need is a circular vibration motor, an NPN transistor, and two resistors (330R & 10k).

![Shematic](https://github.com/NulledFloof/ESP8266-VR-OSC/blob/main/Images/Circuit.png)

**3D Printed mount:** 
You can my HTC Vive (2016) DAS mount for the NodeMCU board on [Thingiverse!](https://www.thingiverse.com/thing:5910348)
For other HMDs, you can modify existing cases for the ESP8266. 

## Firmware Setup
Using the PlatformIO add-on for Visual Studio Code, import the project folder, and open `main.cpp`

Configure the AP and the IP of your PC.
- `ssid` and `password`, will be the name and password of your wifi AP. 

In general VRC and CVR sends out a lot of data that overwhelms the OSC Router, so we will need to use [VRChat Open Sound Control Router](https://github.com/SutekhVRC/VOR) by SutekhVRC, at the moment we can't remap OSC addresses like OSC Router so you will have to plugin the full OSC address for the avatar (which we will create in **Avatar Setup**). 

- `OSCpathHap0Pos` will be the OSC address for the position of the pointer in our contact reveiver/trigger.
Here's an example: 
![ESP_Firmware_OSC_Paths](https://github.com/NulledFloof/ESP8266-VR-OSC/blob/main/Images/ESP_Firmware_OSC_Paths.png)

Finally, upload the firmware!


## Avatar Setup
### CVR
Adding a haptics trigger:
We are going to add two AAS (Advanced Avatar Settings) entries:
- **First**, add an entry to your Advanced Avatar settings list and give it a name with no spaces. The `Type` field should be set to "Game Object Toggle" and `Generate Type` should be set to "Generate Bool". Leave the `Default` check box unselected. You can use the `Use Animation` or `GameObjects` options to add an animation or object to give other in-game users visual confirmation that the haptics are working. 
- Add another entry with the same name as before but add "position" to the end of it. The `Type` field should be set to "Input Single" and `Default` should both be set to "0". 

![AAS_Entries](https://github.com/NulledFloof/ESP8266-VR-OSC/blob/main/Images/AAS_Entries.png)
- **Make sure to re-create and attach your Animator.** This will show up as a toggle and vector within your AAS menu in-game and will also be read by the OSC mod.
- **Second**, add the "CVR Advanced Avatar Settings Trigger" script to the bone of your choosing. Set the position and scale the area accordingly.

![AAS_Trigger Placement](https://github.com/NulledFloof/ESP8266-VR-OSC/blob/main/Images/AAS_Trigger_Placement.png)
- Make sure "Enable Advanced Mode" and "Network Interactable" are selected. Set the `Setting Name` For both the “On Enter” and “On Exit” Trigger to the first Advanced Avatar Settings entry we made. 
- `Setting Value` for the “On Enter Trigger” should be set to 1 and the “On Exit Trigger” should be set to 0. 
- `Update Method` should be set to “Override” for both “On Enter” and “On Exit” Triggers. 
- Next, under "On Stay Trigger" set `Setting Name` to the second AAS entry with "position" at the end of its name. `Update Method` should be set to "Set From Position". `Min Value` should be set to "0" and `Max Value` should be set to "1".
- Under `Sample Direction` set it to either `X-Positive`, `Y-Positive`, or `Z-Positive`depending on where you place your trigger. (I have my trigger set to the X direction for side-to-side movement).

![AAS_Trigger](https://github.com/NulledFloof/ESP8266-VR-OSC/blob/main/Images/AAS_Trigger.png)


### VRC
(Will be added in the future, Not yet tested)


## Software
**(Not recommended)**  but if you choose to download and run ETC Labs [OSC Router](https://github.com/ETCLabs/OSCRouter):

For any OSC traffic exiting the game (haptics) enter the `Incoming IP` as "127.0.0.1" with a port of "9001". `Outgoing IP` will be the local IP address of the ESP board and set the port to "9002".

Now we want to remap paths. Enter the OSC path for the avatar on the incoming side with the name of the trigger. Example: "/avatar/parameters/<your_trigger>". For the outgoing side, set it to "/Hap0".

**Optional**, If you are sending OSC commands to your game (e.g. push button), set the `Incoming IP` as the ESP's IP and port "9003" and the path as "/Toggle0". `Outgoing IP` should be "127.0.0.1'' port "9000" with your  AAS game object toggle. Example: "/avatar/parameters/<your_game_object>"

**Note:** If OSC Router is having trouble receiving commands, Windows Firewall may be blocking traffic for OSC Router.

![Example_OSC_Router](https://github.com/NulledFloof/ESP8266-VR-OSC/blob/main/Images/OSCRouterSnip.png)

**Recommended (Use this)**, Download and install [VRChat Open Sound Control Router](https://github.com/SutekhVRC/VOR) by SutekhVRC.  
Once installed; underneath the `Apps` tab add an app and name it something like "ESP".
- `App Host` will be the IP of the ESP (Shown by the serial monitor in PlatformIO)
- `App Port` will be the port of the ESP, `9002` is default set by it's firmware. 

![VOR_Device](https://github.com/NulledFloof/ESP8266-VR-OSC/blob/main/Images/VOR_Device.png)

Hit Save and navigate to the `PF` tab.
- Enable `OSC Packet Filter` and `Whitelisting`
- Add one entry. For the OSC address, it should be `/avatar/parameters/(Your AAS Haptics Name)`. These names should match up with what you setup in your AAS or parameters in Unity.
- Note: You may want to either disable this or add entries for VRCFT if you are using it alongside with the haptics.

![VOR_Packet_Filtering](https://github.com/NulledFloof/ESP8266-VR-OSC/blob/main/Images/VOR_Packet_Filtering.png)

----

## Debugging
There are a lot of moving parts to this, so here are a few programs that will help you:
- [Protokol](https://hexler.net/protokol) is great for receiving OSC data. 
- [SendOSC](https://github.com/yoggy/sendosc) is a command line app for sending OSC data. 

Other notes:
- Pay attention to the data type being sent and received, they have to be the same (bool/float)
- OSC programs cannot use the same port at the same time.

