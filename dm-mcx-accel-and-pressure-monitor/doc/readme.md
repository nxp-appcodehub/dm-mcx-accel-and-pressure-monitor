Multi sensor demo with FRDM-MCXN947 and LDC-PAR-S035


Overview
========
This demo shows how to integrate the NXP sensors FXLS8974CF (accelerometer) and MPL3115 (pressure) with FRDM-MCXN947 platform. Both sensors share the I2C bus of the Accel & Pressure click board, that is used in the demo for digital communication between the MCXN947 and the sensors.

The demo uses the LVGL graphics framework and a TFT Display (LCD-PAR-S035) with a resolution of 480x320 pixels. The GUI shows a multi-Tab menu that allows the user to:

Monitor the 3-axis accelerometer measurements.
Monitor the pressure sensor measurements in Pascals.
Watch a live chart with the latest 40 points of the 3-axis accelerometer.
The GUI was designed with GUI Guider (NXP).


Software
===========
- IoT Sensing SDK (ISSDK) v1.8 offered as middleware in MCUXpresso SDK for supported platforms
- MCXUpresso SDK 2.14.0 for FRDM-MCXN947
- MCUXpresso IDE v11.9.0
- Git v2.39.0


Toolchain supported
===================
- MCUXpresso  11.9.0


Hardware requirements
=====================
For this demo you will need the next hardware.
- FRDM-MCXN947 MCU board
- LCD-PAR-S035 TFT Display
- Accel & Pressure Click Board
- Personal Computer
- Mini/micro C USB cable


Prepare the Demo
================
1. Connect Display to FRDM-MCXN947
2. Connect Accel & Pressure click board to FRDM-MCXN947
3. Download and Install required Software(s)
4. Clone the APP-CODE-HUB/dm-fxls8974-and-mpl3115-sensor-monitor
5.  Build demo project


Running the demo
================
The demo will start running inmediately after the FW is downloaded to the target from MCUXpresso.
The GUI shows the Accel. panel by default. It will show acceleration measurements coming from the 3-axis accelerometer (FXLS8974CD), note that the vector magnitude is also shown.

The user can easily switches to another tab by tapping, for example to visualize the pressure subjected to the MPL3115 sensor.
The third tab "chart" shows the latest collected 40 samples for the three axis of the accel.
