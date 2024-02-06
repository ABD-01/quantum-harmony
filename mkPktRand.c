#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include "hexdump/hexdump.h"

#if !defined(HEXDUMP_H)
#define NUMCOLS             24
void hexdump(const void *buffer, size_t size) {
    const unsigned char *buf = (const unsigned char *)buffer;

    for (size_t i = 0; i < size; ++i) {
        printf("%02X ", buf[i]);

        // Add a newline after every NUMCOLS bytes for better readability
        if ((i + 1) % NUMCOLS == 0 || i == size - 1) {
            for (size_t j = 0; j < NUMCOLS - (i % NUMCOLS); ++j) {
                printf("   "); // Add extra spaces for alignment
            }
            printf("| ");
            for (size_t j = i - (i % NUMCOLS); j <= i; ++j) {
                if (buf[j] >= ' ' && buf[j] <= '~') {
                    printf("%c", buf[j]);
                } else {
                    printf(".");
                }
            }
            printf("\n");
        }
    }
    printf("\n");
}
#endif


typedef struct {
    uint32_t length;
    uint8_t data[61*1024];
    uint8_t topic[50];
} MSG_NET_MQTT_CVP_t;

static uint32_t g_pktSize = 10*1024;
static uint32_t g_randSeed = 42;

static void mkRandPkt(MSG_NET_MQTT_CVP_t* msgNetMqtt);
static void generatRandomBuffer(uint8_t buffer[], size_t size);

int main()
{
    MSG_NET_MQTT_CVP_t g_msgNetMqtt;
    mkRandPkt(&g_msgNetMqtt);
    // Printf the struct details and data in HEx
    printf("Struct Details: \n");
    printf("Length: %"PRIu32"\n", g_msgNetMqtt.length);
    printf("Topic: %s\n", g_msgNetMqtt.topic);
    printf("Data: \n");
    /**
    for (int i = 0; i < g_msgNetMqtt.length; i++)
    {
        printf("%"PRIx8" ", g_msgNetMqtt.data[i]);
    }
    */
    hexdump(g_msgNetMqtt.data, g_msgNetMqtt.length);
    printf("\n");

}

static void mkRandPkt(MSG_NET_MQTT_CVP_t* msgNetMqtt)
{

    memset(msgNetMqtt->data, 0x00, sizeof(msgNetMqtt->data));
    msgNetMqtt->length = g_pktSize;
    generatRandomBuffer(msgNetMqtt->data, g_pktSize);

    printf("Created qNET_MQTT_CVP MQTT message data of Length =%u\r\n", msgNetMqtt->length);
    sprintf((char*)msgNetMqtt->topic, "%s", "Accolade_Testing");
}

static void generatRandomBuffer(uint8_t buffer[], size_t size)
{
    uint32_t *ptr = (uint32_t *)buffer;
    size_t numIterations = size / sizeof(uint32_t);

    for (size_t i = 0; i < numIterations; ++i) {
    	g_randSeed ^= (g_randSeed << 13);
    	g_randSeed ^= (g_randSeed >> 17);
    	g_randSeed ^= (g_randSeed << 5);
        ptr[i] = g_randSeed;
    }
}

// gcc -g -Wall mkRandPkt.c hexdump.c -o build/mkRandPkt.exe
// clang .\mkPktRand.c -I .\hexdump\ .\hexdump\hexdump.o -o .\mkPktRand.exe