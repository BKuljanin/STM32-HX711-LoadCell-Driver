#include "loadcell.h"


loadcell_type my_loadcell;
void init_weight(loadcell_type *loadcell);
long weight = 0;

int main(void) {

init_weight(&my_loadcell);

	while(1){

		weight = get_weight(&my_loadcell, 10);
	}

}


void init_weight(loadcell_type *loadcell){
	//char buffer[128] = {0};

	/* Initialize the hx711 sensor */
	loadcell_init(loadcell, GPIOA, GPIO_PIN_3, GPIOA, GPIO_PIN_2);

	/* Configure gain for each channel (see datasheet for details) */
	set_parameters(loadcell, 1.0f, 128, 0); //scale, gain, offset

	/* Tare weight */
	tare(loadcell, 10);

}
