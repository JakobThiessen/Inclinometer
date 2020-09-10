/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>
#include <math.h>

#include "driver/bmi160.h"
#include "driver/ssd1306.h"
#include "driver/Icons_16x16.h"

static volatile bool main_b_cdc_enable = false;
static volatile int cdcStat = 0;

struct adc_config adc_conf;
struct adc_module adc_instance;
volatile uint16_t adc_val; //Result

struct spi_module spi_master_instance;
struct spi_slave_inst slave_0;
struct spi_slave_inst slave_1;

struct bmi160_dev sensor;
struct bmi160_dev sensor_1;
volatile struct bmi160_sensor_data accel;
volatile struct bmi160_sensor_data gyro;
volatile struct bmi160_sensor_data accel_1;
volatile struct bmi160_sensor_data gyro_1;

//Roll & Pitch are the angles which rotate by the axis X and y
volatile float roll = 0.00;
volatile float pitch = 0.00;
volatile float roll_1 = 0.00;
volatile float pitch_1 = 0.00;

void user_delay_ms(uint32_t period);
int8_t user_spi_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
int8_t user_spi_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);

void drawAngle(int32_t angle);
void drawAkku(uint8_t stat);
void drawWinkelSymbol(void);
void drawOuterFrame(void);
void drawTemeprature(int32_t t);
void drawAverageFaktor(uint8_t f);
void drawMinMaxWinkel(int32_t minValue, int32_t maxValue);

bool main_cdc_enable(uint8_t port)
{
	main_b_cdc_enable = true;
	return true;
}

void main_cdc_disable(uint8_t port)
{
	main_b_cdc_enable = false;
}

static bool my_flag_DTR = false;
int my_calback_cdc_set_dtr(uint8_t port, uint8_t set)
{
	my_flag_DTR  = set;
}

static bool my_flag_autorize_cdc_transfert = false;
bool my_callback_cdc_enable(void)
{
	my_flag_autorize_cdc_transfert = true;
	return true;
}
void my_callback_cdc_disable(void)
{
	my_flag_autorize_cdc_transfert = false;
}

void configure_adc(void);
void configure_adc(void)
{
	adc_get_config_defaults(&adc_conf);
	adc_conf.clock_source = GCLK_GENERATOR_1;
	adc_conf.clock_prescaler = ADC_CLOCK_PRESCALER_DIV8;
	
	adc_conf.reference = ADC_REFERENCE_INTVCC1;
	adc_conf.resolution = ADC_RESOLUTION_16BIT;
	adc_conf.positive_input = ADC_POSITIVE_INPUT_PIN11;
	adc_conf.negative_input = ADC_NEGATIVE_INPUT_GND;
	adc_init(&adc_instance, ADC, &adc_conf);
	adc_enable(&adc_instance);
}

void configure_spi_master(void);
void configure_spi_master(void)
{
	struct spi_config config_spi_master;
	struct spi_slave_inst_config slave_dev_config;
	struct spi_slave_inst_config slave_dev_config_1;
	
	/* Configure and initialize software device instance of peripheral slave */
	spi_slave_inst_get_config_defaults(&slave_dev_config);
	slave_dev_config.ss_pin = SENSOR_SPI_SS_0;
	spi_attach_slave(&slave_0, &slave_dev_config);
	
	spi_slave_inst_get_config_defaults(&slave_dev_config_1);
	slave_dev_config_1.ss_pin = SENSOR_SPI_SS_1;
	spi_attach_slave(&slave_1, &slave_dev_config_1);
	
	
	/* Configure, initialize and enable SERCOM SPI module */
	spi_get_config_defaults(&config_spi_master);
	config_spi_master.mux_setting = CONF_MASTER_MUX_SETTING;
	config_spi_master.pinmux_pad0 = CONF_MASTER_PINMUX_PAD0;
	config_spi_master.pinmux_pad1 = CONF_MASTER_PINMUX_PAD1;
	config_spi_master.pinmux_pad2 = CONF_MASTER_PINMUX_PAD2;
	config_spi_master.pinmux_pad3 = CONF_MASTER_PINMUX_PAD3;
	config_spi_master.mode_specific.master.baudrate = 4000000;
	
	spi_init(&spi_master_instance, SENSOR_SERCOM, &config_spi_master);
	spi_enable(&spi_master_instance);
}

