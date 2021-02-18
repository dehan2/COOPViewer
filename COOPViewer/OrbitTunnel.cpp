#include "OrbitTunnel.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>


OrbitTunnel::OrbitTunnel()
{
}


void OrbitTunnel::load_orbit_tunnel(const std::string& filePath)
{
	std::ifstream fin;
	fin.open(filePath.c_str());
	if (fin.is_open())
	{
		std::list<rg_Point3D> spine;
		std::list<int> tunnelRSOsID;
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
			//TOKENIZE
			if (!result.empty())
			{
				if (result[0] == "CHANNEL")
				{
					result.erase(result.begin());
					for (auto& ID : result)
					{
						tunnelRSOsID.push_back(stoi(ID));
					}
					
				}
				else if (result[0] == "SPINE")
				{
					result.erase(result.begin());
					int index = 1;
					double x = 0.;
					double y = 0.;
					double z = 0.;
					rg_Point3D currPoint;
					for (auto& coordinate : result)
					{
						if (index % 3 == 0)
						{
							z = stod(coordinate);
							currPoint = rg_Point3D(x, y, z);
							spine.push_back(currPoint);
						}
						else if (index % 3 == 1)
						{
							x = stod(coordinate);
						}
						else if (index % 3 == 2)
						{
							y = stod(coordinate);
						}
						++index;
					}
				}
			}
			else
			{
				break;
			}


			if (!spine.empty())
			{
				m_orbitTunnels.push_back({ spine, tunnelRSOsID });
				spine.clear();
				tunnelRSOsID.clear();
			}
		}
	}
}
