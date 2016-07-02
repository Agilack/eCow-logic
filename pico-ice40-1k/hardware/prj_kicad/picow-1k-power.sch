EESchema Schematic File Version 2
LIBS:picow-1k
LIBS:picow-1k-cache
EELAYER 24 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 5
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
L CONN_DC-2.0 P3
U 1 1 5644BD51
P 1700 1650
F 0 "P3" H 1550 1350 60  0000 C CNN
F 1 "CONN_DC-2.0" H 1650 1950 60  0000 C CNN
F 2 "" H 1700 1650 60  0000 C CNN
F 3 "" H 1700 1650 60  0000 C CNN
	1    1700 1650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR095
U 1 1 5644BD65
P 2050 1900
F 0 "#PWR095" H 2050 1900 30  0001 C CNN
F 1 "GND" H 2050 1830 30  0001 C CNN
F 2 "" H 2050 1900 60  0000 C CNN
F 3 "" H 2050 1900 60  0000 C CNN
	1    2050 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	2050 1900 2050 1800
Wire Wire Line
	2050 1800 1950 1800
$Comp
L C C201
U 1 1 5644CD2E
P 2500 1950
F 0 "C201" H 2550 2050 50  0000 L CNN
F 1 "10uF" H 2550 1850 50  0000 L CNN
F 2 "" H 2500 1950 60  0000 C CNN
F 3 "" H 2500 1950 60  0000 C CNN
	1    2500 1950
	1    0    0    -1  
$EndComp
$Comp
L C C202
U 1 1 5644CD3D
P 2800 1950
F 0 "C202" H 2850 2050 50  0000 L CNN
F 1 "10uF" H 2850 1850 50  0000 L CNN
F 2 "" H 2800 1950 60  0000 C CNN
F 3 "" H 2800 1950 60  0000 C CNN
	1    2800 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 1650 3600 1650
Wire Wire Line
	2500 1550 2500 1750
Wire Wire Line
	2800 1650 2800 1750
Connection ~ 2500 1650
$Comp
L GND #PWR096
U 1 1 5644CD54
P 2500 2250
F 0 "#PWR096" H 2500 2250 30  0001 C CNN
F 1 "GND" H 2500 2180 30  0001 C CNN
F 2 "" H 2500 2250 60  0000 C CNN
F 3 "" H 2500 2250 60  0000 C CNN
	1    2500 2250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR097
U 1 1 5644CD63
P 2800 2250
F 0 "#PWR097" H 2800 2250 30  0001 C CNN
F 1 "GND" H 2800 2180 30  0001 C CNN
F 2 "" H 2800 2250 60  0000 C CNN
F 3 "" H 2800 2250 60  0000 C CNN
	1    2800 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 2250 2800 2150
Wire Wire Line
	2500 2250 2500 2150
$Comp
L TPS560200 U200
U 1 1 5644CEB8
P 4150 1800
F 0 "U200" H 4150 2200 60  0000 C CNN
F 1 "TPS560200" H 4150 1800 60  0000 C CNN
F 2 "" H 4150 1800 60  0000 C CNN
F 3 "" H 4150 1800 60  0000 C CNN
	1    4150 1800
	1    0    0    -1  
$EndComp
Connection ~ 2800 1650
Connection ~ 3400 1650
$Comp
L GND #PWR098
U 1 1 5644CEDF
P 4150 2300
F 0 "#PWR098" H 4150 2300 30  0001 C CNN
F 1 "GND" H 4150 2230 30  0001 C CNN
F 2 "" H 4150 2300 60  0000 C CNN
F 3 "" H 4150 2300 60  0000 C CNN
	1    4150 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 2300 4150 2250
$Comp
L INDUCTOR L200
U 1 1 5644CEFB
P 5100 1650
F 0 "L200" V 5200 1650 50  0000 C CNN
F 1 "10uH" V 5050 1650 40  0000 C CNN
F 2 "" H 5100 1650 60  0000 C CNN
F 3 "" H 5100 1650 60  0000 C CNN
F 4 "Laird Technologies" V 5100 1650 60  0001 C CNN "Manufaturer"
F 5 "TYS4012100M-10" V 5100 1650 60  0001 C CNN "Manuf Ref"
	1    5100 1650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4800 1650 4700 1650
$Comp
L C C203
U 1 1 56451280
P 5450 1900
F 0 "C203" H 5200 2000 50  0000 L CNN
F 1 "C" H 5500 1800 50  0000 L CNN
F 2 "" H 5450 1900 60  0000 C CNN
F 3 "" H 5450 1900 60  0000 C CNN
	1    5450 1900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR099
