# Code for 6Sens Project : LoRaWAN code for my own MAMWLExx (based on STM32WLEx) board

## Overview
This application contains example code to allow testing of LoRaWAN Network Application. This code also allows to test a physical link from the node to the gateway then from the gateway to TTN. The packets sent have the following characteristics:

    - Timer : packets every 30 secondes
    - Random value with the range from min = 0V to max = 3.3V like values from ADC (sensor/battery)
    - random value with the range fron 0 to 100% for humidity
    - random value with the range from -25 to +50 degree celsius
    - Number of packets : forever loop
    - Number of nodes : 5

LoRaWAN Device Address, Device EUI, Application Key and New Key should be entered into the app_lorawan.h file prior to compiling. The example uses ABP type authentification. The version of Zephyr RTOS used is the version v3.6.0.

**Board used** : Original MAMWLExx board, powered by battery/solar panel. (see 6sens_project_stm32wl repository/hardware part, for more information.)

## General Information of Application
You will need to register new devices in your application (with ABP activation method). Once this is done, retain the TTN Device Address(4 Bytes), the TTN Network Key(16 Bytes) and the TTN Application Key (16 Bytes). You also have to make sure that the activation method is OTAA. Note that the zephyr code uses the app_eui pointer which corresponds to the DevEUI activation parameter.

    - After you account was created, you have to create a new application
    - After that, you have to add a new end device on this application. You have to complete the various fields using the available data below in manually mode :
        Frequency Plan                  Europe 863-870 MHz (SF9 for RX2 - Recommended)
        LoRaWAN Version                 MAC V1.0.3
        Regional Parameter Version      RP001 Regional Parameters 1.0.3 revision A
        Activation by personalization   ABP
        Application ID                  give a name
        Device                          random value for 4-byte address
        DevEUI                          random value for 8-byte address
        NwkSKey                         random value for 16-byte address
        AppSKey                         random value for 16-byte address

## Building and Running
The following commands clean build folder, build and flash the sample:

**Command to use**

west build -p always -b lora_e5_mini applications/stm32wle5_rtos_ttn_abp

west flash --runner pyocd