#pragma once

#include <avr/io.h>

// Library for optimized gpio, for attiny861

// Assume that code uses pinouts as defined in ATTinyCore tinyX61_new

// ATMEL ATTINY861
//
//                   +-\/-+
//      (D  8) PB0  1|    |20  PA0 (D  0)
//     *(D  9) PB1  2|    |19  PA1 (D  1)
//      (D 10) PB2  3|    |18  PA2 (D  2) INT1
//     *(D 11) PB3  4|    |17  PA3 (D  3)
//             VCC  5|    |16  AGND
//             GND  6|    |15  AVCC
//      (D 12) PB4  7|    |14  PA4 (D  4)
//     *(D 13) PB5  8|    |13  PA5 (D  5)
// INT0 (D 14) PB6  9|    |12  PA6 (D  6)
//      (D 15) PB7 10|    |11  PA7 (D  7)
//                   +----+

#ifndef __AVR_ATtiny861__
  #error Incompatible chip
#endif

volatile uint8_t* get_ddr_address_from_pin(uint8_t pin)
{
  if (pin >= 8) return &DDRB;
  else return &DDRA;
}

volatile uint8_t* get_port_address_from_pin(uint8_t pin)
{
  if (pin >= 8) return &PORTB;
  else return &PORTA;
}

volatile uint8_t* get_pin_address_from_pin(uint8_t pin)
{
  if (pin >= 8) return &PINB;
  else return &PINA;
}

uint8_t get_bitmask_from_pin(uint8_t pin) {
  return _BV(pin & 0x07);
}

static inline __attribute__((always_inline)) void pinMode_opt(uint8_t pin, uint8_t mode);
void pinMode_opt(uint8_t pin, uint8_t mode)
{
  volatile uint8_t *ddr_reg = get_ddr_address_from_pin(pin);
  volatile uint8_t *port_reg = get_port_address_from_pin(pin);
  uint8_t bitmask = get_bitmask_from_pin(pin);
  if (mode == INPUT) {
    // On ATtiny861, PORTx/PINx/DDRx registers are within sbi/cbi range. Thus, no need for atomic.
    *ddr_reg &= ~bitmask;
    *port_reg &= ~bitmask;
  }
  else if (mode == INPUT_PULLUP) {
    *ddr_reg &= ~bitmask;
    *port_reg |= bitmask;
  }
  else {
    *ddr_reg |= bitmask;
  }
}

static inline __attribute__((always_inline)) void digitalWrite_opt(uint8_t pin, uint8_t val);
void digitalWrite_opt(uint8_t pin, uint8_t val)
{
  volatile uint8_t *port_reg = get_port_address_from_pin(pin);
  uint8_t bitmask = get_bitmask_from_pin(pin);
  if (val == LOW) {
    *port_reg &= ~bitmask;
  }
  else {
    *port_reg |= bitmask;
  }
}

static inline __attribute__((always_inline)) uint8_t digitalRead_opt(uint8_t pin);
uint8_t digitalRead_opt(uint8_t pin)
{
  volatile uint8_t *pin_reg = get_pin_address_from_pin(pin);
  uint8_t bitmask = get_bitmask_from_pin(pin);
  return (*pin_reg & bitmask) ? 1 : 0;
}
