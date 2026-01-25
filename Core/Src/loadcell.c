#include "loadcell.h"

void loadcell_init(loadcell_type *loadcell, GPIO_TypeDef *sck_gpio, uint16_t sck_pin, GPIO_TypeDef *dt_gpio, uint16_t dt_pin){

  // Writing selected ports and pins to the structure
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
  gpio.Pull = GPIO_PULLUP; // Pull up resistor on DT pin, since DT floats when HX711 isn't driving it
  gpio.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio.Pin = dt_pin;
  HAL_GPIO_Init(dt_gpio, &gpio);

}
void set_parameters(loadcell_type *loadcell, float A_scale, uint8_t A_gain, long A_offset){
// Configure the measurement parameters

	loadcell->A_scale = A_scale; // Setting scale

	// Setting gain, user can choose between 128 and 64 for channel A
	if (A_gain == 128){
		loadcell->A_gain = 1;
	}
	else if (A_gain == 64){
		loadcell->A_gain = 3;
	}

	loadcell->A_offset = A_offset; // Setting offset
}

uint8_t read_byte(loadcell_type *loadcell) {
    uint8_t value = 0;
    uint8_t i;

    for(i = 0; i < 8; ++i) {
    	HAL_GPIO_WritePin(loadcell->sck_gpio, hx711->sck_pin, SET);
        value |= HAL_GPIO_ReadPin(loadcell->dt_gpio, loadcell->dat_pin) << (7 - i);
        HAL_GPIO_WritePin(loadcell->sck_gpio, loadcell->sck_pin, RESET);
    }
    return value;
}
