#ifndef INC_LOADCELL_H_
#define INC_LOADCELL_H_

#include "stdint.h"
#include "stdbool.h"
#include "stm32f4xx.h"

typedef struct
{
  GPIO_TypeDef  *sck_gpio; // Pointer to the GPIO port for SCK
  uint16_t      sck_pin; // Which PIN of the port above
  GPIO_TypeDef  *dt_gpio; // Pointer to the DT port
  uint16_t      dt_pin; // Which PIN of the port above
  long       	A_offset; // Zero point adjustment (tare) for channel A
  float         A_scale; // Weigh conversion factor for channel A
  uint8_t		A_gain; // Amplifier gain setting for channel A (1=128x, 3=64x)

}loadcell_type;

#endif /* INC_LOADCELL_H_ */
