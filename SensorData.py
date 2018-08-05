class SensorData:
    latitude = ""
    longitude = ""
    speed = 0.0
    temperature = 0.0
    altitude = 0
    satellites = 0

# TODO: Can i do a interface in python?
class SensorDataSerializer:
    def serialize(self, sensorData):
        seperator = ","
        fieldOrder = [
            sensorData.latitude,
            sensorData.longitude,
            str(sensorData.speed),
            str(sensorData.temperature),
            str(sensorData.altitude),
            str(sensorData.satellites)
        ]

        return seperator.join(fieldOrder)

class DataWriter:
    def __init__(self):
        # Do some SD card stuff here
    
    def append(self, data):
        # append to ne file?
    
    def flush(self):
        # if you want to be positive the stuff was written (uses more power though)