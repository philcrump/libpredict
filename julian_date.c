#include "predict.h"

#include <stdio.h>
#include <time.h>

#include "defs.h"

#define SECONDS_IN_HOUR 3600.0
#define SECONDS_IN_DAY 86400.0
#define UNIX_EPOCH_IN_JULIAN 2440587.5

/**
 * Create time_t in UTC from struct tm.
 *
 * \param timeinfo_utc Broken down time, assumed to be in UTC
 * \return Time in UTC
 **/
time_t mktime_utc(const struct tm* timeinfo_utc)
{
	time_t curr_time = time(NULL);
	int timezone_diff = 0; //deviation of the current timezone from UTC in seconds

	//get UTC time, interpret resulting tm as a localtime
	struct tm timeinfo_gmt;
	gmtime_r(&curr_time, &timeinfo_gmt);
	time_t time_gmt = mktime(&timeinfo_gmt);

	//get localtime, interpret resulting tm as localtime
	struct tm timeinfo_local;
	localtime_r(&curr_time, &timeinfo_local);
	time_t time_local = mktime(&timeinfo_local);

	//find the time difference between the two interpretations
	timezone_diff += difftime(time_local, time_gmt);

	//hack for preventing mktime from assuming localtime: add timezone difference to the input struct.
	struct tm ret_timeinfo;
	ret_timeinfo.tm_sec = timeinfo_utc->tm_sec + timezone_diff;
	ret_timeinfo.tm_min = timeinfo_utc->tm_min;
	ret_timeinfo.tm_hour = timeinfo_utc->tm_hour;
	ret_timeinfo.tm_mday = timeinfo_utc->tm_mday;
	ret_timeinfo.tm_mon = timeinfo_utc->tm_mon;
	ret_timeinfo.tm_year = timeinfo_utc->tm_year;
	ret_timeinfo.tm_isdst = timeinfo_utc->tm_isdst;
	return mktime(&ret_timeinfo);
}

/**
 * Helper function for getting the Julian day start date (1979-12-31 00:00 UTC) as time_t.
 *
 * \return Internally defined Julian start date (fixed)
 **/
time_t get_julian_start_day()
{
	struct tm start_time;
	start_time.tm_sec = 0;
	start_time.tm_min = 0;
	start_time.tm_hour = 0;
	start_time.tm_mday = 31;
	start_time.tm_mon = 11;
	start_time.tm_year = 1979-1900;
	start_time.tm_isdst = 0;
	return mktime_utc(&start_time);
}

predict_julian_date_t julian_from_timestamp(uint64_t timestamp)
{
   return ( (double)timestamp / SECONDS_IN_DAY ) + UNIX_EPOCH_IN_JULIAN;
}

predict_julian_date_t julian_from_timestamp_ms(uint64_t timestamp_ms)
{
   return ( (double)timestamp_ms / (1000 * SECONDS_IN_DAY) ) + UNIX_EPOCH_IN_JULIAN;
}

uint64_t timestamp_from_julian(predict_julian_date_t date)
{
	if(date <= UNIX_EPOCH_IN_JULIAN)
	{
		/* Julian timestamp is on / before unix epoch */
		return 0;
	}

	return (uint64_t)((date - UNIX_EPOCH_IN_JULIAN) * SECONDS_IN_DAY);
}

uint64_t timestamp_ms_from_julian(predict_julian_date_t date)
{
	if(date <= UNIX_EPOCH_IN_JULIAN)
	{
		/* Julian timestamp is on / before unix epoch */
		return 0;
	}

	return (uint64_t)((date - UNIX_EPOCH_IN_JULIAN) * (1000 * SECONDS_IN_DAY));
}