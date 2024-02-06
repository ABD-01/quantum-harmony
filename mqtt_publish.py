import paho.mqtt.client as mqtt
from random import randrange, uniform
import time

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

def on_publish(client, userdata, mid):
    print("Publish Callback:", mid)


mqttBrocker = "test.mosquitto.org"
portNo = 8884
client = mqtt.Client("AEPL-193")
client.on_connect = on_connect
client.on_message = on_message
client.tls_set(ca_certs='ca.pem', certfile='cc.pem', keyfile='ck.pem')
client.on_publish = on_publish
client.connect(mqttBrocker, portNo)

while True:
    randNum = uniform(10.0, 100.0)
    client.publish("Accolade_Testing", f"Hello Device from AEPL-193 - {randNum}")
    time.sleep(10)
