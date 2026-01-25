#include "loadcell.h"

void loadcell_init(loadcell_type *loadcell, GPIO_TypeDef *sck_gpio, uint16_t sck_pin, GPIO_TypeDef *dt_gpio, uint16_t dt_pin){
  // Writing selected pins to the structure
  loadcell->sck_gpio = sck_gpio;
  loadcell->sck_pin = sck_pin;
  loadcell->dt_gpio = dt_gpio;
  loadcell->dt_pin = dt_pin;

  // Configuring GPIOs
  GPIO_InitTypeDef  gpio = {0}; // Creating a GPIO configuration structure
  gpio.Mode = GPIO_MODE_OUTPUT_PP; // Output pin
  gpio.Pull = GPIO_NOPULL;	// No pull up/down resistor
  gpio.Speed = GPIO_SPEED_FREQ_HIGH; // Fast switching
  gpio.Pin = sck_pin; // Choosing the pin for SCK
  HAL_GPIO_Init(sck_gpio, &gpio);	// Initializing the GPIO defined by sck_gpio and pin defined by sck_pin
  gpio.Mode = GPIO_MODE_INPUT;
  gpio.Pull = GPIO_PULLUP;
  gpio.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio.Pin = dat_pin;
  HAL_GPIO_Init(dt_gpio, &gpio);

}