int8_t initSensor(void);
int8_t initSensor(void)
{
	int8_t rslt = BMI160_OK;

	/* Select the Output data rate, range of accelerometer sensor */
	sensor.accel_cfg.odr = BMI160_ACCEL_ODR_1600HZ;
	sensor.accel_cfg.range = BMI160_ACCEL_RANGE_4G; //BMI160_ACCEL_RANGE_2G;
	sensor.accel_cfg.bw = BMI160_ACCEL_BW_NORMAL_AVG4;

	/* Select the power mode of accelerometer sensor */
	sensor.accel_cfg.power = BMI160_ACCEL_NORMAL_MODE;

	/* Select the Output data rate, range of Gyroscope sensor */
	sensor.gyro_cfg.odr = BMI160_GYRO_ODR_3200HZ;
	sensor.gyro_cfg.range = BMI160_GYRO_RANGE_2000_DPS;
	sensor.gyro_cfg.bw = BMI160_GYRO_BW_NORMAL_MODE;

	/* Select the power mode of Gyroscope sensor */
	sensor.gyro_cfg.power = BMI160_GYRO_NORMAL_MODE;


	/* Select the Output data rate, range of accelerometer sensor */
	sensor_1.accel_cfg.odr = BMI160_ACCEL_ODR_1600HZ;
	sensor_1.accel_cfg.range = BMI160_ACCEL_RANGE_4G; //BMI160_ACCEL_RANGE_2G;
	sensor_1.accel_cfg.bw = BMI160_ACCEL_BW_NORMAL_AVG4;

	/* Select the power mode of accelerometer sensor */
	sensor_1.accel_cfg.power = BMI160_ACCEL_NORMAL_MODE;

	/* Select the Output data rate, range of Gyroscope sensor */
	sensor_1.gyro_cfg.odr = BMI160_GYRO_ODR_3200HZ;
	sensor_1.gyro_cfg.range = BMI160_GYRO_RANGE_2000_DPS;
	sensor_1.gyro_cfg.bw = BMI160_GYRO_BW_NORMAL_MODE;

	/* Select the power mode of Gyroscope sensor */
	sensor_1.gyro_cfg.power = BMI160_GYRO_NORMAL_MODE;
	
	/* Set the sensor configuration */
	rslt = bmi160_set_sens_conf(&sensor);
	rslt += bmi160_set_sens_conf(&sensor_1);
	return rslt;
}

void user_delay_ms(uint32_t period)
{
    delay_ms(period);
}

int8_t user_spi_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */
    /*
     * The parameter dev_id can be used as a variable to select which Chip Select pin has
     * to be set low to activate the relevant device on the SPI bus
     */
	
    /*
     * Data on the bus should be like
     * |----------------+---------------------+-------------|
     * | MOSI           | MISO                | Chip Select |
     * |----------------+---------------------|-------------|
     * | (don't care)   | (don't care)        | HIGH        |
     * | (reg_addr)     | (don't care)        | LOW         |
     * | (don't care)   | (reg_data[0])       | LOW         |
     * | (....)         | (....)              | LOW         |
     * | (don't care)   | (reg_data[len - 1]) | LOW         |
     * | (don't care)   | (don't care)        | HIGH        |
     * |----------------+---------------------|-------------|
     */
	
	uint8_t buffer[len];
		
	memset(buffer, 0x00, len);
	
	switch (dev_id)
	{
		case 0:	spi_select_slave(&spi_master_instance, &slave_0, true);
			break;
		case 1: spi_select_slave(&spi_master_instance, &slave_1, true);
			break;
	}

	rslt = spi_transceive_wait(&spi_master_instance, reg_addr, NULL);
	rslt = spi_transceive_buffer_wait(&spi_master_instance, buffer, reg_data, len);
	
	spi_select_slave(&spi_master_instance, &slave_0, false);
	spi_select_slave(&spi_master_instance, &slave_1, false);

    
	return rslt;
}

