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

// downlink callback
static void dl_callback(uint8_t port, bool data_pending, int16_t rssi, int8_t snr, uint8_t len, const uint8_t *data)
{
	printk("downlink data received: ");
    for(int8_t i=0; i < len; i++) {
		printk("%02X ", data[i]);
	}
    printk("\n");
	printk("port: %d, pending: %d, RSSI: %ddB, SNR: %dBm\n", port, data_pending, rssi, snr);
}

// ADR change callback
static void lorwan_datarate_changed(enum lorawan_datarate dr)
{
	uint8_t unused, max_size;

	lorawan_get_payload_sizes(&unused, &max_size);
	printk("new datarate: DR_%d, max 	`payload: %d\n", dr, max_size);
}

//  ======== app_loarwan_init ===================================
int8_t app_lorawan_init(const struct device *dev)
{
	struct lorawan_join_config join_cfg;

    int8_t ret;
	uint8_t dev_eui[]  = LORAWAN_DEV_EUI;
    uint8_t dev_addr[] = LORAWAN_DEV_ADDR;
	uint8_t app_skey[] = LORAWAN_APP_SKEY;
    uint8_t nwk_skey[] = LORAWAN_NWK_SKEY;
	uint8_t app_eui[]  = LORAWAN_DEV_EUI;

	printk("starting lorawan node\n");
    // getting lora sx1276 device
	dev = DEVICE_DT_GET(DT_ALIAS(lora0));
	if (!device_is_ready(dev)) {
		printk("%s: device not ready\n", dev->name);
		return 0;
	}

	printk("starting lorawan stack\n");
    // starting device
	ret = lorawan_set_region(LORAWAN_REGION_EU868);
	if (ret < 0) {
		printk("lorawan_set_region failed: %d\n", ret);
		return 0;
	}

	ret = lorawan_start();
	if (ret < 0) {
		printk("lorawan_start failed. error: %d\n", ret);
		return 0;
	} else {
		k_sleep(K_MSEC(500));
	}
	gpio_pin_set_dt(&led_rx, 0);

	// enable ADR
    lorawan_enable_adr(true);

    // enable callbacks
	struct lorawan_downlink_cb downlink_cb = {
		.port = LW_RECV_PORT_ANY,
		.cb = dl_callback
	};
	lorawan_register_downlink_callback(&downlink_cb);
	lorawan_register_dr_changed_callback(lorwan_datarate_changed);  

	// configuration of lorawan parameters 
    join_cfg.mode = LORAWAN_ACT_ABP;
    join_cfg.dev_eui = app_eui;
    join_cfg.abp.dev_addr = dev_addr;
    join_cfg.abp.app_skey = app_skey;
    join_cfg.abp.nwk_skey = nwk_skey;
//    join_cfg.abp.app_eui  = app_eui;

	printk("joining TTN network over ABP\n");
	do {
		gpio_pin_set_dt(&led_rx, 1);
		ret = lorawan_join(&join_cfg);
		if (ret < 0) {
	    	printk("lorawan join network failed. error: %d\n", ret);
			printk("sleeping for 10s to try again to join network.\n");
			gpio_pin_set_dt(&led_rx, 0);
			k_sleep(K_MSEC(10000));
		}
		gpio_pin_set_dt(&led_rx, 1);
	} while (ret < 0);

	printk("setting device to class A\n");
	ret = lorawan_set_class(LORAWAN_CLASS_A);
	if (ret != 0) {
		printk("failed to set LoRaWAN class. error: %d", ret);
	}
	gpio_pin_set_dt(&led_rx, 0);
    return 0;
}



  
    