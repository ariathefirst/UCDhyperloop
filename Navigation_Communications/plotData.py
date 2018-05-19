import serial
import csv
import re
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time
import datetime
import pandas as pd
import numpy as np
from io import StringIO
import re
import pickle

 
portPath = "/dev/tty.usbmodem1411"       # Must match value shown on Arduino IDE
baud = 9600                     # Must match Arduino baud rate
timeout = 5                      # Seconds
filename = "data.csv"
max_num_readings = 17
num_signals = 1


fig = plt.figure()
ax1 = fig.add_subplot(1,1,1)

clean_data = []
xar = []
yar = []

global iar #index array NOTE: put times?
global zar #zval array 
index = 0
zar = []
iar = []

 
def create_serial_obj(portPath, baud_rate, tout):
    """
    Given the port path, baud rate, and timeout value, creates
    and returns a pyserial object.
    """
    return serial.Serial(portPath, baud_rate, timeout = tout)
    
def read_serial_data(serial):
    """
    Given a pyserial object (serial). Outputs a list of lines read in
    from the serial port
    """
    serial.flushInput()
    
    serial_data = []
    readings_left = True
    timeout_reached = False
    
    while readings_left and not timeout_reached: # will continuously read data as long as not timeout_reached
        serial_line = serial.readline()
        if serial_line == '':
            timeout_reached = True
        else:
            serial_data.append(serial_line)
            if len(serial_data) == max_num_readings:
                readings_left = False
        
    return serial_data
 
def is_number(string):
    """
    Given a string returns True if the string represents a number.
    Returns False otherwise.
    """
    try:
        float(string)
        return True
    except ValueError:
        return False
        
def clean_serial_data(data):
    """
    Given a list of serial lines (data). Removes all characters.
    Returns the cleaned list of lists of digits.
    Given something like: ['0.5000,33\r\n', '1.0000,283\r\n']
    Returns: [[0.5,33.0], [1.0,283.0]]
    """
    
    for line in data:
        # print('line is \n',line)
        # print("length of line is \n", len(line))
        line_data = re.findall(r"[-+]?\d*\.\d+|\d+", line.decode('ascii')) # finds all floating point nums and digits
        # print("length of line_data is \n", len(line_data))
        line_data = list(map(float, line_data)) # Convert strings to float
        # print('line_data is %s\n' % (line_data))

        global index

        if len(line_data) > 0:
            clean_data.append(line_data)
            if len(line_data) == 7:
                iar.append(index)
                zar.append(line_data[2]) 
                index += 1      
    # print("clean_data is \n", clean_data)
    # print("clean_data[0] is \n", clean_data[0])

    return clean_data           
 
def get_z_values(clean_data):
    zVals = np.zeros((10,2))
    i = 0
    for row in clean_data:
        if len(row) == 7:
            zVals[i][0] = i + 1 # ith row 0th num is time step
            zVals[i][1] = row[2] # ith row 1st num is zVal
            i += 1
            # print(zVals[i])
    return zVals
    
# def gen_col_list(num_signals):
#     """
#     Given the number of signals returns
#     a list of columns for the data.
#     E.g. 3 signals returns the list: ['Time','Signal1','Signal2','Signal3']
#     """
#     col_list = ['Time']
#     for i in range(1,num_signals+1):
#         col = 'Signal'+str(i)
#         col_list.append(col)
        
#     return col_list
    
# def map_value(x, in_min, in_max, out_min, out_max):
#     return (((x - in_min) * (out_max - out_min))/(in_max - in_min)) + out_min
 

print ("Creating serial object...")
serial_obj = create_serial_obj(portPath, baud, timeout)
 
# while True:
#     print ("Reading serial data...")
#     serial_data = read_serial_data(serial_obj)
#     # print (len(serial_data))
    
#     print ("Cleaning data...")
#     clean_data = clean_serial_data(serial_data)

#     print("getting z value...")
#     zVals = get_z_values(clean_data)
#     print("zVals are: \n", zVals)

  
def animate(i):
    # pullData = open("zVals.txt","r").read()
    serial_data = read_serial_data(serial_obj)
    clean_data = clean_serial_data(serial_data)
    # zVals = get_z_values(clean_data)

    # dataArray = pullData.split('\n')

    # for eachLine in dataArray:
    #     if len(eachLine)>1:
    #         x = eachLine[0]
    #         y = eachLine[1]
    #         xar.append((x))
    #         yar.append((y))
    ax1.clear()
    # ax1.plot(xar, yar)
    ax1.plot(iar, zar)



ani = animation.FuncAnimation(fig, animate, interval=1000)
plt.show()   

