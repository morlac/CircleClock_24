#define DEBUG

// http://playground.arduino.cc/Code/time

#include <avr/sleep.h>
#include <avr/power.h>

/*
ISR(TIMER1_OVF_vect) {
  return;
}
*/

// http://www.nongnu.org/avr-libc/user-manual/group__avr__eeprom.html
//#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

#include <Wire.h>
#include <Time.h>
#include <TimeLib.h>
#include <Timezone.h>
#include <DS1307RTC.h>

//#include "FastSPI_LED2.h"
#include <FastLED.h>
#define NUM_LEDS 24
struct CRGB leds[NUM_LEDS];

static time_t utc, current_time;

/*
int16_t TimeZone;
uint16_t TimeZoneAddr = 1;
*/

boolean BrightnessChanged = false;
uint8_t Brightness = 0x10;
uint8_t NewBrightness = 0x10;

const uint8_t ButtonUpPin = 10;
const uint8_t ButtonDownPin = 3;

uint8_t ButtonUpState = HIGH; // equals to "not pressed"
uint8_t ButtonDownState = HIGH;

uint8_t LastButtonUpState = HIGH;
uint8_t LastButtonDownState = HIGH;

#define LED_MAX_COUNT 2
uint8_t led_count = 0;
bool led_on = false;

/*
TimeChangeRule euCEST = {"CEST", Last, Sun, Mar, 2, +120};
TimeChangeRule euCET = {"CET", Last, Sun, Oct, 2, +60};

Timezone euDE(euCEST, euCET);
*/

Timezone euDE(100);

/**
 * 
 */
void setup() {
  pinMode(ButtonUpPin, INPUT_PULLUP);
  pinMode(ButtonDownPin, INPUT_PULLUP);

/*-*
  // is only run once for first EEPROM-init
  while (!eeprom_is_ready());
  eeprom_write_word((uint16_t*)TimeZoneAddr, 7200);
*/

  Serial.begin(9600);

  LEDS.setBrightness(Brightness);
  LEDS.addLeds<WS2812, 12, GRB>(leds, NUM_LEDS);//->clearLeds(300);

  Wire.begin();
  
  setSyncProvider(RTC.get);
  
//  RTC.begin();
/*
  if (!RTC.isrunning()) {
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
*/

  TWBR=12;

  power_adc_disable();
  power_spi_disable();
  power_timer1_disable();
  power_timer2_disable();
  
  /* Normal timer operation.*/
//  TCCR1A=0x00; 

  /* Clear the timer counter register.
   * You can pre-load this register with a value in order to 
   * reduce the timeout period, say if you wanted to wake up
   * ever 4.0 seconds exactly.
   */
//  TCNT1=0x0000; 

  /* Configure the prescaler for 1:1024, giving us a 
   * timeout of 4.09 seconds.
   */
//  TCCR1B=0x03;

  /* Enable the timer overlow interrupt. */
//  TIMSK1=0x01;

/*
  // run only once to save Timezone to EEPROM
  euDE.writeRules(100);
*/
}

/**
 * 
 */
void loop() {
  if(Serial.available()) {
    processSyncMessage();
  }

  ButtonUpState = digitalRead(ButtonUpPin);
  
  if (ButtonUpState != LastButtonUpState) {
    if (ButtonUpState == LOW) {
      NewBrightness += NewBrightness < 0x10 ? 0x04 : (NewBrightness < 0xEF ? 0x10 : 0x00);

      BrightnessChanged = true;
    }
  }
  LastButtonUpState = ButtonUpState;

  ButtonDownState = digitalRead(ButtonDownPin);
  if (ButtonDownState != LastButtonDownState) {
    if (ButtonDownState == LOW) {
      NewBrightness -= NewBrightness > 0x10 ? 0x10 : (NewBrightness > 0x00 ? 0x04 : 0);

      BrightnessChanged = true;
    }
  }
  LastButtonDownState = ButtonDownState;

  if (NewBrightness != Brightness) {
    if (NewBrightness > Brightness) {
      Brightness++;
    } else if (NewBrightness < Brightness) {
      Brightness--;
    }
    
#ifdef DEBUG
    Serial.print("old brightness: "); Serial.println(Brightness);
    Serial.print("new brightness: "); Serial.println(NewBrightness);
#endif
    LEDS.setBrightness(Brightness);
    LEDS.show();
  }

  if (BrightnessChanged) {
    BrightnessChanged = false;

    memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));
    for (uint8_t i=0; i<= NewBrightness/0x10; i++) {
      leds[i].r = 255; 
    }
    LEDS.show();
    delay(750);
  }


  if (utc != now()) {
    utc = now();

//    current_time = prevtime + TimeZone;

//    current_time += IsDst(hour(current_time), day(current_time), month(current_time), weekday(current_time)) ? 60 * 60 : 0;

//    showCircleClock(current_time);

    showCircleClock(euDE.toLocal(utc));
/*
    // swith LED
    if ((second() % 10 == 0) && (led_count == 0)) {
      LED_on();
    }
*/
  }

  delay(250);

  led_count += (led_count < LED_MAX_COUNT) && led_on ? 1 : 0;
  
  if (led_count >= LED_MAX_COUNT) {
    LED_off();
    led_count = 0;
  }

//  enterSleep();
}

