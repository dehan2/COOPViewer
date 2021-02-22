#include "OrbitTunnel.h"
#include "FileOperations.h"
#include "StringFunctions.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

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
				if (result[0] == "RSOID")
				{
					result.erase(result.begin());
					for (auto& ID : result)
					{
						tunnelRSOsID.push_back(stoi(ID));
					}

				}
				else if (result[0] == "TRAJECTORY")
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
				else if (result[0] == "BOUNDARY")
				{

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




void OrbitTunnel::load_orbit_tunnel2(const std::string& filePath)
{
	std::list<rg_Point3D> trajectory;
	std::list<int> tunnelRSOsID;
	std::list<Polygon3D> boundaryFaces;

	std::ifstream fin;
	fin.open(filePath.c_str());
	if (fin.is_open())
	{
		const std::string RSOID("RSOID");
		const std::string TRAJECTORY("TRAJECTORY");
		const std::string BOUNDARY("BOUNDARY");
		const char   TAB('\t');
		const char   LF('\n');
		const char   BLANK(' ');
		std::string delimiters = std::string(1, TAB) + std::string(1, BLANK);

		// remove blank lines and lines with comments    
		std::string channelFileInString;
		const char commentMark = '#';
		FileOperations::remove_both_comment_and_blank_lines_from_ascii_file(filePath, commentMark, channelFileInString);
		istringstream istrstream(channelFileInString);

		std::string line;
		while (std::getline(istrstream, line, LF))
		{
			vector<string> tokenVector;
			StringFunctions::tokens_from_string(line, delimiters, tokenVector);

			if (tokenVector.front() == RSOID)
			{
				std::getline(istrstream, line, LF);
				vector<string> tokenVector;
				StringFunctions::tokens_from_string(line, delimiters, tokenVector);
				for (auto& ID : tokenVector)
				{
					tunnelRSOsID.push_back(stoi(ID));
				}
			}
			else if (tokenVector.front() == TRAJECTORY)
			{
				std::getline(istrstream, line, LF);
				vector<string> tokenVector;
				StringFunctions::tokens_from_string(line, delimiters, tokenVector);
				int index = 1;
				double x = 0.;
				double y = 0.;
				double z = 0.;
				rg_Point3D currPoint;
				for (auto& coordinate : tokenVector)
				{
					if (index % 3 == 0)
					{
						z = stod(coordinate);
						currPoint = rg_Point3D(x, y, z);
						trajectory.push_back(currPoint);
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
			else if (tokenVector.front() == BOUNDARY)
			{
				while (std::getline(istrstream, line, LF))
				{
					vector<string> tokenVector;
					StringFunctions::tokens_from_string(line, delimiters, tokenVector);

					int index = 1;
					double x = 0.;
					double y = 0.;
					double z = 0.;
					rg_Point3D currPoint;
					list<rg_Point3D> boundaryVerticesOfOuterloop;
					for (auto& coordinate : tokenVector)
					{
						if (index % 3 == 0)
						{
							z = stod(coordinate);
							currPoint = rg_Point3D(x, y, z);
							boundaryVerticesOfOuterloop.push_back(currPoint);
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
					list< list<rg_Point3D> > boudaryVertices;
					boudaryVertices.push_back(boundaryVerticesOfOuterloop);
					Polygon3D poly;
					poly.set_boundary_vertices(boudaryVertices);
					boundaryFaces.push_back(poly);
				}
			}
			if (boundaryFaces.size() > 0)
			{
				m_orbitTunnels.push_back({ trajectory, tunnelRSOsID, boundaryFaces });
				break;
			}
		}
	}
}
