import socket
import time
import logging
import os
from tqdm.auto import tqdm
import threading

from rich.logging import RichHandler
from rich.progress import (
    BarColumn,
    DownloadColumn,
    Progress,
    TextColumn,
    TimeRemainingColumn,
    TransferSpeedColumn,
)
import copy
from rich.prompt import Prompt
from rich.panel import Panel
from rich import print


DISABLE_PROGRESSBAR = True
LOG_LEVEL = logging.NOTSET

PORTNO = 5000
MAX_BUF_SIZE = 1024

file_handler = logging.FileHandler('server.log')
file_handler.setLevel(logging.NOTSET)
file_handler.setFormatter(logging.Formatter('[%(asctime)s] [%(levelname)-8s] : "%(message)s"'))

# stream_handler = logging.StreamHandler()
stream_handler = RichHandler()

# logging.basicConfig(format='[%(asctime)s] [%(levelname)-8s] : "%(message)s"', level=LOG_LEVEL, handlers=[file_handler, stream_handler])
logging.basicConfig(format="%(message)s", level=LOG_LEVEL, handlers=[file_handler, stream_handler])


lock = threading.Lock()
firmwareName = "5.3.8"

def crc16(InputCRC, data, len):
    crc = InputCRC
    for a in range(len):
        crc ^= (data[a] << 8)
        for i  in range(8):
            if ((crc & 0x8000) != 0):
                crc = ((crc << 1) ^ 0x1021)
            else:
                crc = (crc << 1)
    exabyte = hex(crc)
    return crc & 0xffff


"""
uint32_t crc_calc32(uint32_t runningCrc, const uint8_t* const buf, size_t bufSize)
{
    size_t i;
    size_t j;
    unsigned int msb;

    for ( i = 0 ; i < bufSize ; i++ )
    {
        runningCrc ^= ( ( (unsigned int) buf[i] ) << 24 );
        for ( j = 0 ; j < 8 ; j++ )
        {
            msb = runningCrc >> 31;
            runningCrc <<= 1;
            runningCrc ^= (0 - msb) & 0x04C11DB7;
        }
    }
    return runningCrc;
}
"""
def get_file_crc(file_path, buffer_size=4096):

    def crc32_calc(running_crc, data):
        for byte in data:
            running_crc ^= (byte << 24)
            for _ in range(8):
                msb = running_crc >> 31
                running_crc = (running_crc << 1) & 0xFFFFFFFF
                running_crc ^= (0 - msb) & 0x04C11DB7
        return running_crc

    running_crc = 0xFFFFFFFF 

    with open(file_path, "rb") as file:
        while (data := file.read(buffer_size)):
            running_crc = crc32_calc(running_crc, data)
            progressBar.advance(id, buffer_size)

    running_crc = running_crc & 0xFFFFFFFF
    return running_crc


def handle_login(client_socket, client_id):
    data = client_socket.recv(MAX_BUF_SIZE)

    if data == b'':
        logging.info("Client: %d disconnected", client_id)
        return 2

    logging.debug("Client: %d rx <- %s", client_id, data)

    data = data.decode(errors='ignore').strip()
    tokens = data.split(',')

    if '55AA' not in tokens or len(tokens) < 2:
        response = "Unidentified packet".encode()
        client_socket.send(response)
        logging.info("Client: %d tx -> %s", client_id, response)
        return 1

    if len(tokens) < 27:
        response = "Incorrect packet length".encode()
        client_socket.send(response)
        logging.info("Client: %d tx -> %s", client_id, response)
        return 1

    if tokens[0] == '55AA' and tokens[1] == '1':
        logging.info("Client: %d Login Packet Received from device with UIN No: %s", client_id, tokens[6])
        progressBar.update(client_id, description="Client: " + str(client_id),  name=tokens[6])
        progressBar.start_task(client_id)

        initiate_fota = '1'
        file_crc = firmware_crc
        login_response =  ['55AA',1,'2','0', firmwareName ,initiate_fota, '0',file_crc, firmware_size ,'0','1024','FF']
        login_response = ','.join([str(elem) for elem in login_response])
        client_socket.send(login_response.encode())
        logging.debug("Client: %d tx -> %s", client_id, login_response)
        return 0

    return 1

