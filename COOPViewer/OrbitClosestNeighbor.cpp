#include "OrbitClosestNeighbor.h"
#include <fstream>
#include <sstream>
OrbitClosestNeighbor::OrbitClosestNeighbor()
{
}

void OrbitClosestNeighbor::load_orbit_closest_neighbors(const std::string& filePath)
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
				int pathID = stoi(result[0]);
				result.erase(result.begin());

				int targetRSOID = stoi(result[0]);
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
				for (int i = 0; i < result.size(); ++i)
				{
					if (i % 2 == 0)
						RSOsID.push_back(stoi(result[i]));
					else
						continue;
				}
				m_momentNClosestNeighborRSOsID.push_back({ targetRSOID,  moment, RSOsID });
				
			}
			//TOKENIZE
		}
	}
}

const std::list<int> OrbitClosestNeighbor::find_closest_neighbor_RSOs_ID_in_closest_moment(const double& targetMoment)
{
	std::list<int> lastClosestNeighborRSOsID = m_momentNClosestNeighborRSOsID.front().closestNeighborRSOsID;
	for (const auto& momentNClosestNeighborRSOsID : m_momentNClosestNeighborRSOsID)
	{
		if (momentNClosestNeighborRSOsID.moment < targetMoment)
			lastClosestNeighborRSOsID = momentNClosestNeighborRSOsID.closestNeighborRSOsID;
		else
			break;
	}

	return lastClosestNeighborRSOsID;
}
