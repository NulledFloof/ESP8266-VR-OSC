# ESP8266-VR-OSC
A quick and cheap way to add haptics to your VRheadset or body for social VR apps. I've designed this to work with ChilloutVR with respect to [kafeijao’s](https://github.com/kafeijao/Kafe_CVR_Mods/tree/master/OSC) OSC mod. Should work just fine with other social VR apps like VRchat.


## Hardware
**ESP8266:** You can use any ESP8266 based dev board, I've tested this with a NodeMCU v2 (ESP-12E module).

**Vibration Motor:** The ESP simply sends out a PWM signal, so you have the option to use a “Vibration motor module” **or** save a few bucks and make your own.

- Module: Simply connect  its power, ground and input respectively to your ESP dev board.

- DIY: All you need is a circular vibration motor, NPN transistor, and two resistors (330R & 10k).

![screenshot](https://github.com/NulledFloof/ESP8266-VR-OSC/blob/main/Images/Circuit.png)

**Push button:** Use a normally open push button. Connect one pin to GND and the other to the designated pin on the dev-board.


## Code Setup
Using the PlatformIO add-on for Visual Studio Code, import the project folder, and open `main.cpp`

From line 9-15, edit the following constant characters:
- `ssid` and `password`, this will be the name and password of your wifi AP.
- `OSCip` This will be the IP address of the computer running the VR application.

**Changing of these OSC paths won't be necessary as you can remap these with ETC labs OSC router.**


## Avatar
### CVR
You will need to edit your avatar.
- First, add an entry to your Advanced Avatar settings list and give it a name with no spaces. `Type` should be set to "Game Object Toggle" and `Generate Type` should be set to "Generate Float". `Default` and `Use Animation` should be left unselected. You can leave the "GameObjects'' list empty. Make sure to re-create your Animator. This will show up as a toggle within your advanced avatar settings menu in-game and will also be read with the OSC mod.
- Second, add the "CVR Advanced Avatar Settings Trigger" script to the bone of your choosing. Set the position and scale the area accordingly, make sure "Enable Advanced Mode" and "Network Interactable" are selected. Set the `Setting Name` For both “On Enter” and “On Exit” Trigger to the Advanced Avatar Settings name we just made. `Setting Value` for the “On Enter Trigger” should be set to 1 and 0 for the “On Exit Trigger”. `Update Method` should be set to “Override” for both “On Enter” and “On Exit” Triggers.

### VRC
(Not yet tested)


## Software
You will need to download and run ETC Labs [OSC Router](https://github.com/ETCLabs/OSCRouter)

For any OSC traffic exiting the game (haptics) enter the `Incoming IP` as "127.0.0.1" with a port of "9001". `Outgoing IP` will be the local IP address of the ESP board and set the port to "9002".

Now we want to remap paths. Enter the OSC path for the avatar on the incoming side with the name of the trigger. Example: "/avatar/parameters/<your_trigger>". For the outgoing side, set it to "/Hap0".

If you are sending OSC commands to your game, set the `Incoming IP` as the ESP's IP and port "9003" and the path as "/Toggle0". `Outgoing IP` should be "127.0.0.1'' port "9000" with your avatar's game object toggle Example: "/avatar/parameters/<your_game_object>"

**Note:** If OSC Router is having trouble receving commands, windows firewall may be blocking traffic for OSC Router.

![screenshot](https://github.com/NulledFloof/ESP8266-VR-OSC/blob/main/Images/OSCRouterSnip.png)



