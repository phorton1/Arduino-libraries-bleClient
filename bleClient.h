//----------------------------------------------------
// bleClient.h
//----------------------------------------------------
// Interface to Android bleController application
// using an HM10 ble module on an Arduino.
// The HM-10 is currently known to work with arduino
// pins 2 and 3.
//
//
// The buttons are:
//
//   A              I
//   B              J
//   C              K
//   D  E  F  G  H  L  M  N  O

#include <Arduino.h>
#include <SoftwareSerial.h>


class bleClient : public SoftwareSerial
{
public:
    
    bleClient(int TXD, int RXD);
		// The arduino pins that are connected to the
		// HM10 TXD and RXD pins

	// call these before start();

    void setName(const char *name);
		// Call this from setup() to set the name of your HM-10
    void setButton(char value, const char *name);
		// Call this from setup() to set the name of a button (A through O)
    void setJoystick(bool spring=true, int step=4, int deadzone=20);
		// You can set the joystick to be "springy", have "steps"
		// so that it doesn't change as often, and have a deadzone
		// near zero so that it doesnt change if you just touch it.

	void start();
		// Call this from setup() to start the HM10.

    char getButton();
		// MUST BE CALLED FROM LOOP
		// Called as the main "process" method, will see if any
		// characters are pending from the HM10 and will return
		// 0 if no button was pressed, or "A" through "O" if a
		// button was pressed.

	bool joyChanged(int *joy_x, int *joy_y);
		// if getButton() returns 0, you may call joyChanged()
		// to see if the joystick position has changed.
		// If it returns true, then the values are valid,
		// otherwise, they are not touched.

};




