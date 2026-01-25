#include "loadcell.h"
#include <stdbool.h>

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

uint8_t read_byte_shifting(loadcell_type *loadcell) {
    uint8_t value = 0;
    uint8_t i;
    // Reading the byte starting with MSB and shifting left on every bit reading

    for(i = 0; i < 8; ++i) {
    	HAL_GPIO_WritePin(loadcell->sck_gpio, hx711->sck_pin, SET);
        value |= HAL_GPIO_ReadPin(loadcell->dt_gpio, loadcell->dat_pin) << (7 - i);
        HAL_GPIO_WritePin(loadcell->sck_gpio, loadcell->sck_pin, RESET);
    }
    return value;
}

bool sensor_ready(loadcell_type *loadcell) {
	// Flag that the sensor measurement is ready
	if(HAL_GPIO_ReadPin(loadcell->dt_gpio, loadcell->dt_pin) == GPIO_PIN_RESET){
		return 1;
	}
	return 0;
}

void wait_sensor_ready(loadcell_type *loadcell) {
	// Wait for the measurement to become ready
	while (!sensor_ready(loadcell)) { // Keeps checking if the sensor is ready
		HAL_Delay(0);
	}
}

long read_data(loadcell_type *loadcell){
	wait_sensor_ready(loadcell);
	unsigned long value = 0;
	uint8_t data[3] = { 0 };
	uint8_t filler = 0x00;

	__disable_irq(); // Disabling interrupts during the reading

	// Reading the data
	data[2] = read_byte_shifting(loadcell);
	data[1] = read_byte_shifting(loadcell);
	data[0] = read_byte_shifting(loadcell);

	// Sending additional pulses to select gain on the next measurement
	for (unsigned int i = 0; i < loadcell->A_gain; i++) {
		HAL_GPIO_WritePin(loadcell->sck_gpio, loadcell->sck_pin, SET);
		HAL_GPIO_WritePin(loadcell->sck_gpio, loadcell->sck_pin, RESET);
	}

	__enable_irq(); // Enabling interrupts once the data is read

	// Extracting the most significant bit to determine the sign
	if (data[2] & 0x80) {
		filler = 0xFF;
	} else {
		filler = 0x00;
	}

	// Construct a 32-bit signed integer
	value = ( (unsigned long)(filler) << 24
			| (unsigned long)(data[2]) << 16
			| (unsigned long)(data[1]) << 8
			| (unsigned long)(data[0]) );

	return (long)(value);
}

long read_average(loadcell_type *loadcell, int8_t times) {

	// Takes the average reading of n times

	long sum = 0;
	for (int8_t i = 0; i < times; i++) {
		sum += read_data(loadcell);
		HAL_Delay(0);
	}
	return sum / times;
}

double get_average_value(loadcell_type *loadcell, int8_t times) {
	long offset = 0;
	offset = loadcell->A_offset;
	return read_average(loadcell, times) - offset;
}

void tare(loadcell_type *loadcell, uint8_t times) {
	read_data(loadcell);
	double sum = read_average(loadcell, times);
	loadcell->A_offset = sum;
}

float get_weight(loadcell_type *loadcell, int8_t times) {
  // Read load cell measurement
	read_data(loadcell);
	return get_average_value(loadcell, times) / loadcell->A_scale;
}
