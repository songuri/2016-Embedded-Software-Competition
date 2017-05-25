#ifndef _ROBOT_PROTOCOL_
#define _ROBOT_PROTOCOL_

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <cstdio>

void Uart_open(void);
void Init_console(void);
void Motion(unsigned char move_value);
unsigned char Check_Read();
void Uart_close(void);

#endif //_ASDFROBOT_PROTOCOL



//#include <stdio.h>
//#include <iostream>
//#include <fcntl.h>
//#include <termios.h>
//#include <iostream>


//int uart0_filestream = -1; // if uart0_filestream is -1 , failed



//struct termios options;

//unsigned char read_value;