U 1 1 56451299
P 5450 2150
F 0 "#PWR099" H 5450 2150 30  0001 C CNN
F 1 "GND" H 5450 2080 30  0001 C CNN
F 2 "" H 5450 2150 60  0000 C CNN
F 3 "" H 5450 2150 60  0000 C CNN
	1    5450 2150
	1    0    0    -1  
$EndComp
$Comp
L R R201
U 1 1 564512A8
P 5700 1950
F 0 "R201" V 5800 1950 50  0000 C CNN
F 1 "R" V 5700 1950 50  0000 C CNN
F 2 "" H 5700 1950 60  0000 C CNN
F 3 "" H 5700 1950 60  0000 C CNN
	1    5700 1950
	1    0    0    -1  
$EndComp
$Comp
L R R202
U 1 1 564512B7
P 5700 2650
F 0 "R202" V 5800 2650 50  0000 C CNN
F 1 "R" V 5700 2650 50  0000 C CNN
F 2 "" H 5700 2650 60  0000 C CNN
F 3 "" H 5700 2650 60  0000 C CNN
	1    5700 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 2200 5700 2400
Wire Wire Line
	5700 2300 5100 2300
Wire Wire Line
	5100 2300 5100 1950
Wire Wire Line
	5100 1950 4700 1950
Connection ~ 5700 2300
Wire Wire Line
	5400 1650 6200 1650
Wire Wire Line
	5450 1650 5450 1700
Wire Wire Line
	5450 2150 5450 2100
Wire Wire Line
	5700 1650 5700 1700
Connection ~ 5450 1650
Wire Wire Line
	5700 2950 5700 2900
$Comp
L GND #PWR0100
U 1 1 5645134A
P 5700 2950
F 0 "#PWR0100" H 5700 2950 30  0001 C CNN
F 1 "GND" H 5700 2880 30  0001 C CNN
F 2 "" H 5700 2950 60  0000 C CNN
F 3 "" H 5700 2950 60  0000 C CNN
	1    5700 2950
	1    0    0    -1  
$EndComp
$Comp
L CP C204
U 1 1 56451386
P 6000 1900
F 0 "C204" H 6050 2000 50  0000 L CNN
F 1 "CP" H 6050 1800 50  0000 L CNN
F 2 "" H 6000 1900 60  0000 C CNN
F 3 "" H 6000 1900 60  0000 C CNN
	1    6000 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6000 1650 6000 1700
Connection ~ 5700 1650
Wire Wire Line
	6000 2150 6000 2100
$Comp
L GND #PWR0101
U 1 1 564513CB
P 6000 2150
F 0 "#PWR0101" H 6000 2150 30  0001 C CNN
F 1 "GND" H 6000 2080 30  0001 C CNN
F 2 "" H 6000 2150 60  0000 C CNN
F 3 "" H 6000 2150 60  0000 C CNN
	1    6000 2150
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR0102
U 1 1 564516C2
P 6200 1550
F 0 "#PWR0102" H 6200 1510 30  0001 C CNN
F 1 "+3.3V" H 6200 1660 30  0000 C CNN
F 2 "" H 6200 1550 60  0000 C CNN
F 3 "" H 6200 1550 60  0000 C CNN
	1    6200 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 1650 6200 1550
Connection ~ 6000 1650
NoConn ~ 1950 1500
$Comp
L LD6815 U201
U 1 1 564B24B7
P 4150 6200
F 0 "U201" H 4150 6600 60  0000 C CNN
F 1 "LD6815" H 4150 6200 60  0000 C CNN
F 2 "" H 4150 6200 60  0000 C CNN
F 3 "" H 4150 6200 60  0000 C CNN
F 4 "NXP" H 4150 6200 60  0001 C CNN "Manufacturer"
F 5 "LD6815TD/25P" H 4150 6200 60  0001 C CNN "Manuf Ref"
	1    4150 6200
	1    0    0    -1  
$EndComp
$Comp
L +2.5V #PWR0103
U 1 1 564B24C6
P 5200 5950
F 0 "#PWR0103" H 5200 5910 30  0001 C CNN
F 1 "+2.5V" H 5200 6060 30  0000 C CNN
F 2 "" H 5200 5950 60  0000 C CNN
F 3 "" H 5200 5950 60  0000 C CNN
	1    5200 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	5200 5950 5200 6050
Wire Wire Line
	5200 6050 4700 6050
Wire Wire Line
	3200 5950 3200 6050
Wire Wire Line
	3200 6050 3600 6050
$Comp
L +3.3V #PWR0104
U 1 1 564B2515
P 3200 5950
F 0 "#PWR0104" H 3200 5910 30  0001 C CNN
F 1 "+3.3V" H 3200 6060 30  0000 C CNN
F 2 "" H 3200 5950 60  0000 C CNN
F 3 "" H 3200 5950 60  0000 C CNN
	1    3200 5950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR0105
