#include <util/delay.h>
#include "contiki.h"
#include <avr/io.h>
#include <stdio.h>
#include "dht22.h"

#define MAXTIMINGS 85
#define HIGH 0x1

#define PROGLED_PORT  PORTD
#define PROGLED_DDR   DDRD
#define PROGLED_PIN   PD1 // Pin 9

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
  uint8_t laststate2 = HIGH;
  uint8_t counter = 0;
  uint8_t j = 0, i;
  uint8_t _count;

  // clear all the data
  dhtData[0] = dhtData[1] = dhtData[2] = dhtData[3] = dhtData[4] = 0;
  
  // DDRD = 0xFF; // output

  // // pull the pin high and wait 250 milliseconds
  // printf("high\n");
  // PORTD |= 0b00000010; // high
  // _delay_ms(250);

  // // now pull it low for ~20 milliseconds
  // printf("low\n");
  // PORTD &= ~(0b00000000); // low
  // _delay_ms(20);

 // cli();
  
 // PORTD |= 0x01; // high
//  _delay_us(40);

  DDRD = 0x00; // input
  PORTD = 0x02; // high
  
  _delay_us(40);
  

  if (laststate2 != (PIND & _BV(PD1))) {
    laststate2 = PIND & _BV(PD1);
    printf("ls2 - %d\n", laststate2);
  }
  
  // read in timings
  for ( i=0; i< MAXTIMINGS; i++) {
    
    counter = 0;
    
    while ((PIND & _BV(PD1)) == laststate) {
      counter++;
      _delay_us(1);
      if (counter == 255) {
        break;
      }
    }

    //printf("first value %d\n", counter);

    laststate = PIND & _BV(PD1);

    if (counter == 255) {
     break;
      //printf("woops break second\n");
    }
    // ignore first 3 transitions
    if ((i >= 4) && (i%2 == 0)) {

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
    if (dhtData[2] & 0x80) {
      sample.temp *= -1;
    }

    return 1;

  }
  return 0;
}
