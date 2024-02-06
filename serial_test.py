import serial
import threading
import time

serial_port = 'COM14'  # Change to your serial port
baud_rate = 115200

ser = serial.Serial(serial_port, baud_rate)

def receive_data(serial_port, baud_rate):
    try:
        print(f"Connected to {serial_port} at {baud_rate} baud.")
        
        while True:
            response = ser.readline()
            if response:
                response = response.decode().strip()
                print(f"Received: {response}")

                if response.find("reserved for LAUNCH task stack.") > -1:
                    ser.flushInput()
                    ser.write(b"\r\n")
                    print("tx-> ", repr(b"\r\n"))


    except Exception as e:
        print(f"Error: {str(e)}")

# Create a thread to continuously receive data
receive_thread = threading.Thread(target=receive_data, args=(serial_port, baud_rate))
receive_thread.daemon = True
receive_thread.start()

# Main loop to send data
while True:
    user_input = input("Enter a message: ")
    ser.flushInput()
    ser.write(user_input.encode() + b'\r\n')  # Send user input
    print("TX->", repr(user_input))
