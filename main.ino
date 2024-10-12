#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>


const char* ssid = "Genie";
const char* password = "password";


// WiFiServer server(80);

// String header;

unsigned long lastTime = 0;
unsigned long timerDelay = 1000;
int alarmHour = 0;
int alarmMinute = 0;

WiFiServer server(80);

String header;


const int output33 = 33;

unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;

void setup() {
  pinMode(output33,OUTPUT);
  Serial.begin(115200); // Starts the serial communication

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  digitalWrite(output33,HIGH);
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  if(WiFi.status()== WL_CONNECTED ){
    HTTPClient http;

    http.begin("http://62.169.17.47:1029/ip");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");


    String httpRequestData =  "ip=" + WiFi.localIP().toString(); 
    Serial.print(httpRequestData);    


    int httpResponseCode = http.POST(httpRequestData);
    
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      // }
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
}

void loop() {
 WiFiClient client = server.available(); 
  // digitalWrite(output33,HIGH);
  if (client) {                            
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client."); 
    String currentLine = "";                
    while (client.connected() && currentTime - previousTime <= timeoutTime) { 
      currentTime = millis();
      if (client.available()) {            
        char c = client.read();
        // client.
        Serial.write(c); 
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            if(header.indexOf("GET /ip") >= 0){
              client.println(WiFi.localIP());
            }
            if (header.indexOf("POST /alarm") >= 0) {
              int indexOfHour = header.indexOf("alarmhour");
              int indexofMin = header.indexOf("alarmmin");
              if(indexOfHour >= 0 && indexofMin >= 0){
                  Serial.println(header);
                  String inString = ""; 
                  for (int i = indexOfHour+10; i <= (indexOfHour+12); i++) {
                      inString+=header[i];
                      // Serial.print(header[i]);
                  }
                  alarmHour = inString.toInt();
                  inString = "";
                   for (int i = indexofMin+9; i <= (indexofMin+11); i++) {
                      inString+=header[i];
                      // console.log(inString);

                  }
                  alarmMinute = inString.toInt();
                  Serial.print("Alarm Hour is: ");
                  Serial.println(alarmHour);
                  Serial.print("Alarm minute is: ");
                  Serial.println(alarmMinute);
                  inString = "";
                  client.println("{\"Success\": 1}");
              }
              else{
                client.println("Bad parameters");
              }
              

              delay(2000);

            } 
      
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;    
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }

if ((millis() - lastTime) > timerDelay) {
  if(WiFi.status()== WL_CONNECTED ){
    HTTPClient http;

    http.begin("https://timeapi.io/api/time/current/coordinate?latitude=23.661506&longitude=79.926214");
    

    int httpResponseCode = http.GET();
    
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      // Serial.println(payload);
      JSONVar myObject = JSON.parse(payload);
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
      // Serial.print("JSON object = ");
      int currentHour = myObject["hour"];
      int currentMin = myObject["minute"];
      // Serial.println(myHour);
      Serial.print("currentHour");
      Serial.println(currentHour >= alarmHour && currentMin >= alarmMinute && alarmHour != 0);
      if(currentHour >= alarmHour && currentMin >= alarmMinute && alarmHour != 0){
        Serial.println("in here");
        digitalWrite(output33,LOW);

      }
      JSONVar keys = myObject.keys();
    
      // for (int i = 0; i < keys.length(); i++) {
      //   JSONVar value = myObject[keys[i]];
      //   Serial.print(keys[i]);
      //   Serial.print(" = ");
      //   Serial.println(value);
      // }
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    lastTime = millis();
    http.end();
  }
}

}

