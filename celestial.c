#include "predict.h"
#include "unsorted.h"
#include <string.h>

#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)

// RA/DEC Source: http://simbad.u-strasbg.fr/simbad/
// RF Flux Source: http://www.eme2008.org/papers/Sistem%20components/Radio_Sources_Eng.pdf
#define PREDICT_CELESTIAL_BODIES_NUMBER		6
const predict_celestial_body_t predict_celestial_bodies[PREDICT_CELESTIAL_BODIES_NUMBER] =
{
	/**
	 * Flux, between 1000 MHz and 35000 MHz
	 * S = −220.85 - 2.99 log F
     * S in dB(W/m²/Hz). F in MHz:
     **/
	{
		.name = "TAURUS A",
		.right_ascension = 83.633083333333, // 05 34 31.94
		.declination = 22.0145, // +22 00 52.2
	},

	/**
	 * Flux, between 400 MHz and 25000 MHz
	 * S = −209.77 - 8.56 log F
     * S in dB(W/m²/Hz). F in MHz:
     **/
	{
		.name = "VIRGO A",
		.right_ascension = 187.70593075958334, // 12 30 49.42338230
		.declination = 12.391123293916666, // +12 23 28.0438581
	},

	/**
	 * Flux, between 2000 MHz and 31000 MHz 
	 * S = −225.71 - 12.44 log F
     * S in dB(W/m²/Hz). F in MHz:
     **/
	{
		.name = "CYGNUS A",
		.right_ascension = 299.86815190954167, // 19 59 28.35645829
		.declination = 40.733915736, // +40 44 02.0966496
	},

	/**
	 * Flux, between 300 MHz and 31000 MHz 
	 * A = 5.745 - 0.77log(F)
	 * B = (0.0187 - 0.003log(F))(y-1980.0)
	 * S = (1-B)*10^A
     * S in Jansky(Jy). F in MHz:
     **/
	{
		.name = "CASSIOPEIA A",
		.right_ascension = 350.85, // 23 23 24.000
		.declination = 58.815, // +58 48 54.00
	},
	
	/**
	 * No Flux information (X-ray?)
     **/
	{
		.name = "SAGITTARIUS A",
		.right_ascension = 266.416816625, // 17 45 40.03599
		.declination = -29.007824972222224, // -29 00 28.1699
	},
	
	/**
	 * No Flux information (optical?)
     **/
	{
		.name = "OMEGA NEBULA",
		.right_ascension = 275.1958333333333, // 18 20 47.0
		.declination = -16.171666666666667, // -16 10 18
	},
};

const predict_celestial_body_t *predict_celestial_bodies_search(char *search)
{
	uint32_t i, search_len;

	search_len = strlen(search);

	for(i=0; i<PREDICT_CELESTIAL_BODIES_NUMBER; i++)
	{
		if(0 == strncasecmp(search, predict_celestial_bodies[i].name, MIN(search_len, strlen(predict_celestial_bodies[i].name))))
		{
			return &predict_celestial_bodies[i];
		}
	}

	return NULL;
}

void predict_observe_celestial(const predict_observer_t *observer, predict_julian_date_t time, const predict_celestial_body_t *body, struct predict_observation *obs)
{
	double h = FMod2p(Sidereal_from_Julian(time) + observer->longitude) - body->right_ascension;

	obs->time = time;
	obs->azimuth = atan2(sin(h),cos(h)*sin(observer->latitude)-tan(body->declination)*cos(observer->latitude))+M_PI;
	obs->elevation = asin(sin(observer->latitude)*sin(body->declination)+cos(observer->latitude)*cos(body->declination)*cos(h));
}

void predict_observe_ra_dec(const predict_observer_t *observer, predict_julian_date_t time, double ra, double dec, struct predict_observation *obs)
{
	double h = FMod2p(Sidereal_from_Julian(time) + observer->longitude) - ra;

	obs->time = time;
	obs->azimuth = atan2(sin(h),cos(h)*sin(observer->latitude)-tan(dec)*cos(observer->latitude))+M_PI;
	obs->elevation = asin(sin(observer->latitude)*sin(dec)+cos(observer->latitude)*cos(dec)*cos(h));
}