def handle_fota(client_socket, client_id):
    # progressBar = tqdm(total=firmware_size, unit='B', unit_scale=True, unit_divisor=1024, position=0, disable=DISABLE_PROGRESSBAR)
    firmware_file = open(currentFilePath, 'rb')
    try:
        while True:
            data = client_socket.recv(MAX_BUF_SIZE)

            if data == b'':
                logging.info("Client: %d disconnected", client_id)
                return 1

            logging.debug("Client: %d rx <- %s", client_id, data)

            data = data.decode(errors='ignore').strip()
            tokens = data.split(',')
            if tokens[0]=='55AA' and tokens[1]=='2':
                offset = int(tokens[2])
                chunkSize = int(tokens[3])

                if offset > firmware_size:
                    logging.warning("Client: %d Invalid Offset: %d", client_id, offset)
                    return 1

                logging.debug("Client: %d Requested Offset: %d, Chunk Size: %d", client_id,offset, chunkSize)

                ## read file at the ossfet and send the chunk
                firmware_file.seek(offset)
                chunk = firmware_file.read(chunkSize)

                crc = crc16(0, chunk, chunkSize)
                blockCrc = bytes([(crc >> 8) & 0x00FF, crc & 0x00FF])
                # blockCrc2 = crc.to_bytes(2, byteorder='big') # same as above

                response_data = f"55AA,2,{offset},{chunkSize},".encode() + chunk + ",".encode() + blockCrc + ",FF,".encode()

                client_socket.send(response_data)
                logging.debug("Client: %d tx -> %s...%s", client_id, response_data[:16], response_data[-16:])
                logging.debug("Client: %d Remaining Length: %d", client_id, firmware_size - offset - chunkSize)

                # progressBar.n = offset + chunkSize
                # progressBar.last_print_n = offset + chunkSize
                progressBar.update(client_id, completed=offset + chunkSize)

                if offset + chunkSize == firmware_size:
                    logging.info("Client: %d Download Completed", client_id)
                    # return 0 # TODO: Do we disconnect to the client here?
            
            elif tokens[0]=='55AA' and tokens[1]=='1':
                logging.info("Client: %d Login Packet Received again from device with UIN No: %s", client_id, tokens[6])

                initiate_fota = '1'
                file_crc = firmware_crc
                login_response =  ['55AA',1,'2','0', firmwareName, initiate_fota, '0',file_crc, firmware_size ,'0','1024','FF']
                login_response = ','.join([str(elem) for elem in login_response])
                client_socket.send(login_response.encode())
                logging.debug("Client: %d tx -> %s", client_id, login_response)

    except Exception as e:
        logging.error("Client: %d Error Occurred during FOTA", client_id, exc_info=True)


def handle_client(client_socket, id):
    logging.info("Thread started for client: %d", id)
    logged_in = False
    while not logged_in:
        status = handle_login(client_socket, id)
        if status == 0:
            logged_in = True
            logging.info("Client: %d Login Successful", id)
            break
        elif status == 2: # Disconnected
            progressBar.remove_task(id)
            break
        else:               # status = 1 Retry
            logging.info("Client: %d Login Failed. Re-trying", id)
            time.sleep(1)

    if logged_in:
        status = handle_fota(client_socket, id)
        if status == 0:
            logging.info("Client: %d FOTA Successful", id)
        else:
            logging.info("Client: %d FOTA Failed", id)
            progressBar.remove_task(id)

    client_socket.close()
    logging.info("Client: %d disconnected", id)


progressBar = Progress(
    TextColumn("[cyan bold]{task.description}"), "•", 
    TextColumn("[bold blue]{task.fields[name]}"), 
    BarColumn(), "•", 
    DownloadColumn(), "•", 
    TransferSpeedColumn(), "•", 
    TimeRemainingColumn())
progressBar.start()

# currentFilePath = "ATCU_5.2.8_TST03.bin"
currentFilePath = "C:/Users/Muhammed/Downloads/ATCU_ODISHA_1JAN24 3/ATCU.bin"
firmware_size = os.path.getsize(currentFilePath)

id = progressBar.add_task("Calculating CRC", total=firmware_size, name=currentFilePath)
firmware_crc = get_file_crc(currentFilePath)
if firmware_crc == 0:
    with open(currentFilePath, "rb") as f:
        contents = f.read()
        firmware_crc = contents[-4:]
        firmware_crc = int.from_bytes(firmware_crc, byteorder='big') 
progressBar.remove_task(id)

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
# server_socket.settimeout(3600)
server_socket.bind(('', PORTNO))
server_socket.listen(5)

logging.info("Server is listening on port %d", PORTNO)
logging.info("File Name: %s and file CRC: %08X", currentFilePath, firmware_crc) 

# def get_firmwareName():
    # global firmwareName
    # with lock:
        # return copy.deepcopy(firmwareName)

# def temp():
    # global firmwareName
    # while True:
        # prompt_panel = Panel("[cyan bold]Enter firmware name if you want to change:", title="Prompt")
        # print(prompt_panel)
        # fwName = input()
        # if fwName:
            # with lock:
                # firmwareName = fwName
            # logging.info("Firmware name changed to: %s", get_firmwareName())

# t = threading.Thread(target=temp,)
# t.start()

while True:
    try:
        client_socket, client_address = server_socket.accept()
        client_socket.settimeout(900)

        client_id = progressBar.add_task("Client: ", total=firmware_size, start=False, name="") 
        logging.info("Client: %d connected with address %s", client_id, client_address)
        t = threading.Thread(target=handle_client, args=(client_socket, client_id))
        t.start()


    except Exception as e:
        logging.error("Server: Error Occurred", exc_info=True)
        # server_socket.close()
server_socket.close()