U 1 1 564B2524
P 4150 6700
F 0 "#PWR0105" H 4150 6700 30  0001 C CNN
F 1 "GND" H 4150 6630 30  0001 C CNN
F 2 "" H 4150 6700 60  0000 C CNN
F 3 "" H 4150 6700 60  0000 C CNN
	1    4150 6700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 6700 4150 6650
Wire Wire Line
	3600 6350 3500 6350
Wire Wire Line
	3500 6350 3500 6050
Connection ~ 3500 6050
$Comp
L C C211
U 1 1 564B2C0B
P 4850 6350
F 0 "C211" H 4900 6450 50  0000 L CNN
F 1 "10uF" H 4900 6250 50  0000 L CNN
F 2 "" H 4850 6350 60  0000 C CNN
F 3 "" H 4850 6350 60  0000 C CNN
F 4 "Walsin" H 4850 6350 60  0001 C CNN "Manufacturer"
F 5 "0805X106K160CT" H 4850 6350 60  0001 C CNN "Manuf Ref"
	1    4850 6350
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 6150 4850 6050
Connection ~ 4850 6050
Wire Wire Line
	4850 6700 4850 6550
$Comp
L GND #PWR0106
U 1 1 564B2C93
P 4850 6700
F 0 "#PWR0106" H 4850 6700 30  0001 C CNN
F 1 "GND" H 4850 6630 30  0001 C CNN
F 2 "" H 4850 6700 60  0000 C CNN
F 3 "" H 4850 6700 60  0000 C CNN
	1    4850 6700
	1    0    0    -1  
$EndComp
$Comp
L TPS560200 U202
U 1 1 564B3025
P 4150 4000
F 0 "U202" H 4150 4400 60  0000 C CNN
F 1 "TPS560200" H 4150 4000 60  0000 C CNN
F 2 "" H 4150 4000 60  0000 C CNN
F 3 "" H 4150 4000 60  0000 C CNN
	1    4150 4000
	1    0    0    -1  
$EndComp
$Comp
L INDUCTOR L202
U 1 1 564B3034
P 5100 3850
F 0 "L202" V 5250 3850 40  0000 C CNN
F 1 "10uH" V 5050 3850 40  0000 C CNN
F 2 "" H 5100 3850 60  0000 C CNN
F 3 "" H 5100 3850 60  0000 C CNN
F 4 "Laird Technologies" V 5100 3850 60  0001 C CNN "Manufacturer"
F 5 "TYS4012100M-10" V 5100 3850 60  0001 C CNN "Manuf Ref"
	1    5100 3850
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4800 3850 4700 3850
$Comp
L CP C213
U 1 1 564B340E
P 6000 4150
F 0 "C213" H 6050 4250 50  0000 L CNN
F 1 "CP" H 6050 4050 50  0000 L CNN
F 2 "" H 6000 4150 60  0000 C CNN
F 3 "" H 6000 4150 60  0000 C CNN
	1    6000 4150
	1    0    0    -1  
$EndComp
$Comp
L C C212
U 1 1 564B341D
P 5500 4100
F 0 "C212" H 5250 4200 50  0000 L CNN
F 1 "C" H 5550 4000 50  0000 L CNN
F 2 "" H 5500 4100 60  0000 C CNN
F 3 "" H 5500 4100 60  0000 C CNN
	1    5500 4100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR0107
U 1 1 564B3440
P 5500 4350
F 0 "#PWR0107" H 5500 4350 30  0001 C CNN
F 1 "GND" H 5500 4280 30  0001 C CNN
F 2 "" H 5500 4350 60  0000 C CNN
F 3 "" H 5500 4350 60  0000 C CNN
	1    5500 4350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR0108
U 1 1 564B344F
P 6000 4400
F 0 "#PWR0108" H 6000 4400 30  0001 C CNN
F 1 "GND" H 6000 4330 30  0001 C CNN
F 2 "" H 6000 4400 60  0000 C CNN
F 3 "" H 6000 4400 60  0000 C CNN
	1    6000 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6000 4400 6000 4350
Wire Wire Line
	5500 4350 5500 4300
Wire Wire Line
	5400 3850 6500 3850
Wire Wire Line
	6000 3850 6000 3950
Connection ~ 5500 3850
$Comp
L +1.2V #PWR0109
U 1 1 564B3511
P 6500 3750
F 0 "#PWR0109" H 6500 3710 30  0001 C CNN
F 1 "+1.2V" H 6500 3860 30  0000 C CNN
F 2 "" H 6500 3750 60  0000 C CNN
F 3 "" H 6500 3750 60  0000 C CNN
	1    6500 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	6500 3850 6500 3750
