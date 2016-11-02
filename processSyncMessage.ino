#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by Unix time_t as ten ASCII digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message

/*

  echo T$(($(date +%s)+60*60)) > /dev/cu.usbserial-A1017SEK
  echo T$(($(date +%s))) > /dev/cu.usbserial-A1017SEK

*/

#define TIMEZONE_MSG_LEN 4
#define TIMEZONE_HEADER 'Z'

/*

  echo Z+01 > /dev/cu.usbserial-A1017SEK

*/

void LED_on() {
  digitalWrite(13, HIGH);
  led_on = true;

#ifdef DEBUG
  Serial.println("LED on");
#endif
}

void LED_off() {
  digitalWrite(13, LOW);
  led_on = false;

#ifdef DEBUG
  Serial.println("LED off");
#endif
}

void processSyncMessage() {
  // if time sync available from serial port, update time and return true
  while(Serial.available()) {
    char c = Serial.read();
    #ifdef DEBUG
    Serial.print(c);
    #endif

    if (c == TIME_HEADER) {
      uint32_t pctime = 0;

      for (uint8_t i=TIME_MSG_LEN; i>0; i--) {
        c = Serial.read();
        if (c >= '0' && c <= '9') {
          pctime = (10 * pctime) + (c - '0'); // convert digits to a number
        }
      }

      RTC.set(pctime);
      setTime(pctime);

//      RTC.adjust(DateTime(pctime)); // Sync Arduino clock to the time received on the serial port

#ifdef DEBUG
      Serial.println(RTC.get());
#endif
/*
    } else if (c == TIMEZONE_HEADER) {
      uint8_t NewTimeZone = 0;
      int8_t TZ_Neg = 1;
      
      for (uint8_t i=TIMEZONE_MSG_LEN; i>0; i--) {
        c = Serial.read();
        if (c >= '0' && c <= '9') {
          NewTimeZone = (10 * NewTimeZone) + (c - '0');
        } else if (c == '-') {
          TZ_Neg = -1;
        }
      }
      NewTimeZone *= TZ_Neg;

      while (!eeprom_is_ready());
      eeprom_write_word((uint16_t*)TimeZoneAddr, 3600 * NewTimeZone);

      //#ifdef DEBUG
      Serial.println(NewTimeZone);
      //#endif
*/
    }
  }
}

