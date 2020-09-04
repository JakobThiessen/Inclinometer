/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

#if defined(__GNUC__)
void board_init(void) WEAK __attribute__((alias("system_board_init")));
#elif defined(__ICCARM__)
void board_init(void);
#  pragma weak board_init=system_board_init
#endif

void system_board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	
		struct port_config pin_conf;
		port_get_config_defaults(&pin_conf);

		/* Configure PINs as outputs, turn them off */
		pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
		port_pin_set_config(TP_2, &pin_conf);
		port_pin_set_output_level(TP_2, false);
		
		port_pin_set_config(SENSOR_SPI_SS_0, &pin_conf);
		port_pin_set_output_level(SENSOR_SPI_SS_0, true);
		
		port_pin_set_config(SENSOR_SPI_SS_1, &pin_conf);
		port_pin_set_output_level(SENSOR_SPI_SS_1, true);
		
		port_pin_set_config(SENSOR_SPI_SCK, &pin_conf);
		port_pin_set_output_level(SENSOR_SPI_SCK, true);
		
		port_pin_set_config(SENSOR_SPI_MOSI, &pin_conf);
		port_pin_set_output_level(SENSOR_SPI_MOSI, true);
		
		/* Set buttons as inputs */
		pin_conf.direction  = PORT_PIN_DIR_INPUT;
		pin_conf.input_pull = PORT_PIN_PULL_UP;
		port_pin_set_config(BUTTON_0_PIN, &pin_conf);
		port_pin_set_config(BUTTON_1_PIN, &pin_conf);
		
		port_pin_set_config(ANALOG_0_PIN, &pin_conf);
		//port_pin_set_config(TP_2, &pin_conf);
		port_pin_set_config(TP_3, &pin_conf);
		port_pin_set_config(TP_4, &pin_conf);
		port_pin_set_config(TP_5, &pin_conf);
		
		port_pin_set_config(USB_VOLTAGE_PIN, &pin_conf);
		
		pin_conf.input_pull = SYSTEM_PINMUX_PIN_PULL_NONE;
		port_pin_set_config(CHARGE_STATUS, &pin_conf);
		
}