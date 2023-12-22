This is an automatic translation, may be incorrect in some places. See sources and examples!

# Gyverpower
Gyverpower - Library for Energy Executive Management MK AVR
- System shock management
- Turning on/off the periphery:
    - Bod
    - Tiemers
    - i2c/uart/spi
    - USB
    - ADC
- sleep in different modes (list below)
- sleep for any period
    - Tiemer calibration for the exact time of sleep
    - adjustment Millis ()

## compatibility
- Atmega2560/32U4/328
- Attiny85/84/167

### Documentation
There is [expanded documentation] to the library (https://alexgyver.ru/gyverpower/)

## Content
- [installation] (# Install)
- [initialization] (#init)
- [use] (#usage)
- [Example] (# Example)
- [versions] (#varsions)
- [bugs and feedback] (#fedback)

<a id="install"> </a>
## Installation
- The library can be found by the name ** gyverpower ** and installed through the library manager in:
    - Arduino ide
    - Arduino ide v2
    - Platformio
- [download the library] (https://github.com/gyverlibs/gyverpower/archive/refs/heads/main.zip) .Zip archive for manual installation:
    - unpack and put in * C: \ Program Files (X86) \ Arduino \ Libraries * (Windows X64)
    - unpack and put in * C: \ Program Files \ Arduino \ Libraries * (Windows X32)
    - unpack and put in *documents/arduino/libraries/ *
    - (Arduino id) Automatic installation from. Zip: * sketch/connect the library/add .Zip library ... * and specify downloaded archive
- Read more detailed instructions for installing libraries [here] (https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%BD%D0%BE%BE%BE%BED0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Update
- I recommend always updating the library: errors and bugs are corrected in the new versions, as well as optimization and new features are added
- through the IDE library manager: find the library how to install and click "update"
- Manually: ** remove the folder with the old version **, and then put a new one in its place.“Replacement” cannot be done: sometimes in new versions, files that remain when replacing are deleted and can lead to errors!


<a id="init"> </a>
## initialization
No

<a id="usage"> </a>
## Usage
`` `CPP
VOID Hardwareenable (Uint16_T DATA);// inclusion of the specified periphery (see below the "peripheral constants")
VOID HardwareDisable (Uint16_T DATA);// Turning off the specified periphery (see below the "Constant of the Periphery")
VOID SetSystemprescaler (Prescalers_t PressCaler); // Installation of a system frequency divider (see below "Delichela Constant)
VOID Adjustinternalclock (Int8_T Adj);// adjustment of the frequency of the internal generator (number -120 ...+120)

VOID BODINSLEP (BOOL EN);// Brown -out Detector in sleep mode (True vkl - False cap) by the silence.Disconnected!
VOID setsleepmode (Sleepmodes_t Mode);// Installation of the current sleep regime (see below "Sleep modes")
VOID setsleepResolution (Uint8_t Period);// Set a resolution of sleep (see below the "periods of sleep")

VOID autocalibrate (VOID);// Automatic calibration of sleep timer, 16 ms is performed
VOID Sleep (Uint8_t Period);// sleep for a fixed period (see below "periods of sleep")
Uint8_t SleepDelay (Uint32_T MS);// Sleep to arbitrary peRiodes in milliseconds (up to 52 days), returns the rest of the time to correct timers
VOID Correctmillis (Bool State);// Correct millis for sleep time SleepDelay () (by default on)
VOID Wakeup (VOID);// Helps to leave SleepDelay an interruption (call in a waking interruption)
Bool Insleep (Void);// will return True if MK sleeps (for verification in interruption)
`` `

`` `CPP
===== Sleep modes for setsleepmode () =======
Idle_sleep - light sleep, only a CPU and Flash clock is turned off, it wakes up instantly from any interruption
ADC_SLEP is a light dream, CPU and System Clock are disconnected, ADC begins to transform it when leaving (see Adcinsleep example)
Extstandby_sleep - deep sleep, identical to PowerSave_Sleep + System Clock is active
Standby_sleep - deep sleep, identical to PowerDown_Sleep + System Clock is active
PowerSave_Sleep - deep sleep, identical to powerdown_sleep + timer 2 is active ( + you can wake up from its interruptions), you can use time (see example of powerSavemillis)
PowerDown_Sleep - the deepest sleep, everything is turned off except WDT and external interruptions, wakes up from hardware (ordinary + PCINT) or WDT

===== Sleep periods for Sleep () and setsleepresolution () ===========
Sleep_16ms
Sleep_32MS
Sleep_64ms
Sleep_128MS
Sleep_256MS
Sleep_512MS
Sleep_1024ms
Sleep_2048MS
Sleep_4096ms
Sleep_8192MS
Sleep_Forever - Eternal Dream

===== The divider constant for setsyStemprescaler () ===========
PressCaler_1
PressCaler_2
PressCaler_4
PressCaler_8
PressCaler_16
PressCaler_32
PressCaler_64
PressCaler_128
PressCaler_256

===== The periphery constants for Hardwareedisable () and Hardwareenable () ==========
Pwr_all - all iron
PWR_ADC - ACP and comparator
PWR_Timer1 - timer 0
PWR_Timer0 - TIMER 1
PWR_Timer2 - TIMER 2
PWR_Timer3 - TIMER 3
PWR_Timer4 - TIMER 4
PWR_Timer5 - TIMER 5
PWR_UART0 - Serial 0
PWR_UART1 - Serial 1
PWR_UART2 - Serial 2
PWR_UART3 - Serial 3
PWR_I2C - Wire
PWR_SPI - SPI
PWR_USB - USB
PWR_USI - Wire + SPI (Attinyxx)
PWR_LIN - USART LIN (Attinyxx)
`` `

### simple sleep
- Sleep mode is tuned in `Power.setsleepmode ()`, by default, `PowerDown_Sleep` is active (the rest see above).
- To fall asleep, we call `Power.sleep ()` with one of the standard periods (see above).
- The real time of sleep will be slightly different, since the "sleep timer" is not very accurate.

### sleep for any period
- Sleep mode is tuned in `Power.setsleepmode ()`, by default, `PowerDown_Sleep` is active (the rest see above).
- To fall asleep, we call `Power.sleepDelay ()` with a period in milliseconds (`uint32_t`, up to ~ 50 days).
*How it works?Just a cycle with standard periods of sleep inside this function.*
- By default, this function “sleeps” with periods of 128 milliseconds.The wakefulness between the periods of sleep is about 2.2 μs (at 16 MHz),
which is 0.0017% of the time of sleep.Accordingly, the accuracy of the time of sleep is multiple of one period of sleep.This period can be configured in
`Power.SetSleepResolution ()`, which accepts the same constants as `Sleep ()`.If you need a more accurate dream, you can put 16 ms (`Sleep_16MS`),
If the maximum energy conservation is 8 seconds (`Sleep_8192MS`).
- For premature awakening for interruption, it is necessary to call `Power.wakeup ()` inside the interruption processor.
- Sleep `SleepDelay ()` has two very useful opportunities:
  - sleep for a very accurate period with a calibrated timer (see below)
  - Preservation of the time account `millis ()` during sleep (see example of Sleeptime)

### Tymer Calibration
In version 2.0 of the library, the calibration was simplified: it is enough to call `Power.autocalibrate ()` when the microcontroller is launched.The function is performed ~ 16 ms.
**Attention!Power.SetsleepResolution () should be called after calibration of the timer. **

<a id="EXAMPLE"> </a>
## Example
The rest of the examples look at ** Examples **!
`` `CPP
// Demo of the library capabilities
#include <gyverpower.h>

VOID setup () {
  Pinmode (13, output);// set up inCranberries with LED output
  Serial.Begin (9600);

  Power.autocalibrate ();// Automatic calibration

  // Disconnecting unnecessary periphery
  Power.hardwardedisable (pwr_adc | pwr_timer1);// see Constant section in gyverpower.h, separating the sign "|

  // System frequency management
  Power.SetSySteMprescaler (Prescaler_2);// cm constants in gyverpower.h
  
  // Sleep settings
  Power.Setsleepmode (Standby_Sleep);// If you need another sleep mode, cm constants in gyverpower.h (by default PowerDown_Sleep)
  //power.BodinSleep (False);// It is recommended to turn off BOD in a dream to preserve energy (by default FALSE - already turned off !!)

  // Example of single care of sleep
  Serial.println ("Go to Sleep");
  DELAY (100);// Give the time to send
  
  Power.sleep (Sleep_2048MS);// Sleep ~ 2 seconds
  
  Serial.println ("Wake Up!");
  DELAY (100);// Give the time to send
}

VOID loop () {
  // example of cyclic sleep
  Power.SleepDelay (1500);// Sleep 1.5 seconds
  DigitalWrite (13,! DigitalRead (13));// Inverting the condition on the pin
}
`` `

<a id="versions"> </a>
## versions
- V1.2 - Calibration Fix
- V1.3 - Fix for 32U4
- v1.4 - Adjustinternalclock added
- V1.5 - compatibility with Attini
- V1.6 - more compatibility with Attini
- V1.7 - optimization, compatibility with Attiny13
- V1.8 - Compatibility with atmega32u4
- V2.0 - memory optimization, redepdlavia remoded, you can definitely find out the actual sleep time
- V2.0.1 - FIX Compiler Warnings
- v2.0.2 - Fixed compilation error attiny85
- V2.1 - Added Bool Insleep (), whether MK sleeps for verification
- V2.2 - Improved stability

<a id="feedback"> </a>
## bugs and feedback
Create ** Issue ** when you find the bugs, and better immediately write to the mail [alex@alexgyver.ru] (mailto: alex@alexgyver.ru)
The library is open for refinement and your ** pull Request ** 'ow!


When reporting about bugs or incorrect work of the library, it is necessary to indicate:
- The version of the library
- What is MK used
- SDK version (for ESP)
- version of Arduino ide
- whether the built -in examples work correctly, in which the functions and designs are used, leading to a bug in your code
- what code has been loaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed.Not a canvas of a thousand lines, but a minimum code