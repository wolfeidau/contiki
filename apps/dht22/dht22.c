#include <util/delay.h>
#include "contiki.h"
#include <avr/io.h>
#include <stdio.h>
#include "dht22.h"

#define MAXTIMINGS 85
#define HIGH 0x1

static uint8_t dhtData[6];

int dhtRead(void) {

  uint8_t laststate = HIGH;
  uint8_t counter = 0;
  uint8_t j = 0, i;
  uint8_t _count = 5;

  // clear all the data
  dhtData[0] = dhtData[1] = dhtData[2] = dhtData[3] = dhtData[4] = 1;
  
  PORTD &= ~(_BV(PIND0));  // Sensor pin low (floating)

  // read in timings
  for ( i=0; i< MAXTIMINGS; i++) {  // 85 timings

    counter = 0;

    while ((PIND & _BV(PD1)) == laststate) { 
      counter++;
//      PORTD |= _BV(PIND0);  // Sensor pin high
//      _delay_us(1);
//      PORTD &= ~(_BV(PIND0));  // Sensor pin low (floating)
      _delay_us(10);
      if (counter == 255) {
        break;
      }
    }

    //printf("counter %d\n", counter);
    laststate = PIND & _BV(PD1);

    if (counter == 255) {
     break;
    }

    // ignore first 3 transitions
    if ((i >= 4) && (laststate == 0)) {

      // shove each bit into the storage bytes
      dhtData[j/8] <<= 1;

      if (counter > _count) {
        dhtData[j/8] |= 1;
      }
      j++;
    }

  }
 // sei();
  
  /*
  Serial.println(j, DEC);
  Serial.print(data[0], HEX); Serial.print(", ");
  Serial.print(data[1], HEX); Serial.print(", ");
  Serial.print(data[2], HEX); Serial.print(", ");
  Serial.print(data[3], HEX); Serial.print(", ");
  Serial.print(data[4], HEX); Serial.print(" =? ");
  Serial.println(data[0] + data[1] + data[2] + data[3], HEX);
  */
  printf("dhtData %x %x %x %x\n", dhtData[0], dhtData[1], dhtData[2], dhtData[3] & 0xFF);

  // check we read 40 bits and that the checksum matches
  if ((j >= 40) && 
      (dhtData[4] == ((dhtData[0] + dhtData[1] + dhtData[2] + dhtData[3]) & 0xFF)) ) {
    return 1;
  }
  
  return 0;

}

int dhtAcquire(dhtsample_t *sample) {

  if (dhtRead()) {
    
    printf("dhtRead read ok.\n");

    sample->hum = dhtData[0];
    sample->hum *= 256;
    sample->hum += dhtData[1];
    sample->hum /= 10;

    sample->temp = dhtData[2] & 0x7F;
    sample->temp *= 256;
    sample->temp += dhtData[3];
    sample->temp /= 10;

    if (dhtData[2] & 0x80) {
      sample->temp *= -1;
    }

    printf("dhtRead %2.1f %2.1f\n", sample->hum, sample->temp);

    return 1;

  }
  printf("dhtRead failed.\n");

  return 0;
}
