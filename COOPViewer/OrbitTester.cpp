// OrbitTester.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <fstream>
#include <array>
#include <iomanip>
#include <vector>
#include <filesystem>
#include <list>
#include <string>
#include <sstream>

#include "RSOManager.h"
#include "constForCOOP.h"



const int numEccentricitySegments = 10;

using namespace std;


void read_prediction_command_file(const string& filePath, list<PredictionCommand>& commands);

void record_approx_errors(RSOManager& manager, int numTimeSegments);
void record_L2C_L2K_ratio(RSOManager& manager, const string& fileName, int numTimeSegments);
void record_max_approx_error_ratio(RSOManager& manager, const string& fileName);
void record_max_approx_errors_for_line_segments(RSOManager& manager, const string& fileName, int numLineSegments);

list<string> read_file_names_in_directory(const string& directory, const string& ext);
list<pair<int, int>> extract_time_fragments(const list<string>& fileNames);

list<OutlierInfo> read_outlier_info(const string& filePath);

bool compare_time_fragments_in_ascending_order(const pair<int, int>& lhs, const pair<int, int>& rhs)
{
	return lhs.first < rhs.first;
}

int main()
{
	list<PredictionCommand> commands;
	read_prediction_command_file("..\\PredictionCommand.txt", commands);
	PredictionCommand& command = commands.front();

    RSOManager manager;
	manager.initialize_RSO_manager(command);

	/*list<OutlierInfo> infos = read_outlier_info("MissingCases.txt");

	for (auto& info : infos)
	{
		OrbitalBall* primary = manager.find_RSO_from_ID(info.primary);
		OrbitalBall* secondary = manager.find_RSO_from_ID(info.secondary);

		float timeKey = (info.day - command.day) * 86400 + info.hour * 3600 + info.min * 60 + info.sec;
		rg_Point3D primaryCoord = primary->calculate_point_on_Kepler_orbit_at_time(timeKey);
		rg_Point3D secondaryCoord = secondary->calculate_point_on_Kepler_orbit_at_time(timeKey);
		float calculatedDistance = primaryCoord.distance(secondaryCoord);

		cout << "Outlier [" << info.ID << "]: [ " << info.primary << ", " << info.secondary << "] at " << info.day << ", " << info.hour << "/" << info.min << "/" << info.sec << " - STK: " << info.distance << ", calculated: " << calculatedDistance << endl;
	}*/


	list<OrbitalBall*> RSOForAnalysis;
	RSOForAnalysis.push_back(manager.find_RSO_that_has_eccentricity_similar_to_given(0));
	RSOForAnalysis.push_back(manager.find_RSO_that_has_eccentricity_similar_to_given(0.05));
	RSOForAnalysis.push_back(manager.find_RSO_that_has_eccentricity_similar_to_given(0.1));
	RSOForAnalysis.push_back(manager.find_RSO_that_has_eccentricity_similar_to_given(0.15));
	RSOForAnalysis.push_back(manager.find_RSO_that_has_eccentricity_similar_to_given(0.2));


	
	for (auto& rso : RSOForAnalysis)
	{
		if (rso->is_SGP4_available())
		{
			string fileName = string("ErrorAnalysis_") + rso->get_satellite()->Name() + string(".txt");
			ofstream fout(fileName);
			double perigee = rso->get_satellite()->Orbit().Perigee() + XKMPER_WGS72;
			double eccentricity = rso->get_satellite()->Orbit().Eccentricity();
			double meanRadius = 0.5 * perigee * (1 + sqrt(1 - pow(eccentricity, 2)));

			fout << "% RSO: " << rso->get_satellite()->Name() << "("<<rso->get_satellite()->Orbit().SatId()<<") , eccentricity: " << eccentricity << ", MeanR: "<< meanRadius<<"\n";
			fout << "% Index	L2K	C2K	L2C	L2K/R	C2K/R	L2C/R\n";

			int indexForPerigee = 0;
			double minDistanceFromEarth = DBL_MAX;
			for (int i = 0; i <= command.numLineSegments; i++)
			{
				pair<double, double> maxErrors = rso->calculate_max_L2K_and_L2C_error_for_current_line_segment(10);
				double C2KErr = rso->calculate_max_C2K_error_for_current_line_segment(10);
				fout << i << "\t" << maxErrors.first << "\t" << C2KErr << "\t" << maxErrors.second <<"\t";
				fout << maxErrors.first / meanRadius*100 << "\t" << C2KErr / meanRadius * 100 << "\t" << maxErrors.second / meanRadius * 100 << "\n";

				/*double distanceFromEarth = rso->get_coord().magnitude();
				if (distanceFromEarth < minDistanceFromEarth)
				{
					minDistanceFromEarth = distanceFromEarth;
					indexForPerigee = i;
				}*/

				rso->move_to_next_segment();
			}
			fout.close();
		}
	}
	

	cout << "Computation end" << endl;
}



