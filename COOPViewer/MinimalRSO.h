#pragma once

#include "coreLib.h"
#include "orbitLib.h"
#include "rg_Point3D.h"
#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>

//For new STK - 201012
#include "sgp4ext.h"
#include "sgp4unit.h"


class MinimalRSO
{
protected:
	int		m_ID = -1;
	rg_Point3D	m_coord;
	rg_Point3D	m_velocity;
	cSatellite* m_satellite = nullptr;
	cJulian* m_COOPEpoch = nullptr;
	elsetrec* m_TLEData = nullptr;

	bool isSGP4Available = true;


public:
	MinimalRSO();
	MinimalRSO(int ID, int numSegments, cSatellite* satellite, cJulian* localEpoch, elsetrec* TLEData);
	MinimalRSO(const MinimalRSO& rhs);

	virtual ~MinimalRSO();

	MinimalRSO& operator=(const MinimalRSO& rhs);

	virtual void copy(const MinimalRSO& rhs);
	virtual void clear();

	inline int get_ID() const { return m_ID; }
	inline rg_Point3D get_coord() const { return m_coord; }
	inline rg_Point3D get_velocity() const { return m_velocity; }
	inline cSatellite* get_satellite() const { return m_satellite; }
	inline cJulian* get_COOP_epoch() const { return m_COOPEpoch; }
	inline const bool& is_SGP4_available() const { return isSGP4Available; }

	inline void set_ID(int ID) { m_ID = ID; }
	inline void set_coord(const rg_Point3D& coord) { m_coord = coord; }
	inline void set_velocity(const rg_Point3D& velocity) { m_velocity = velocity; }


	void update_status_to_given_moment(const double& givenMoment);
	rg_Point3D calculate_coord_at_given_moment(double givenMoment) const;
};

