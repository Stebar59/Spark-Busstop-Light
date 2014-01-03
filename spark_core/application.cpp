// configure the server and request
char server[] = "www.yourserver.com";
char request[] = "GET /webservice.php?from=Bern&to=Bern,Inselspital HTTP/1.0";
// HTTP Version 1.0 is used to avoid chunked transfer encoding

#define DEBUG false

/* Function prototypes -------------------------------------------------------*/
void evalResponse(String& response);
void animateGreen();

// Configure the interval and brightness
#define CHECK_INTERVAL_MILLIS 30000
#define LED_MAX_VALUE 255 // dim the lights

TCPClient client;

/* This function is called once at start up ----------------------------------*/
void setup() {
    Serial.begin(9600);

    RGB.control(true);
    RGB.color(0,0,10);   // dimmed blue
}

char buffer [400];            // string to hold http response
unsigned int nextTime = 0;    // next time to contact the server



/* Color led animation --------------------------------------------*/
unsigned int red = 0;
unsigned int green = 0;
unsigned int blue = 0;

unsigned int incr_red = 0;
unsigned int incr_green = 0;
unsigned int incr_blue = 0;

void setColor(int r, int g, int b) {
    red = r;
    green = g;
    blue = b;
}

/** defines how much and in which direction the color changes per loop */
void setAnimation(int r, int g, int b) {
    incr_red = r;
    incr_green = g;
    incr_blue = b;
}


/* This function loops forever --------------------------------------------*/
void loop() {
    animateColors();
    RGB.color(red,green,blue);

    if ( nextTime > millis() ) {
        return;
    }

    if(DEBUG) Serial.println("connecting to server...");
    if (client.connect(server, 80))  {
        if(DEBUG) Serial.println("connected");
        
        client.println(request);
        client.print("HOST: ");
        client.println(server);
        client.println("Connection: close");

        client.println();
        client.flush();
    } else  {
        Serial.println("connection failed");
        client.stop();
        
        // retry in 5 sec
        nextTime = millis() + 5*1000;
        return;
    }

    //wait for request to be processed.
    delay(1000);

    if(DEBUG) Serial.println("reading response....");
    for(unsigned int i=0; i < sizeof(buffer) && client.available(); i++) {
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

    // this (--) should make the token unique, so it does not match anything in the
    // http response header

    if ( response.indexOf("(--)off")!=-1) {
        Serial.println("OFF");
        setColor(0, 0, 0);
        setAnimation(0, 0, 0);
    } else if ( response.indexOf("(--)red") != -1) {
        Serial.println("RED");
        setColor(LED_MAX_VALUE, 0, 0);
        setAnimation(0, 0, 0);
    } else if ( response.indexOf("(--)orange")!=-1) {
        Serial.println("ORANGE");
        setColor(LED_MAX_VALUE, 50, 0);
        setAnimation(0, 0, 0);
    } else if ( response.indexOf("(--)green_slow")!= -1) {
        Serial.println("GREEN slow");
        setColor(0, LED_MAX_VALUE, 0);
        setAnimation(0, 0, 0);
    }else if ( response.indexOf("(--)green_fast")!= -1) {
        Serial.println("GREEN fast");
        setColor(0, LED_MAX_VALUE, 0);
        setAnimation(0, 4, 0);
    } else {
        Serial.println("unknown command: " + response);
        setColor(0, 0, LED_MAX_VALUE);
        setAnimation(0, 0, 0);
    }
}



void animateColors() {
    green += incr_green;
    red += incr_red;
    blue += incr_blue;

    if ( incr_red != 0) {
        // swap direction on max/min values
        if(red > 250) {
            incr_red = -incr_red;
            red = 250;
        } else if (red < 50) {
            incr_red = -incr_red;
            red = 50;
        }
    }

    if ( incr_green != 0) {
        // swap direction on max/min values
        if(green > 250) {
            incr_green = -incr_green;
            green = 250;
        } else if (green < 50) {
            incr_green = -incr_green;
            green = 50;
        }
    }

    if ( incr_blue != 0) {
            // swap direction on max/min values
            if(blue > 250) {
                incr_blue = -incr_blue;
                blue = 250;
            } else if (blue < 50) {
                incr_blue = -incr_blue;
                blue = 50;
            }
        }
}