void read_prediction_command_file(const string& filePath, list<PredictionCommand>& commands)
{
	ifstream fin;	
	fin.open(filePath);

	if (fin.is_open()) 	
	{
		while (!fin.eof()) 
		{
			char lineData[256];
			fin.getline(lineData, 256);    

			if (lineData[0] != '#')
			{
				PredictionCommand command;

				char* context;
				string delimiter = " \t";
				
				string token = strtok_s(lineData, delimiter.c_str(), &context);
				command.directory = token;
				
				token = strtok_s(NULL, delimiter.c_str(), &context);
				command.tleFile = token;

				token = strtok_s(NULL, delimiter.c_str(), &context);
				command.numObject = stoi(token);

				token = strtok_s(NULL, delimiter.c_str(), &context);
				command.numLineSegments = stoi(token);

				token = strtok_s(NULL, delimiter.c_str(), &context);
				command.predictionTimeWindow = stoi(token);

				token = strtok_s(NULL, delimiter.c_str(), &context);
				command.year = stoi(token);

				token = strtok_s(NULL, delimiter.c_str(), &context);
				command.month = stoi(token);

				token = strtok_s(NULL, delimiter.c_str(), &context);
				command.day = stoi(token);

				token = strtok_s(NULL, delimiter.c_str(), &context);
				command.hour = stoi(token);

				token = strtok_s(NULL, delimiter.c_str(), &context);
				command.min = stoi(token);

				token = strtok_s(NULL, delimiter.c_str(), &context);
				command.sec = stoi(token);

				commands.push_back(command);
			}
		}
	}

	fin.close();
}



void record_approx_errors(RSOManager& manager, int numTimeSegments)
{
	for (int eccentricity = 1; eccentricity < numEccentricitySegments; eccentricity++)
	{
		OrbitalBall* ball = manager.find_RSO_that_has_eccentricity_similar_to_given(1.0/numEccentricitySegments * eccentricity);

		string name = ball->get_satellite()->Orbit().SatName();
		string ID = ball->get_satellite()->Orbit().SatId();
		double RSOEccentricity = ball->get_satellite()->Orbit().Eccentricity();
		double period = ball->get_satellite()->Orbit().Period();
		double perigee = ball->get_satellite()->Orbit().Perigee();

		cout << "RSO " << eccentricity << ": " << ID << ", " << name << ", " << RSOEccentricity << ", " << period << endl;

		string fileName = "Sat" + to_string(eccentricity) + ".csv";
		ofstream approxErrorFile(fileName);
		approxErrorFile << fixed << setprecision(2);
		approxErrorFile << "# " << name << " (" << ID << "), eccentricity: " << RSOEccentricity << ", perigee(km): " << perigee << "\n";

		double nextSegmentTransitionTime = ball->get_next_segment_transition_time();
		for (int time = 0; time < numTimeSegments; time++)
		{
			double targetTime = period * time / numTimeSegments;

			while (targetTime > nextSegmentTransitionTime)
			{
				//cout << "Segment Changed: " << nextSegmentTransitionTime << endl;
				ball->move_to_next_segment();
				nextSegmentTransitionTime = ball->get_next_segment_transition_time();
			}

			rg_Point3D coordOnKepler = ball->calculate_point_on_Kepler_orbit_at_time(targetTime);
			rg_Point3D coordOfReplica = ball->calculate_replica_position_at_time(targetTime);
			rg_Point3D coordOfCircularReplica = ball->calculate_coord_of_circular_replica_at_time(targetTime);

			double distanceL2K = coordOnKepler.distance(coordOfReplica);
			double distanceC2K = coordOnKepler.distance(coordOfCircularReplica);
			double distanceL2C = coordOfReplica.distance(coordOfCircularReplica);

			approxErrorFile << time << "\t" << distanceL2K << "\t" << distanceC2K << "\t" << distanceL2C << "\n";
		}
		cout << "Error recording finish" << endl;
		approxErrorFile.close();
	}
	cout << "Error estimation finish" << endl;
}



