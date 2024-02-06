import paho.mqtt.client as mqtt
import ssl
import sys
import logging
from rich.logging import RichHandler

LOG_LEVEL = logging.NOTSET
stream_handler = RichHandler()

# logging.basicConfig(format='[%(asctime)s] [%(levelname)-8s] : "%(message)s"', level=LOG_LEVEL, handlers=[file_handler, stream_handler])
logging.basicConfig(format="%(message)s", level=LOG_LEVEL, handlers=[stream_handler])


# Callback function for when the client receives a CONNACK response from the server
def on_connect(client, userdata, flags, rc):
    logging.info("Connected with result code %s", str(rc))
    client.subscribe("Accolade_Testing")

# Callback function for when a PUBLISH message is received from the server
def on_message(client, userdata, msg):
    msg_len = len(msg.payload)
    logging.info("Topic: %s Received Message of lenght: %d (%2.2f kB)", msg.topic, msg_len, msg_len/1024)

# Callback function for when the client disconnects from the broker
def on_disconnect(client, userdata, rc):
    logging.info("Disconnected with result code %s", str(rc))


# Callback function for when the client receives an input from the keyboard
def on_keyboard_input(input):
    client.publish("/device/ACCDEV14012078186/MQTTPROTOBUF/Accolade_Test_Device", input)
    logging.info("Published message of lenght %d", len(input))

# Create an MQTT client and set the callback functions
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.on_disconnect = on_disconnect

# Set the TLS/SSL parameters
client.tls_set(ca_certs='ca.pem', certfile='cc.pem', keyfile='ck.pem')

# Connect to the MQTT broker
client.connect("test.mosquitto.org", 8884)

# Start the network loop to process network traffic and maintain the connection to the broker
client.loop_start()

# Read input from the keyboard and publish it to the topic
while True:
    input = sys.stdin.readline()
    if input:
        on_keyboard_input(input.strip())
    else:
        break

# Disconnect from the MQTT broker and stop the network loop
client.disconnect()
client.loop_stop()