/*
 * DS3231.h
 *
 * Arduino Library for the DS3231 Real-Time Clock chip
 *
 * (c) Eric Ayars
 * 4/1/11
 * released into the public domain. If you use this, please let me know
 * (just out of pure curiosity!) by sending me an email:
 * eric@ayars.org
 *
 * Changed the parameter type in isleapYear() to uint16_t from uint8_t
 * for two reasons: the function uses 16-bit arithmetic, i.e., (y % 400); and
 * one of the calling functions sends a 16-bit parameter.
 * David Sparks
 * 08 Sept 2022
 *
 */


#ifndef DS3231_h
#define DS3231_h

#include <time.h>
#include "periphery.h"

// Registers address
static const uint8_t DS3231_SECONDS					= 0x00;
static const uint8_t DS3231_MINUTES					= 0x01;
static const uint8_t DS3231_HOURS						= 0x02;
static const uint8_t DS3231_DAY							= 0x03;
static const uint8_t DS3231_DATE						= 0x04;
static const uint8_t DS3231_MONTH_CENTURY		= 0x05;
static const uint8_t DS3231_YEAR						= 0x06;
static const uint8_t DS3231_ALARM_1_SECONDS	= 0x07;
static const uint8_t DS3231_ALARM_1_MINUTES	= 0x08;
static const uint8_t DS3231_ALARM_1_HOURS		= 0x09;
static const uint8_t DS3231_ALARM_1_DAY			= 0x0A;
static const uint8_t DS3231_ALARM_2_MINUTES	= 0x0B;
static const uint8_t DS3231_ALARM_2_HOURS		= 0x0C;
static const uint8_t DS3231_ALARM_2_DAY			= 0x0D;
static const uint8_t DS3231_CONTROL					= 0x0E;
static const uint8_t DS3231_CONTROL_STATUS	= 0x0F;
static const uint8_t DS3231_AGING_OFFSET		= 0x10;
static const uint8_t DS3231_TEMPERATURE_MSB = 0x11;
static const uint8_t DS3231_TEMPERATURE_LSM = 0x12;

// Masks for registers
static const uint8_t DS3231_SECONS_REG_MSK	= 0x7F;
static const uint8_t DS3231_MINUTES_REG_MSK	= 0x7F;
static const uint8_t DS3231_HOURS_12_24_MSK = 0x40;
static const uint8_t DS3231_HOURS_AM_PM_MSK = 0x20;
static const uint8_t DS3231_HOURS_PM_TIME		= 0x1F;
static const uint8_t DS3231_HOURS_AM_TIME		= 0x3F;
static const uint8_t DS3231_DAY_REG_MSK			= 0x7F;
static const uint8_t DS3231_DAY_MSK					= 0x0F;
static const uint8_t DS3231_DATE_MSK				= 0x3F;
static const uint8_t DS3231_CENTURY_MSK			= 0x80;
static const uint8_t DS3231_MONTH_MSK				= 0x7F;
static const uint8_t DS3231_ALARM_BIT_MSK		= 0x80;
static const uint8_t DS3231_OSCIL_BIT_MSK		= 0x80;

// Control byte masks
static const uint8_t A1IE_MSK			= 0x01;
static const uint8_t A2IE_MSK			= 0x02;
static const uint8_t INTCN_MSK		= 0x04;
static const uint8_t RS1_MSK			= 0x08;
static const uint8_t RS2_MSK			= 0x10;
static const uint8_t CONV_MSK			= 0x20;
static const uint8_t BBSQW_MSK		= 0x40;
static const uint8_t nEOSC_MSK		= 0x80;

// Control and Status byte masks
static const uint8_t A1F_MSK			= 0x01;
static const uint8_t A2F_MSK			= 0x02;
static const uint8_t BSY_MSK			= 0x04;
static const uint8_t EN32_KHZ_MSK = 0x08;
static const uint8_t OSF_MSK			= 0x80;



