import time
#import board
#import busio
import adafruit_gps
#from pyb import UART 
from machine import UART
from micropyGPS import MicropyGPS
import utime
import SensorData

print("starting gps code")

my_gps = MicropyGPS(-4)
serializer = SensorData.SensorDataSerializer()


#RX = 12
#TX = 27
RX = 16
TX = 17

#uart = UART(TX, RX, 9600, 3000)
#uart = UART(1, 9600, baudrate=9600, rx=RX, tx=TX, timeout=10)                         # init with given baudrate   
#uart = UART(1, 9600, pins=(RX,TX))
#uart.init(9600, bits=8, parity=None, stop=1) 
uart = UART(2, 9600)

gps = adafruit_gps.GPS(uart)

gps.send_command('PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0')

gps.send_command('PMTK220,1000')

last_print = time.time()
while True:
    #current = time.time()
    #if current - last_print >= 10.0:
    #last_print = current
    #print(uart.readline())
    #sentence = uart.readline()
    #print(sentence)
    
    # for x in sentence:
    #     my_gps.update(x)
    
    # if uart.any():
    #     for x in uart.readline():
    #         my_gps.update(x)

    #     if not my_gps.satellite_data_updated():
    #         print('Waiting for satellite data updated...')
    #         continue
        
    #     print('Fix: {}'.format(my_gps.fix_type))
    
    # if uart.any():
    #     print(uart.readline(), end='')
    
    
    len = uart.any()
    if len>0:
        b = uart.read(len)
        for x in b:
            if 10 <= x <= 126:
                stat = my_gps.update(chr(x))
                if stat:
                    #print(stat)
                    #if not my_gps.satellite_data_updated():
                       # print('Waiting for satellite data updated...')
                        #continue
                    
                    data = SensorData.SensorData()
                    data.latitude = my_gps.latitude_string()
                    data.longitude = my_gps.longitude_string()
                    data.speed = my_gps.speed[1]
                    data.altitude = my_gps.altitude
                    data.satellites = my_gps.satellites_in_use

                    data.hours = my_gps.timestamp[0]
                    data.minutes = my_gps.timestamp[1]
                    data.seconds = my_gps.timestamp[2]

                    data.day = my_gps.date[0]
                    data.month = my_gps.date[1]
                    data.year = my_gps.date[2]


                    print(serializer.serialize(data))
                    print(my_gps.latitude_string())
                    print(my_gps.longitude_string())
                    print(my_gps.satellites_in_use)
                    print('Fix {}'.format(my_gps.fix_type))
                    print('hdop {}'.format(my_gps.hdop))
                    print('vdop {}'.format(my_gps.vdop))
                    print('pdop {}'.format(my_gps.pdop))
                    print(my_gps.timestamp)
                    utime.sleep_ms(5000)
    else:
        utime.sleep_ms(100)

    
    # Make sure to call gps.update() every loop iteration and at least twice
    # as fast as data comes from the GPS unit (usually every second).
    # This returns a bool that's true if it parsed new data (you can ignore it
    # though if you don't care and instead look at the has_fix property).
    # gps.update()
    # # Every second print out current location details if there's a fix.
    # current = time.time()
    # if current - last_print >= 1.0:
    #     last_print = current
    #     if not gps.has_fix:
    #         # Try again if we don't have a fix yet.
    #         print('Waiting for fix...')
    #         continue
    #     # We have a fix! (gps.has_fix is true)
    #     # Print out details about the fix like location, date, etc.
    #     print('=' * 40)  # Print a separator line.
    #     print('Fix timestamp: {}/{}/{} {:02}:{:02}:{:02}'.format(
    #         gps.timestamp_utc.tm_mon,   # Grab parts of the time from the
    #         gps.timestamp_utc.tm_mday,  # struct_time object that holds
    #         gps.timestamp_utc.tm_year,  # the fix time.  Note you might
    #         gps.timestamp_utc.tm_hour,  # not get all data like year, day,
    #         gps.timestamp_utc.tm_min,   # month!
    #         gps.timestamp_utc.tm_sec))
    #     print('Latitude: {} degrees'.format(gps.latitude))
    #     print('Longitude: {} degrees'.format(gps.longitude))
    #     print('Fix quality: {}'.format(gps.fix_quality))
    #     # Some attributes beyond latitude, longitude and timestamp are optional
    #     # and might not be present.  Check if they're None before trying to use!
    #     if gps.satellites is not None:
    #         print('# satellites: {}'.format(gps.satellites))
    #     if gps.altitude_m is not None:
    #         print('Altitude: {} meters'.format(gps.altitude_m))
    #     if gps.track_angle_deg is not None:
    #         print('Speed: {} knots'.format(gps.speed_knots))
    #     if gps.track_angle_deg is not None:
    #         print('Track angle: {} degrees'.format(gps.track_angle_deg))
    #     if gps.horizontal_dilution is not None:
    #         print('Horizontal dilution: {}'.format(gps.horizontal_dilution))
    #     if gps.height_geoid is not None:
    #         print('Height geo ID: {} meters'.format(gps.height_geoid))
