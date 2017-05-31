#include <stdint.h>

#include "predict.h"

#define SECONDS_IN_HOUR 3600.0
#define SECONDS_IN_DAY 86400.0
#define UNIX_EPOCH_IN_JULIAN 2440587.5

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