// DateTime (get everything at once) from JeeLabs / Adafruit
// Simple general-purpose date/time class (no TZ / DST / leap second handling!)
class DateTime {
public:
    DateTime (uint32_t t =0);
    DateTime (uint16_t year, uint8_t month, uint8_t day,
                uint8_t hour =0, uint8_t min =0, uint8_t sec =0);
    DateTime (const char* date, const char* time);
    uint16_t year() const       { return 2000 + yOff; }
    uint8_t month() const       { return m; }
    uint8_t day() const         { return d; }
    uint8_t hour() const        { return hh; }
    uint8_t minute() const      { return mm; }
    uint8_t second() const      { return ss; }
    uint8_t dayOfTheWeek() const;

    // 32-bit times as seconds since 1/1/2000
    long secondstime() const;
    // 32-bit times as seconds since 1/1/1970
    // THE ABOVE COMMENT IS CORRECT FOR LOCAL TIME; TO USE THIS COMMAND TO
    // OBTAIN TRUE UNIX TIME SINCE EPOCH, YOU MUST CALL THIS COMMAND AFTER
    // SETTING YOUR CLOCK TO UTC
    uint32_t unixtime(void) const;
protected:
    uint8_t yOff, m, d, hh, mm, ss;
};

// Checks if a year is a leap year
bool isleapYear(const uint16_t);

class RTClib {
  public:
//    static DateTime now(TwoWire & _Wire = Wire);				// Get date and time snapshot
};

// Eric's original code is everything below this line
class DS3231 {
	public:

		//Constructor
	//	DS3231();
		// Time-retrieval functions

		// the get*() functions retrieve current values of the registers.
		bool getSeconds(uint8_t &seconds);
		bool getMinutes(uint8_t &minutes);
		bool getHours(uint8_t &hours, bool& h12, bool& PM_time); 	// In addition to returning the hour register, this function
																															// returns the values of the 12/24-hour flag and the AM/PM flag.
		bool getDoW(uint8_t &day_of_week);
		bool getDate(uint8_t &date);
		bool getMonth(uint8_t &month, bool& century);	  					// Also sets the flag indicating century roll-over.
		bool getYear(uint8_t &year);															// Last 2 digits only

		// Time-setting functions
		// Note that none of these check for sensibility: You can set the
		// date to July 42nd and strange things will probably result.

		// set epoch function gives the epoch as parameter and feeds the RTC
		// epoch = UnixTime and starts at 01.01.1970 00:00:00
		void setEpoch(time_t epoch = 0, bool flag_localtime = false);

		bool setSecond(const uint8_t seconds); 							// In addition to setting the seconds, this clears the "Oscillator Stop Flag".
		bool setMinute(const uint8_t minutes);							// Sets the minute
		bool setHour(const uint8_t hours);									// Sets the hour
		bool setDoW(const uint8_t DoW);											// Sets the Day of the Week (1-7);
		bool setDate(const uint8_t date);										// Sets the Date of the Month
		bool setMonth(const uint8_t month);									// Sets the Month of the year
		bool setYear(const uint8_t year);										// Last two digits of the year
		bool setClockMode(bool h12);									// Set 12/24h mode. True is 12-h, false is 24-hour.

		bool getTemperature(float &temperature);			// Temperature function

		// Alarm functions

