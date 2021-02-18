#include "OrbitShortestLink.h"
#include <fstream>
#include <sstream>
#include <vector>
#include "constForCOOP.h"
OrbitShortestLink::OrbitShortestLink()
{
}

void OrbitShortestLink::load_orbit_shortest_link(const string& filePath)
{
	ifstream fin;
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
			while (getline(ss, token, '\t')) {
				result.push_back(token);
			}
			if (!result.empty())
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
					double moment = stod(result[0]);
					result.erase(result.begin());
					std::list<int> RSOsID;
					for (auto& ID : result)
					{
						RSOsID.push_back(stoi(ID));
					}
					m_momentNShortestLinkRSOsID.push_back({ moment,RSOsID });
				}
			}
			else
			{
				break;
			}
			//TOKENIZE
		}
	}
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

