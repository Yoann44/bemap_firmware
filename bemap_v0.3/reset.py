#!/usr/bin/env python
import serial, sys, os, time

serialPort = sys.argv[1]
ser = serial.Serial(
    port=serialPort,
    baudrate=1200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)
ser.isOpen()
ser.close() # always close port

time.sleep(1)

os.system('avrdude C bin/avrdude.conf -v -patmega32u4 -cavr109 -PCOM16 -b57600 -D -Uflash:w:' + sys.argv[2] + '.elf.hex:i ')
