#ifndef DEVICE_H
#define DEVICE_H
/*
 * Minimal register-level device header for STM32F072 (Cortex-M0).
 * Purpose: give transparent access to RCC and GPIOC without HAL/CMSIS.
 *
 * This header mirrors a tiny subset of the official stm32f072xb.h:
 *  - Only RCC and GPIOC are described.
 *  - All registers are exposed as volatile 32-bit memory-mapped I/O.
 *
 * Notes:
 *  - The MCU is little-endian.
 *  - All addresses come from the STM32F0 reference manual memory map.
 *  - This file is intentionally minimal to aid bare-metal learning.
 */

#include <stdint.h> /* fixed-width integer types (e.g., uint32_t) */

#ifdef __cplusplus
  #define   __I     volatile             /*!< Defines 'read only' permissions                 */
#else
  #define   __I     volatile const       /*!< Defines 'read only' permissions                 */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions                */
#define     __IO    volatile             /*!< Defines 'read / write' permissions              */

/* ---------------------------------------------------------------------------
 * Memory map bases (bus segments)
 * ---------------------------------------------------------------------------
 * STM32 MCUs expose peripherals via memory-mapped I/O. The top-level
 * PERIPH_BASE is 0x4000_0000, with AHB/APB segments offset from it.
 *
 * AHBPERIPH_BASE (0x4002_0000): RCC, DMA, FLASH interface, etc.
 * AHB2PERIPH_BASE (0x4800_0000): GPIO ports (GPIOA..), CRC, etc.
 * We only define what we need for this example.
 */
#define PERIPH_BASE        (0x40000000UL)
#define APBPERIPH_BASE     PERIPH_BASE                   /* 0x4000_0000 */
#define AHBPERIPH_BASE     (PERIPH_BASE + 0x00020000UL)  /* 0x4002_0000 */
#define AHB2PERIPH_BASE    (PERIPH_BASE + 0x08000000UL)  /* 0x4800_0000 */

/* ---------------------------------------------------------------------------
 * Peripheral base addresses used in this example
 * ---------------------------------------------------------------------------
 * RCC: Reset and Clock Control. Controls clock gating to each peripheral,
 *      clock sources, prescalers, resets, etc.
 * GPIOC: General-Purpose I/O Port C (pins PC0..PC15).
 */
#define RCC_BASE              (AHBPERIPH_BASE + 0x00001000UL) /* 0x4002_1000 */
#define GPIOA_BASE            (AHB2PERIPH_BASE + 0x00000000) /* 0x4800_0000 */
#define GPIOB_BASE            (AHB2PERIPH_BASE + 0x00000400) /* 0x4800_0400 */
#define GPIOC_BASE            (AHB2PERIPH_BASE + 0x00000800) /* 0x4800_0800 */
#define GPIOD_BASE            (AHB2PERIPH_BASE + 0x00000C00) /* 0x4800_0C00 */
#define GPIOE_BASE            (AHB2PERIPH_BASE + 0x00001000) /* 0x4800_1000 */
#define GPIOF_BASE            (AHB2PERIPH_BASE + 0x00001400) /* 0x4800_1400 */

/* ---------------------------------------------------------------------------
 * RCC register layout
 * ---------------------------------------------------------------------------
 * Each field is 'volatile uint32_t' to prevent the compiler from optimizing
 * out reads/writes: hardware can change these values outside of program flow.
 *
 * The order of fields must match the reference manual exactly; offsets are
 * indicated as comments for quick cross-checking and debugging.
 *
 * Common use here:
 *  - Enable GPIOC clock by setting AHBENR.IOPCEN (bit 19 on STM32F0).
 *  - Optionally adjust system clocks via CR/CFGR (not needed for a simple LED).
 */
typedef struct {
  __IO uint32_t CR;        /* 0x00: Clock control register (HSI/HSE/PLL on/off) */
  __IO uint32_t CFGR;      /* 0x04: Clock configuration (muxing, prescalers)   */
  __IO uint32_t CIR;       /* 0x08: Clock interrupt register                   */
  __IO uint32_t APB2RSTR;  /* 0x0C: APB2 peripheral reset register             */
  __IO uint32_t APB1RSTR;  /* 0x10: APB1 peripheral reset register             */
  __IO uint32_t AHBENR;    /* 0x14: AHB peripheral clock enable register       */
  __IO uint32_t APB2ENR;   /* 0x18: APB2 peripheral clock enable               */
  __IO uint32_t APB1ENR;   /* 0x1C: APB1 peripheral clock enable               */
  __IO uint32_t BDCR;      /* 0x20: Backup domain control                      */
  __IO uint32_t CSR;       /* 0x24: Control/status (LSI, reset flags)          */
  __IO uint32_t AHBRSTR;   /* 0x28: AHB peripheral reset register              */
  __IO uint32_t CFGR2;     /* 0x2C: Additional clock configuration             */
  __IO uint32_t CFGR3;     /* 0x30: Additional peripheral clock config         */
  __IO uint32_t CR2;       /* 0x34: Additional clock control (device-specific) */
} RCC_TypeDef;