		bool getA1Time(uint8_t& A1Day, uint8_t& A1Hour, uint8_t& A1Minute, uint8_t& A1Second, uint8_t& AlarmBits, bool& A1Dy, bool& A1h12, bool& A1PM);
		/* Retrieves everything you could want to know about alarm
		 * one.
		 * A1Dy true makes the alarm go on A1Day = Day of Week,
		 * A1Dy false makes the alarm go on A1Day = Date of month.
		 *
		 * byte AlarmBits sets the behavior of the alarms:
		 *	Dy	A1M4	A1M3	A1M2	A1M1	Rate
		 *	X	1		1		1		1		Once per second
		 *	X	1		1		1		0		Alarm when seconds match
		 *	X	1		1		0		0		Alarm when min, sec match
		 *	X	1		0		0		0		Alarm when hour, min, sec match
		 *	0	0		0		0		0		Alarm when date, h, m, s match
		 *	1	0		0		0		0		Alarm when DoW, h, m, s match
		 *
		 *	Dy	A2M4	A2M3	A2M2	Rate
		 *	X	1		1		1		Once per minute (at seconds = 00)
		 *	X	1		1		0		Alarm when minutes match
		 *	X	1		0		0		Alarm when hours and minutes match
		 *	0	0		0		0		Alarm when date, hour, min match
		 *	1	0		0		0		Alarm when DoW, hour, min match
		 *
		 *	Note: byte AlarmBits is not explicitly cleared for the getAXTime methods to
		 *	support sequential retreival of both alarms with the same byte AlarmBits.
		 *	Use the flag bool clearAlarmBits=True to explicitly clear byte AlarmBits on
		 *  call to getAXTime.
		 */
		bool getA2Time(uint8_t& A2Day, uint8_t& A2Hour, uint8_t& A2Minute, uint8_t& AlarmBits, bool& A2Dy, bool& A2h12, bool& A2PM);
			// Same as getA1Time();, but A2 only goes on seconds == 00.
		void getA1Time(uint8_t& A1Day, uint8_t& A1Hour, uint8_t& A1Minute, uint8_t& A1Second, uint8_t& AlarmBits, bool& A1Dy, bool& A1h12, bool& A1PM, bool clearAlarmBits);
			// Same as getA1Time();, but clears byte AlarmBits.
		void getA2Time(uint8_t& A1Day, uint8_t& A1Hour, uint8_t& A1Minute,uint8_t& AlarmBits, bool& A1Dy, bool& A1h12, bool& A1PM, bool clearAlarmBits);
			// Same as getA1Time();, but clears byte AlarmBits.
		bool setA1Time(uint8_t A1Day, uint8_t A1Hour, uint8_t A1Minute, uint8_t A1Second, uint8_t AlarmBits, bool A1Dy, bool A1h12, bool A1PM);
			// Set the details for Alarm 1
		bool setA2Time(uint8_t A2Day, uint8_t A2Hour, uint8_t A2Minute, uint8_t AlarmBits, bool A2Dy, bool A2h12, bool A2PM);
			// Set the details for Alarm 2
		bool turnOnAlarm(const uint8_t alarm_number);
			// Enables alarm 1 or 2 and the external interrupt pin.
			// If Alarm != 1, it assumes Alarm == 2.
		bool turnOffAlarm(const uint8_t alarm_number);
			// Disables alarm 1 or 2 (default is 2 if Alarm != 1);
			// and leaves the interrupt pin alone.
		bool checkAlarmEnabled(bool &alarm_en, uint8_t alarm_number);
			// Returns T/F to indicate whether the requested alarm is
			// enabled. Defaults to 2 if Alarm != 1.
		bool checkIfAlarm(bool &alarm_happen, uint8_t alarm_number);
			// Checks whether the indicated alarm (1 or 2, 2 default);
			// has been activated. Always clears flag.
		bool checkIfAlarm(bool &alarm_happen, uint8_t alarm_number, bool clearflag);
			// Checks whether the indicated alarm (1 or 2, 2 default);
			// has been activated. IF clearflag is set, clears alarm flag.


		// Oscillator functions
		bool enableOscillator(bool TF, bool battery, uint8_t frequency);
			// turns oscillator on or off. True is on, false is off.
			// if battery is true, turns on even for battery-only operation,
			// otherwise turns off if Vcc is off.
			// frequency must be 0, 1, 2, or 3.
			// 0 = 1 Hz
			// 1 = 1.024 kHz
			// 2 = 4.096 kHz
			// 3 = 8.192 kHz (Default if frequency byte is out of range);
		bool enable32kHz(bool TF);
			// Turns the 32kHz output pin on (true); or off (false).
		bool oscillatorCheck(bool &result);
			// Checks the status of the OSF (Oscillator Stop Flag);.
			// If this returns false, then the clock is probably not
			// giving you the correct time.
			// The OSF is cleared by function setSecond();.

	private:

		uint8_t decToBcd(uint8_t val); 			// Convert normal decimal numbers to binary coded decimal
		uint8_t bcdToDec(uint8_t val);			// Convert binary coded decimal to normal decimal numbers

	protected:
		bool readControlByte(uint8_t &control_byte, bool which); 			// Read selected control byte: (0); reads 0x0e, (1) reads 0x0f
		bool writeControlByte(uint8_t control_byte, bool which);			// Write the selected control byte. Which == false -> 0x0e, true->0x0f.

};

extern DS3231 RTC_DS3231;

#endif