Connection ~ 6000 3850
$Comp
L GND #PWR0110
U 1 1 564B38CA
P 4150 4500
F 0 "#PWR0110" H 4150 4500 30  0001 C CNN
F 1 "GND" H 4150 4430 30  0001 C CNN
F 2 "" H 4150 4500 60  0000 C CNN
F 3 "" H 4150 4500 60  0000 C CNN
	1    4150 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 4500 4150 4450
Wire Wire Line
	3600 4150 3500 4150
Wire Wire Line
	2600 3850 3600 3850
$Comp
L R R203
U 1 1 564B401D
P 5700 4150
F 0 "R203" V 5800 4150 50  0000 C CNN
F 1 "R" V 5700 4150 50  0000 C CNN
F 2 "" H 5700 4150 60  0000 C CNN
F 3 "" H 5700 4150 60  0000 C CNN
	1    5700 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 3900 5700 3850
Connection ~ 5700 3850
$Comp
L R R204
U 1 1 564B4065
P 5700 4750
F 0 "R204" V 5800 4750 50  0000 C CNN
F 1 "R" V 5700 4750 50  0000 C CNN
F 2 "" H 5700 4750 60  0000 C CNN
F 3 "" H 5700 4750 60  0000 C CNN
	1    5700 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5500 3900 5500 3850
Wire Wire Line
	5700 4500 5700 4400
Wire Wire Line
	5700 4450 5100 4450
Wire Wire Line
	5100 4450 5100 4150
Wire Wire Line
	5100 4150 4700 4150
Connection ~ 5700 4450
$Comp
L GND #PWR0111
U 1 1 564B4130
P 5700 5050
F 0 "#PWR0111" H 5700 5050 30  0001 C CNN
F 1 "GND" H 5700 4980 30  0001 C CNN
F 2 "" H 5700 5050 60  0000 C CNN
F 3 "" H 5700 5050 60  0000 C CNN
	1    5700 5050
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 5050 5700 5000
$Comp
L CONN_1 TP202
U 1 1 564B4516
P 6400 4100
F 0 "TP202" H 6480 4100 40  0000 L CNN
F 1 "CONN_1" H 6400 4155 30  0001 C CNN
F 2 "" H 6400 4100 60  0000 C CNN
F 3 "" H 6400 4100 60  0000 C CNN
	1    6400 4100
	0    1    1    0   
$EndComp
Wire Wire Line
	6400 3950 6400 3850
Connection ~ 6400 3850
$Comp
L C C214
U 1 1 564B4925
P 7150 4100
F 0 "C214" H 7200 4200 50  0000 L CNN
F 1 "100nF" H 7200 4000 50  0000 L CNN
F 2 "" H 7150 4100 60  0000 C CNN
F 3 "" H 7150 4100 60  0000 C CNN
F 4 "Kemet" H 7150 4100 60  0001 C CNN "Manufacturer"
F 5 "C0603C104K4RACTU" H 7150 4100 60  0001 C CNN "Manuf Ref"
	1    7150 4100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR0112
U 1 1 564B4940
P 6750 4400
F 0 "#PWR0112" H 6750 4400 30  0001 C CNN
F 1 "GND" H 6750 4330 30  0001 C CNN
F 2 "" H 6750 4400 60  0000 C CNN
F 3 "" H 6750 4400 60  0000 C CNN
	1    6750 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6850 4350 6850 4300
$Comp
L +1.2V #PWR0113
U 1 1 564B734E
P 6750 3750
F 0 "#PWR0113" H 6750 3710 30  0001 C CNN
F 1 "+1.2V" H 6750 3860 30  0000 C CNN
F 2 "" H 6750 3750 60  0000 C CNN
F 3 "" H 6750 3750 60  0000 C CNN
	1    6750 3750
	1    0    0    -1  
$EndComp
$Comp
L C C217
U 1 1 564B8ED4
P 7450 1900
F 0 "C217" H 7500 2000 50  0000 L CNN
F 1 "100nF" H 7500 1800 50  0000 L CNN
F 2 "" H 7450 1900 60  0000 C CNN
F 3 "" H 7450 1900 60  0000 C CNN
	1    7450 1900
	1    0    0    -1  
$EndComp
$Comp
L C C218
U 1 1 564B8EE3
P 7750 1900
F 0 "C218" H 7800 2000 50  0000 L CNN
F 1 "100nF" H 7800 1800 50  0000 L CNN
F 2 "" H 7750 1900 60  0000 C CNN
F 3 "" H 7750 1900 60  0000 C CNN
	1    7750 1900
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR0114
U 1 1 564B8EF2
P 7350 1600
F 0 "#PWR0114" H 7350 1560 30  0001 C CNN
F 1 "+3.3V" H 7350 1710 30  0000 C CNN
F 2 "" H 7350 1600 60  0000 C CNN
F 3 "" H 7350 1600 60  0000 C CNN
	1    7350 1600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR0115
