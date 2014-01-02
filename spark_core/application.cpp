
/* Includes ------------------------------------------------------------------*/  
#include "application.h"

/* Function prototypes -------------------------------------------------------*/
void evalResponse(String& response);

// Configure the interval and brightness
#define CHECK_INTERVAL_MILLIS 30000
#define LED_MAX_VALUE 150 // dim the lights

// configure the server and request
char server[] = "example.ch";
char request[] = "GET /ampel/?from=Bern&to=Bern,Inselspital HTTP/1.1";

#define DEBUG 0
TCPClient client;

/* This function is called once at start up ----------------------------------*/
void setup() {
 Serial.begin(9600);

  RGB.control(true);
  // dimmed blue
  RGB.color(20, 10, 150);
}


char buffer [400];            // string to hold the text from server
unsigned int nextTime = 0;

/* This function loops forever --------------------------------------------*/
void loop() {
	  if ( nextTime > millis() ) {
	  	return;
	  }
	  
      if(DEBUG) Serial.println("connecting to server...");
      if (client.connect(server, 80))  {
        if(DEBUG) Serial.println("connected");
        
        client.println(request);
        client.println("HOST: synox.ch");
        client.print("Connection: close\n");

        client.println();
        client.flush();
      } else  {
        Serial.println("connection failed");
        client.stop();
        
        nextTime = millis() + 5*1000;
        return;
      }

    //wait for request to be processed.
    delay(1000);        

    if(DEBUG) Serial.println("reading response....");
    for(int i =0; i < sizeof(buffer) && client.available(); i++) {
        char c = client.read();
        if(c == -1) {
            break;
        }
        buffer[i] = c;
    }
    client.stop();
    
    String response(buffer);
    
		evalResponse(response);
	
    
   nextTime = millis() + CHECK_INTERVAL_MILLIS;
}


void evalResponse(String& response) {
		if(DEBUG)  Serial.println("parsing response");
			
    if ( response.indexOf("(--)off")!=-1) {
        Serial.println("OFF");
        RGB.color(0, 0, 5);
    } else if ( response.indexOf("(--)red") != -1) {
        Serial.println("RED");
        RGB.color(LED_MAX_VALUE, 0, 0);
    } else if ( response.indexOf("(--)orange")!=-1) {
        Serial.println("ORANGE");
        RGB.color(LED_MAX_VALUE, LED_MAX_VALUE/2, 0);
    }else if ( response.indexOf("(--)green")!= -1) {
        Serial.println("GREEN");
        RGB.color(0, LED_MAX_VALUE, 0);
    } else {
        Serial.println("unknown command: " + response);
         RGB.color(0, 0, LED_MAX_VALUE);
    }
}