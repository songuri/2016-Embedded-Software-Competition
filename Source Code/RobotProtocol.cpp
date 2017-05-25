#include "RobotProtocol.h"

struct termios options;
int uart0_filestream  = -1;
unsigned char read_value;

void Uart_open(void){

                uart0_filestream = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);          //Open in non blocking re$
                if (uart0_filestream == -1) {
                        //ERROR - CAN'T OPEN SERIAL PORT
                        printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
                }
}


void  Init_console(void){
	 tcgetattr(uart0_filestream, &options);
       	 options.c_cflag = B4800 | CS8 | CLOCAL | CREAD;         //<Set baud rate
       	 options.c_iflag = IGNPAR;
       	 options.c_oflag = 0;
       	 options.c_lflag = 0;
       	 tcflush(uart0_filestream, TCIFLUSH);
       	 tcsetattr(uart0_filestream, TCSANOW, &options);
}

void Motion(unsigned char move_value){

	 if (uart0_filestream != -1){                                       //      &tx_buffer[0] , (p_tx_buffer - &tx_buffer[0])
                int count = write(uart0_filestream,&move_value, 1 );           //Filestream, bytes to write, number of bytes to $
                if (count < 0) {
                        printf("UART TX error\n");
                }
        }
}

void Uart_close(void){
	close(uart0_filestream);
}


unsigned char Check_Read(){

	unsigned char read_length;
	do{
		read_length = read(uart0_filestream , &read_value, 1);
	}while(read_length > 0);

	return read_value;
}
