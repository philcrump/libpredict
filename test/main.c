#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "../predict.h"
#include "../unsorted.h"

#define TXT_NORM "\x1B[0m"
#define TXT_RED  "\x1B[31m"
#define TXT_GRN  "\x1B[32m"
#define TXT_YEL  "\x1B[33m"
#define TXT_BLU  "\x1B[34m"
#define TXT_MAG  "\x1B[35m"
#define TXT_CYN  "\x1B[36m"
#define TXT_WHT  "\x1B[37m"

/* This file presents comparison of SGP4 and SDP4 orbit propagation against the Sample Test Cases in SPACETRACK REPORT NO. 3 - December 1980 */

const char* sample_tles[2*2] = {
  /* SGP4 Test */
  "1 88888U          80275.98708465  .00073094  13844-3  66816-4 0    87",
  "2 88888  72.8435 115.9689 0086731  52.6988 110.5714 16.05824518  1058",

  /* SDP4 Test */
  "1 11801U          80230.29629788  .01431103  00000-0  14311-1       2",
  "2 11801U 46.7916 230.4354 7318036  47.4722  10.4117  2.28537848     2"
};

typedef struct {
  double position[3];
  double velocity[3];
} sample_row_t;

typedef struct {
  sample_row_t tsince[5]; // 0, 360, 720, 1080, 1440
} sample_set_t;

static sample_set_t sample_results[] = {
  /* SGP4 Results */
  {
    .tsince = {
      {
        // 0
        .position = { 2328.97048951,  -5995.22076416,   1719.97067261 },
        .velocity = { 2.91207230,     -0.98341546,      -7.09081703 }
      },
      {
        // 360
        .position = { 2456.10705566,  -6071.93853760,   1222.89727783 },
        .velocity = { 2.67938992,     -0.44829041,      -7.22879231 }
      },
      {
        // 720
        .position = { 2567.56195068,  -6112.50384522,    713.96397400 },
        .velocity = { 2.44024599,      0.09810869,      -7.31995916 }
      },
      {
        // 1080
        .position = { 2663.09078980,  -6115.48229980,    196.39640427 },
        .velocity = { 2.19611958,      0.65241995,      -7.36282432 }
      },
      {
        // 1440
        .position = { 2742.55133057,  -6079.67144775,   -326.38095856 },
        .velocity = { 1.94850229,      1.21106251,      -7.35619372 }
      }
    }
  },

  /* SDP4 Results */
  {
    .tsince = {
      {
        // 0
        .position = { 7473.37066650,    428.95261765,   5828.74786377 },
        .velocity = { 5.10715413,      6.44468284,      -0.18613096 }
      },
      {
        // 360
        .position = { -3305.22537232,  32410.86328125, -24697.1767581 },
        .velocity = { -1.30113538,     -1.15131518,      -0.28333528 }
      },
      {
        // 720
        .position = { 14271.28759766,  24110.46411133,  -4725.76837158 },
        .velocity = { -0.32050445,      2.67984074,      -2.08405289 }
      },
      {
        // 1080
        .position = { -9990.05883789,  22717.35522461, -23616.89062501 },
        .velocity = { -1.01667246,     -2.29026759,       0.72892364 }
      },
      {
        // 1440
        .position = { 9787.86975097,  33753.34667969, -15030.81176758 },
        .velocity = { -1.09425066,      0.92358845,      -1.52230928 }
      }
    }
  }        
};


