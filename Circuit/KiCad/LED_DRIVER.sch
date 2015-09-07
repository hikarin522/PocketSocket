EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:RoboKUT_communication
LIBS:RoboKUT_conn
LIBS:RoboKUT_conn_KRP
LIBS:RoboKUT_IC
LIBS:RoboKUT_Passive
LIBS:RoboKUT_regul
LIBS:RoboKUT_sensor
LIBS:RoboKUT_special
LIBS:RoboKUT_SW
LIBS:RoboKUT_uC
LIBS:PocketSocket-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 26
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L GND #PWR029
U 1 1 55EC5EE0
P 2650 2550
F 0 "#PWR029" H 2650 2300 50  0001 C CNN
F 1 "GND" H 2650 2400 50  0000 C CNN
F 2 "" H 2650 2550 60  0000 C CNN
F 3 "" H 2650 2550 60  0000 C CNN
	1    2650 2550
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR030
U 1 1 55EC5EF4
P 4050 1950
F 0 "#PWR030" H 4050 1800 50  0001 C CNN
F 1 "VCC" H 4050 2100 50  0000 C CNN
F 2 "" H 4050 1950 60  0000 C CNN
F 3 "" H 4050 1950 60  0000 C CNN
	1    4050 1950
	1    0    0    -1  
$EndComp
Text HLabel 2550 2450 0    60   Input ~ 0
PWM
Text HLabel 3550 2750 0    60   Output ~ 0
LEDA
$Comp
L CL6807 U?
U 1 1 55EC6028
P 3150 2350
F 0 "U?" H 3150 2550 60  0000 C CNN
F 1 "CL6807" H 3150 2150 60  0000 C CNN
F 2 "" H 3100 2300 60  0000 C CNN
F 3 "" H 3100 2300 60  0000 C CNN
	1    3150 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 2450 2750 2450
Wire Wire Line
	2750 2350 2650 2350
Wire Wire Line
	2650 2350 2650 2550
Text HLabel 1850 2050 0    60   Input ~ 0
LEDC
$Comp
L INDUCTOR L?
U 1 1 55EC62FB
P 2250 2050
F 0 "L?" V 2200 2050 50  0000 C CNN
F 1 "68u" V 2350 2050 50  0000 C CNN
F 2 "" H 2250 2050 60  0000 C CNN
F 3 "" H 2250 2050 60  0000 C CNN
	1    2250 2050
	0    1    1    0   
$EndComp
$Comp
L C C?
U 1 1 55EC639A
P 4050 2500
F 0 "C?" H 4075 2600 50  0000 L CNN
F 1 "4.7u" H 4075 2400 50  0000 L CNN
F 2 "" H 4088 2350 30  0000 C CNN
F 3 "" H 4050 2500 60  0000 C CNN
	1    4050 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 2250 2650 2250
Wire Wire Line
	3650 2450 3550 2450
$Comp
L GND #PWR?
U 1 1 55EC6949
P 4050 2750
F 0 "#PWR?" H 4050 2500 50  0001 C CNN
F 1 "GND" H 4050 2600 50  0000 C CNN
F 2 "" H 4050 2750 60  0000 C CNN
F 3 "" H 4050 2750 60  0000 C CNN
	1    4050 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 2650 4050 2750
$Comp
L R R?
U 1 1 55EC6B8C
P 3800 2500
F 0 "R?" V 3880 2500 50  0000 C CNN
F 1 "0.27" V 3800 2500 50  0000 C CNN
F 2 "" V 3730 2500 30  0000 C CNN
F 3 "" H 3800 2500 30  0000 C CNN
	1    3800 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 2450 3650 2750
Wire Wire Line
	3550 2750 3800 2750
Wire Wire Line
	3800 2750 3800 2650
Wire Wire Line
	3550 2250 4050 2250
Wire Wire Line
	3800 2250 3800 2350
Wire Wire Line
	4050 1950 4050 2350
Connection ~ 3800 2250
Connection ~ 4050 2250
Connection ~ 3650 2750
$Comp
L D_Schottky D?
U 1 1 55EC6E68
P 3750 2050
F 0 "D?" H 3750 2150 50  0000 C CNN
F 1 "D_Schottky" H 3750 1950 50  0000 C CNN
F 2 "" H 3750 2050 60  0000 C CNN
F 3 "" H 3750 2050 60  0000 C CNN
	1    3750 2050
	-1   0    0    1   
$EndComp
Wire Wire Line
	3900 2050 4050 2050
Connection ~ 4050 2050
Wire Wire Line
	2550 2050 3600 2050
Wire Wire Line
	2650 2250 2650 2050
Connection ~ 2650 2050
Wire Wire Line
	1850 2050 1950 2050
$EndSCHEMATC
