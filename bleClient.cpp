//----------------------------------------------------
// bleClient.cpp
//----------------------------------------------------

#include "bleClient.h"

#define DEBUG_BLE   1


#define MAX_HM10_CHARS  18

#define NUM_BUTTONS     15

#define DEFAULT_SPRING      1
#define DEFAULT_STEP        4
#define DEFAULT_DEADZONE    20

const char *ble_name = "";
const char *ble_button_name[NUM_BUTTONS];

int   ble_joy_spring = DEFAULT_SPRING;
int   ble_joy_step   = DEFAULT_STEP;
int   ble_joy_deadzone = DEFAULT_DEADZONE;

int  ble_len = 0;
bool ble_connected = false;
bool ble_user_chars = false;
char ble_buf[MAX_HM10_CHARS + 1];
char ble_user_buf[MAX_HM10_CHARS + 1];


bool ble_joy_changed = 0;
int ble_joy_x = 0;
int ble_joy_y = 0;


bool isDefaultButton(int i)
{
    char buf[2];
    buf[0] = 'A' + i;
    buf[1] = 0;
    return !strcmp(buf,ble_button_name[i]);
}


void checkJoy()
{
    // if (ble_user_buf[0] >= '0' &&
    //    ble_user_buf[0] <= '9')
    {
        int joy_x = 0;
        int joy_y = 0;

        char *p = ble_user_buf;
        while (*p && *p != ',') p++;
        if (*p == ',')
        {
            *p++ = 0;
            // sticking with tumbler definition y then x
            joy_y = atoi(ble_user_buf);
            joy_x = atoi(p);
            if (joy_x != ble_joy_x ||
                joy_y != ble_joy_y)
            {
                ble_joy_changed = 1;
                ble_joy_x = joy_x;
                ble_joy_y = joy_y;
            }
        }
    }
}


//----------------------------------------------------
// class methods
//----------------------------------------------------



bleClient::bleClient(int TXD, int RXD) :
    SoftwareSerial(TXD,RXD)
        // SoftwareSerial is defined as rx,tx pins, so the HM10
        // TXD comes first and goes to the arduino rx, and the HM10
        // RXD pin is connected to the SoftwareSerial tx pin.
{
    ble_len = 0;
    ble_connected = false;
    ble_user_chars = false;

    ble_button_name[0] = "A";
    ble_button_name[1] = "B";
    ble_button_name[2] = "C";
    ble_button_name[3] = "D";
    ble_button_name[4] = "E";
    ble_button_name[5] = "F";
    ble_button_name[6] = "G";
    ble_button_name[7] = "H";
    ble_button_name[8] = "I";
    ble_button_name[9] = "J";
    ble_button_name[10] = "K";
    ble_button_name[11] = "L";
    ble_button_name[12] = "M";
    ble_button_name[13] = "N";
    ble_button_name[14] = "O";

}


void bleClient::setName(const char *name)
{
    ble_name = name;
}
void bleClient::setButton(char value, const char *name)
{
    if (strlen(name) > 5)
    {
        if (Serial)
        {
            Serial.print("ERROR name must be 5 chars or less in setButton(");
            Serial.print(name);
            Serial.println(")");
        }
        return;
    }

    int num = value - 'A';
    if (num >= 0 && num < NUM_BUTTONS)
    {
        #if DEBUG_BLE
            if (Serial)
            {
                Serial.print("setButton(");
                Serial.print(value);
                Serial.print(",");
                Serial.print(name);
                Serial.println(")");
            }
        #endif

        ble_button_name[num] = name;
    }
}
void bleClient::setJoystick(bool spring, int step, int deadzone)
{
    #if DEBUG_BLE
        if (Serial)
        {
            Serial.print("setJoystick(");
            Serial.print(spring);
            Serial.print(",");
            Serial.print(step);
            Serial.print(",");
            Serial.print(deadzone);
            Serial.println(")");
        }
    #endif

    ble_joy_spring = spring ? 1 : 0;
    ble_joy_step = step;
    ble_joy_deadzone = deadzone;
}



void bleClient::start()
{
    begin(9600);
    delay(500);
    #if DEBUG_BLE
        if (Serial)
            Serial.println("bleClient started");
    #endif

    if (ble_name[0])
    {
        #if DEBUG_BLE
            if (Serial)
            {
                Serial.print("bleClient nameX=");
                Serial.println(ble_name);
            }
        #endif

        print("AT+NAME");
        println(ble_name);
        delay(100);
        println("AT+RESET");
    }
}





char bleClient::getButton()
{
    if (available())
    {
        int c = read();

        #if 0
            if (Serial)
            {
                Serial.print("<--(");
                Serial.print(c);
                Serial.print(")");
                if (c > 32)
                {
                    Serial.print("=");
                    Serial.write(c);
                    Serial.println();
                }
            }
        #endif
        
        if (c == '<')
        {
            ble_user_chars = true;
        }
        else if (c == '>')
        {
            ble_len = 0;
            ble_user_chars = false;
            strcpy(ble_user_buf,ble_buf);
            if (ble_user_buf[0] >= 'A' &&
                ble_user_buf[0] <= ('A' + NUM_BUTTONS - 1))
            {
                return ble_user_buf[0];
            }
            else
            {
                checkJoy();
            }
        }
        else if (c > 0)
        {
            if (c != 10 && c != 13 && ble_len < MAX_HM10_CHARS)
            {
                ble_buf[ble_len++] = c;
                ble_buf[ble_len] = 0;
            }
            if (!ble_user_chars)
            {
                if (!strcmp(ble_buf,"OK+CONN"))
                {
                    ble_len = 0;
                    ble_connected = 1;
                    if (Serial)
                    {
                        Serial.print(ble_name[0] ? ble_name : "HM10");
                        Serial.println(" CONNECTED");
                    }


                    // ON CONNECT

                    delay(500);

                    sprintf(ble_buf,"joy:%d,%d,%d",
                        ble_joy_spring,
                        ble_joy_step,
                        ble_joy_deadzone);
                    println(ble_buf);

                    delay(100);
                    
                    for (int i=0; i<NUM_BUTTONS; i++)
                    {
                        if (!isDefaultButton(i))
                        {
                            char btn = 'A' + i;
                            sprintf(ble_buf,"button:%c,%s",
                                btn,ble_button_name[i]);
                            println(ble_buf);
                            delay(100);
                        }
                    }
                }
                else if (!strcmp(ble_buf,"OK+LOST"))
                {
                    ble_len = 0;
                    ble_connected = 0;
                    if (Serial)
                    {
                        Serial.print(ble_name[0] ? ble_name : "HM10");
                        Serial.println(" DISCONNECTED");
                    }
                }
                else if (c == 10)
                {
                    ble_len = 0;
                    #if DEBUG_BLE > 1
                        if (Serial)
                        {
                            Serial.println(ble_buf);
                        }
                    #endif
                }
            }
        }
    }
    return 0;
}



bool bleClient::joyChanged(int *joy_x, int *joy_y)
{
    if (ble_joy_changed)
    {
        ble_joy_changed = 0;
        *joy_x = ble_joy_x;
        *joy_y = ble_joy_y;
        return 1;
    }
    return 0;
}













