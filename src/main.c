/*
arm-none-eabi-objdump -d main.elf
make clean
make
STM32_Programmer_CLI -c port=SWD freq=1000  sn=066EFF485570854967101553 -w main.bin 0x08000000 -rst
*/
#include <stdint.h>
#include "device.h"

///////////////////////
//  IO C0~C15
///////////////////////
#define PINx_LED_RED_V3		GPIO_PIN_6
#define PINx_LED_GREEN_V3	GPIO_PIN_7

#define RCC_AHBENR_IOPCEN (1u << 19)  /* GPIOC clock enable bit on F0 */


static void delay(volatile uint32_t t) {
    while (t--) __asm__("nop");
}

typedef enum
{
  GPIO_PIN_RESET = 0,
  GPIO_PIN_SET
}GPIO_PinState;

void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
  /* Check the parameters */
  //assert_param(IS_GPIO_PIN(GPIO_Pin));
  //assert_param(IS_GPIO_PIN_ACTION(PinState));

  if (PinState != GPIO_PIN_RESET)
  {
    GPIOx->BSRR = (uint32_t)GPIO_Pin;
  }
  else
  {
    GPIOx->BRR = (uint32_t)GPIO_Pin;
  }
}

int main(void) {
    /* 1) Enable GPIOC clock (AHB) */
    RCC->AHBENR |= RCC_AHBENR_IOPCEN;

    /* 2) Set PC6 as general purpose output
       MODER: 2 bits per pin.
       - clear bits [13:12], then set to 01b.
    */
    GPIOC->MODER &= ~(3u << (6 * 2));
    GPIOC->MODER |=  (1u << (6 * 2));

    /* 3) Optional: push-pull, no pull-up/down, low speed (defaults are OK on reset)
       GPIOC->OTYPER  &= ~(1u << 6);
       GPIOC->OSPEEDR &= ~(3u << (6 * 2));
       GPIOC->PUPDR   &= ~(3u << (6 * 2));
    */

    while (1) {
        /* Set PC6 high (atomic set via BSRR lower half) */
        GPIOC->BSRR = (1u << 6);
        delay(120000);

        /* Reset PC6 low (atomic reset via BSRR upper half) */
        GPIOC->BSRR = (1u << (6 + 16));
        delay(120000);
    }
}
