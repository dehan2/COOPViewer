#pragma once

#include <ctime>
#include <cwchar>
#include <list>
#include <string>
#include "coreLib.h"
#include "orbitLib.h"
#include "sgp4ext.h"
#include "sgp4unit.h"

#define DISABLE_COLLISION
//#define CHECK_TLE

//enum ORBIT_SOURCE_TYPE { NONE, TLE };

//All parameters uses km for length, kg for mass, and second for time unit.

//const float GRAVITY_G = 6.6742E-11; // gravitational constant https://en.wikipedia.org/wiki/Gravitational_constant
//const float GRAVITY_M = 5.9722E24; // mass of the earth https://en.wikipedia.org/wiki/Earth_mass
const float GRAVITY_MU = 3.986E5;	//G*M, standard gravitational parameter https://en.wikipedia.org/wiki/Standard_gravitational_parameter

//const cJulian SIMULATION_EPOCH(2018, 3, 22, 0, 0, 0); //00:00:00 22. Mar. 2018 
//const cJulian SIMULATION_EPOCH(2018, 7, 12, 0, 0, 0); 
//const cJulian SIMULATION_EPOCH(2018, 7, 20, 6, 0, 0); 

const double SATELLITE_SIZE = 1.0E-10;
const int BASIC_NUM_APPROXIMATION =5;
const int MAX_APPROXIMATION_LEVEL = 30;

const int BEGINNING_ID_OF_ORBIT_BALL = 1;

const float ORBIT_BOUNDING_TETRAHEDRON_SCALE = 4;

const float INITIAL_SEARCH_INTERVAL = 1;
const int MAX_SEARCH_ITERATION = 100;
const float INTERVAL_EPSILON = 0.01;

class ResidentSpaceObject;

struct SegmentTransitionEvent
{
	double time;
	ResidentSpaceObject* ball;
};


struct compare_segment_transition_events_in_ascending_order {
	bool operator()(const SegmentTransitionEvent& lhs, const SegmentTransitionEvent& rhs)
	{
		return lhs.time > rhs.time;
	}
};


#ifndef _WIN32
#include <limits>
#define DBL_MAX std::numeric_limits<double>::max()
#endif

#define SCAN_MINIMAL_PROXIMITY

const int OPTIMAL_MANEUVER_SEARCH_RANGE = 10;
const int MANEUVER_ANGLE_INCREMENT = 1;


struct PredictionCommand
{
	string directory, tleFile;
	int numObject, numLineSegments;
	float predictionTimeWindow, cutoffValue;
	int year, month, day, hour, min, sec;
};


//SGP4_06 Parameters
// opsmode = 'a' best understanding of how afspc code works, 'i' imporved sgp4 resulting in smoother behavior
// GRAV_CONST_TYPE = wgs72old / wgs72 / wgs84;

struct TLEFileInfo
{
	elsetrec SGP4_06Info;
	cSatellite SGP4_80Info;
};


const char OPS_MODE = 'i';
const gravconsttype GRAV_CONST_TYPE = wgs84;

const float OVERLAP_THRESHOLD = 1.0e-3;



struct TCAReport
{
	int ID;
	int primaryID;
	int secondaryID;
	double distanceOfClosestApproach;
	double timeOfClosestApproach;
	double closeApproachEnteringTime;
	double closeApproachLeavingTime;
	int year, mon, day, hour, min;
	float sec, distance;
	string tag;
};

struct TCAReport_less
{
	bool operator()(TCAReport const& a, TCAReport const& b) const
	{
		return a.distanceOfClosestApproach < b.distanceOfClosestApproach;
	}
};


struct TPDBReport
{
	int ID;
	int primaryID;
	int secondaryID;
	int tertiaryID;
	double minRadius;
	double time;
	int year, mon, day, hour, min;
	float sec;
};

struct TPDBReport_less
{
	bool operator()(TPDBReport const& a, TPDBReport const& b) const
	{
		return a.minRadius < b.minRadius;
	}
};

struct ErrorAnalysisReport
{
	int ID;
	string satName;
	int resolution;
	float eccentricity;
	float meanRadius;
	float perigee;
	float period;
	float maxL2K;
	float maxL2C;
	float maxC2K;
};



struct ErrorAnalysisReport2
{
	int ID;
	string satName;
	int resolution;
	float eccentricity;
	float meanRadius;
	float perigee;
	list<float> L2K;
	list<float> L2C;
	list<float> C2K;
};


const int NUM_COL_PPDB = 6;
const int COL_PPDB_PRIMARY = 0;
const int COL_PPDB_SECONDARY = 1;
const int COL_PPDB_DCA = 2;
const int COL_PPDB_TCA = 3;
const int COL_PPDB_CASTART = 4;
const int COL_PPDB_CAEND = 5;

//enum class 

enum class COOP_OPERATION_MODE
{
	PPDB, TPDB, SPDB, EVAL_SAFETY, CLOSESTNEIGHBORS
};