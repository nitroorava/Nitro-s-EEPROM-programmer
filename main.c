#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h> 
#include <windows.h>
#include <windef.h>
#include <winnt.h>
#include <winbase.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
	
	char containsfilename[64];
	char *filename=containsfilename;
	
	char *port_as_char;
	
	int portnumber;
	
	HANDLE hSerial;
	
	int exit_and_break(char error_message[128]){
		fprintf(stderr, "%s\n",error_message);
			CloseHandle(hSerial);
			sleep(3);
			return 1;
	}
	
	
	
	//this asks for a filename and COM port number if not provided as arguments
	if (argc==1){
		printf("give a filename\n");
		scanf("%s",filename);
		getchar();

		printf("give serial port number\n");
		scanf("%s",port_as_char);
		portnumber = atoi(port_as_char);
		getchar();

	}
	else if(argc==2){
		filename = argv[1];
		printf("give serial port number\n");
		scanf("%d",portnumber);
		portnumber = atoi(port_as_char);
		getchar();

	}
	else if(argc==3){
		filename = argv[1];
		portnumber = atoi(argv[2]);
	}
		
	else{
		printf ("too many arguments");
		return 1;
	}
	
	//printf ("kaks\n");
	//make a pointer to the file  and container for data
		
	FILE *hexfile_pointer;
	hexfile_pointer = fopen(filename,"r");
	
	if(hexfile_pointer==NULL){
		perror ("Unable to open file");
		exit (EXIT_FAILURE);
	}
	//printf("pointer at location: %x\n",hexfile_pointer);
	
	//find file size in bytes
	fseek(hexfile_pointer, 0, SEEK_END);
	
	long int size = ftell(hexfile_pointer);
	printf("size of file: %d bytes\n",size);
	
	rewind(hexfile_pointer);
	
	
	const int chunk=32;
	
	
	uint8_t bytes_to_send[size];
	
	

	
	int c;
	unsigned int n=0;
	
	//get the data from the file into an array
	while(1){
		c=fgetc(hexfile_pointer);
		if(feof(hexfile_pointer)){
			break;
		}
		bytes_to_send[n]=c;
		n++;
	}
	fclose(hexfile_pointer);
	
	//print what you are about to send
	/*
	printf("Here's what you are writing:\n");
	n=0;
	while(bytes_to_send[n]!='\n'&&n<size){
		for(int i=0;i<8;i++){
			printf(" %02x",bytes_to_send[n]);
			n++;
			
		}
		printf("\n");
	}
	*/
	
	
	
	
	//open the specified serial port
	char serialport[64];
	snprintf(serialport, sizeof(serialport),"\\\\.\\COM%d",portnumber);
	
	
	DCB dcbSerialParams = {0};
	COMMTIMEOUTS timeouts = {0};
	
	hSerial = CreateFile(
                serialport, GENERIC_READ|GENERIC_WRITE, 0, NULL,
                OPEN_EXISTING, 0, NULL );
	
	if (hSerial == INVALID_HANDLE_VALUE){
        fprintf(stderr, "Error: %02x \n",GetLastError() );
        sleep(3);
		return 1;
    }
    else {
		fprintf(stderr, "OK\n");
	}
	
	// Set device parameters (57600 baud, 1 start bit,
    // 1 stop bit, no parity)
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (GetCommState(hSerial, &dcbSerialParams) == 0){
        exit_and_break("Error getting device state");
    }
     
    dcbSerialParams.BaudRate = CBR_19200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if(SetCommState(hSerial, &dcbSerialParams) == 0){
        exit_and_break("Error setting device parameters");
    }
	
	// Set COM port timeout settings
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    
	if(SetCommTimeouts(hSerial, &timeouts) == 0){
        exit_and_break("Error setting timeouts");
    }
	
	//setup event to check for serial input
	if (SetCommMask(hSerial, EV_RXCHAR)==FALSE){
		exit_and_break("Error setting event check");
	}
		
	
	DWORD dwEventMask, bytes_read;
	
	uint8_t byte_[1];
	float delay=0.01;
	uint8_t temp_byte;
	
	// Send specified text (remaining command line arguments)
    DWORD bytes_written, total_bytes_written = 0;
    fprintf(stderr, "Sending bytes...\n");
	
	for(int i=0;i<size;i++){
		byte_ [0]= bytes_to_send[i];
		if(!WriteFile(hSerial, byte_, 1, &bytes_written, NULL)){
			exit_and_break("Error writing");
			break;
		}
		fprintf(stderr, "%02X  --",byte_ [0]);
		if(!WaitCommEvent(hSerial,&dwEventMask, NULL)){
			exit_and_break("Error reading");
			break;
		}
		
		ReadFile(hSerial, &temp_byte, sizeof(temp_byte),&bytes_read,NULL);
		fprintf(stderr, ">  %02X\n",temp_byte);
		
		if(byte_[0] != temp_byte){
			exit_and_break("Error, something went wrong!");
			break;
		}
		//Sleep(delay);
	}
	
	fprintf(stderr, "%d bytes written, %d bytes read\n", bytes_written, bytes_read);
	CloseHandle(hSerial);
	//sleep(3);
	
}





