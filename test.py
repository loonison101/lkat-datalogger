import SensorData


# Read our data
data = SensorData.SensorData()
data.latitude = "32"
data.longitude = "7"
data.speed = 4

# Serialize our data
serializer = SensorData.SensorDataSerializer()
s = serializer.serialize(data)

# Persist our data
# Eventually flush the data just to be sure
print(s)