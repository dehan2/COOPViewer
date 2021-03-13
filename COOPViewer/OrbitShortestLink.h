#pragma once
#include <map>
#include <list>
#include "Sphere.h"
#include <string>

struct MomentNShortestLinkRSOsID
{
    double moment;
    double pathLength;
    list<int> shortestLinkRSOsID;
};

class OrbitShortestLink
{
private:
    std::list<MomentNShortestLinkRSOsID> m_momentNShortestLinkRSOsID;
    Sphere m_srcGroundCoordinate;
    Sphere m_destGroundCoordinate;

public:
    OrbitShortestLink();

    void load_orbit_shortest_link(const std::string& filePath);
    const list<MomentNShortestLinkRSOsID>& get_shortest_paths() const { return m_momentNShortestLinkRSOsID; }

    inline const Sphere& get_src_ground_coordinate() { return m_srcGroundCoordinate; }
    inline const Sphere& get_dest_ground_coordinate() { return m_destGroundCoordinate; }
    const MomentNShortestLinkRSOsID* find_shortest_path_imminent_to_moment(const double& targetMoment);
    const std::list<int> find_shortest_link_RSOs_ID_in_closest_moment(const double& targetMoment);
};

