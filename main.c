// Waveform generator on ARD_D0 to ARD_D3

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define NUM_BITS   4   // number of output pins / bits 
#define NUM_PHASES 5    // number of phases in one cycle 

// One GPIO spec for each bit with index 0 = ARD_D0 for bit 0 
static const struct gpio_dt_spec wave_pin[NUM_BITS] = {
	GPIO_DT_SPEC_GET(DT_NODELABEL(wave_bit0), gpios),
	GPIO_DT_SPEC_GET(DT_NODELABEL(wave_bit1), gpios),
	GPIO_DT_SPEC_GET(DT_NODELABEL(wave_bit2), gpios),
	GPIO_DT_SPEC_GET(DT_NODELABEL(wave_bit3), gpios),
};

// pin levels for each phase, bit 0 of the value goes to D0 and so on for each bit
static const uint8_t wave_data[NUM_PHASES] = {
	0x0F, // phase 1: 1111 - all high          
	0x0E, // phase 2: 1110 - D0 goes low     
	0x0C, // phase 3: 1100 - D0, D1 low       
	0x08, // phase 4: 1000 - D0, D1, D2 low      
	0x00  // phase 5: 0000 - D3 falls so all low            
};

// Sets how long to hold each phase (in ms)
static const uint32_t wave_time_ms[NUM_PHASES] = {
	60, 20, 20, 20, 80			// adds up to 200
};

//sets all four pins from the bottom 4 bits of value
static void write_wave(uint8_t value)
{
	for (int bit = 0; bit < NUM_BITS; bit++) {
		gpio_pin_set_dt(&wave_pin[bit], (value >> bit) & 1);	// shifts bit down to position 0 and masks off the rest
	}
}

int main(void)
{
	for (int bit = 0; bit < NUM_BITS; bit++) {				// set each pin up as an output starting low
		gpio_pin_configure_dt(&wave_pin[bit], GPIO_OUTPUT_INACTIVE);
	}

	// Repeat the 200 ms cycle forever 
	while (1) {
		for (int phase = 0; phase < NUM_PHASES; phase++) {
			write_wave(wave_data[phase]);   // set the pins for this phase 
			k_msleep(wave_time_ms[phase]);  // hold for this phase's duration
		}
	}
	return 0;
}