#include "delay.h"
// C helper classes
#include "System/variants.h"


static u16 fac_ms = 0;
static u8 fac_us  = 0;

void Delay_init(u8 clk) {
  SysTick->CTRL &= 0xfffffffb;   //bit2 is cleared, select external clock HCLK /8
  fac_us = clk / 8;              //Frequency after dividing by 8 Unit M is 1us
  fac_ms = (u16)fac_us * 1000;   //The number of times in 1ms is 1000 times of 1us
}

void Delay_us(u32 us)   //Delay is less than 1800 *1000us
{
  u32 temp;
  SysTick->LOAD = us * fac_us;   //Load times, 1us fac_us times, us microsecond us *fac_us times
  SysTick->VAL  = 0x00;          //Clear the counter
  SysTick->CTRL = 0x01;          //start counting down
  do {
    temp = SysTick->CTRL;
  } while (temp & 0x01 && !(temp & (1 << 16)));   //Wait time arrives (After the countdown is completed, the hardware sets SysTick-> CTRL 16th bit to 1)
  SysTick->CTRL = 0x00;                           //Close the counter
  SysTick->VAL  = 0x00;                           //Clear the counter
}

void Delay_ms(u16 ms)   //Delay is less than 1800ms
{
  u32 temp;
  SysTick->LOAD = (u32)ms * fac_ms;
  SysTick->VAL  = 0x00;
  SysTick->CTRL = 0x01;
  do {
    temp = SysTick->CTRL;
  } while (temp & 0x01 && !(temp & (1 << 16)));
  SysTick->CTRL = 0x00;
  SysTick->VAL  = 0x00;
}

void Delay(u32 time) {
  int i;
  while (time--)
    for (i = 0; i < 120; i++)
      ;
}
