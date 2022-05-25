#include "PPMHandler.h"
#include "Arduino.h"

PPMHandler* PPMHandler::instance;

void PPMHandler::setup()
{
	for (int i = 0; i < NUMBER_OF_CHANNELS; i++) {
		this->values[i] = DEFAULT_CHANNEL_VALUE + correction;
	}

	pinMode(PPM_PIN, OUTPUT);
	digitalWrite(PPM_PIN, !ON_STATE);  //set the PPM signal pin to the default state (off)

	cli();
	TCCR1A = 0; // set entire TCCR1 register to 0
	TCCR1B = 0;

	OCR1A = 100;  // compare match register, change this
	TCCR1B |= (1 << WGM12);  // turn on CTC mode
	TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
	TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
	sei();
}

PPMHandler::PPMHandler() {
  
}

PPMHandler* PPMHandler::getInstance() {
	if(instance == NULL) {
		instance = new PPMHandler();
	}
	return instance;
}

void PPMHandler::setPitch(int value)
{
	this->values[0] = value;
}

void PPMHandler::setRoll(int value)
{
	this->values[1] = value;
}

void PPMHandler::setThrottle(int value)
{
	this->values[2] = value;
}

void PPMHandler::setYaw(int value)
{
	this->values[3] = value;
}

void PPMHandler::setAux1(int value)
{
	this->values[4] = value;
}

void PPMHandler::setAux2(int value)
{
	this->values[5] = value;
}

void PPMHandler::setAux3(int value)
{
	this->values[6] = value;
}

void PPMHandler::setAux4(int value)
{
	this->values[7] = value;
}

void PPMHandler::setAux5(int value)
{
	this->values[8] = value;
}

void PPMHandler::setAux6(int value)
{
	this->values[9] = value;
}

void PPMHandler::setAux7(int value)
{
	this->values[10] = value;
}

void PPMHandler::setAux8(int value)
{
	this->values[11] = value;
}

void PPMHandler::printValues() {
	Serial.println("pitch: " + String(this->values[0]) +  + "|" +
		"roll: " + String(this->values[1]) + "|" +
		"throttle: " + String(this->values[2]) + "|"
		"yaw: " + String(this->values[3])
	);
}

void PPMHandler::sendData() {
    static boolean state = true;

    TCNT1 = 0;

    if (state) {  //start pulse
        digitalWrite(PPM_PIN, ON_STATE);
        OCR1A = PULSE_LENGTH * 2;
        state = false;
    }
    else {  //end pulse and calculate when to start the next pulse
        static byte cur_chan_numb;
        static unsigned int calc_rest;

        digitalWrite(PPM_PIN, !ON_STATE);
        state = true;

        if (cur_chan_numb >= NUMBER_OF_CHANNELS) {
            cur_chan_numb = 0;
            calc_rest = calc_rest + PULSE_LENGTH;// 
            OCR1A = (FRAME_LENGTH - calc_rest) * 2;
            calc_rest = 0;
        }
        else {
            OCR1A = (this->values[cur_chan_numb] - PULSE_LENGTH) * 2;
            calc_rest = calc_rest + this->values[cur_chan_numb];
            cur_chan_numb++;
        }
    }
}
