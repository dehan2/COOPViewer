#include "MinimalRSO.h"
#include "constForCOOP.h"

MinimalRSO::MinimalRSO()
{

}



MinimalRSO::MinimalRSO(int ID, int numSegments, cSatellite* satellite, cJulian* localEpoch, elsetrec* TLEData)
{
	m_ID = ID;
	m_satellite = satellite;
	m_COOPEpoch = localEpoch;
	m_TLEData = TLEData;

	update_state_to_given_moment(0.0);
}



MinimalRSO::MinimalRSO(const MinimalRSO& rhs)
{
	copy(rhs);
}



MinimalRSO::~MinimalRSO()
{
	clear();
}



MinimalRSO& MinimalRSO::operator=(const MinimalRSO& rhs)
{
	if (this == &rhs)
		return *this;

	copy(rhs);
	return *this;
}



void MinimalRSO::copy(const MinimalRSO& rhs)
{
	m_ID = rhs.m_ID;
	m_coord = rhs.m_coord;
	m_velocity = rhs.m_velocity;
	m_satellite = rhs.m_satellite;
	m_COOPEpoch = rhs.m_COOPEpoch;
	m_TLEData = rhs.m_TLEData;

	isSGP4Available = rhs.isSGP4Available;
}



void MinimalRSO::clear()
{

}




void MinimalRSO::update_state_to_given_moment(const double& givenMoment)
{
	cJulian targetTime = *m_COOPEpoch;
	targetTime.AddSec(givenMoment);

	double secFromSatEpochToCOOPEpoch = (targetTime.Date() - m_TLEData->jdsatepoch) * 1440;

	double coordArray[3];
	double velocityArray[3];
	sgp4(GRAV_CONST_TYPE, *m_TLEData, secFromSatEpochToCOOPEpoch, coordArray, velocityArray);

	m_coord = rg_Point3D(coordArray[0], coordArray[1], coordArray[2]);
	m_velocity = rg_Point3D(velocityArray[0], velocityArray[1], velocityArray[2]);
}



rg_Point3D MinimalRSO::calculate_coord_at_given_moment(double givenMoment) const
{
	cJulian targetTime = *m_COOPEpoch;
	targetTime.AddSec(givenMoment);

	double secFromSatEpochToCOOPEpoch = (targetTime.Date() - m_TLEData->jdsatepoch) * 1440;

	double coord[3];
	double velocity[3];
	sgp4(GRAV_CONST_TYPE, *m_TLEData, secFromSatEpochToCOOPEpoch, coord, velocity);

	return rg_Point3D(coord[0], coord[1], coord[2]);
}
