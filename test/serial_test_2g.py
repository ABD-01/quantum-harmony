import serial
import threading
import time

# Initialize serial connection
serial_port = 'COM24'
baud_rate = 115200

ser = serial.Serial(serial_port, baud_rate, timeout=1)

# Response mappings
response_map = {
    "CMN *GET#DEVNWSW#": "<STATUS#DEVNWSW#1#>",
    "CMN *GET#SIMTYP#": "<STATUS#SIMTYP#0#>",
    "CMN *GET#CIP2#": "<STATUS#CIP2#ais-data.accoladeelectronics.com#5555#>",
    "CMN *GET#UIN#": "<STATUS#UIN#ACON4IA107230000001#>",
    "CMN *GET#CHNO#": "<STATUS#CHNO#ACCDEV07240045162#>",
    "CMN *GET#CERT#": "<STATUS#CERT#2048,1984,3294#>\n<STATUS#CERT#2050,1984,3294#>",
    "CMN *SET#CRST#1#": "<STATUS#CRST#1#OK#>",
    "CMN *SET*SACK#1#": "<STATUS#SACK#1#OK#>",
    "CMN *SET#DEVNWSW#1#": "<STATUS#DEVNWSW#1#OK#>",
    "CMN *SET#SIMTYP#0#": "<STATUS#SIMTYP#0#OK#>",
    "CMN *SET#CIP2#ais-data.accoladeelectronics.com#5555#": "<STATUS#CIP2#ais-data.accoladeelectronics.com#5555#OK#>",
    "CMN *SET#UIN#ACON4IA107230000001#": "<STATUS#UIN#ACON4IA107230000001#OK#>",
    "CMN *SET#CHNO#ACCDEV07240045162#": "<STATUS#CHNO#ACCDEV07240045162#OK#>"
}

# Boot signature response for SET commands
boot_signature = "BOOT SIGNATURE"

# Function to read from the file and send commands every 100ms
def send_commands(file_path):
    while True:
        with open(file_path, 'r') as f:
            for line in f:
                ser.write((line+'\r\n').encode())
                time.sleep(0.1)  # Wait for 100ms

# Function to handle received data
def handle_received_data():
    while True:
        if ser.in_waiting > 0:
            received_line = ser.readline().decode().strip()
            if received_line:
                print(f"Received: {received_line}")
                response = response_map.get(received_line)
                if response:
                    ser.write((response + '\n').encode())
                    print(f"Sent: {response}")
                    if "CMN *SET#" in received_line:
                        time.sleep(0.5)
                        ser.write((boot_signature + '\n').encode())
                        print(f"Sent: {boot_signature}")

# Start thread for sending commands
file_path = 'D:/LOGS/USB_TCP_FOTA_testing_13June_12PM.log'  # Replace with your actual file path
sender_thread = threading.Thread(target=send_commands,
                                 args=(file_path,), daemon=True)
sender_thread.start()

# Start receiving data
handle_received_data()
