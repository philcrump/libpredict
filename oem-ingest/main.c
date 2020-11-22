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

#define OBSERVER_LATITUDE         50.0
#define OBSERVER_LONGITUDE        -5.1
#define OBSERVER_ALTITUDE         100

#define TLE_LINE_1  "1 40424U 15010A   20218.09216399 -.00000043  00000-0  00000+0 0  9990"
#define TLE_LINE_2  "2 40424   0.0173  77.9868 0002188 334.4620 291.8184  1.00271511 19914"

#define OEM_FILENAME "test.oem"

void observer_calculate(const predict_observer_t *observer, double time, const double pos[3], const double vel[3], struct predict_observation *result);

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

typedef struct {
  predict_julian_date_t time;
  ///ECI position in km
  double position[3];
  ///ECI velocity in km/s
  double velocity[3];
} predict_oem_row_t;

typedef struct {
  predict_oem_row_t *rows;
  int32_t rows_count;
} predict_oem_t;

predict_oem_t predict_oem = {
  .rows_count = 0
};

/* Assumes ICRF/J2000 (ECI), so can be directly imported into libpredict */
void ingest_oem(char *oem_filename)
{
  FILE* oem_file = fopen(oem_filename, "r");

  char line[256];

  char line_time[20];
  struct tm row_tm;

  char line_data[200];

  predict_oem.rows = malloc(sizeof(predict_oem_row_t) * 2036);
  int32_t row_index = 0;

  while (fgets(line, 255, oem_file))
  {
    if(line[0] < '0' || line[0] > '9')
    {
      /* Hacky way to ignore non-data rows */
      continue;
    }

    printf("%s", line);

    memcpy(line_time, line, 19);
    line_time[19] = '\0';

    strptime(line_time, "%Y-%m-%dT%H:%M:%S", &row_tm);
    predict_oem.rows[row_index].time = julian_from_timestamp_ms(1000.0*mktime(&row_tm)); // Note milliseconds are ignored

    memcpy(line_data, (&line[33]), 141);
    line_data[141] = '\0';

    sscanf(line_data,"%lf  %lf   %lf   %lf  %lf  %lf",
      &predict_oem.rows[row_index].position[0], &predict_oem.rows[row_index].position[1], &predict_oem.rows[row_index].position[2],
      &predict_oem.rows[row_index].velocity[0], &predict_oem.rows[row_index].velocity[1], &predict_oem.rows[row_index].velocity[2]
    );
    row_index++;
  }

  fclose(oem_file);

  predict_oem.rows_count = row_index;
}

int main(void)
{
  predict_orbital_elements_t sc_tle;
  struct predict_position sc_orbit;

  predict_observer_t obs;

  char obs_timestamp[25];
  struct predict_observation oem_observation;

  // Near-space (period <225 minutes)
  struct predict_sgp4 sgp;

  // Deep-space (period >225 minutes)
  struct predict_sdp4 sdp;

  char tle_lines[2][150];
  char *tle[2];
  
  tle[0] = &tle_lines[0][0];
  tle[1] = &tle_lines[1][0];

  printf("=== libpredict example application ===\n");

  predict_create_observer(&obs, "Self", PREDICT_DEG2RAD(OBSERVER_LATITUDE), PREDICT_DEG2RAD(OBSERVER_LONGITUDE), OBSERVER_ALTITUDE);
  printf("Observer Position:        %+.4f°N, %+.4f°E, %+dm\n", OBSERVER_LATITUDE, OBSERVER_LONGITUDE, OBSERVER_ALTITUDE);

  printf("Parsing OEM..\n");
  ingest_oem(OEM_FILENAME);

  printf("Parsing TLE..         ");

  strcpy(tle_lines[0], TLE_LINE_1);
  strcpy(tle_lines[1], TLE_LINE_2);

  if(!predict_parse_tle(&sc_tle, tle[0], tle[1], &sgp, &sdp))
  {
    printf(TXT_RED"Error!"TXT_NORM"\n");
    exit(1);
  }
  printf(TXT_GRN"OK"TXT_NORM" (Epoch: 20%01d.%2.2f)\n",
    sc_tle.epoch_year,
    sc_tle.epoch_day
  );

  for(int32_t i = 0; i < predict_oem.rows_count; i++)
  {
    timestamp_ms_toString(obs_timestamp, sizeof(obs_timestamp), timestamp_ms_from_julian(predict_oem.rows[i].time));
    printf("Time:                     %s\n", obs_timestamp);

    // Calculate from TLE
    predict_orbit(&sc_tle, &sc_orbit, predict_oem.rows[i].time);
    predict_observe_orbit(&obs, &sc_orbit, &oem_observation);
    printf("TLE:  AZ: %8.3f°, EL: %8.3f°\n",
      PREDICT_RAD2DEG(oem_observation.azimuth),
      PREDICT_RAD2DEG(oem_observation.elevation)
    );

    // Calculate from OEM:
    observer_calculate(&obs, predict_oem.rows[i].time, predict_oem.rows[i].position, predict_oem.rows[i].velocity, &oem_observation);
    printf("OEM:  AZ: %8.3f°, EL: %8.3f°\n",
      PREDICT_RAD2DEG(oem_observation.azimuth),
      PREDICT_RAD2DEG(oem_observation.elevation)
    );
    printf("============================\n");
  }
  
  return 0;
}
