/**********************************************************************
* Filename    : Joystick.cpp
* Description : Read Joystick
* Author      : www.freenove.com
* modification: 2020/03/09
**********************************************************************/
#include <wiringPi.h>
#include <stdio.h>
#include <softPwm.h>
#include <ADCDevice.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>

#define Z_Pin 1     //define pin for axis Z

ADCDevice *adc;  // Define an ADC Device class object

int main(void){
    int server_fd;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        fprintf(stderr, "Failed to create socket fd\n");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        fprintf(stderr, "Error in setsockopt\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address))<0)
    {
        fprintf(stderr, "Bind failed\n");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        fprintf(stderr, "Failed to listen\n");
        exit(EXIT_FAILURE);
    }

    printf("Server: Listening for connections\n");

    int addrlen = sizeof(address);
    int new_socket;

    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0)
    {
        fprintf(stderr, "Error when accepting connection\n");
        exit(EXIT_FAILURE);
    }

    printf("Server: accepted connection\n");

    adc = new ADCDevice();
    printf("Program is starting ... \n");

    if(adc->detectI2C(0x48)){    // Detect the pcf8591.
        delete adc;                // Free previously pointed memory
        adc = new PCF8591();    // If detected, create an instance of PCF8591.
    }
    else if(adc->detectI2C(0x4b)){// Detect the ads7830
        delete adc;               // Free previously pointed memory
        adc = new ADS7830();      // If detected, create an instance of ADS7830.
    }
    else{
        printf("No correct I2C address found, \n"
        "Please use command 'i2cdetect -y 1' to check the I2C address! \n"
        "Program Exit. \n");
        return -1;
    }
    wiringPiSetup();
    pinMode(Z_Pin,INPUT);       //set Z_Pin as input pin and pull-up mode
    pullUpDnControl(Z_Pin,PUD_UP);
    while(1){
        int val_Z = digitalRead(Z_Pin);  //read digital value of axis Z
        int val_Y = adc->analogRead(0);      //read analog value of axis X and Y
        int val_X = adc->analogRead(1);
        char s[100] = { 0 };
        const char *tmp = "%d %d";
        sprintf(s, tmp, val_X, val_Y);
        send(new_socket, s, 100 * sizeof(char), 0);
        printf("val_X: %d  ,\tval_Y: %d  ,\tval_Z: %d \n",val_X,val_Y,val_Z);
        delay(20);
    }
    return 0;
}