int8_t user_spi_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */
	
    /*
     * The parameter dev_id can be used as a variable to select which Chip Select pin has
     * to be set low to activate the relevant device on the SPI bus
     */

    /*
     * Data on the bus should be like
     * |---------------------+--------------+-------------|
     * | MOSI                | MISO         | Chip Select |
     * |---------------------+--------------|-------------|
     * | (don't care)        | (don't care) | HIGH        |
     * | (reg_addr)          | (don't care) | LOW         |
     * | (reg_data[0])       | (don't care) | LOW         |
     * | (....)              | (....)       | LOW         |
     * | (reg_data[len - 1]) | (don't care) | LOW         |
     * | (don't care)        | (don't care) | HIGH        |
     * |---------------------+--------------|-------------|
     */

	uint8_t buffer[len];
	memset(buffer, 0x00, len);
	
	switch (dev_id)
	{
		case 0:	spi_select_slave(&spi_master_instance, &slave_0, true);
		break;
		case 1: spi_select_slave(&spi_master_instance, &slave_1, true);
		break;
	}
	
	rslt = spi_transceive_wait(&spi_master_instance, reg_addr, NULL);
	rslt = spi_transceive_buffer_wait(&spi_master_instance, reg_data, buffer, len);

	spi_select_slave(&spi_master_instance, &slave_0, false);
	spi_select_slave(&spi_master_instance, &slave_1, false);

    return rslt;
}

void RP_calculate( void );
void RP_calculate( void )
{
	float x_Buff = accel.x;
	float y_Buff = accel.y;
	float z_Buff = accel.z;
	roll  = atan2(x_Buff , sqrt(y_Buff * y_Buff + z_Buff * z_Buff)) * 57.3;
	pitch = atan2(y_Buff , sqrt(x_Buff * z_Buff + z_Buff * z_Buff)) * 57.3;
	
	x_Buff = accel_1.x;
	y_Buff = accel_1.y;
	z_Buff = accel_1.z;
	roll_1  = atan2(x_Buff , sqrt(y_Buff * y_Buff + z_Buff * z_Buff)) * 57.3;
	pitch_1 = atan2(y_Buff , sqrt(x_Buff * x_Buff + z_Buff * z_Buff)) * 57.3;
}