void record_L2C_L2K_ratio(RSOManager& manager, const string& fileName, int numTimeSegments)
{
	array<OrbitalBall*, numEccentricitySegments-1> RSOs;
	array<vector<double>, numEccentricitySegments-1> L2CRatios;
	for (int eccentricity = 1; eccentricity < numEccentricitySegments; eccentricity++)
	{
		OrbitalBall* ball = manager.find_RSO_that_has_eccentricity_similar_to_given(1.0/numEccentricitySegments * eccentricity);

		string name = ball->get_satellite()->Orbit().SatName();
		string ID = ball->get_satellite()->Orbit().SatId();
		double RSOEccentricity = ball->get_satellite()->Orbit().Eccentricity();
		double period = ball->get_satellite()->Orbit().Period();
		double perigee = ball->get_satellite()->Orbit().Perigee();

		RSOs.at(eccentricity - 1) = ball;
		L2CRatios.at(eccentricity - 1) = vector<double>(numTimeSegments);

		cout << "RSO " << eccentricity << ": " << ID << ", " << name << ", " << RSOEccentricity << ", " << period << endl;

		double nextSegmentTransitionTime = ball->get_next_segment_transition_time();
		for (int time = 0; time < numTimeSegments; time++)
		{
			double targetTime = period * time / numTimeSegments;

			while (targetTime > nextSegmentTransitionTime)
			{
				//cout << "Segment Changed: " << nextSegmentTransitionTime << endl;
				ball->move_to_next_segment();
				nextSegmentTransitionTime = ball->get_next_segment_transition_time();
			}

			rg_Point3D coordOnKepler = ball->calculate_point_on_Kepler_orbit_at_time(targetTime);
			rg_Point3D coordOfReplica = ball->calculate_replica_position_at_time(targetTime);
			rg_Point3D coordOfCircularReplica = ball->calculate_coord_of_circular_replica_at_time(targetTime);

			double distanceL2K = coordOnKepler.distance(coordOfReplica);
			double distanceC2K = coordOnKepler.distance(coordOfCircularReplica);
			double distanceL2C = coordOfReplica.distance(coordOfCircularReplica);

			double L2CRatio = distanceL2C / distanceL2K;
			L2CRatios.at(eccentricity - 1).at(time) = L2CRatio;
		}
		cout << "Error recording finish" << endl;
	}
	cout << "Error estimation finish" << endl;

	ofstream L2CRatioRecord(fileName);

	for (int eccentricity = 0; eccentricity < numEccentricitySegments-1; eccentricity++)
	{
		OrbitalBall* ball = RSOs.at(eccentricity);

		string name = ball->get_satellite()->Orbit().SatName();
		string ID = ball->get_satellite()->Orbit().SatId();
		double RSOEccentricity = ball->get_satellite()->Orbit().Eccentricity();
		double period = ball->get_satellite()->Orbit().Period();
		double perigee = ball->get_satellite()->Orbit().Perigee();
		double apogee = ball->get_satellite()->Orbit().Apogee();
		L2CRatioRecord << "# " << name << " (" << ID << "), e: " << RSOEccentricity << ", p: " << perigee << ", a: " << apogee << "\n";
	}


	for (int time = 0; time < numTimeSegments; time++)
	{
		L2CRatioRecord << time << "\t";
		for (int eccentricity = 0; eccentricity < numEccentricitySegments-1; eccentricity++)
		{
			L2CRatioRecord << L2CRatios.at(eccentricity).at(time) << "\t";
		}
		L2CRatioRecord << "\n";
	}
	L2CRatioRecord.close();
	cout << "L2C ratio recording finish" << endl;
}



void record_max_approx_error_ratio(RSOManager& manager, const string& fileName)
{

}



