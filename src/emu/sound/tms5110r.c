/* TMS5110 ROM Tables */

/* Kx is (5-bits -> 9 bits+sign, 2's comp. fractional (-1 < x < 1) */

/* The following table is assumed to be for TMS5100
 *
 * US Patent 4209836
 *           4331836
 *           4304964
 *           4234761
 *           4189779
 *           4449233
 * 
 * All patents give interpolation coefficients
 *	{ 8, 8, 8, 4, 4, 2, 2, 1 }
 *  This sequence will not calculate the published 
 *  fractions:
 * 1 8 0.125
 * 2 8 0.234
 * 3 8 0.330
 * 4 4 0.498
 * 5 4 0.623
 * 6 2 0.717
 * 7 2 0.859
 * 0 1 1.000
 * 
 * Instead,  { 8, 8, 8, 4, 4, 4, 2, 1 }
 * will calculate those coefficients and this has been used below.
 */

static const struct tms5100_coeffs pat4209836_coeff =
{
	/* subtype */
	SUBTYPE_TMS5100,
	10,
	4,
	5,
	{ 5, 5, 4, 4, 4, 4, 4, 3, 3, 3 },
	/* E   */
	{ 0, 0, 1, 1, 2, 3, 5, 7, 10, 15, 21, 31, 43, 61, 86, 511 },
	/* P   */
	{   0,  41,  43,  45,  47,  49,  51,  53,
	   55,  58,  60,  63,  66,  70,  73,  76,
	   79,  83,  87,  90,  94,  99, 103, 104,
	  112, 118, 120, 129, 134, 140, 147, 153 },
	{
		/* K1  */
		{ -504, -497, -493, -488, -471, -471, -460, -446,
		  -427, -405, -378, -344, -501, -259, -206, -148,
		   -86,   -5,   45,  110,  168,  131,  277,  320,
		   357,  388,  413,  434,  451,  464,  474,  482 },
		/* K2  */
		{ -349, -376, -305, -264, -252, -223, -192, -158,
		  -124,  -88,  -51,  -14,  23,    60,   97,  133,
		   167,  215,  230,  259, 286,   310,  333,  354,
		   372,  389,  404,  417, 429,   439,   449, 506 },
		/* K3  */
		{ -397, -365, -327, -266, -229, -170, -104, -36,
		    35,  104,  169,  228,  281,  326,  364, 396 },
		/* K4  */
		{ -369, -334, -296, -246, -191, -131, -67,  -1,
		    64,  128,  188,  243,  291,  332, 367, 397 },
		/* K5  */
		{ -319, -286, -250, -211, -168, -122, -74, -25,
		    24,   73,  121,  167,  210,  249, 285, 318 },
		/* K6  */
		{ -290, -252, -209, -163, -114, -62,  -9,  44,
		    97,  147,  194,  255,  278, 313, 344, 371 },
		/* K7  */
		{ -291, -256, -216, -174, -128, -96, -31,  19,
		    69,  117,  163,  206,  246, 280, 316, 345 },
		/* K8  */
		{ -218, -133, -38, 56, 152, 251, 305, 361 },
		/* K9  */
		{ -225, -157, -82, -3, 76, 151, 220, 280 },
		/* K10 */
		{ -179, -122, -61, 1, 62, 123, 179, 247 },
	},
	/* Chirptable */
	{   0,  42, -44, 50, -78, 18, 37, 20,
	    2, -31, -59,  2,  95, 90,  5, 15,
	   38, -4,  -91,-91, -42,-35,-36, -4,
       37, 43,   34, 33,  15, -1, -8,-18,
	  -19,-17,   -9,-10,  -6,  0,  3,  2,
	    1,  0,    0,  0,   0,  0,  0,  0,
	    0,  0,    0 },
	/* interpolation coefficients */
	{ 8, 8, 8, 4, 4, 4, 2, 1 }
};

/*
 * This table is from patent 4403965
 * also listed in 4331836
 * also listed in 4946391
 *
 * Works best with M58817 ...
 */

