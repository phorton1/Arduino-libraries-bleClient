// This is a test program to use the Arduino bleClient library
// and the bleController Android app.

#include <bleClient.h>

#define ONBOARD_LED     13

bleClient ble(2,3);
    // This says that the Arduino pin 2 is connected to the HM-10 TXD pin
    // and pin 3 is connected to the TXD pin.

bool led_on = 0;


void setup()
{
    // This stuff is typical stuff you would have
    // in your setup() method

    Serial.begin(115200);
    pinMode(ONBOARD_LED,OUTPUT);
    for (int i=0; i<21; i++)
    {
        digitalWrite(ONBOARD_LED,i & 1);
        delay(30);
    }

    // Here we setup the bleController app as we like it ..

    ble.setName("Robo3");
    ble.setButton('A',"test");
    ble.setButton('B',"");
    ble.setButton('C',"LED");
    ble.setJoystick(1,0,0);

    // The next line is the only line that is really necessary
    // to use the bleController

    ble.start();
}




void loop()
{
    int joy_x, joy_y;
    char c = ble.getButton();

    if (c)
    {
        Serial.print("GOT BUTTON ");
        Serial.println(c);

        if (c == 'C')
        {
            Serial.println("toggling led");
            led_on = !led_on;
            digitalWrite(ONBOARD_LED,led_on);
        }
        else if (c == 'D')
        {
            ble.println("<D pressed>");
        }
        else if (c == 'E')
        {
            ble.println("<E pressed>");
        }
        else if (c == 'F')
        {
            ble.println("F pressed");
        }
        else if (c == 'G')
        {
            ble.println("G pressed");
        }
        
    }
    else if (ble.joyChanged(&joy_x,&joy_y))
    {
        Serial.print("JOY X(");
        Serial.print(joy_x);
        Serial.print(") Y(");
        Serial.print(joy_y);
        Serial.println(")");
    }
}