int main (void)
{
	system_init();
	irq_initialize_vectors();
	cpu_irq_enable();

	configure_adc();
	configure_spi_master();

	// Start USB stack to authorize VBus monitoring
	udc_start();
	stdio_usb_init();
	stdio_usb_enable();
	
	int8_t rslt = 0;
	
	sensor.id = 0;
	sensor.interface = BMI160_SPI_INTF;
	sensor.read = user_spi_read;
	sensor.write = user_spi_write;
	sensor.delay_ms = user_delay_ms;
	
	sensor_1.id = 1;
	sensor_1.interface = BMI160_SPI_INTF;
	sensor_1.read = user_spi_read;
	sensor_1.write = user_spi_write;
	sensor_1.delay_ms = user_delay_ms;

	rslt = BMI160_OK;
	rslt = bmi160_init(&sensor);
	rslt = bmi160_init(&sensor_1);
	rslt = initSensor();
	// Read chip_id
	uint8_t chip_id = 0;
	rslt = bmi160_get_regs(BMI160_CHIP_ID_ADDR, &chip_id, 1, &sensor);
	rslt = bmi160_get_regs(BMI160_CHIP_ID_ADDR, &chip_id, 1, &sensor_1);

	SSD1306_Begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
	SSD1306_ClearDisplay();
	
	uint8_t stat = 0;
	bool heartbeat = true;
	
	int32_t avrgRoll = 0;
	int32_t zeroAngle = 0;
	int32_t currentAngle = 0;
	int32_t currentRawAngle = 0;
	
	int32_t avrgRoll_1 = 0;
	int32_t zeroAngle_1 = 0;
	int32_t currentAngle_1 = 0;
	int32_t currentRawAngle_1 = 0;
	
	int32_t minAngle = 360;
	int32_t maxAngle = -360;
	
	int32_t cycleCounter = 0;
	int32_t adc_val_avg = 0;
	int32_t storeAdcValue = 0;
	
	uint8_t avgPotenz = 7;
	uint8_t avgFaktor = pow (2, avgPotenz);
	
	
	while (1)
	{
		SSD1306_ClearDisplay();
		
		port_pin_toggle_output_level(TP_2);
		bool sw_0 = port_pin_get_input_level(BUTTON_0_PIN);		// CALIB
		bool sw_1 = port_pin_get_input_level(BUTTON_1_PIN);		// ZERO
		bool chgStatus = port_pin_get_input_level(CHARGE_STATUS);		// True if Full, Low if charge
		
		adc_start_conversion(&adc_instance);
		while((adc_get_status(&adc_instance) & ADC_STATUS_RESULT_READY) != 1);	
		adc_read(&adc_instance, &adc_val);
		adc_val = adc_val / 6.62;
		
		if(adc_val	> 1000)
		{
			//SSD1306_DrawBMP(74, 0, plug_icon16x16, 16, 16);
			if(!chgStatus)
			{
				drawAkku(stat++);
				if(stat > 3) stat = 0;
			}
			else
			{
				drawAkku(3);
			}
		}
		
		for (int n = 0; n < avgFaktor; n++)
		{
			/* To read both Accel and Gyro data */
			rslt = bmi160_get_sensor_data((BMI160_ACCEL_SEL | BMI160_GYRO_SEL), &accel,   &gyro,   &sensor);
			rslt = bmi160_get_sensor_data((BMI160_ACCEL_SEL | BMI160_GYRO_SEL), &accel_1, &gyro_1, &sensor_1);
			RP_calculate();
			avrgRoll += (int32_t)(roll*100);
			avrgRoll_1 += (int32_t)(roll_1*100);
		}
		currentRawAngle = avrgRoll / avgFaktor;
		avrgRoll = 0;
		currentRawAngle_1 = avrgRoll_1 / avgFaktor;
		avrgRoll_1 = 0;
		
		currentAngle = (zeroAngle - currentRawAngle);
		currentAngle_1 = (zeroAngle_1 - currentRawAngle_1);
		
		int16_t temperature = 0;
		rslt = bmi160_get_regs(BMI160_TEMPERATURE_ADDR, &temperature, 2, &sensor);
		if (temperature > 0)
		{
			temperature = (temperature * 0.001953) + 23;
		}
		else
		{
			temperature = -100;
		}
		
		int16_t temperature_1 = 0;
		rslt = bmi160_get_regs(BMI160_TEMPERATURE_ADDR, &temperature_1, 2, &sensor_1);
		if (temperature_1 > 0)
		{
			temperature_1 = (temperature_1 * 0.001953) + 23;
		}
		else
		{
			temperature_1 = -100;
		}
		
		if(!sw_1) // ZERO BUTTON
		{
			zeroAngle = currentRawAngle;
			zeroAngle_1 = currentRawAngle_1;
			
			minAngle = currentRawAngle;
			maxAngle = currentRawAngle;
		}
		
		if (!sw_0) // CALIB BUTTON
		{
			delay_ms(10);

			if (avgPotenz >= 7)
			{
				avgPotenz = 0;
			}
			else
			{
				avgPotenz++;
			}
			avgFaktor = pow (2, avgPotenz);

			while(port_pin_get_input_level(BUTTON_0_PIN) == false)
			{
				delay_ms(10);
			}

		}
		drawAverageFaktor(avgFaktor);
		
		int tmp = max(temperature, temperature_1);
		
		if(currentAngle < minAngle) minAngle = currentAngle;
		if(currentAngle > maxAngle) maxAngle = currentAngle;
		
		drawOuterFrame();
		drawTemeprature(tmp);
		drawAngle(currentAngle);
		drawMinMaxWinkel( minAngle, maxAngle );
	
		if(accel.y < -5000)
		{
			SSD1306_FlipHorizontalDisplay(false);
		}
		else
		{
			SSD1306_FlipHorizontalDisplay(true);
		}
			
		SSD1306_Display();      // Show initial text


		
		delay_ms(10);
		
		if(my_flag_DTR)	
		{	
			//printf("Temp: %03d --> Sensor [1]: sts %d; ACC[x,y,z] %04d %04d %04d, GYR[x,y,z] %04d %04d %04d Ang: [x,y] %03d %03\n\r", rslt, accel_1.x, accel_1.y, accel_1.z, gyro_1.x,  gyro_1.y, gyro_1.z, (int32_t)(roll_1*100), (int16_t)pitch_1);
			printf("Temp: %03d --> Sensor [0]: ACC[x,y,z] %04d %04d %04d, Ang: [x,y] %03d %03; [%03d]\t\t", tmp, accel.x, accel.y, accel.z, (int32_t)(roll*100), (int16_t)(pitch*100), currentAngle);
			printf("Temp: %03d --> Sensor [1]: ACC[x,y,z] %04d %04d %04d, Ang: [x,y] %03d %03; [%03d]\n\r", tmp, accel_1.x, accel_1.y, accel_1.z, (int32_t)(roll_1*100), (int16_t)(pitch_1*100), currentAngle_1);
		}
	}
}