int main(void)
{
  predict_orbital_elements_t orbit_elements;
  struct predict_position orbit_position;
  double tle_julian_epoch;

  // Near-space (period <225 minutes)
  struct predict_sgp4 sgp;
  // Deep-space (period >225 minutes)
  struct predict_sdp4 sdp;

  printf("=== libpredict Test against Sample Data ===\n");

  printf("Parsing 88888 (SGP Reference)..         ");
  if(!predict_parse_tle(&orbit_elements, sample_tles[0], sample_tles[1], &sgp, &sdp))
  {
    printf(TXT_RED"Error!"TXT_NORM"\n");
    exit(1);
  }
  printf(TXT_GRN"OK"TXT_NORM"\n");

  tle_julian_epoch = Julian_Date_of_Epoch((1000.0*orbit_elements.epoch_year) + orbit_elements.epoch_day);

  switch(orbit_elements.ephemeris)
  {
    case 0:
      printf(" - Using SGP4 model\n");
      break;
    case 1:
      printf(" - Using SDP4 model\n");
      break;
    case 2:
      printf(" - Using SGP8 model\n");
      break;
    case 3:
      printf(" - Using SDP8 model\n");
      break;
  }

  printf("                   X/Xdot           Y/Ydot           Z/Zdot\n");

  for(double tle_tsince = 0; tle_tsince < (1440.0 / 1440); tle_tsince += (360.0 / 1440))
  {
    if(predict_orbit(&orbit_elements, &orbit_position, (tle_tsince + tle_julian_epoch)) < 0)
    {
      printf(TXT_RED"Error!"TXT_NORM"\n");
      exit(1);
    }

    printf("       %12.4f  %16.8f %16.8f %16.8f \n",
      (tle_tsince * 1440),
      orbit_position.position[0], orbit_position.position[1], orbit_position.position[2]
    );
    printf("                   %16.8f %16.8f %16.8f \n",
      orbit_position.velocity[0], orbit_position.velocity[1], orbit_position.velocity[2]
    );
    printf("Difference:\n");
    printf("                   %16.8f %16.8f %16.8f \n",
      orbit_position.position[0] - sample_results[0].tsince[(int)(tle_tsince/(360.0 / 1440))].position[0],
      orbit_position.position[1] - sample_results[0].tsince[(int)(tle_tsince/(360.0 / 1440))].position[1],
      orbit_position.position[2] - sample_results[0].tsince[(int)(tle_tsince/(360.0 / 1440))].position[2]
    );
    printf("                   %16.8f %16.8f %16.8f \n",
      orbit_position.velocity[0] - sample_results[0].tsince[(int)(tle_tsince/(360.0 / 1440))].velocity[0],
      orbit_position.velocity[1] - sample_results[0].tsince[(int)(tle_tsince/(360.0 / 1440))].velocity[1],
      orbit_position.velocity[2] - sample_results[0].tsince[(int)(tle_tsince/(360.0 / 1440))].velocity[2]
    );
    printf("\n ======================== \n");
  }



  printf("Parsing 11801 (SDP Reference)..         ");
  if(!predict_parse_tle(&orbit_elements, sample_tles[2], sample_tles[3], &sgp, &sdp))
  {
    printf(TXT_RED"Error!"TXT_NORM"\n");
    exit(1);
  }
  printf(TXT_GRN"OK"TXT_NORM"\n");

  tle_julian_epoch = Julian_Date_of_Epoch((1000.0*orbit_elements.epoch_year) + orbit_elements.epoch_day);

  switch(orbit_elements.ephemeris)
  {
    case 0:
      printf(" - Using SGP4 model\n");
      break;
    case 1:
      printf(" - Using SDP4 model\n");
      break;
    case 2:
      printf(" - Using SGP8 model\n");
      break;
    case 3:
      printf(" - Using SDP8 model\n");
      break;
  }

  printf("                   X/Xdot           Y/Ydot           Z/Zdot\n");

  for(double tle_tsince = 0; tle_tsince < (1440.0 / 1440); tle_tsince += (360.0 / 1440))
  {
    if(predict_orbit(&orbit_elements, &orbit_position, (tle_tsince + tle_julian_epoch)) < 0)
    {
      printf(TXT_RED"Error!"TXT_NORM"\n");
      exit(1);
    }

    printf("       %12.4f  %16.8f %16.8f %16.8f \n",
      (tle_tsince * 1440),
      orbit_position.position[0], orbit_position.position[1], orbit_position.position[2]
    );
    printf("                   %16.8f %16.8f %16.8f \n",
      orbit_position.velocity[0], orbit_position.velocity[1], orbit_position.velocity[2]
    );
    printf("Difference:\n");
    printf("                   %16.8f %16.8f %16.8f \n",
      orbit_position.position[0] - sample_results[1].tsince[(int)(tle_tsince/(360.0 / 1440))].position[0],
      orbit_position.position[1] - sample_results[1].tsince[(int)(tle_tsince/(360.0 / 1440))].position[1],
      orbit_position.position[2] - sample_results[1].tsince[(int)(tle_tsince/(360.0 / 1440))].position[2]
    );
    printf("                   %16.8f %16.8f %16.8f \n",
      orbit_position.velocity[0] - sample_results[1].tsince[(int)(tle_tsince/(360.0 / 1440))].velocity[0],
      orbit_position.velocity[1] - sample_results[1].tsince[(int)(tle_tsince/(360.0 / 1440))].velocity[1],
      orbit_position.velocity[2] - sample_results[1].tsince[(int)(tle_tsince/(360.0 / 1440))].velocity[2]
    );
    printf("\n ======================== \n");
  }

  return 0;
}