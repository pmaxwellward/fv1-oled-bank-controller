## fv1-oled-bank-controller

Arduino sketch that drives a 128x64 SSD1306 OLED, reads a rotary encoder, selects FV-1 presets via 3 digital lines, switches external ROM banks by button, and outputs three PWM control signals from pots.

Credit goes to Hagiwo, who designed the circuit and whose source code this is based off of.   
https://note.com/solder_state/n/n1c402a78a0fe  
https://www.patreon.com/hagiwo  

presets can be found here:
https://mstratman.github.io/fv1-programs/

### Features
- Encoder navigation for 8 presets per bank
- Button to cycle 3 banks
- SSD1306 I2C UI with titles, subtitles, and 3 parameter labels
- Three PWM outputs from analog inputs
- 3-bit preset select lines for external hardware
- Preset label text stored in flash (PROGMEM)

### Hardware
- Arduino Uno or compatible ATmega328P board  
- SSD1306 128x64 OLED on I2C (address 0x3C)  
- Rotary encoder with push button  
- Three potentiometers for control inputs  
- External FV-1 hardware or selector that reads the 3-bit preset and bank control pins

### Pinout

| Role | Pin | Notes |
|---|---:|---|
| Encoder A | D2 | `Encoder myEnc(2,4)` |
| Encoder B | D4 |  |
| Bank button | D12 | `INPUT_PULLUP`, active low |
| Preset bit 0 | D7 | `bitRead(i,0)` |
| Preset bit 1 | D6 | `bitRead(i,1)` |
| Preset bit 2 | D5 | `bitRead(i,2)` |
| Bank control A | D8 | Toggled on bank change |
| Bank control B | D9 | Toggled on bank change |
| Status LED | D13 | Pulsed on bank change |
| PWM 0 | D11 | From `POT0` |
| PWM 1 | D10 | From `POT1` |
| PWM 2 | D3  | From `POT2` |
| I2C SDA | A4 | OLED |
| I2C SCL | A5 | OLED |
| Analog in 0 | A0 | Summed with A3 for `POT0` |
| Analog in 1 | A1 | Summed with A6 for `POT1` |
| Analog in 2 | A2 | Summed with A7 for `POT2` |
| Analog in 3 | A3 | Sums with A0 |
| Analog in 6 | A6 | Sums with A1 (analog only) |
| Analog in 7 | A7 | Sums with A2 (analog only) |

### Libraries
Install via Arduino Library Manager:
- Encoder (Paul Stoffregen)
- Adafruit GFX Library
- Adafruit SSD1306  
`Wire`, `avr/io`, and `avr/pgmspace` are part of the AVR core.

### Build and upload

**Arduino IDE**
1. Board: Arduino Uno  
2. Tools → Manage Libraries. Install the three libraries above.  
3. Open the sketch in `src/main.ino` 
4. Verify and Upload.

**PlatformIO**
```ini
; platformio.ini
[env:uno]
platform = atmelavr
board = uno
framework = arduino
lib_deps =
  adafruit/Adafruit SSD1306
  adafruit/Adafruit GFX Library
  paulstoffregen/Encoder