U 1 1 564B8F01
P 7350 2200
F 0 "#PWR0115" H 7350 2200 30  0001 C CNN
F 1 "GND" H 7350 2130 30  0001 C CNN
F 2 "" H 7350 2200 60  0000 C CNN
F 3 "" H 7350 2200 60  0000 C CNN
	1    7350 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	7350 2200 7350 2150
Wire Wire Line
	7750 2150 7750 2100
Wire Wire Line
	7450 2100 7450 2150
Connection ~ 7450 2150
Wire Wire Line
	7350 1600 7350 1650
Wire Wire Line
	7450 1650 7450 1700
Wire Wire Line
	7750 1650 7750 1700
Connection ~ 7450 1650
$Comp
L C C215
U 1 1 564BABBE
P 7450 4100
F 0 "C215" H 7500 4200 50  0000 L CNN
F 1 "100nF" H 7500 4000 50  0000 L CNN
F 2 "" H 7450 4100 60  0000 C CNN
F 3 "" H 7450 4100 60  0000 C CNN
F 4 "Kemet" H 7450 4100 60  0001 C CNN "Manufacturer"
F 5 "C0603C104K4RACTU" H 7450 4100 60  0001 C CNN "Manuf Ref"
	1    7450 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6750 3750 6750 3850
Wire Wire Line
	6750 3850 7750 3850
Wire Wire Line
	7150 3850 7150 3900
Wire Wire Line
	6850 3900 6850 3850
Connection ~ 6850 3850
Wire Wire Line
	6750 4400 6750 4350
Wire Wire Line
	6750 4350 7750 4350
Wire Wire Line
	7150 4350 7150 4300
Connection ~ 6850 4350
$Comp
L C C216
U 1 1 564BAD39
P 7750 4100
F 0 "C216" H 7800 4200 50  0000 L CNN
F 1 "100nF" H 7800 4000 50  0000 L CNN
F 2 "" H 7750 4100 60  0000 C CNN
F 3 "" H 7750 4100 60  0000 C CNN
F 4 "Kemet" H 7750 4100 60  0001 C CNN "Manufacturer"
F 5 "C0603C104K4RACTU" H 7750 4100 60  0001 C CNN "Manuf Ref"
	1    7750 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	7450 3850 7450 3900
Connection ~ 7150 3850
Wire Wire Line
	7450 4350 7450 4300
Connection ~ 7150 4350
$Comp
L CP C219
U 1 1 564BCF48
P 6850 4100
F 0 "C219" H 6900 4200 50  0000 L CNN
F 1 "2,2uF" H 6900 4000 50  0000 L CNN
F 2 "" H 6850 4100 60  0000 C CNN
F 3 "" H 6850 4100 60  0000 C CNN
	1    6850 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	7750 3850 7750 3900
Connection ~ 7450 3850
Wire Wire Line
	7750 4350 7750 4300
Connection ~ 7450 4350
$Comp
L C C220
U 1 1 564BD3FB
P 8050 1900
F 0 "C220" H 8100 2000 50  0000 L CNN
F 1 "1uF" H 8100 1800 50  0000 L CNN
F 2 "" H 8050 1900 60  0000 C CNN
F 3 "" H 8050 1900 60  0000 C CNN
	1    8050 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	8050 1650 8050 1700
Connection ~ 7750 1650
Wire Wire Line
	8050 2150 8050 2100
Connection ~ 7750 2150
$Comp
L +VIN #PWR0116
U 1 1 564C50D4
P 2500 1550
F 0 "#PWR0116" H 2500 1510 30  0001 C CNN
F 1 "+VIN" H 2500 1660 30  0000 C CNN
F 2 "" H 2500 1550 60  0000 C CNN
F 3 "" H 2500 1550 60  0000 C CNN
	1    2500 1550
	1    0    0    -1  
$EndComp
$Comp
L +VIN #PWR0117
U 1 1 568FE9AE
P 2600 3750
F 0 "#PWR0117" H 2600 3710 30  0001 C CNN
F 1 "+VIN" H 2600 3860 30  0000 C CNN
F 2 "" H 2600 3750 60  0000 C CNN
F 3 "" H 2600 3750 60  0000 C CNN
	1    2600 3750
	1    0    0    -1  
$EndComp
$Comp
L C C205
U 1 1 568FED78
P 2800 4150
F 0 "C205" H 2550 4250 50  0000 L CNN
F 1 "22uF" H 2850 4050 50  0000 L CNN
F 2 "" H 2800 4150 60  0000 C CNN
F 3 "" H 2800 4150 60  0000 C CNN
F 4 "Murata" H 2800 4150 60  0001 C CNN "Manufacturer"
F 5 "GRM21BR61E226ME44L" H 2800 4150 60  0001 C CNN "Manuf Ref"
	1    2800 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 3850 2800 3950
