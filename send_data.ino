#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>

const char* ssid = "NotACiscoRouter";
const char* password = "y2z?zng?r2hn";

const char* serverUrl = "192.168.1.203"; //Raspberry Pi Server IP.
String serverEndpoint = "/sendsensorvalue"; // rest API used to send values to server.
const int httpPort = 5000;

SoftwareSerial mbed(D7, D8);  //rx, tx for reciving information from mbed.

                //JSON-formatted template to send to server.
                //String data = "{\"Hello\":\"Hi2\"}";
String dataToSend;

String sampleTemp = "12.02";
String samplePress = "12.02";
String sampleHum = "12.02";
String timeStamp = "12.02";
String mbedData;


String parseJSONData(String json); //Prototype 
void extractData();
void sendPOSTData();

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

  // Grab data from mbed and parse as needed.
  /*Serial.println("Receiving data from mbed...");
  while (mbed.available()) {
    mbedData = mbed.readString();
  }
  mbed.print('y'); //send a confirmation 'y' char to mbed to tell it the packet was recieved in full.

  Serial.println(mbedData);

  extractData();*/
  


  

}

void loop() {
  
  if (mbed.available()) {
    Serial.println("Received data packet from mbed:");
    mbedData = mbed.readString();
    Serial.println(mbedData);

    extractData();
    dataToSend = "{\"data\":[{\"variableName\":\"Temperature\",\"timeStamp\": \"TIMESTAMP\",\"value\": TEMPTOPARSE},{\"variableName\": \"Pressure\",\"timeStamp\": \"TIMESTAMP\",\"value\": PRESSTOPARSE},{\"variableName\": \"Humidity\",\"timeStamp\": \"TIMESTAMP\",\"value\": HUMTOPARSE}]}";
    dataToSend = parseJSONData(dataToSend);
    Serial.println(dataToSend);
    //dataToSend = parseJSONData(dataToSend);
    //Serial.println(dataToSend);
    
    
    sendPOSTData();
    //delay(5000);
    mbed.print('y'); // Send confirmation 'y' char to mbed to tell it the packet was recieved in full, parsed and sent to the server.
  }

  //delay(100); //Prevent extraneous results.
}

//Parses values into the JSON template packet.
String parseJSONData(String json) {
  json.replace("TEMPTOPARSE", sampleTemp);
  json.replace("PRESSTOPARSE", samplePress);
  json.replace("HUMTOPARSE", sampleHum);
  json.replace("TIMESTAMP", timeStamp);

  return json;

}

//Extracts sensor and time data from the UART Packet sent from mbed.
void extractData() {
  
  //Grabs portions of the string based on predetermined delimeters on the packet.
  sampleTemp = mbedData.substring(mbedData.indexOf("!") + 1, mbedData.indexOf("degC"));
  samplePress = mbedData.substring(mbedData.indexOf("@") + 1, mbedData.indexOf("hPa"));
  sampleHum = mbedData.substring(mbedData.indexOf("#") + 1, mbedData.indexOf("%"));
  timeStamp = mbedData.substring(mbedData.indexOf("$") + 1, mbedData.length() - 1);
}

//Send the data to the server.
void sendPOSTData() {
  // Use WiFiClient class to create TLS connection
  WiFiClient client;
  Serial.print("connecting to ");
  Serial.println(serverUrl);
  if (!client.connect(serverUrl, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // Sending POST Request to Raspberry Pi Server.
  Serial.print("requesting endpoint: ");
  Serial.println(serverEndpoint);
  
  client.print("POST " + serverEndpoint + " HTTP/1.1\r\n" +

    "Host: " + serverUrl + "\r\n" +

    "Accept: *" + "/" + "*\r\n" +

    "Content-Length: " + dataToSend.length() + "\r\n" +

    "Content-Type: application/json\r\n" +

    "\r\n" + dataToSend);

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
  
}
