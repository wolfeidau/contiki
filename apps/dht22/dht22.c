#include <util/delay.h>
#include "contiki.h"
#include <avr/io.h>
#include <stdio.h>
#include "dht22.h"

#define MAXTIMINGS 85
#define HIGH 0x1

#define PROGLED_PORT  PORTD
#define PROGLED_DDR   DDRD
#define PROGLED_PIN   PIND1 // Pin 9

// static struct etimer et;
// static uint8_t dhtPin; 
// static uint32_t dhtSampleTime;

// void dhtInitialise(uint8_t pin, uint32_t sampleTime) {
//   dhtPin = pin;
//   dhtSampleTime = sampleTime;
//   PROGLED_DDR |= _BV(PROGLED_PIN);
// }

//void dhtEnable(bool flag) {
  // setup timer
  // if (flag) {
  //   etimer_set(&et, CLOCK_SECOND * 1 % (CLOCK_SECOND * 16));
  // }

//}

static uint8_t dhtData[6];

int dhtRead(void) {

  uint8_t laststate = HIGH;
  uint8_t counter = 0;
  uint8_t j = 0, i;
  uint8_t _count;


  dhtData[0] = dhtData[1] = dhtData[2] = dhtData[3] = dhtData[4] = 0;
  
  // pull the pin high and wait 250 milliseconds
  PROGLED_PORT |= _BV(PROGLED_PIN); // high
  _delay_ms(250);

  // now pull it low for ~20 milliseconds
  PROGLED_DDR |= _BV(PROGLED_PIN); // output
  _delay_ms(20);

  cli();
  
  PROGLED_PORT |= _BV(PROGLED_PIN); // high
  _delay_us(40);

  PROGLED_DDR &= ~_BV(PROGLED_PIN); // input

  // read in timings
  for ( i=0; i< MAXTIMINGS; i++) {
    counter = 0;
    while ((PIND & _BV(PROGLED_PIN)) == laststate) {
      counter++;
      _delay_us(1);
      if (counter == 255) {
        break;
      }
    }
    laststate = PIND & _BV(PROGLED_PIN);

    if (counter == 255) break;

    // ignore first 3 transitions
    if ((i >= 4) && (i%2 == 0)) {
      // shove each bit into the storage bytes
      dhtData[j/8] <<= 1;
      if (counter > _count)
        dhtData[j/8] |= 1;
      j++;
    }

  }
  sei();
  
  /*
  Serial.println(j, DEC);
  Serial.print(data[0], HEX); Serial.print(", ");
  Serial.print(data[1], HEX); Serial.print(", ");
  Serial.print(data[2], HEX); Serial.print(", ");
  Serial.print(data[3], HEX); Serial.print(", ");
  Serial.print(data[4], HEX); Serial.print(" =? ");
  Serial.println(data[0] + data[1] + data[2] + data[3], HEX);
  */

  // check we read 40 bits and that the checksum matches
  if ((j >= 40) && 
      (dhtData[4] == ((dhtData[0] + dhtData[1] + dhtData[2] + dhtData[3]) & 0xFF)) ) {
    return 1;
  }
  
  return 0;

}

int dhtAcquire(dhtsample_t sample) {

  if (dhtRead()) {

    sample.hum = dhtData[0];
    sample.hum *= 256;
    sample.hum += dhtData[1];
    sample.hum /= 10;

    sample.temp = dhtData[2] & 0x7F;
    sample.temp *= 256;
    sample.temp += dhtData[3];
    sample.temp /= 10;
    if (dhtData[2] & 0x80)
      sample.temp *= -1;

    return 1;

  }
  return 0;
}
