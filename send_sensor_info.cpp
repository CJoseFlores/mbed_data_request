#include "mbed.h"
#include "BME280.h"
#include <string>
#define UTC_TO_EST 18000

Serial pc(USBTX, USBRX);
Serial esp(p28, p27); // tx, rx
BME280 sensor(p9, p10);

float temp, pressure, humidity; //Used to store data grabbed from the BME280 Sensor.
time_t seconds; //Used to grab seconds since Epoch.
char timeBuffer[32]; //Holds formatted timeStamp.

bool packetRecieved; //Used to tell if the packet on the wifi chip was recieved.
 
int main() {
    //Setup for UART Communication.
    pc.baud(115200);
    esp.baud(115200);
    
    //Setting up timestamp information.
    set_time(1510877553 - UTC_TO_EST);  // Setting RTC Time, and converting from UTC to EST. 
    
    pc.printf("Starting...\n\r");
    
    while(1) {
        //Grabing information from sensor, and storing a timestamp.
        wait(5);
        temp = sensor.getTemperature();
        pressure = sensor.getPressure();
        humidity = sensor.getHumidity();
        seconds = time(NULL); // Returns the time in seconds since UTC.
        strftime(timeBuffer, 32, "%m.%d.%Y %I:%M %p\n", localtime(&seconds)); // Converting Epoch to formatted string.
        
        //Send formatted string with sensor data to the NCU chip and timestamp.
        pc.printf("Temp: !%2.2fdegC | Pressure: @%04.2fhPa | Humidity: #%2.2f%% | Time: $%s\n\r", temp, pressure, humidity, timeBuffer);
        //esp.printf("Temp: %2.2f degC | Pressure: %04.2f hPa | Humidity: %2.2f %%\n\r", sensor.getTemperature(), sensor.getPressure(), sensor.getHumidity());
        esp.printf("Temp: !%2.2fdegC | Pressure: @%04.2fhPa | Humidity: #%2.2f%% | Time: $%s", temp, pressure, humidity, timeBuffer);
        
        //Only get more sensor values once ESP has recieved the packet.
        while(!packetRecieved){
            if(esp.readable() == 1)
            {
                //ESP sends a "y" when the packet is recieved.
                if(esp.getc() == 'y'){
                    packetRecieved = true;
                }
            }
        }
        wait(1);
    }
}