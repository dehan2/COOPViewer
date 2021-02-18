#pragma once
#include <list>
#include "rg_Point3D.h"
#include <string>

struct OrbitTunnelData
{
	std::list<rg_Point3D> spine;
	std::list<int> tunnelRSOsID;
};

class OrbitTunnel
{
private:
	std::list<OrbitTunnelData> m_orbitTunnels;
	

public:
	OrbitTunnel();

	const std::list<OrbitTunnelData>& get_orbit_tunnels() { return m_orbitTunnels; }

	void load_orbit_tunnel(const std::string& filePath);
};

