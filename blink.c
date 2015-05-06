/*
 *  Blinks a LED forever (until Ctrl+C)
 */
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define IN   0
#define OUT  1

#define LOW  0
#define HIGH 1

#define POUT 11

static int GPIOExport(int pin);
static int GPIOUnexport(int pin);
static int GPIODirection(int pin, int dir);
static int GPIORead(int pin);
static int GPIOWrite(int pin, int value);

volatile int done = 0;
 
void term(int signum)
{
    done = 1;
}

int main(int argc, char *argv[]) {
	printf("START...\n");
	
	//catch SIGTERM for graceful exit
	struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);
	
	printf("INIT GPIO\n");
    //Enable GPIO pins
    if (-1 == GPIOExport(POUT))
		return(1);

    //Set GPIO directions
    if (-1 == GPIODirection(POUT, OUT))
		return(2);

    while(done == 0) {//while not done...
		//Write GPIO value
		printf("ON\n");
		if (-1 == GPIOWrite(POUT, HIGH))
			return(3);
		usleep(1000 * 1000);
		printf("OFF\n");
		if (-1 == GPIOWrite(POUT, LOW))
			return(3);
		usleep(1000 * 1000);
    }

    //Disable GPIO pins
    if (-1 == GPIOUnexport(POUT))
		return(4);

    return(0);
}

int GPIOExport(int pin) {
#define BUFFER_MAX 3
    char buffer[BUFFER_MAX];
    ssize_t bytes_written;
    int fd;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (-1 == fd) {
	fprintf(stderr, "Failed to open export for writing!\n");
	return(-1);
    }

    bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
    write(fd, buffer, bytes_written);
    close(fd);
    return(0);
}

int GPIOUnexport(int pin) {
    char buffer[BUFFER_MAX];
    ssize_t bytes_written;
    int fd;

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (-1 == fd) {
	fprintf(stderr, "Failed to open unexport for writing!\n");
	return(-1);
    }

    bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
    write(fd, buffer, bytes_written);
    close(fd);
    return(0);
}

int GPIODirection(int pin, int dir) {
    static const char s_directions_str[]  = "in\0out";

#define DIRECTION_MAX 35
    char path[DIRECTION_MAX];
    int fd;

    snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
    fd = open(path, O_WRONLY);
    if (-1 == fd) {
	fprintf(stderr, "Failed to open gpio direction for writing!\n");
	return(-1);
    }

    if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
	fprintf(stderr, "Failed to set direction!\n");
	return(-1);
    }

    close(fd);
    return(0);
}

int GPIORead(int pin) {
#define VALUE_MAX 30
    char path[VALUE_MAX];
    char value_str[3];
    int fd;

    snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_RDONLY);
    if (-1 == fd) {
	fprintf(stderr, "Failed to open gpio value for reading!\n");
	return(-1);
    }

    if (-1 == read(fd, value_str, 3)) {
	fprintf(stderr, "Failed to read value!\n");
	return(-1);
    }

    close(fd);

    return(atoi(value_str));
}

int GPIOWrite(int pin, int value) {
    static const char s_values_str[] = "01";

    char path[VALUE_MAX];
    int fd;

    snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_WRONLY);
    if (-1 == fd) {
	fprintf(stderr, "Failed to open gpio value for writing!\n");
	return(-1);
    }

    if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
	fprintf(stderr, "Failed to write value!\n");
	return(-1);
    }

    close(fd);
    return(0);
}