static const struct tms5100_coeffs pat4403965_coeff =
{
	/* subtype */
	SUBTYPE_M58817,
	10,
	4,
	5,
	{ 5, 5, 4, 4, 4, 4, 4, 3, 3, 3 },
	/* E   */
	{  0,1,2,3,4,6,8,11,16,23,33,47,63,85,114,511, },
	/* P   */
	{ 0,41,43,45,47,49,51,53,55,58,60,63,66,70,73,76,79,83,87,90,94,99,103,107,112,118,123,129,134,140,147,153,
	},
	{
		/* K1  */
		{
		-501,-498,-495,-490,-485,-478,-469,-459,-446,-431,-412,-389,-362,-331,-295,-253,-207,-156,-102,-45,13,70,126,179,228,272,311,345,374,399,420,437,
		},
		/* K2  */
		{
		-376,-357,-335,-312,-286,-258,-227,-195,-161,-124,-87,-49,-10,29,68,106,143,178,212,243,272,299,324,346,366,384,400,414,427,438,448,506,
		},
		/* K3  */
		{
		-407,-381,-349,-311,-268,-218,-162,-102,-39,25,89,149,206,257,302,341,
		},
		/* K4  */
		{
		-290,-252,-209,-163,-114,-62,-9,44,97,147,194,238,278,313,344,371,
		},
		/* K5  */
		{
		-318,-283,-245,-202,-156,-107,-56,-3,49,101,150,196,239,278,313,344,
		},
		/* K6  */
		{
		-193,-152,-109,-65,-20,26,71,115,158,198,235,270,301,330,355,377,
		},
		/* K7  */
		{
		-254,-218,-180,-140,-97,-53,-8,36,81,124,165,204,240,274,304,332,
		},
		/* K8  */
		{
		-205,-112,-10,92,187,269,336,387,
		},
		/* K9  */
		{
		-249,-183,-110,-19,48,126,198,261,
		},
		/* K10 */
		{
		-190,-133,-73,-10,53,115,173,227,
		},
	},
	/* Chirptable */
	{ 0,43,-44,51,-77,18,37,20,
	2,-30,-58,3,96,91,5,15,
	38,-4,-90,-91,-42,-35,-35,-3,
	37,43,35,34,15,-1,-8,-17,
	-19,-17,-9,-9,-6,1,4,3,
	1,0,0,0,0,0,0,0,
	0,0,0
	 },
	/* interpolation coefficients */
	{ 8, 8, 8, 4, 4, 4, 2, 1 }
};


/* Note: the following tables in this file were read from the real TMS5110A chip, except
         for the chirptable and the interp_coeff
*/


static const struct tms5100_coeffs tms5110_coeff =
{
	/* subtype */
	SUBTYPE_TMS5110,
	10,
	4,
	5,
	{ 5, 5, 4, 4, 4, 4, 4, 3, 3, 3 },
	/* E   */
	{ 0,   1,   2,   3, 4,   6,   8,   11,
	 16,  23,  33,  47, 63,  85,  114, 511 },
	/* P   */
	{ 0,   15,  16,  17, 19,  21,  22,  25,
	 26,  29,  32,  36, 40,  42,  46,  50,
	 55,  60,  64,  68, 72,  76,  80,  84,
	 86,  93,  101, 110, 120, 132, 144, 159 },
	{
		/* K1  */
		{ -501, -498, -497, -495, -493, -491, -488, -482,
		  -478, -474, -469, -464, -459, -452, -445, -437,
		  -412, -380, -339, -288, -227, -158,  -81,   -1,
		    80,  157,  226,  287,  337,  379,  411,  436 },
		/* K2  */
		{ -328, -303, -274, -244, -211, -175, -138,  -99,
	 	   -59,  -18,   24,   64,  105,  143,  180,  215,
		   248,  278,  306,  331,  354,  374,  392,  408,
		   422,  435,  445,  455,  463,  470,  476,  506 },
		/* K3  */
		{ -441, -387, -333, -279, -225, -171, -117,  -63,
		    -9,   45,   98,  152,  206,  260,  314,  368 },
		/* K4  */
		{ -328, -273, -217, -161, -106,  -50,    5,   61,
		   116,  172,  228,  283,  339,  394,  450,  506  },
		/* K5  */
		{ -328, -282, -235, -189, -142,  -96,  -50,   -3,
		    43,   90,  136,  182,  229,  275,  322,  368  },
		/* K6  */
		{ -256, -212, -168, -123,  -79,  -35,   10,   54,
		    98,  143,  187,  232,  276,  320,  365,  409 },
		/* K7  */
		{ -308, -260, -212, -164, -117,  -69,  -21,   27,
		    75,  122,  170,  218,  266,  314,  361,  409  },
		/* K8  */
		{ -256, -161,  -66,   29,  124,  219,  314,  409  },
		/* K9  */
		{ -256, -176,  -96,  -15,  65,  146,  226,  307  },
		/* K10 */
		{ -205, -132,  -59,   14,  87,  160,  234,  307  },
	},
	/* Chirptable */
	{   0,  42, -44, 50, -78, 18, 37, 20,
	    2, -31, -59,  2,  95, 90,  5, 15,
	   38, -4,  -91,-91, -42,-35,-36, -4,
       37, 43,   34, 33,  15, -1, -8,-18,
	  -19,-17,   -9,-10,  -6,  0,  3,  2,
	    1,  0,    0,  0,   0,  0,  0,  0,
	    0,  0,    0 },
	/* interpolation coefficients */
	{ 8, 8, 8, 4, 4, 4, 2, 1 }
};


