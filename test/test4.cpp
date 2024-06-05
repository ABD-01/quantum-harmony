#include <iostream>
#include <string>
#include <thread>

#include "serialib.h"

#define COM_PATH  "\\\\.\\"

std::string getErrorMessage(int returnCode) {
    switch (returnCode) {
        case  1:    return "Success";
        case -1:    return "Device not found";
        case -2:    return "Error while opening the device";
        case -3:    return "Error while getting port parameters";
        case -4:    return "Speed (Bauds) not recognized";
        case -5:    return "Error while writing port parameters";
        case -6:    return "Error while writing timeout parameters";
        case -7:    return "Databits not recognized";
        case -8:    return "Stopbits not recognized";
        case -9:    return "Parity not recognized";
        default:    return "Unknown error code";
    }
}

void readThread(serialib& serial) {
    std::cout << "Thread Started.\n";

    char rc;
    char buf[1024];
    std::fill(buf, buf + sizeof(buf), 0);

    while (1)
    {
        if ((rc = serial.readString(buf, '\n', 1024, 10'000)) < 1) {
            switch (rc)
            {
            case  0:    std::cout << "Timeout\n"; break;
            case -1:    std::cout << "error while setting the Timeout\n"; break;
            case -2:    std::cout << "error while reading the character\n"; break;
            case -3:    std::cout << "MaxNbBytes is reached\n"; break;
            default:    std::cout << "Unknown error code\n"; break;
            }
            std::cout << "Thread Stopped.\n";
            break;
        }
        std::cout <<  "rx <- " << buf << std::endl;
    }
}

int main(int argc, char **argv) {

    serialib serial;
    char rc;
    if ((rc = serial.openDevice(COM_PATH"COM24", 115200)) != 1) {
        std::cout << getErrorMessage(rc) << std::endl;
        rc = EXIT_FAILURE;
        goto exit;
    }
    std::cout << "Device opened" << std::endl;

    char buf[1024];
    std::fill(buf, buf + sizeof(buf), 0);

    { // This allows t to be properly initialized before the label exit
        std::cout << "Creating Thread.\n";
        std::thread t(readThread, std::ref(serial));
    // }

    // # dummy for loop for 10 values
    for (int i = 0; i < 10; i++) {

        std::cout << "tx -> " << i << std::endl;
    }

    t.join();
    }
exit:
    if (serial.isDeviceOpen()) {
        serial.closeDevice();
    }

    return rc;

}