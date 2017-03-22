>>> import serial
>>> ser = serial.Serial('/dev/ttyUSB0', 19200, stopbits = 2)  # open serial port
>>> print(ser.name)         # check which port was really used
>>> ser.write(b'Ampl -13.0DB')     # write a string
>>> ser.close()             # close port