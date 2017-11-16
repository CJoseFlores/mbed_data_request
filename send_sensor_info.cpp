#include "mbed.h"
#include "BME280.h"

Serial pc(USBTX, USBRX);
Serial esp(p28, p27); // tx, rx
BME280 sensor(p9, p10);

 
int main() {
    pc.baud(115200);
    esp.baud(115200);
    pc.printf("Starting...\n\r");
    
    while(1) {
        pc.printf("Temp: %2.2f degC | Pressure: %04.2f hPa | Humidity: %2.2f %%\n\r", sensor.getTemperature(), sensor.getPressure(), sensor.getHumidity());
        esp.printf("Temp: %2.2f degC | Pressure: %04.2f hPa | Humidity: %2.2f %%\n\r", sensor.getTemperature(), sensor.getPressure(), sensor.getHumidity());
        //esp.putc('T');
        wait(1);
    }
}