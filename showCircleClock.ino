/**
 * 
 */
void showCircleClock(time_t dt) {
  memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));
//  LEDS.clear();

  uint8_t H = hour(dt);
  uint8_t M = minute(dt);
  uint8_t S = second(dt);

  // we only want to show 12 of 24 hours ..
  H += H < 12 ? 0 : -12;

  float HourLED_1 = NUM_LEDS * (H + ((M + (S / 60.0)) / 60.0)) / 12.0;

  // make a nice alignment of minutes and seconds
  M += M < 59 ? 1 : -59;
  S += S < 59 ? 1 : -59;

  uint8_t MinuteLED = NUM_LEDS * M / 60;
  uint8_t SecondLED = NUM_LEDS * S / 60;

  float SecondLED_F = (NUM_LEDS * S / 60.0) - SecondLED;

  uint8_t HourLED_2 = (int8_t)(HourLED_1 < (NUM_LEDS-1) ? HourLED_1 + 1 : HourLED_1 - (NUM_LEDS-1));

#ifdef DEBUG
  Serial.print(hour(dt)); Serial.print(":"); Serial.print(minute(dt)); Serial.print(":"); Serial.print(second(dt));  
  Serial.print(" => "); Serial.print("(");
  Serial.print(HourLED_1); Serial.print(", "); Serial.print(HourLED_2); Serial.print(")");
  Serial.print(" : "); Serial.print(MinuteLED); Serial.print(" : "); Serial.println(SecondLED);
#endif

  leds[(uint8_t)(HourLED_1)].r = (uint8_t)(255 * (1 - (HourLED_1 - (uint8_t)HourLED_1)));
  leds[HourLED_2].r = (uint8_t)(255 * (HourLED_1 - (uint8_t)HourLED_1));
  leds[MinuteLED].g = 255;

//  leds[SecondLED].b = S % 2 == 0 ? 255 : 128;

  leds[SecondLED].b = 255 * SecondLED_F;
  for (uint8_t c = 1; c <= (uint8_t)(NUM_LEDS / 2); c++) {
    int8_t d = SecondLED - c;
    d += (d < 0 ? 24 : 0);
    leds[d].b = 255 - (uint8_t)(255 / (NUM_LEDS / 2) * c);
  }

//  if () {}

  LEDS.show();
}