Wire Wire Line
	2600 3850 2600 3750
Connection ~ 2800 3850
Wire Wire Line
	2800 4350 2800 4450
$Comp
L GND #PWR0118
U 1 1 568FEF21
P 2800 4450
F 0 "#PWR0118" H 2800 4450 30  0001 C CNN
F 1 "GND" H 2800 4380 30  0001 C CNN
F 2 "" H 2800 4450 60  0000 C CNN
F 3 "" H 2800 4450 60  0000 C CNN
	1    2800 4450
	1    0    0    -1  
$EndComp
$Comp
L C C221
U 1 1 56957418
P 8350 1900
F 0 "C221" H 8400 2000 50  0000 L CNN
F 1 "100nF" H 8400 1800 50  0000 L CNN
F 2 "" H 8350 1900 60  0000 C CNN
F 3 "" H 8350 1900 60  0000 C CNN
	1    8350 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	8350 2150 8350 2100
Connection ~ 8050 2150
Wire Wire Line
	8350 1650 8350 1700
Connection ~ 8050 1650
$Comp
L CONN_1 TP201
U 1 1 56957884
P 6700 1650
F 0 "TP201" H 6780 1650 40  0000 L CNN
F 1 "CONN_1" H 6700 1705 30  0001 C CNN
F 2 "" H 6700 1650 60  0000 C CNN
F 3 "" H 6700 1650 60  0000 C CNN
	1    6700 1650
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR0119
U 1 1 56957893
P 6450 1550
F 0 "#PWR0119" H 6450 1510 30  0001 C CNN
F 1 "+3.3V" H 6450 1660 30  0000 C CNN
F 2 "" H 6450 1550 60  0000 C CNN
F 3 "" H 6450 1550 60  0000 C CNN
	1    6450 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6450 1550 6450 1650
Wire Wire Line
	6450 1650 6550 1650
$Comp
L C C222
U 1 1 56957CA0
P 8650 1900
F 0 "C222" H 8700 2000 50  0000 L CNN
F 1 "1uF" H 8700 1800 50  0000 L CNN
F 2 "" H 8650 1900 60  0000 C CNN
F 3 "" H 8650 1900 60  0000 C CNN
	1    8650 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	8650 1650 8650 1700
Connection ~ 8350 1650
Wire Wire Line
	8650 2150 8650 2100
Connection ~ 8350 2150
$Comp
L C C223
U 1 1 56958118
P 8950 1900
F 0 "C223" H 9000 2000 50  0000 L CNN
F 1 "1uF" H 9000 1800 50  0000 L CNN
F 2 "" H 8950 1900 60  0000 C CNN
F 3 "" H 8950 1900 60  0000 C CNN
	1    8950 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 1650 8950 1700
Connection ~ 8650 1650
Wire Wire Line
	8950 2150 8950 2100
Connection ~ 8650 2150
$Comp
L C C224
U 1 1 5697033B
P 7750 2800
F 0 "C224" H 7800 2900 50  0000 L CNN
F 1 "100nF" H 7800 2700 50  0000 L CNN
F 2 "" H 7750 2800 60  0000 C CNN
F 3 "" H 7750 2800 60  0000 C CNN
F 4 "Kemet" H 7750 2800 60  0001 C CNN "Manufacturer"
F 5 "C0603C104K4RACTU" H 7750 2800 60  0001 C CNN "Manuf Ref"
	1    7750 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	7450 2550 7450 2600
$Comp
L C C225
U 1 1 5697DD05
P 8050 2800
F 0 "C225" H 8100 2900 50  0000 L CNN
F 1 "100nF" H 8100 2700 50  0000 L CNN
F 2 "" H 8050 2800 60  0000 C CNN
F 3 "" H 8050 2800 60  0000 C CNN
	1    8050 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	7750 2550 7750 2600
$Comp
L C C226
U 1 1 5697E1BD
P 8350 2800
F 0 "C226" H 8400 2900 50  0000 L CNN
F 1 "100nF" H 8400 2700 50  0000 L CNN
F 2 "" H 8350 2800 60  0000 C CNN
F 3 "" H 8350 2800 60  0000 C CNN
	1    8350 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	8050 2550 8050 2600
$Comp
L C C227
U 1 1 56981F98
P 7450 2800
F 0 "C227" H 7500 2900 50  0000 L CNN
F 1 "10uF" H 7500 2700 50  0000 L CNN
F 2 "" H 7450 2800 60  0000 C CNN
F 3 "" H 7450 2800 60  0000 C CNN
	1    7450 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	8350 2550 8350 2600
