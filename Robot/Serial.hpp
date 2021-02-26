#ifndef SERIAL_H
#define SERIAL_H

// C library headers
#include <cstdio>
#include <cstring>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

int serialInit(char *serial_path);
void sendYawAngleSpeed(unsigned char angleSpeed);

#endif
