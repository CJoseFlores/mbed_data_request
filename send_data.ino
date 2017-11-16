#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>

const char* ssid = "NotARouter";
const char* password = "Blep200k";

const char* serverUrl = "192.168.43.29"; //Raspberry Pi Server IP.
String serverEndpoint = "/sendsensorvalue"; // rest API used to send values to server.
const int httpPort = 5000;

SoftwareSerial mbed(D7, D8);  //rx, tx for reciving information from mbed.

//JSON-formatted template to send to server.
//String data = "{\"Hello\":\"Hi2\"}";
String data = "{\"data\":[{\"variableName\":\"Temperature\",\"timeStamp\": \"10.22.2017 06:35 PM\",\"value\": TEMPTOPARSE},{\"variableName\": \"Pressure\",\"timeStamp\": \"10.22.2017 06:35 PM\",\"value\": PRESSTOPARSE},{\"variableName\": \"Humidity\",\"timeStamp\": \"10.22.2017 06:35 PM\",\"value\": HUMTOPARSE}]}";

String sampleTemp = "12.02";
String samplePress = "1002.231";
String sampleHum = "52.02";


void parseJSONData(); //Prototype 

void setup() {

  //Communicate with ESP8266 and establish Wi-Fi connection.
  Serial.begin(115200);
  mbed.begin(115200); //Communicate with mbed to recieve data packets.

  
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Use WiFiClient class to create TLS connection
  WiFiClient client;
  Serial.print("connecting to ");
  Serial.println(serverUrl);
  if (!client.connect(serverUrl, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  Serial.print("requesting endpoint: ");
  Serial.println(serverEndpoint);


  parseJSONData();
  client.print("POST " + serverEndpoint + " HTTP/1.1\r\n" +

        "Host: " + serverUrl + "\r\n" +

        "Accept: *" + "/" + "*\r\n" +

        "Content-Length: " + data.length() + "\r\n" +

        "Content-Type: application/json\r\n" +

        "\r\n" + data);

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  
  Serial.println("Response from the server was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");

  Serial.println("Receiving data from mbed...");
  
}

void loop() {
  
  if(mbed.available()){
    Serial.println("Received data packet from mbed:");
    data = mbed.readString();
    Serial.println(data);
    
    
  }
}

void parseJSONData(){
    data.replace("TEMPTOPARSE", sampleTemp);
    data.replace("PRESSTOPARSE", samplePress);
    data.replace("HUMTOPARSE", sampleHum);
}
