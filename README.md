# Animated (Mini) Moonboard BLE LED System

## Purpose
This is an experimental software implementing a 
BLE (Mini) Moonboard LED system featuring vivid and
animated LED highlighting plus pre-defined
"animations".
"Experimental" means that while the LED animations 
are working quite well the BLE connection is 
unstable and problem highlighting is unreliable.
The overall experience might be quite annoying 
for users.

# When to use

As this is experimental software and I don't have 
too much time to support you, I recommend the following:

* Only try the software if you are an experienced IT professional/ programmer etc.
* It helps if you have an existing DIY LED system and you are ready and able 
to adapt this software in order to try it out on your system.

# How to use

Just a short summary for now:

* Power on LED system
* Open the Moonboard app.
* Connect to LED system in your app.
* Select problems
* Use special mode (see below)

## Special mode

The special mode is used to display the hidden/
built in animations instead of a problem.

The special mode is active if only exactly
one hold is highlighted on the moonboard.

* In the app, select "new problem" (Plus symbol)
* Select a "special mode" hold in the problem view
* Watch animation
* To leave animation
** Either: toggle the hold until unselected, then select another special mode hold
** or: Select more holds to edit a new problem
** or: leave problem editor and select another problem

Special mode holds are currently:
TBD, see code ;-) I need to translate the holds from my
layout to generic Mini Moonboard layout first...

# Known issues

## Works better if "v2 LED system" is used.
With latest app versions it almost exclusively
works only if you select "v2 LED system" in the
app settings.

## Unstable BLE connection
BLE connection frequently cannot be established
or gets lost again unexpectedly.

## Problem highlighting gets stuck
Highlighting problems sometimes gets "stuck".
If you swipe through problems of tap/ select 
new problems they are sometimes not highlighted.
A workaround is to continue selecting new problems
until suddenly it works again.
It appears almost as if the problem data gets 
stuck in some kind of queue or buffer which is 
only flushed or released later.
Pretty annoying tbh.

## Where to get help
I must be honest here. This is a hobby/ fun project
I created when I had more free time.

## How to build
### Manual build in the Arduino IDE
* Install the latest Arduino IDE
* Use the library manager of the IDE to install the libraries required (see below)
* Use the board manager to install dependencies for the board you will be using (e.g. Arduino Nano IoT 33)
* Select the board (e.g. Arduino Nano IoT 33) either in the dropdown or in the main menu Tool-->Board
* Click Verify => Everything should be fine
* Click Upload to flash your board
* Try it out

### Dependencies

Libraries:
* FastLED: 
    https://github.com/FastLED/FastLED
* HardwareBLWSerial: 
    https://github.com/Uberi/Arduino-HardwareBLESerial

Hardware used:
- Arduino Nano 33 IoT
- Level shifter 74AHCT125 to convert the 3.3v Arduino Output to 5V for the Neopixel LEDs
- PL9823-F5 (compatible to WS2812B) Neopixel LED chain

## How to contribute

TBD, try to contact me via github.
