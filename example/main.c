#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "../predict.h"

#define TXT_NORM "\x1B[0m"
#define TXT_RED  "\x1B[31m"
#define TXT_GRN  "\x1B[32m"
#define TXT_YEL  "\x1B[33m"
#define TXT_BLU  "\x1B[34m"
#define TXT_MAG  "\x1B[35m"
#define TXT_CYN  "\x1B[36m"
#define TXT_WHT  "\x1B[37m"

#define OBSERVER_LATITUDE         50.9
#define OBSERVER_LONGITUDE        -1.39

#define RADIANS_FROM_DEGREES(x)   (x*(M_PI/180.0))
#define DEGREES_FROM_RADIANS(x)   (x*(180.0/M_PI))

uint64_t timestamp_ms(void)
{
  struct timespec spec;
  clock_gettime(CLOCK_REALTIME, &spec);
  return ((uint64_t) spec.tv_sec) * 1000 + (((uint64_t) spec.tv_nsec) / 1000000);
}

void timestamp_ms_toString(char *destination, uint32_t destination_size, uint64_t timestamp_ms)
{
  uint64_t timestamp;
  struct tm tim;

  if(destination_size < 25)
  {
    return;
  }

  timestamp = timestamp_ms / 1000;

  gmtime_r((const time_t *)&timestamp, &tim);

  strftime(destination, 20, "%Y-%m-%d %H:%M:%S", &tim);
  snprintf(destination+19, 6, ".%03ldZ", (timestamp_ms - (timestamp * 1000)));
}

int main(void)
{
  predict_orbital_elements_t iss_tle;
  struct predict_position iss_orbit;
  predict_observer_t obs;

  struct predict_observation iss_observation;
  struct predict_observation sun_observation;
  struct predict_observation moon_observation;

  // Near-space (period <225 minutes)
  struct predict_sgp4 sgp;

  // Deep-space (period >225 minutes)
  struct predict_sdp4 sdp;

  char tle_lines[2][150];
  char *tle[2];
  
  tle[0] = &tle_lines[0][0];
  tle[1] = &tle_lines[1][0];

  printf("=== libpredict example application ===\n");

  predict_julian_date_t curr_time = julian_from_timestamp_ms(timestamp_ms());

  char current_timestamp[25];
  timestamp_ms_toString(current_timestamp, sizeof(current_timestamp), timestamp_ms_from_julian(curr_time));
  printf("Time:                     %s\n", current_timestamp);

  predict_create_observer(&obs, "Self", RADIANS_FROM_DEGREES(OBSERVER_LATITUDE), RADIANS_FROM_DEGREES(OBSERVER_LONGITUDE), 0);
  printf("Observer Position:        %+.4f°N, %+.4f°E\n", OBSERVER_LATITUDE, OBSERVER_LONGITUDE);

  printf("== Planetary Bodies ==\n");

  predict_observe_sun(&obs, curr_time, &sun_observation);
  printf("Current Sun Observation:  AZ: %8.3f°, EL: %8.3f°\n",
    DEGREES_FROM_RADIANS(sun_observation.azimuth),
    DEGREES_FROM_RADIANS(sun_observation.elevation)
  );

  predict_observe_moon(&obs, curr_time, &moon_observation);
  printf("Current Moon Observation: AZ: %8.3f°, EL: %8.3f°\n",
    DEGREES_FROM_RADIANS(moon_observation.azimuth),
    DEGREES_FROM_RADIANS(moon_observation.elevation)
  );

  printf("== Spacecraft ==\n");

  /* Up to date as of 7th November 2018, get new version from: https://live.ariss.org/tle/ */
  strcpy(tle_lines[0], "1 25544U 98067A   18311.69881946  .00003236  00000-0  56524-4 0  9995");
  strcpy(tle_lines[1], "2 25544  51.6417  23.5568 0004767  22.5396  79.5368 15.53922927140835");

  printf("Parsing ISS TLE..         ");
  if(!predict_parse_tle(&iss_tle, tle[0], tle[1], &sgp, &sdp))
  {
    printf(TXT_RED"Error!"TXT_NORM"\n");
    exit(1);
  }
  printf(TXT_GRN"OK"TXT_NORM" (Epoch: 20%01d.%2.2f)\n",
    iss_tle.epoch_year,
    iss_tle.epoch_day
  );

  printf("Calculating ISS orbit..   ");
  if(predict_orbit(&iss_tle, &iss_orbit, curr_time) < 0)
  {
    printf(TXT_RED"Error!"TXT_NORM"\n");
    exit(1);
  }
  printf(TXT_GRN"OK"TXT_NORM"\n");

  predict_observe_orbit(&obs, &iss_orbit, &iss_observation);
  printf("Current ISS Observation:  AZ: %8.3f°, EL: %8.3f°\n",
    DEGREES_FROM_RADIANS(iss_observation.azimuth),
    DEGREES_FROM_RADIANS(iss_observation.elevation)
  );

  if(predict_aos_happens(&iss_tle, RADIANS_FROM_DEGREES(OBSERVER_LATITUDE)))
  {
    char aos_timestamp[25];
    char los_timestamp[25];
    char maxel_timestamp[25];

    struct predict_observation iss_next_aos;
    struct predict_observation iss_next_los;
    struct predict_observation iss_next_maxel;

    iss_next_aos = predict_next_aos(&obs, &iss_tle, curr_time);
    timestamp_ms_toString(aos_timestamp, sizeof(aos_timestamp), timestamp_ms_from_julian(iss_next_aos.time));
    printf("Next ISS AoS Azimuth:    %8.3f° at %s\n",
      DEGREES_FROM_RADIANS(iss_next_aos.azimuth),
      aos_timestamp
    );

    iss_next_maxel = predict_at_max_elevation(&obs, &iss_tle, iss_next_aos.time);
    timestamp_ms_toString(maxel_timestamp, sizeof(maxel_timestamp), timestamp_ms_from_julian(iss_next_maxel.time));
    printf("      Max Elevation:     %8.3f° at %s\n",
      DEGREES_FROM_RADIANS(iss_next_maxel.elevation),
      maxel_timestamp
    );

    iss_next_los = predict_next_los(&obs, &iss_tle, iss_next_aos.time);
    timestamp_ms_toString(los_timestamp, sizeof(los_timestamp), timestamp_ms_from_julian(iss_next_los.time));
    printf("         LoS Azimuth:    %8.3f° at %s\n",
      DEGREES_FROM_RADIANS(iss_next_los.azimuth),
      los_timestamp
    );
  }
  else
  {
    printf("No ISS Passes for Observer's Latitude.\n");
  }
  


  return 0;
}