#include "loadcell.h"


loadcell_type my_loadcell;
void init_weight(loadcell_type *loadcell);
int main(void) {

loadcell_init(my_loadcell);

	while(1){


	}

}


void init_weight(loadcell_type *loadcell){
	char buffer[128] = {0};

	/* Initialize the hx711 sensor */
	loadcell_init(loadcell, GPIOA, GPIO_PIN_3, GPIOA, GPIO_PIN_2);

	/* Configure gain for each channel (see datasheet for details) */
	set_parameters(loadcell, -44.25, 64, 30); //scale, gain, offset

	/* Tare weight */
	tare(loadcell, 10);

}

float measure_weight(loadcell_type loadcell){
	long weightA = 0;

	// Measure the weight for channel A
	weightA = get_weight(&loadcell, 10);
	// Weight cannot be negative
	weightA = (weightA < 0) ? 0 : weightA;

	// Measure the weight for channel B
	weightB = get_weight(&hx711, 10, CHANNEL_B);
	// Weight cannot be negative
	weightB = (weightB < 0) ? 0 : weightB;

	return weightA;
}