void record_max_approx_errors_for_line_segments(RSOManager& manager, const string& fileName, int numLineSegments)
{
	array<OrbitalBall*, numEccentricitySegments - 1> RSOs;
	array<vector<double>, numEccentricitySegments - 1> L2KErrors;
	array<vector<double>, numEccentricitySegments - 1> L2CErrors;
	
	for (int eccentricity = 1; eccentricity < numEccentricitySegments; eccentricity++)
	{
		OrbitalBall* ball = manager.find_RSO_that_has_eccentricity_similar_to_given(1.0 / numEccentricitySegments * eccentricity);

		string name = ball->get_satellite()->Orbit().SatName();
		string ID = ball->get_satellite()->Orbit().SatId();
		double RSOEccentricity = ball->get_satellite()->Orbit().Eccentricity();
		double period = ball->get_satellite()->Orbit().Period();
		double perigee = ball->get_satellite()->Orbit().Perigee();
		double apogee = ball->get_satellite()->Orbit().Apogee();

		RSOs.at(eccentricity - 1) = ball;
		L2KErrors.at(eccentricity - 1) = vector<double>();
		L2CErrors.at(eccentricity - 1) = vector<double>();

		cout << "RSO " << eccentricity << ": " << ID << ", " << name << ", " << RSOEccentricity << ", " << period << endl;

		string fileName = "Sat" + to_string(eccentricity) + ".csv";
		ofstream approxErrorFile(fileName);
		approxErrorFile << fixed << setprecision(2);
		approxErrorFile << "# " << name << " (" << ID << "), e: " << RSOEccentricity << ", p: " << perigee << ", a: "<<apogee<<"\n";

		double nextSegmentTransitionTime = ball->get_next_segment_transition_time();
		for (int segment = 0; segment < numLineSegments; segment++)
		{
			pair<double, double> maxErrors = ball->calculate_max_L2K_and_L2C_error_for_current_line_segment(10);
			L2KErrors.at(eccentricity - 1).push_back(maxErrors.first);
			L2CErrors.at(eccentricity - 1).push_back(maxErrors.second);
			double ratio = maxErrors.first / maxErrors.second;

			ball->move_to_next_segment();
			nextSegmentTransitionTime = ball->get_next_segment_transition_time();

			approxErrorFile << fixed << setprecision(5);
			approxErrorFile << segment << "\t"<<maxErrors.first << "\t" << maxErrors.second << "\t" << ratio << "\n";
		}
		cout << "Error recording finish" << endl;
		approxErrorFile.close();
	}
	cout << "Error estimation finish" << endl;

	ofstream outputFile(fileName);

	for (int eccentricity = 0; eccentricity < numEccentricitySegments - 1; eccentricity++)
	{
		OrbitalBall* ball = RSOs.at(eccentricity);

		string name = ball->get_satellite()->Orbit().SatName();
		string ID = ball->get_satellite()->Orbit().SatId();
		double RSOEccentricity = ball->get_satellite()->Orbit().Eccentricity();
		double period = ball->get_satellite()->Orbit().Period();
		double perigee = ball->get_satellite()->Orbit().Perigee();
		double apogee = ball->get_satellite()->Orbit().Apogee();
		outputFile << "# " << name << " (" << ID << "), e: " << RSOEccentricity << ", p: " << perigee << ", a: " << apogee << "\n";
	}


	for (int segment = 0; segment < numLineSegments; segment++)
	{
		outputFile << segment << "\t";
		for (int eccentricity = 0; eccentricity < numEccentricitySegments - 1; eccentricity++)
		{
			double ratio = L2KErrors.at(eccentricity).at(segment) / L2CErrors.at(eccentricity).at(segment);
			outputFile << ratio << "\t";
		}
		outputFile << "\n";
	}
	outputFile.close();
	cout << "L2K/L2C ratio recording finish" << endl;
}


list<string> read_file_names_in_directory(const string& directory, const string& ext)
{
	list<string> fileNamesInDirectory;
	for (auto& p : filesystem::recursive_directory_iterator(directory))
	{		
		//cout << p.path().filename() << endl;			//결과: "test.txt"
		//cout << p.path().filename().string() << endl;	//결과: test.txt

		if (filesystem::is_regular_file(p) && p.path().extension() == ext)
			fileNamesInDirectory.push_back(p.path().filename().stem().string());
	}
	return fileNamesInDirectory;
}



list<pair<int, int>> extract_time_fragments(const list<string>& fileNames)
{
	list<pair<int, int>> timeFragments;
	for (auto& fileName : fileNames)
	{
		//cout << "FileName: " << fileName << endl;
		array<string, 5> tokens;
		string s = fileName;
		string delimiter = "_";

		size_t pos = 0;
		std::string token;
		int index = 0;
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//std::cout << token << std::endl;
			tokens.at(index++) = token;
			s.erase(0, pos + delimiter.length());
		}
		//std::cout << s << std::endl;
		tokens.at(index++) = s;

		int startTime = stoi(tokens.at(3));
		int timeWindow = stoi(tokens.at(4));
		timeFragments.push_back({ startTime, timeWindow });
	}
	cout << "Parse finish" << endl;
	return timeFragments;
}



list<OutlierInfo> read_outlier_info(const string& filePath)
{
	ifstream fin;
	fin.open(filePath);

	list<OutlierInfo> outlierInfos;
	
	if (fin.is_open())
	{
		string line;
		while (getline(fin, line))
		{
			istringstream iss(line);
			string token;
			iss >> token;
			if (token.compare("%") != 0)
			{
				OutlierInfo info;
				info.ID = stoi(token);
				iss >> info.primary >> info.secondary >> info.distance >> info.year >> info.mon >> info.day >> info.hour >> info.min >> info.sec >> info.tag;
				outlierInfos.push_back(info);
			}
		}
	}
	fin.close();

	return outlierInfos;
}

