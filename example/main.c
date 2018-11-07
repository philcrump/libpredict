#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "../predict.h"

uint64_t timestamp_ms(void)
{
  struct timespec spec;
  clock_gettime(CLOCK_REALTIME, &spec);
  return ((uint64_t) spec.tv_sec) * 1000 + (((uint64_t) spec.tv_nsec) / 1000000);
}

int main(void)
{
    predict_orbital_elements_t iss_tle;
    struct predict_position iss_orbit;
    predict_observer_t obs;

    struct predict_observation iss_observation;
    struct predict_observation sun_observation;
    struct predict_observation moon_observation;

    /* Up to date as of 21st Jan 2016, get new version from: https://hamtv.batc.tv/tle/ */
    char tle_lines[2][150];
	char *tle[2];
	
	tle[0] = &tle_lines[0][0];
	tle[1] = &tle_lines[1][0];

    strcpy(tle_lines[0], "1 25544U 98067A   18311.69881946  .00003236  00000-0  56524-4 0  9995");
    strcpy(tle_lines[1], "2 25544  51.6417  23.5568 0004767  22.5396  79.5368 15.53922927140835");

    predict_julian_date_t curr_time = julian_from_timestamp_ms(timestamp_ms());

    if(!predict_parse_tle(&iss_tle, tle[0], tle[1]))
    {
        printf("Error parsing TLE!\n");
        exit(1);
    }

    if(predict_orbit(&iss_tle, &iss_orbit, curr_time) < 0)
    {
        printf("Failed to initialize orbit from tle!\n");
        exit(1);
    }

    predict_create_observer(&obs, "Self", 50.9*M_PI/180.0, -1.39*M_PI/180.0, 0);

    predict_observe_orbit(&obs, &iss_orbit, &iss_observation);
    printf("Current ISS Position: AZ: %03.1f, EL: %03.1f\n",
      (iss_observation.azimuth*180.0/M_PI),
      (iss_observation.elevation*180.0/M_PI)
    );

    predict_observe_sun(&obs, curr_time, &sun_observation);
    printf("Current Sun Position: AZ: %03.1f, EL: %03.1f\n",
      (sun_observation.azimuth*180.0/M_PI),
      (sun_observation.elevation*180.0/M_PI)
    );

    predict_observe_moon(&obs, curr_time, &moon_observation);
    printf("Current Moon Position: AZ: %03.1f, EL: %03.1f\n",
      (moon_observation.azimuth*180.0/M_PI),
      (moon_observation.elevation*180.0/M_PI)
    );

    // Free memory
    predict_destroy_orbital_elements(&iss_tle);

	return 0;
}
