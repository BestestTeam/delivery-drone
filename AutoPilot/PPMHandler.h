#pragma once


#define NUMBER_OF_CHANNELS 12
#define	DEFAULT_CHANNEL_VALUE 1500
#define FRAME_LENGTH 22500
#define PULSE_LENGTH 300
#define ON_STATE 1
#define PPM_PIN 11

const int correction = -10;

class PPMHandler
{
	private:
		int values[12] = {0};
		static PPMHandler* instance;
		PPMHandler();
	public:
		static PPMHandler* getInstance();

		void printValues();
		/// <summary>
		/// Configure PPM to send data
		/// </summary>
		void setup();

		/// <summary>
		/// Sets pitch channel value
		/// </summary>
		/// <param name="value"></param>
		void setPitch(int value);

		/// <summary>
		/// Sets roll channel value
		/// </summary>
		/// <param name="value"></param>
		void setRoll(int value);

		/// <summary>
		/// Sets throttle channel value
		/// </summary>
		/// <param name="value"></param>
		void setThrottle(int value);

		/// <summary>
		/// Sets Yaw channel value
		/// </summary>
		/// <param name="value"></param>
		void setYaw(int value);

		/// <summary>
		/// Sets Aux1 channel value
		/// </summary>
		/// <param name="value"></param>
		void setAux1(int value);

		/// <summary>
		/// Sets Aux2 channel value
		/// </summary>
		/// <param name="value"></param>
		void setAux2(int value);

		/// <summary>
		/// Sets Aux3 channel value
		/// </summary>
		/// <param name="value"></param>
		void setAux3(int value);

		/// <summary>
		/// Sets Aux4 channel value
		/// </summary>
		/// <param name="value"></param>
		void setAux4(int value);

		/// <summary>
		/// Sets Aux5 channel value
		/// </summary>
		/// <param name="value"></param>
		void setAux5(int value);

		/// <summary>
		/// Sets Aux6 channel value
		/// </summary>
		/// <param name="value"></param>
		void setAux6(int value);

		/// <summary>
		/// Sets Aux7 channel value
		/// </summary>
		/// <param name="value"></param>
		void setAux7(int value);

		/// <summary>
		/// Sets Aux8 channel value
		/// </summary>
		/// <param name="value"></param>
		void setAux8(int value);

		/// <summary>
		/// Sends the values from values array through
		/// PPM to the Flight Controller
		/// </summary>
		void sendData();
};
