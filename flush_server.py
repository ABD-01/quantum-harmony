import socket
import threading
import sys
import queue
import logging
from rich.logging import RichHandler

LOG_LEVEL = logging.NOTSET
file_handler = logging.FileHandler('server.log')
file_handler.setLevel(LOG_LEVEL)
file_handler.setFormatter(logging.Formatter('[%(asctime)s] [%(levelname)-8s] : "%(message)s"'))

stream_handler = RichHandler()

# logging.basicConfig(format='[%(asctime)s] [%(levelname)-8s] : "%(message)s"', level=LOG_LEVEL, handlers=[file_handler, stream_handler])
logging.basicConfig(format="%(message)s", level=LOG_LEVEL, handlers=[file_handler, stream_handler])

CLIENTS = {}

PORTNO = 5000
MAX_BUF_SIZE = 1024
BUFFER = queue.Queue()

# Event object to signal threads to exit
exit_event = threading.Event()

def receive_data(sock, id):
    logging.info("Thread started for client: %d", id)
    try:
        while not exit_event.is_set():
            data = sock.recv(MAX_BUF_SIZE)
            if data == b'':
                logging.info("Client: %d disconnected", id)
                break
            logging.debug("Client: %d rx <- %s", id, data.decode(errors='ignore').strip())
            sys.stdout.flush()  # Flush the standard output to show the received text immediately
    except socket.timeout:
        logging.warning("Client: %d timed out", id)
    except Exception as e:
        logging.error("Client: %d Error On Receiving", id, exc_info=True)
    finally:
        if id in CLIENTS:
            del CLIENTS[id]
        sock.close()
    logging.info("Thread stopped for client: %d", id)

def server_broadcast(msg):
    for k,v in CLIENTS.items():
        try:
            v.send(msg.encode())
            logging.debug("Client: %d tx -> %s", k, msg)
        except Exception as e:
            logging.error("Client: %d Error On Sending", k, exc_info=True)

def server_send():
    logging.info("Thread started for server")
    while not exit_event.is_set():
        if not BUFFER.empty():
            data = BUFFER.get()
            if ":" not in data:
                logging.info("Broadcasing Message of length %d to %d clients", len(data), len(CLIENTS))
                server_broadcast(data)
                continue
            id, msg = map(str.strip, data.split(':', 1))
            if not id.isdigit():
                logging.warning("Invalid client id")
                continue
            id = int(id)
            sock = CLIENTS.get(id, None)
            if sock is None:
                logging.warning("Client with id %d not connected", id)
                continue
            try:
                sock.send(msg.encode())
                logging.debug("Client: %d tx -> %s", id, msg)
            except Exception as e:
                logging.error("Client: %d Error On Sending", id, exc_info=True)
                break
    logging.info("Thread stopped for server")


def keyboard_input():
    logging.info("Thread started for keyboard input")
    while not exit_event.is_set():
        user_input = input()
        if user_input == "$GET":
            logging.info("%d clients connected", len(CLIENTS))
            for k,v in CLIENTS.items():
                logging.info("Client: %d connected with address %s", k, v.getpeername())
            continue
        if user_input == "$EXIT":
            exit_server()
            break
        BUFFER.put(user_input)
        logging.info("Added {} bytes to the queue".format(len(user_input)))
    logging.info("Thread stopped for keyboard input")


def exit_server():
    exit_event.set()
    logging.info("Exiting server")
    for client_socket in CLIENTS.values():
        client_socket.close()
    server_socket.close()

def main():
    global server_socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind(('', PORTNO))
    server_socket.listen()

    logging.info("Server is listening on port %d", PORTNO)

    client_id = 1

    input_thread = threading.Thread(target=keyboard_input)
    input_thread.start()

    send_thread = threading.Thread(target=server_send, daemon=True)
    send_thread.start()

    try:
        while not exit_event.is_set():

            client_socket, client_address = server_socket.accept()
            client_socket.settimeout(900)

            # Start the thread to receive data
            receive_thread = threading.Thread(target=receive_data, args=(client_socket, client_id))
            receive_thread.start()

            CLIENTS[client_id] = client_socket
            logging.info("Client: %d connected with address %s", client_id, client_address)
            client_id += 1

    except KeyboardInterrupt:
        exit_server()
    except socket.error:
        logging.error("Server: Socket Closed Abruptly")
    finally:
        input_thread.join()
        send_thread.join()

if __name__ == "__main__":
    main()

