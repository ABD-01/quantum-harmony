///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                                  FIRMWARE                                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include <string.h>

// Export functions for use in a shared library
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

extern void sendProgress(int num_to_send);

typedef enum
{
	APP_FW_INIT_ST,
	APP_FW_WR_PKT_ST,
	APP_FW_RD_WAIT_ACK_ST,
	APP_FW_IDLE_ST,
	APP_FW_RD_ONLY,
	APP_FW_SET_FLAG_ST,
	APP_FW_RBT_ST,
	APP_FW_PARSE_RCVD_STRING,
	
	APP_FW_MAX_ST
}FW_APP_FW_STATES;

#define FW_MAX_CHUNK_SIZE	512
#define FW_STX  			'('	
#define FW_ETX  			')'		

EXPORT int serial_upload_firmware(HANDLE hComm, char filePath[200])
{
	printf("ARGS HANDLE %d FILE PATH %s\r\n", hComm, filePath);

	FILE *fp;
	int len, Status;
	char winComPortName[50];
	int RemainBytes;
	int percentComplete = 0;
	long int actual_remaing_bytes = 0; // TODO: get this variable a good name, `RemainBytes` is already taken

	char lpBuffer[1024] = { 0 };
	DWORD dNoOFBytestoWrite;         // No of bytes to write into the port
	DWORD dNoOfBytesWritten = 0;     // No of bytes written to the port

	char TempChar; 					//Temporary character used for reading
	char SerialBuffer[20] = {0};	//Buffer for storing Rxed Data
	DWORD NoBytesRead;
	int Itr = 0, offset = 0;
	int Address = 0x44200;				//0xA00;
	
	fp = fopen(filePath, "rb");
	if(fp == NULL)
	{
		printf("ERR FILE OPEN\r\n");
		return -1;
	}

	fseek(fp, 0L, SEEK_END); 
  
    // calculating the size of the file 
    long int FileSize = ftell(fp); 
    
    // closing the file 
    fclose(fp); 
    
    fp = fopen(filePath, "rb");

    printf("File size %ld\r\n",FileSize);
	actual_remaing_bytes = FileSize;

	printf("SUCC FILE OPEN\r\n");
	
	int AppState = APP_FW_INIT_ST;
	int MatchFlag = 0xFF;
	int ReadComplete = 0;
	int ReadingStart = 0;
	
    while(1)
    {
		switch(AppState)
		{
			case APP_FW_IDLE_ST:
			{
				break;
			}
			case APP_FW_INIT_ST:
			{
				offset  = sprintf(lpBuffer,"StartP");
				printf("STARTING FIRMWARE UPGRADE....");
				dNoOFBytestoWrite = offset;
			    Status = WriteFile(hComm,        // Handle to the Serial port
			               lpBuffer,     // Data to be written to the port
			               dNoOFBytestoWrite,  //No of bytes to write
			               &dNoOfBytesWritten, //Bytes written
			               NULL);
			    if(dNoOFBytestoWrite != dNoOfBytesWritten)
				{
					printf("ERR WR ON COM PORT %d %d\r\n", dNoOFBytestoWrite, dNoOfBytesWritten);
					return -1;
				}
				Sleep(3000);
				printf("OK\r\n");
				AppState = APP_FW_WR_PKT_ST;
				break;
			}
			case APP_FW_WR_PKT_ST:
			{
				memset(lpBuffer, 0, sizeof(lpBuffer));
				offset = sprintf(lpBuffer,"WRFWFile %d %d ", Address, FW_MAX_CHUNK_SIZE);
				RemainBytes = fread(&lpBuffer[offset], FW_MAX_CHUNK_SIZE, 1, fp);
				dNoOFBytestoWrite = offset + FW_MAX_CHUNK_SIZE;
								
				Status = WriteFile(hComm,   	// Handle to the Serial port
				      		lpBuffer,        	// Data to be written to the port
				            dNoOFBytestoWrite,  //No of bytes to write
				            &dNoOfBytesWritten, //Bytes written
				            NULL);

				if(dNoOFBytestoWrite != dNoOfBytesWritten)
				{
					printf("ERR WR ON COM PORT WR %d %d\r\n", dNoOFBytestoWrite, dNoOfBytesWritten);
					return -1;
				}

				actual_remaing_bytes = max(0, actual_remaing_bytes - FW_MAX_CHUNK_SIZE);
				percentComplete = 100 - ((actual_remaing_bytes * 100) / FileSize);
				sendProgress(percentComplete);
				printf("Remain Bytes: %d Percent Complete: %d%%\r\n", actual_remaing_bytes, percentComplete);
								
				if(RemainBytes != 1)
				{	
					Sleep(1000);
					AppState = APP_FW_SET_FLAG_ST;
					break;
				}
				AppState = APP_FW_RD_ONLY;
				break;
			}
			
			case APP_FW_RD_ONLY:
			{ 	
				ReadComplete = 0;		
				memset(SerialBuffer, 0, sizeof(SerialBuffer));		
				Itr = 0;
				ReadingStart = 0;
				do
				{			
				    ReadFile(hComm,           	//Handle of the Serial port
				             &TempChar,       	//Temporary character
				             1, 				//Size of TempChar
				             &NoBytesRead,    	//Number of bytes read
				             NULL);
									
					SerialBuffer[Itr] = TempChar;	
				    if(TempChar == FW_STX)
				    {
						ReadingStart = 1;	
					}
				    if(ReadingStart == 1)
				    {
				    	SerialBuffer[Itr] = TempChar; 	// Store Tempchar into buffer
				    	if(TempChar == '\0' || Itr >= 20)
				    	{
				    		Itr = 0;
				    		continue;
						}
						Itr++;
					}
							
				}while(TempChar != FW_ETX);			
			
		 		AppState = APP_FW_PARSE_RCVD_STRING;		
							    
				break;
			}
			
			case APP_FW_PARSE_RCVD_STRING:
			{
				MatchFlag = 0xFF;
				if(0 == strncmp(SerialBuffer, "(#NEXT CHUNK#)", strlen(SerialBuffer)))	
				{
						MatchFlag = 1;
				}	
				else if(0 == strncmp(SerialBuffer, "(#PREV CHUNK#)", strlen(SerialBuffer)))
				{
						MatchFlag = 0;
				}
				
				if(MatchFlag == 1)
				{
					printf("---MATCH---\r\n");
					Address += FW_MAX_CHUNK_SIZE;
				}
				AppState = APP_FW_WR_PKT_ST;
				break;
			}	

			case APP_FW_SET_FLAG_ST:
			{	
				printf("]\r\n");
				printf("FIRMWARE DOWNLOAD... COMPLETE\r\n");
				char Buff[20] = "SETBLFlag FU";
				WriteFile(hComm,        // Handle to the Serial port
				               Buff,     // Data to be written to the port
				               strlen(Buff),  //No of bytes to write
				               &dNoOfBytesWritten, //Bytes written
				               NULL);
				Sleep(2000);
				AppState = APP_FW_RBT_ST;
				break;
			}
			case APP_FW_RBT_ST:
			{
				printf("REBOOTING.....  ");
				char Buff[10] = "RBT";
				WriteFile(hComm,        // Handle to the Serial port
				               Buff,     // Data to be written to the port
				               strlen(Buff),  //No of bytes to write
				               &dNoOfBytesWritten, //Bytes written
				               NULL);
				AppState = APP_FW_RD_ONLY;
				printf("DONE\r\n");
				return 0;
				break;
			}
			case APP_FW_RD_WAIT_ACK_ST:
			{
				do
				{			
				    ReadFile( hComm,           //Handle of the Serial port
				             &TempChar,       //Temporary character
				             sizeof(TempChar),//Size of TempChar
				             &NoBytesRead,    //Number of bytes read
				             NULL);
				
				    SerialBuffer[Itr] = TempChar;	// Store Tempchar into buffer
				    Itr++;
				    printf("ITR %d, Char %c, RCVD %s\r\n", Itr, TempChar, SerialBuffer);
				}while (TempChar != '\r' && NoBytesRead > 0);
						
			    printf("RECVD %s\r\n", SerialBuffer);
			    AppState = APP_FW_WR_PKT_ST;
			    break;
			}
		}
	}	
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                                  CERTIFICATE                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#define BOOT_PART_START_ADDRESS					(0x000000L)
#define BOOT_PART_END_ADDRESS					(BOOT_PART_START_ADDRESS + 0x1FFL) //	512 Bytes
#define FW_METADATA_START_ADDRESS				(0x010)
#define FW_METADATA_END_ADDRESS					(FW_METADATA_START_ADDRESS + 0x04FL)

/* NOTE:::::   DO NOT MODIFY BELOW PARTITIONS  NEW PARTITIONS */

#define CONF_MAIN_CPY_STRT_ADDR					(BOOT_PART_END_ADDRESS + 1) 		//(0x200)					//1k allocated
#define CONF_MAIN_CPY_END_ADDR					(CONF_MAIN_CPY_STRT_ADDR + 0x3FFL)

#define CONF_CC_CPY_STRT_ADDR					(CONF_MAIN_CPY_END_ADDR + 1)									//1k allocated
#define CONF_CC_CPY_END_ADDR					(CONF_CC_CPY_STRT_ADDR + 0x3FFL)

#define CONF_TML_MAIN_CPY_STRT_ADDR				(CONF_CC_CPY_END_ADDR + 1) 		//(0x200)						//1k allocated
#define CONF_TML_MAIN_CPY_END_ADDR				(CONF_TML_MAIN_CPY_STRT_ADDR + 0x3FFL)

#define CONF_TML_CC_CPY_STRT_ADDR				(CONF_TML_MAIN_CPY_END_ADDR + 1)								//1k allocated
#define CONF_TML_CC_CPY_END_ADDR				(CONF_TML_CC_CPY_STRT_ADDR + 0x3FFL)

#define CONF_DEVICE_MAIN_CPY_STRT_ADDR			(CONF_TML_CC_CPY_END_ADDR + 1) 		//(0x200)					//1k allocated
#define CONF_DEVICE_MAIN_CPY_END_ADDR			(CONF_DEVICE_MAIN_CPY_STRT_ADDR + 0x3FFL)

#define CONF_DEVICE_CC_CPY_STRT_ADDR			(CONF_DEVICE_MAIN_CPY_END_ADDR + 1)								//1k allocated
#define CONF_DEVICE_CC_CPY_END_ADDR				(CONF_DEVICE_CC_CPY_STRT_ADDR + 0x3FFL)

#define CONF_ACCELEROMETER_CAL_STRT_ADDR		(CONF_DEVICE_CC_CPY_END_ADDR + 1)
#define CONF_ACCELEROMETER_CAL_END_ADDR			(CONF_ACCELEROMETER_CAL_STRT_ADDR + 0x3FFL)						//1k allocated

#define DEVICE_CAN_CONFIG_START_ADDR 			(CONF_ACCELEROMETER_CAL_END_ADDR + 1)
#define DEVICE_CAN_CONFIG_END_ADDR				(DEVICE_CAN_CONFIG_START_ADDR + 0x3FFL)							//1k allocated

#define DEVICE_RESERVE1_START_ADDR				(DEVICE_CAN_CONFIG_END_ADDR + 1)								//Reserve 8K for future use
#define DEVICE_RESERVE1_END_ADDR				(DEVICE_RESERVE1_START_ADDR + 0x1FFFL)

#define DEF_FIRMWARE_PART_START_ADDR			(DEVICE_RESERVE1_END_ADDR + 1)									//256 K
#define DEF_FIRMWARE_PART_END_ADDR				(DEF_FIRMWARE_PART_START_ADDR + 0x3FFFFL)

#define UP_FIRMWARE_PART_START_ADDR				(DEF_FIRMWARE_PART_END_ADDR + 1)
#define UP_FIRMWARE_PART_END_ADDR				(UP_FIRMWARE_PART_START_ADDR + 0x3FFFFL)						//256 K

#define GENERIC_CAN_CONFIG_START_ADDR			(UP_FIRMWARE_PART_END_ADDR + 1)
#define GENERIC_CAN_CONFIG_END_ADDR				(GENERIC_CAN_CONFIG_START_ADDR + 0x3FFL)						//1 KB

#define IC_CAN_CONFIG_START_ADDR				(GENERIC_CAN_CONFIG_END_ADDR + 1)
#define IC_CAN_CONFIG_END_ADDR					(IC_CAN_CONFIG_START_ADDR + 0x3FFL)								//1 KB

#define PROGNOSTIC_CAN_CONFIG_START_ADDR		(IC_CAN_CONFIG_END_ADDR + 1)
#define PROGNOSTIC_CAN_CONFIG_END_ADDR			(PROGNOSTIC_CAN_CONFIG_START_ADDR + 0x3FFL)						//1 KB

#define DEF_FIRM_META_START_ADDR				(PROGNOSTIC_CAN_CONFIG_END_ADDR+1)
#define DEF_FIRM_META_END_ADDR					(DEF_FIRM_META_START_ADDR + 0x1FFL)								//512 Bytes

#define DEV_CONFIG_META_START_ADDR				(DEF_FIRM_META_END_ADDR + 1)									//1 KB
#define DEV_CONFIG_META_END_ADDR				(DEV_CONFIG_META_START_ADDR + 0x3FFL)

#define CAN_CONFIG_META_START_ADDR				(DEV_CONFIG_META_END_ADDR + 1)									//1 KB
#define CAN_CONFIG_META_END_ADDR				(CAN_CONFIG_META_START_ADDR + 0x3FFL)

#define ROOTCA_META_START_ADDR					(CAN_CONFIG_META_END_ADDR + 1)									//1 KB
#define ROOTCA_META_END_ADDR					(ROOTCA_META_START_ADDR + 0x3FFL)

#define CLI_CERT_META_START_ADDR				(ROOTCA_META_END_ADDR + 1)										//1 KB
#define CLI_CERT_META_END_ADDR					(CLI_CERT_META_START_ADDR + 0x3FFL)

#define CLI_KEY_META_START_ADDR					(CLI_CERT_META_END_ADDR + 1)									//1 KB
#define CLI_KEY_META_END_ADDR					(CLI_KEY_META_START_ADDR + 0x3FFL)

#define ROOTCA_CERTI_START_ADDR					(CLI_KEY_META_END_ADDR + 1)
#define ROOTCA_CERTI_END_ADDR					(ROOTCA_CERTI_START_ADDR + 0xFFFL)								//4KB

#define CLIENT_CERTI_START_ADDR					(ROOTCA_CERTI_END_ADDR + 1)
#define CLIENT_CERTI_END_ADDR					(CLIENT_CERTI_START_ADDR + 0xFFFL)								//4KB

#define CLIENT_KEY_START_ADDR					(CLIENT_CERTI_END_ADDR + 1)
#define CLIENT_KEY_END_ADDR						(CLIENT_KEY_START_ADDR + 0xFFFL)								//4KB

#define ROOT_FILE_NAME	"ca-crt.pem"
#define ROOT_FILE_ADDR 	ROOTCA_CERTI_START_ADDR
#define CERT_FILE_NAME	"device-crt.pem"
#define CERT_FILE_ADDR 	CLIENT_CERTI_START_ADDR
#define KEY_FILE_NAME	"device-key.pem"
#define KEY_FILE_ADDR 	CLIENT_KEY_START_ADDR

typedef enum
{
	APP_CERT_INIT_ST,
	APP_CERT_WR_PKT_ST,
	APP_CERT_RD_WAIT_ACK_ST,
	APP_CERT_IDLE_ST,
	APP_CERT_RD_ONLY,
	APP_CERT_SET_FLAG_ST,
	APP_CERT_RBT_ST,
	APP_CERT_PARSE_RCVD_STRING,
	
	APP_CERT_MAX_ST
}CERT_APP_CERT_STATES;

typedef enum
{
	CERT_TYPE_CA_CRT,
	CERT_TYPE_DEVICE_CRT,
	CERT_TYPE_DEVICE_KEY
}CERT_TYPE;

#define MAX_CHUNK_SIZE	512
#define STX  			'$'	
#define ETX  			'^'		

EXPORT int serial_upload_certificate(HANDLE hComm, CERT_TYPE certType, char filePath[200])
{
	printf("ARGS HANDLE %d FILE PATH %s\r\n", hComm, filePath);

	FILE *fp;
	int len, Status;
	char winComPortName[50];
	int RemainBytes;
	int fileId = certType;	
	int FileOffset=0,PacketSize = 0;
	int Address = 0;
	int percentComplete = 0;
	long int actual_remaing_bytes = 0; // TODO: get this variable a good name, `RemainBytes` is already taken
	
	char lpBuffer[1024] = { 0 };
	DWORD dNoOFBytestoWrite;         // No of bytes to write into the port
	DWORD dNoOfBytesWritten = 0;     // No of bytes written to the port

	char TempChar; 					//Temporary character used for reading
	char SerialBuffer[2000] = {0};		//Buffer for storing Rxed Data
	DWORD NoBytesRead;
	int Itr = 0, offset = 0;
	
	switch(fileId)
	{
		case CERT_TYPE_CA_CRT:
			Address = ROOT_FILE_ADDR;
			break;
		case CERT_TYPE_DEVICE_CRT:
			Address = CERT_FILE_ADDR;
			break;
		case CERT_TYPE_DEVICE_KEY:
			Address = KEY_FILE_ADDR;
			break;
	}
	
	fp = fopen(filePath, "rb");
	if(fp == NULL)
	{
		printf("ERR FILE OPEN\r\n");
		return -1;
	}
	
	fseek(fp, 0L, SEEK_END); 
  
    // calculating the size of the file 
    long int FileSize = ftell(fp); 
    
    // closing the file 
    fclose(fp); 
    
    fp = fopen(filePath, "rb");

    printf("File size %ld\r\n",FileSize);
	actual_remaing_bytes = FileSize;
    
	printf("SUCC FILE OPEN\r\n");
	
	int AppState = APP_CERT_INIT_ST;
	int MatchFlag = 0xFF;
	int ReadComplete = 0;
	int ReadingStart = 0;
	
    while(1)
    {	
		
		switch(AppState)
		{
			case APP_CERT_IDLE_ST:
			{
				break;
			}
			case APP_CERT_INIT_ST:
			{
				offset  = sprintf(lpBuffer,"StartP");
				printf("STARTING CERTIFICATE UPGRADE....");
				dNoOFBytestoWrite = offset;
			    Status = WriteFile(hComm,        // Handle to the Serial port
			               lpBuffer,     // Data to be written to the port
			               dNoOFBytestoWrite,  //No of bytes to write
			               &dNoOfBytesWritten, //Bytes written
			               NULL);
			    if(dNoOFBytestoWrite != dNoOfBytesWritten)
				{
					printf("ERR WR ON COM PORT %d %d\r\n", dNoOFBytestoWrite, dNoOfBytesWritten);
					return -1;
				}
				Sleep(3000);
				printf("OK\r\n");
				AppState = APP_CERT_WR_PKT_ST;
				break;
			}
			case APP_CERT_WR_PKT_ST:
			{
				memset(lpBuffer, 0, sizeof(lpBuffer));
				FileOffset += PacketSize;
				if((FileSize - FileOffset) > MAX_CHUNK_SIZE)
				{
					PacketSize = MAX_CHUNK_SIZE;
				}
				else
				{
					PacketSize = FileSize - FileOffset;
				}
				printf("Packet Size %d\r\n",PacketSize);
				offset = sprintf(lpBuffer,"WRFWFile %d %d ", Address, PacketSize);
				RemainBytes = fread(&lpBuffer[offset], PacketSize, 1, fp);
				printf("Remaining Bytes %d\r\n",RemainBytes);
				dNoOFBytestoWrite = offset + PacketSize;
				
				Status = WriteFile(hComm,   // Handle to the Serial port
				      		lpBuffer,        // Data to be written to the port
				            dNoOFBytestoWrite,  //No of bytes to write
				            &dNoOfBytesWritten, //Bytes written
				            NULL);

				if(dNoOFBytestoWrite != dNoOfBytesWritten)
				{
					printf("ERR WR ON COM PORT WR %d %d\r\n", dNoOFBytestoWrite, dNoOfBytesWritten);
					return -1;
				}
				
				actual_remaing_bytes = actual_remaing_bytes - PacketSize;
				percentComplete = 100 - ((actual_remaing_bytes * 100) / FileSize);
				sendProgress(percentComplete);
				printf("PERCENT COMPLETE %d\r\n",percentComplete);
				
				if(RemainBytes != 1)
				{	
					Sleep(1000);
					AppState = APP_CERT_SET_FLAG_ST;
					break;
				}
				AppState = APP_CERT_RD_ONLY;
				break;
			}
			
			case APP_CERT_RD_ONLY:
			{ 	
				ReadComplete = 0;		
				memset(SerialBuffer, 0, sizeof(SerialBuffer));		
				Itr = 0;
				ReadingStart = 0;
				do
				{			
				    ReadFile(hComm,           	//Handle of the Serial port
				             &TempChar,       	//Temporary character
				             1, 				//Size of TempChar
				             &NoBytesRead,    	//Number of bytes read
				             NULL);
			 						
					SerialBuffer[Itr] = TempChar;
						
				    if(TempChar == STX)
				    {
						ReadingStart = 1;	
					}
				    if(ReadingStart == 1)
				    {
				    	SerialBuffer[Itr] = TempChar; 	// Store Tempchar into buffer
						Itr++;
					}
							
				}while(TempChar != ETX);			
			
		 		AppState = APP_CERT_PARSE_RCVD_STRING;		
							    
				break;
			}
			
			case APP_CERT_PARSE_RCVD_STRING:
			{
				MatchFlag = 0xFF;
				if(strstr(SerialBuffer, "(#NEXT CHUNK#)"))	
				{
						MatchFlag = 1;
				}	
				else if(strstr(SerialBuffer, "(#PREV CHUNK#)"))
				{
						MatchFlag = 0;
				}
				
				if(MatchFlag == 1)
				{
					printf("---MATCH---\r\n");
					Address += MAX_CHUNK_SIZE;
				}
				else
				{
					printf("---NOT MATCH---\r\n");
				}
				AppState = APP_CERT_WR_PKT_ST;
				break;
			}	

			case APP_CERT_SET_FLAG_ST:
			{	
				printf("]\r\n");
				printf("CERTIFICATE DOWNLOAD... COMPLETE\r\n");
				char Buff[20] = "SET";
				WriteFile(hComm,        // Handle to the Serial port
				               Buff,     // Data to be written to the port
				               strlen(Buff),  //No of bytes to write
				               &dNoOfBytesWritten, //Bytes written
				               NULL);
				Sleep(2000);
				AppState = APP_CERT_RBT_ST;
				break;
			}
			case APP_CERT_RBT_ST:
			{
				printf("REBOOTING.....  ");
				char Buff[10] = "RBT";
				WriteFile(hComm,        // Handle to the Serial port
				               Buff,     // Data to be written to the port
				               strlen(Buff),  //No of bytes to write
				               &dNoOfBytesWritten, //Bytes written
				               NULL);
				AppState = APP_CERT_RD_ONLY;
				printf("DONE\r\n");
				return 0;
				break;
			}
			case APP_CERT_RD_WAIT_ACK_ST:
			{
				do
				{			
				    ReadFile( hComm,           //Handle of the Serial port
				             &TempChar,       //Temporary character
				             sizeof(TempChar),//Size of TempChar
				             &NoBytesRead,    //Number of bytes read
				             NULL);
				
				    SerialBuffer[Itr] = TempChar;	// Store Tempchar into buffer
				    Itr++;
				    printf("ITR %d, Char %c, RCVD %s\r\n", Itr, TempChar, SerialBuffer);
				}while (TempChar != '\r' && NoBytesRead > 0);
						
			    printf("RECVD %s\r\n", SerialBuffer);
			    AppState = APP_CERT_WR_PKT_ST;
			    break;
			}
		}
	}	
	return 0;
}
