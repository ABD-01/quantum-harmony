/**
 * @file        mqtt_client.c
 *
 * @date        7 February 2024
 * @author      Muhammed Abdullah Shaikh
 *
 * @brief       Made for testing the EC20 modem at test.mosquitto.org
 *              Sibcribes to topic and keeps track to the number of packets
 *              received and total payload delivered.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "MQTTClient.h"
#include "hexdump.h"

/**
 * The delay in log file updates may be due to output buffering. By default,
 * `printf` buffers output, delaying the write to the console or file until the
 * program exits. To address this, use `fflush(stdout)` and `fflush(logfile)` 
 * after printf and fprintf to ensure immediate writing to both console and file.
*/
#define PRINT(format, ...) do { printf(format, ##__VA_ARGS__); fprintf(logfile, format, ##__VA_ARGS__); fflush(stdout); fflush(logfile); } while (0)

#ifdef SSL_ENABLED
#define ADDRESS     "ssl://test.mosquitto.org:8884"
#define CA_CERT     "ca.pem"
#define CLIENT_CERT "cc.pem"
#define CLIENT_KEY  "ck.pem"
#define LOG_FILE    "logfile_ssl.txt"
#else
#define ADDRESS     "tcp://test.mosquitto.org:1883"
#define LOG_FILE    "logfile.txt"
#endif

#define CLIENTID    "AEPL_Test_Client"
#define TOPIC       "Accolade_Testing"
#define QOS         1
#define TIMEOUT     10000L

static int NUM_PACKETS = 0;
static float  TOTAL_KBYTES = 0;
static FILE *logfile;

int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void connectionLost(void *context, char *cause);

int main(int argc, char* argv[]) 
{
    printf("Starting MQTT Client\n");

    logfile = fopen(LOG_FILE, "a");
    if (logfile == NULL) {
        printf("Failed to open log file %s\n", LOG_FILE);
        return EXIT_FAILURE;
    }

    MQTTClient client;
    int rc = EXIT_SUCCESS;

    if ((rc = MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS) {
        PRINT("Failed to create client, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto exit;
    }

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 3600;
    conn_opts.cleansession = 1;
    conn_opts.username = NULL;
    conn_opts.password = NULL;

#ifdef SSL_ENABLED
    MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer ;
    // ssl_opts.verify = 1;
    ssl_opts.trustStore = CA_CERT;
    ssl_opts.keyStore = CLIENT_CERT;
    ssl_opts.privateKey = CLIENT_KEY;
    conn_opts.ssl = &ssl_opts;
#endif

    if ((rc = MQTTClient_setCallbacks(client, NULL, connectionLost, messageArrived, NULL)) != MQTTCLIENT_SUCCESS) {
        PRINT("Failed to set callbacks, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        PRINT("Failed to connect, return code %d\n", rc);
        rc =  EXIT_FAILURE;
        goto destroy_exit;
    }
    PRINT("Client connected to server %s with clientID: %s\n", ADDRESS, CLIENTID);

    if ((rc = MQTTClient_subscribe(client, TOPIC, QOS)) != MQTTCLIENT_SUCCESS) {
        PRINT("Failed to subscribe to the topic, return code %d\n", rc);
        rc = EXIT_FAILURE;
    }
    else
    {
        PRINT("Successfully subscribed to topic \"%s\"\n", TOPIC);
    }

    PRINT("Use q/Q to quit cleanly\n");
    int ch;
    do
    {
        ch = getchar();
    } while (ch!='Q' && ch != 'q');

    if ((rc = MQTTClient_unsubscribe(client, TOPIC)) != MQTTCLIENT_SUCCESS) {
        PRINT("Failed to unsubscribe, return code %d\n", rc);
        rc = EXIT_FAILURE;
    }

    if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS) {
        PRINT("Failed to disconnect, return code %d\n", rc);
        rc - EXIT_FAILURE;
    }

destroy_exit:
    MQTTClient_destroy(&client);
exit:
    fclose(logfile);
    return rc;
}

void connectionLost(void *context, char *cause)
{
    PRINT("\nConnection lost\n");
    PRINT("     cause: %s\n", cause);
}


int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    size_t numRows = (message->payloadlen + NUMCOLS - 1) / NUMCOLS; // same as ceil(message->payloadlen / NUMCOLS)
    size_t bufferSize = numRows * (NUMCOLS + (NUMCOLS * 3) + 6);   
    // NUMCOLLS will be printed, then there is "02X " 3chars for each byte, then "   | " make extra 5 chars and +1 for \n
    char *hexdumpBuffer = (char *)malloc(bufferSize);
    if (hexdumpBuffer == NULL) {
        return -1;
    }
    hexdumpToBuffer(message->payload, message->payloadlen, hexdumpBuffer);
    NUM_PACKETS++;
    TOTAL_KBYTES += (float)message->payloadlen / 1024.0;


    PRINT("Received Message on: %s\n", topicName);
    PRINT("Hexdump:\n");
    PRINT("%s\n", hexdumpBuffer);
    PRINT("Size: %d bytes\n", message->payloadlen);
    PRINT("Total packets received: %d\n", NUM_PACKETS);
    PRINT("Total Kilo Bytes received: %.2fk B\n", TOTAL_KBYTES);
    PRINT("\n");

    free(hexdumpBuffer);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}



// gcc mqtt_client.c -I eclipse-paho-mqtt-c/include/ -I hexdump/ -L eclipse-paho-mqtt-c/lib/ hexdump/hexdump.o -lpaho-mqtt3cs-static
// cl .\mqtt_client.c /I .\hexdump\ /I C:\dev\vcpkg\packages\paho-mqtt_x64-windows\include\ /I C:\dev\vcpkg\packages\openssl_x64-windows\include\ /link C:\dev\vcpkg\packages\paho-mqtt_x64-windows\lib\paho-mqtt3cs.lib .\hexdump\hexdump.o
// clang .\mqtt_client.c -I 'C:\dev\vcpkg\packages\paho-mqtt_x64-windows\include\' -I .\hexdump\ -L C:\dev\vcpkg\packages\paho-mqtt_x64-windows\lib\ -l paho-mqtt3cs .\hexdump\hexdump.o -o .\mqtt_client.exe