$Comp
L C C228
U 1 1 569834AB
P 8650 2800
F 0 "C228" H 8700 2900 50  0000 L CNN
F 1 "100nF" H 8700 2700 50  0000 L CNN
F 2 "" H 8650 2800 60  0000 C CNN
F 3 "" H 8650 2800 60  0000 C CNN
	1    8650 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	8650 2550 8650 2600
Wire Wire Line
	7350 2550 8650 2550
Connection ~ 7750 2550
Connection ~ 8050 2550
Connection ~ 8350 2550
Wire Wire Line
	7450 3050 7450 3000
Wire Wire Line
	7350 3050 8650 3050
Wire Wire Line
	7750 3050 7750 3000
Wire Wire Line
	8050 3050 8050 3000
Connection ~ 7750 3050
Wire Wire Line
	8350 3050 8350 3000
Connection ~ 8050 3050
Wire Wire Line
	8650 3050 8650 3000
Connection ~ 8350 3050
$Comp
L GND #PWR0120
U 1 1 569841D5
P 7350 3100
F 0 "#PWR0120" H 7350 3100 30  0001 C CNN
F 1 "GND" H 7350 3030 30  0001 C CNN
F 2 "" H 7350 3100 60  0000 C CNN
F 3 "" H 7350 3100 60  0000 C CNN
	1    7350 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	7350 3100 7350 3050
Connection ~ 7450 3050
Wire Wire Line
	7350 2500 7350 2550
Connection ~ 7450 2550
$Comp
L +3.3V #PWR0121
U 1 1 56984310
P 7350 2500
F 0 "#PWR0121" H 7350 2460 30  0001 C CNN
F 1 "+3.3V" H 7350 2610 30  0000 C CNN
F 2 "" H 7350 2500 60  0000 C CNN
F 3 "" H 7350 2500 60  0000 C CNN
	1    7350 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	7350 2150 8950 2150
Wire Wire Line
	7350 1650 8950 1650
$Comp
L CONN_FIX P200
U 1 1 577249D3
P 1550 4050
F 0 "P200" H 1550 4300 40  0000 C CNN
F 1 "CONN_FIX" V 1550 4050 40  0000 C CNN
F 2 "" H 1550 4050 60  0000 C CNN
F 3 "" H 1550 4050 60  0000 C CNN
	1    1550 4050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR0122
U 1 1 577249E2
P 1900 4300
F 0 "#PWR0122" H 1900 4300 30  0001 C CNN
F 1 "GND" H 1900 4230 30  0001 C CNN
F 2 "" H 1900 4300 60  0000 C CNN
F 3 "" H 1900 4300 60  0000 C CNN
	1    1900 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 3900 1900 4300
Wire Wire Line
	1900 4200 1800 4200
Wire Wire Line
	1900 4100 1800 4100
Connection ~ 1900 4200
Wire Wire Line
	1900 4000 1800 4000
Connection ~ 1900 4100
Wire Wire Line
	1900 3900 1800 3900
Connection ~ 1900 4000
$Comp
L CONN_1 TP203
U 1 1 5772507F
P 1550 4600
F 0 "TP203" H 1630 4600 40  0000 L CNN
F 1 "CONN_1" H 1550 4655 30  0001 C CNN
F 2 "" H 1550 4600 60  0000 C CNN
F 3 "" H 1550 4600 60  0000 C CNN
	1    1550 4600
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR0123
U 1 1 5772508E
P 1800 5100
F 0 "#PWR0123" H 1800 5100 30  0001 C CNN
F 1 "GND" H 1800 5030 30  0001 C CNN
F 2 "" H 1800 5100 60  0000 C CNN
F 3 "" H 1800 5100 60  0000 C CNN
	1    1800 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1800 4600 1800 5100
Wire Wire Line
	1800 4600 1700 4600
$Comp
L CONN_1 TP204
U 1 1 57726E91
P 1550 4750
F 0 "TP204" H 1630 4750 40  0000 L CNN
F 1 "CONN_1" H 1550 4805 30  0001 C CNN
F 2 "" H 1550 4750 60  0000 C CNN
F 3 "" H 1550 4750 60  0000 C CNN
	1    1550 4750
	-1   0    0    1   
$EndComp
Wire Wire Line
	1800 4750 1700 4750
Connection ~ 1800 4750
$Comp
L CONN_1 TP205
U 1 1 57727BA6
P 1550 4900
F 0 "TP205" H 1630 4900 40  0000 L CNN
F 1 "CONN_1" H 1550 4955 30  0001 C CNN
F 2 "" H 1550 4900 60  0000 C CNN
F 3 "" H 1550 4900 60  0000 C CNN
	1    1550 4900
	-1   0    0    1   
