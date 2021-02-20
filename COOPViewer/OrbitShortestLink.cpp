#include "OrbitShortestLink.h"
#include <fstream>
#include <sstream>
#include <vector>
#include "constForCOOP.h"
OrbitShortestLink::OrbitShortestLink()
{
}

void OrbitShortestLink::load_orbit_shortest_link(const std::string& filePath)
{
	std::ifstream fin;
	fin.open(filePath.c_str());
	if (fin.is_open())
	{
		while (!fin.eof())
		{
			std::vector<std::string> result;
			std::string token;
			std::string line;
			getline(fin, line);
			std::stringstream ss(line);
			//TOKENIZE

			while (getline(ss, token, '\t')) 
			{
				result.push_back(token);
			}


			if (!result.empty() && result[0] != "%")
			{
				if (result[0] == "SOURCE_LOCAL_COORDINATE")
				{
					m_srcGroundCoordinate = Sphere(stod(result[1]), stod(result[2]), stod(result[3]), SATELLITE_SIZE);
				}
				else if (result[0] == "DESTINATION_LOCAL_COORDINATE")
				{
					m_destGroundCoordinate = Sphere(stod(result[1]), stod(result[2]), stod(result[3]), SATELLITE_SIZE);
				}
				else
				{
					int pathID = stoi(result[0]);
					result.erase(result.begin());

					double pathLength = stod(result[0]);
					result.erase(result.begin());

					double moment = stod(result[0]);
					result.erase(result.begin());

					double year = stod(result[0]);
					result.erase(result.begin());

					double month = stod(result[0]);
					result.erase(result.begin());

					double day = stod(result[0]);
					result.erase(result.begin());

					double hour = stod(result[0]);
					result.erase(result.begin());

					double min = stod(result[0]);
					result.erase(result.begin());

					double sec = stod(result[0]);
					result.erase(result.begin());

					std::list<int> RSOsID;
					for (auto& ID : result)
					{
						RSOsID.push_back(stoi(ID));
					}
					m_momentNShortestLinkRSOsID.push_back({ moment, pathLength, RSOsID });
				}
			}
			//TOKENIZE
		}
	}
}



const MomentNShortestLinkRSOsID* OrbitShortestLink::find_shortest_path_imminent_to_moment(const double& targetMoment)
{
	for (const auto& momentNShortestLinkRSOsID : m_momentNShortestLinkRSOsID)
	{
		if (momentNShortestLinkRSOsID.moment > targetMoment)
			return &momentNShortestLinkRSOsID;
	}

	return &m_momentNShortestLinkRSOsID.back();
}



const std::list<int>& OrbitShortestLink::find_shortest_link_RSOs_ID_in_closest_moment(const double& targetMoment)
{
	std::list<int>& lastshortestLinkRSOsID = m_momentNShortestLinkRSOsID.front().shortestLinkRSOsID;
	for (const auto& momentNShortestLinkRSOsID : m_momentNShortestLinkRSOsID)
	{
		if (momentNShortestLinkRSOsID.moment < targetMoment)
			lastshortestLinkRSOsID = momentNShortestLinkRSOsID.shortestLinkRSOsID;
		else
			break;
	}

	return lastshortestLinkRSOsID;
}

