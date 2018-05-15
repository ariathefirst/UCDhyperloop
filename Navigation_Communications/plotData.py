import serial
import csv
import re
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time
import pandas as pd
import numpy as np
from io import StringIO
import re

 
portPath = "/dev/tty.usbmodem1411"       # Must match value shown on Arduino IDE
baud = 9600                     # Must match Arduino baud rate
timeout = 5                       # Seconds
filename = "data.csv"
max_num_readings = 10
num_signals = 1


# fig = plt.figure()
# ax1 = fig.add_subplot(1,1,1)

# def animate(i):
#     pullData = open("filename","r").read()
#     dataArray = pullData.split('\n')
#     xar = []
#     yar = []
#     for eachLine in dataArray:
#         if len(eachLine)>1:
#             x,y = eachLine.split(',')
#             xar.append(int(x))
#             yar.append(int(y))
#     ax1.clear()
#     ax1.plot(xar,yar)
# ani = animation.FuncAnimation(fig, animate, interval=1000)
# plt.show() 
 
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
    clean_data = []
    
    for line in data:
        print('line is \n',line)
        print("length of line is \n", len(line))
        line_data = re.findall(r"[-+]?\d*\.\d+|\d+", line.decode('ascii')) # finds all floating point nums and digits
        print("length of line_data is \n", len(line_data))
        line_data = list(map(float, line_data)) # Convert strings to float
        print('line_data is %s\n' % (line_data))


        if len(line_data) > 0:
            clean_data.append(line_data)
    print(clean_data)
    return clean_data           
 
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
 
while True:
    print ("Reading serial data...")
    serial_data = read_serial_data(serial_obj)
    # print (len(serial_data))
     
    print ("Cleaning data...")
    clean_data =  clean_serial_data(serial_data)
         
    print ("Saving to csv...")
    np.savetxt('arduinoData.csv', clean_data, delimiter= ",", fmt='%s') # .csv is a dataframe

    with open('arduinoData.csv', 'rb') as d:
        data = np.load(d)
    heightCol = data['Z']
    print(heightCol)

    # df = pd.read_csv('filename')
    # df.to_csv('filename.csv')


    print ("Plotting data...")