/* Helpful bit mask for later reference (usage example in your .c):
 *   RCC->AHBENR |= (1u << 19); // enable GPIOC clock (IOPCEN on F0)
 * To keep this header lean, we don't enumerate all bit fields here.
 */

/* ---------------------------------------------------------------------------
 * GPIO register layout
 * ---------------------------------------------------------------------------
 * GPIO ports share a common register layout. Key registers used most often:
 *
 *  MODER  (mode): each pin uses 2 bits: 00=input, 01=output, 10=AF, 11=analog
 *  OTYPER (type): push-pull (0) or open-drain (1)
 *  OSPEEDR(speed): low/med/high/very high speed (drive strength/timing)
 *  PUPDR (pull): 00=no pull, 01=pull-up, 10=pull-down, 11=reserved
 *  IDR    (input data): read pin state
 *  ODR    (output data): write/read output state (RMW; not atomic)
 *  BSRR   (bit set/reset): lower 16 bits set pins, upper 16 bits reset pins
 *         -> preferred for atomic pin toggling without RMW hazards
 *  LCKR   (lock): lock configuration until next reset
 *  AFR[0] (AFR-L): alternate function (pins 0..7)
 *  AFR[1] (AFR-H): alternate function (pins 8..15)
 *  BRR    (bit reset register): alternative way to reset bits (on some F0)
 *
 *  NOTE: Each GPIO port is allocated a 1KB (0x400) block in the AHB bus memory
 *  map for standardized bus decoding and future register expansion, even though
 *  the current set of registers (MODER to BRR) occupies only about 44 bytes.
 */
typedef struct {
  __IO uint32_t MODER;   /* 0x00: Port mode register (2 bits per pin)         */
  __IO uint32_t OTYPER;  /* 0x04: Output type register                        */
  __IO uint32_t OSPEEDR; /* 0x08: Output speed register                       */
  __IO uint32_t PUPDR;   /* 0x0C: Pull-up/Pull-down register                  */
  __IO uint32_t IDR;     /* 0x10: Input data register                         */
  __IO uint32_t ODR;     /* 0x14: Output data register (RMW; not atomic)      */
  __IO uint32_t BSRR;    /* 0x18: Bit set/reset (atomic set: bits 0..15,
                                         atomic reset: bits 16..31)                */
  __IO uint32_t LCKR;    /* 0x1C: Configuration lock register                 */
  __IO uint32_t AFR[2];  /* 0x20: AFR[0]=low pins (0..7), AFR[1]=high (8..15) */
  __IO uint32_t BRR;     /* 0x28: Bit reset register (device-specific option) */
} GPIO_TypeDef;

/* ---------------------------------------------------------------------------
 * Convenience pointers (typed views over the base addresses)
 * ---------------------------------------------------------------------------
 * Casting the base address to a struct pointer provides field-accurate
 * access with natural syntax, e.g.:
 *
 *   RCC->AHBENR |= (1u << 19);       // enable GPIOC clock
 *   GPIOC->MODER &= ~(3u << (6*2));  // clear PC6 mode bits
 *   GPIOC->MODER |=  (1u << (6*2));  // set PC6 to '01' = general purpose output
 *   GPIOC->BSRR = (1u << 6);         // atomically set PC6 high
 *   GPIOC->BSRR = (1u << (6+16));    // atomically reset PC6 low
 *
 * All accesses are volatile, so the compiler will emit actual loads/stores
 * to these addresses without optimizing them away or reordering improperly.
 */
#define RCC                 ((RCC_TypeDef *) RCC_BASE)
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD               ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE               ((GPIO_TypeDef *) GPIOE_BASE)
#define GPIOF               ((GPIO_TypeDef *) GPIOF_BASE)

#endif /* DEVICE_H */