void drawAkku(uint8_t stat)
{
	switch(stat)
	{
		case 0: SSD1306_DrawBMP(100, 0, bat0_icon16x16, 16, 16);
		break;
		case 1: SSD1306_DrawBMP(100, 0, bat1_icon16x16, 16, 16);
		break;
		case 2: SSD1306_DrawBMP(100, 0, bat2_icon16x16, 16, 16);
		break;
		case 3: SSD1306_DrawBMP(100, 0, bat3_icon16x16, 16, 16);
		break;
		default:
		break;
	}
}


void drawOuterFrame(void)
{
	SSD1306_DrawLine(0, 0, 127, 0, TRUE);		// ----------
	SSD1306_DrawLine(0, 63, 127, 63, TRUE);		// |
	SSD1306_DrawLine(0, 0, 0, 63, TRUE);		//			 | 
	SSD1306_DrawLine(127, 0, 127, 63, TRUE);	// ___________
}

void drawTemeprature(int32_t t)
{
	char str_adc[7];
	sprintf(str_adc, "%d%c", t, 167);
	SSD1306_DrawText(17, 1, str_adc, 2);
	
	SSD1306_DrawBMP(1, 1, temperature_icon16x16, 16, 16);
	SSD1306_DrawLine(63, 0, 63, 17, TRUE);
}

void drawAverageFaktor(uint8_t f)
{
		char str[7];
		sprintf(str, "%c:%03d", 0xED, f);
		SSD1306_DrawText(65, 4, str, 1);
}

void drawAngle(int32_t angle)
{
	SSD1306_DrawLine(1, 17, 126, 17, TRUE);
	
	int vorkomma = (int)angle/100;
	int nachkomma = abs((int)(angle - vorkomma*100));
	
	char str[10];
	//Gradzeichen ASCII = 167
	sprintf(str, "%03d.%02d", vorkomma, nachkomma);
	SSD1306_DrawText(5, 25, str, 3);
}

void drawWinkelSymbol(void)
{
	SSD1306_DrawLine(5, 10, 15, 10, TRUE);
	SSD1306_DrawLine(5, 10, 15, 5, TRUE);
}

void drawMinMaxWinkel(int32_t minValue, int32_t maxValue)
{	
	SSD1306_DrawLine(1, 52, 126, 52, TRUE);
	SSD1306_DrawLine(63, 52, 63, 64, TRUE);
	
	char text[12];
	memset(text, 0, 12);
	int vorkomma = (int)minValue/100;
	int nachkomma = abs((int)(minValue - vorkomma*100));

	sprintf(text, "%c %d.%02d", 31, vorkomma, nachkomma);
	SSD1306_DrawText(5, 55, text, 1);
	
	memset(text, 0, 12);
	vorkomma = (int)maxValue/100;
	nachkomma = abs((int)(maxValue - vorkomma*100));

	sprintf(text, "%c %d.%02d", 30, vorkomma, nachkomma);
	SSD1306_DrawText(70, 55, text, 1);
}