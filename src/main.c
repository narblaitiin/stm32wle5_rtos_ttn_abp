/*
 * Copyright (c) 2024
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app_lorawan.h"

//  ======== globals ============================================
static const struct gpio_dt_spec led_tx = GPIO_DT_SPEC_GET(LED_TX, gpios);
static const struct gpio_dt_spec led_rx = GPIO_DT_SPEC_GET(LED_RX, gpios);

struct values {
	int16_t vbat;
	int16_t temp;
	int16_t hum;
	int16_t vadc;
};

//  ======== main ===============================================
int8_t main(void)
{
	const struct device *dev;
	struct values data;
	uint8_t payload[9];
    int8_t ret;

	// configuration of LEDs
	gpio_pin_configure_dt(&led_tx, GPIO_OUTPUT_ACTIVE);
	gpio_pin_configure_dt(&led_rx, GPIO_OUTPUT_ACTIVE);
	gpio_pin_set_dt(&led_tx, 0);
	gpio_pin_set_dt(&led_rx, 0);

	// initialization of LoRaWAN - TTN
	app_lorawan_init(dev);

	printk("Geophone Measurement Simulation and Process Information - Board: %s\n", CONFIG_BOARD);
	
	// beginning forever loop (polling mode)
	while (1) {
		// constrution of random data structure
		data.vbat = sys_rand16_get() % (3300 - 0 + 1) + 0;
		data.temp = -15 + sys_rand16_get() % (50 - 25 +1);
		data.hum = sys_rand16_get() % (100 - 0 + 1) + 0;
		data.vadc = sys_rand16_get() % (3300 - 0 + 1) + 0;
		printk("vbat: %"PRId16", temp: %"PRId16", hum: %"PRId16", vadc: %"PRId16"\n", data.vbat, data.temp, data.hum, data.vadc);
		
		// transmission of packets on lorawan protocole - encode payload to bytes
		// battery needs 2 bytes
		payload[0] = (data.vbat >> 8) & 0xFF;
        payload[1] = data.vbat & 0xFF;

		// temperature needs 3 byte 327.67 max value (signed short)
		if (data.temp < 0) {
			payload[2] = 1 & 0xff;
		} else {
			payload[2] = 1 & 0xff;
		}
		payload[3] = (data.temp >> 8) & 0xFF;
        payload[4] = data.temp & 0xFF;

		// humidity needs 2 bytes
		payload[5] = (data.hum >> 8) & 0xFF;
        payload[6] = data.hum & 0xFF;

		// adc needs 2 bytes
		payload[7] = (data.vadc >> 8) & 0xFF;
        payload[8] = data.vadc & 0xFF;

		printk("sending random data...\n");
		gpio_pin_set_dt(&led_tx, 1);
		ret = lorawan_send(LORAWAN_PORT, payload, sizeof(payload), LORAWAN_MSG_CONFIRMED);
		if (ret == -EAGAIN) {
			printk("lorawan_send failed: %d. continuing...\n", ret);
			k_sleep(DELAY);
			continue;
		}
		
		if (ret < 0) {
			printk("lorawan_send failed: %d.\n", ret);
			//return 0;
			k_sleep(DELAY);
			continue;
		} else {
			// flashing of the LED when a packet is transmitted
			gpio_pin_set_dt(&led_tx, 0);
			printk("data sent!\n"); 
			k_sleep(DELAY);
		}
	}
	return 0;
}