$EndComp
Wire Wire Line
	1700 4900 1800 4900
Connection ~ 1800 4900
$Comp
L R R205
U 1 1 5772A35F
P 3400 1950
F 0 "R205" V 3500 1950 50  0000 C CNN
F 1 "15k" V 3400 1950 45  0000 C CNN
F 2 "" H 3400 1950 60  0000 C CNN
F 3 "" H 3400 1950 60  0000 C CNN
F 4 "Vishay" V 3400 1950 60  0001 C CNN "Manufacturer"
F 5 "CRCW060315K0FKEA" V 3400 1950 60  0001 C CNN "Manuf Ref"
	1    3400 1950
	-1   0    0    1   
$EndComp
$Comp
L R R206
U 1 1 5772A37D
P 3400 2550
F 0 "R206" V 3500 2550 50  0000 C CNN
F 1 "7,5k" V 3400 2550 45  0000 C CNN
F 2 "" H 3400 2550 60  0000 C CNN
F 3 "" H 3400 2550 60  0000 C CNN
F 4 "Vishay" V 3400 2550 60  0001 C CNN "Manufacturer"
F 5 "CRCW06037K50FKEA" V 3400 2550 60  0001 C CNN "Manuf Ref"
	1    3400 2550
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR0124
U 1 1 5772A3A5
P 3400 2850
F 0 "#PWR0124" H 3400 2850 30  0001 C CNN
F 1 "GND" H 3400 2780 30  0001 C CNN
F 2 "" H 3400 2850 60  0000 C CNN
F 3 "" H 3400 2850 60  0000 C CNN
	1    3400 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 2850 3400 2800
Wire Wire Line
	3400 2300 3400 2200
Wire Wire Line
	3400 1700 3400 1650
Wire Wire Line
	3350 2250 3550 2250
Wire Wire Line
	3550 2250 3550 1950
Wire Wire Line
	3550 1950 3600 1950
Connection ~ 3400 2250
$Comp
L R R207
U 1 1 5772AE05
P 3300 4350
F 0 "R207" V 3400 4350 50  0000 C CNN
F 1 "1k" V 3300 4350 50  0000 C CNN
F 2 "" H 3300 4350 60  0000 C CNN
F 3 "" H 3300 4350 60  0000 C CNN
	1    3300 4350
	-1   0    0    1   
$EndComp
$Comp
L +3.3V #PWR0125
U 1 1 5772B17F
P 3300 4050
F 0 "#PWR0125" H 3300 4010 30  0001 C CNN
F 1 "+3.3V" H 3300 4160 30  0000 C CNN
F 2 "" H 3300 4050 60  0000 C CNN
F 3 "" H 3300 4050 60  0000 C CNN
	1    3300 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3300 4100 3300 4050
Wire Wire Line
	3500 4150 3500 4650
$Comp
L R R208
U 1 1 5772B36C
P 3300 4950
F 0 "R208" V 3400 4950 50  0000 C CNN
F 1 "R" V 3300 4950 50  0000 C CNN
F 2 "" H 3300 4950 60  0000 C CNN
F 3 "" H 3300 4950 60  0000 C CNN
	1    3300 4950
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR0126
U 1 1 5772B39E
P 3300 5250
F 0 "#PWR0126" H 3300 5250 30  0001 C CNN
F 1 "GND" H 3300 5180 30  0001 C CNN
F 2 "" H 3300 5250 60  0000 C CNN
F 3 "" H 3300 5250 60  0000 C CNN
	1    3300 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3300 5250 3300 5200
Wire Wire Line
	3300 4600 3300 4700
Wire Wire Line
	3500 4650 3300 4650
Connection ~ 3300 4650
$Comp
L CONN_1 TP207
U 1 1 5773B6DB
P 3200 2250
F 0 "TP207" H 3280 2250 40  0000 L CNN
F 1 "CONN_1" H 3200 2305 30  0001 C CNN
F 2 "" H 3200 2250 60  0000 C CNN
F 3 "" H 3200 2250 60  0000 C CNN
	1    3200 2250
	-1   0    0    1   
$EndComp
$Comp
L CONN_1 TP206
U 1 1 5773BB6D
P 1550 5050
F 0 "TP206" H 1630 5050 40  0000 L CNN
F 1 "CONN_1" H 1550 5105 30  0001 C CNN
F 2 "" H 1550 5050 60  0000 C CNN
F 3 "" H 1550 5050 60  0000 C CNN
	1    1550 5050
	-1   0    0    1   
$EndComp
Wire Wire Line
	1800 5050 1700 5050
Connection ~ 1800 5050
$EndSCHEMATC
