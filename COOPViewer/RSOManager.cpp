#include "RSOManager.h"
#include "constForVDRCOpenGLWidget.h"
#include "rg_TMatrix3D.h"
#include <direct.h>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <sstream>
#include <QString>
#include <cTle.h>

void RSOManager::clear()
{

}




void RSOManager::read_prediction_command_file(const string& filePath)
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
				char* context;
				string delimiter = " \t";

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				string token = strtok_s(lineData, delimiter.c_str(), &context);
#elif __linux__
				string token = strtok_r(lineData, delimiter.c_str(), &context);
#endif
				m_command.directory = token;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.tleFile = token;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.numObject = stoi(token);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.numLineSegments = stoi(token);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.cutoffValue = stof(token);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.predictionTimeWindow = stoi(token);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.year = stoi(token);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.month = stoi(token);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.day = stoi(token);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.hour = stoi(token);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.min = stoi(token);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.sec = stoi(token);
			}
		}
	}

	fin.close();

	initialize_RSO_manager(m_command);
}



void RSOManager::initialize_RSO_manager(const PredictionCommand& command)
{
	string delimiter = " ";
	string filePath = command.directory + command.tleFile;
	filePath.erase(filePath.begin());
	string resultpath = ".\\result";
	filePath = resultpath + filePath;
	QString qstr = QString::fromStdString(filePath);
	filePath = translate_to_window_path(qstr);
	load_two_line_element_set_file(filePath, command.numObject);

	m_numSegments = command.numLineSegments;
	m_startMomentOfPredictionTimeWindow = cJulian(command.year, command.month, command.day, command.hour, command.min, command.sec);

	int numFilteredTLE = filter_invalid_TLEs();

	for (auto& TLEInfo : m_TLEFileInfos)
	{
		int catalogID = stoi(TLEInfo.SGP4_80Info.Orbit().SatId());
		m_RSOs.push_back(MinimalRSO(catalogID, m_numSegments, &TLEInfo.SGP4_80Info, &m_startMomentOfPredictionTimeWindow, &TLEInfo.SGP4_06Info));
		m_mapFromIDToRSO[catalogID] = &m_RSOs.back();
	}

#ifdef CHECK_TLE
	cout << "Data check..." << endl;
	auto dangerClosePairs = find_danger_close_pairs(1.0);
	for (auto& dangerClosePair : dangerClosePairs)
	{
		auto primary = dangerClosePair.at(0);
		auto secondary = dangerClosePair.at(1);
		double interRSODistance = primary->get_coord().distance(secondary->get_coord());
		cout << "Short distance pair [" << primary->get_satellite()->Orbit().SatId() << ", " << secondary->get_satellite()->Orbit().SatId() << "] with d: " << interRSODistance << "km" << endl;
	}
#endif
}



void RSOManager::load_two_line_element_set_file(const string& filePath, const int& numObjects)
{
	ifstream fin;
	fin.open(filePath);

	string delimiter = " \t";
	int targetNumRSOs = numObjects;
	if (numObjects == 0)
		targetNumRSOs = INT_MAX;

	for (int i = 0; i < targetNumRSOs; i++)
	{
		string firstLine, secondLine, thirdLine;
		getline(fin, firstLine);
		if (firstLine.length() > 0)
		{
			getline(fin, secondLine);
			getline(fin, thirdLine);

			size_t pos = 0;
			pos = firstLine.find(delimiter);
			firstLine = firstLine.substr(pos + delimiter.length());
			cTle tleSGP4(firstLine, secondLine, thirdLine);
			cSatellite satSGP4(tleSGP4);
			char c_secondLine[130];
			char c_thirdLine[130];

			strcpy(c_secondLine, secondLine.c_str());
			strcpy(c_thirdLine, thirdLine.c_str());
			elsetrec currData = convert_TLE_to_elsetrec(c_secondLine, c_thirdLine);
			m_TLEFileInfos.push_back({ currData, satSGP4 });
		}
		else
		{
			break;
		}
	}

	fin.close();
}

void RSOManager::read_prediction_command_file_STARLINK(const string& filePath)
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
				char* context;
				string delimiter = " \t";

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				string token = strtok_s(lineData, delimiter.c_str(), &context);
#elif __linux__
				string token = strtok_r(lineData, delimiter.c_str(), &context);
#endif
				m_command.directory = token;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif

				string tempStr = string("latest.tle");
				m_command.tleFile = tempStr;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.numObject = stoi(token);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.numLineSegments = stoi(token);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.cutoffValue = stof(token);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.predictionTimeWindow = stoi(token);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.year = stoi(token);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.month = stoi(token);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.day = stoi(token);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.hour = stoi(token);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.min = stoi(token);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				token = strtok_s(NULL, delimiter.c_str(), &context);
#elif __linux__
				token = strtok_r(NULL, delimiter.c_str(), &context);
#endif
				m_command.sec = stoi(token);
			}
		}
	}

	fin.close();
	initialize_RSO_manager_STARLINK(m_command);
}

void RSOManager::initialize_RSO_manager_STARLINK(const PredictionCommand& command)
{
	string filePath = command.directory + command.tleFile;
	load_two_line_element_set_file_STARLINK(filePath, command.numObject);

	m_numSegments = command.numLineSegments;
	m_startMomentOfPredictionTimeWindow = cJulian(command.year, command.month, command.day, command.hour, command.min, command.sec);

	int numFilteredTLE = filter_invalid_TLEs();

	for (auto& TLEInfo : m_TLEFileInfos)
	{
		int catalogID = stoi(TLEInfo.SGP4_80Info.Orbit().SatId());
		m_RSOs.push_back(MinimalRSO(catalogID, m_numSegments, &TLEInfo.SGP4_80Info, &m_startMomentOfPredictionTimeWindow, &TLEInfo.SGP4_06Info));
		m_mapFromIDToRSO[catalogID] = &m_RSOs.back();
	}

#ifdef CHECK_TLE
	cout << "Data check..." << endl;
	auto dangerClosePairs = find_danger_close_pairs(1.0);
	for (auto& dangerClosePair : dangerClosePairs)
	{
		auto primary = dangerClosePair.at(0);
		auto secondary = dangerClosePair.at(1);
		double interRSODistance = primary->get_coord().distance(secondary->get_coord());
		cout << "Short distance pair [" << primary->get_satellite()->Orbit().SatId() << ", " << secondary->get_satellite()->Orbit().SatId() << "] with d: " << interRSODistance << "km" << endl;
	}
#endif
}

void RSOManager::load_two_line_element_set_file_STARLINK(const string& filePath, const int& numObjects)
{
	ifstream fin;
	fin.open(filePath.c_str());
	string delimiter = " ";

	int targetNumRSOs = numObjects;
	if (numObjects == 0)
		targetNumRSOs = INT_MAX;

	for (int i = 0; i < targetNumRSOs; i++)
	{
		string firstLine, secondLine, thirdLine;
		getline(fin, firstLine);
		if (firstLine.length() > 0)
		{
			getline(fin, secondLine);
			getline(fin, thirdLine);
			if (firstLine.find("STARLINK") != string::npos)
			{
				size_t pos = 0;
				pos = firstLine.find(delimiter);
				firstLine = firstLine.substr(pos + delimiter.length());
				cTle tleSGP4(firstLine, secondLine, thirdLine);
				cSatellite satSGP4(tleSGP4);

				char c_secondLine[130];
				char c_thirdLine[130];

				strcpy(c_secondLine, secondLine.c_str());
				strcpy(c_thirdLine, thirdLine.c_str());
				elsetrec currData = convert_TLE_to_elsetrec(c_secondLine, c_thirdLine);
				m_TLEFileInfos.push_back({ currData, satSGP4 });
			}
		}
		else
		{
			break;
		}

	}

	fin.close();
}



elsetrec RSOManager::convert_TLE_to_elsetrec(char* longstr1, char* longstr2)
{
	const double deg2rad = pi / 180.0;         //   0.0174532925199433
	const double xpdotp = 1440.0 / (2.0 * pi);  // 229.1831180523293

	double sec, mu, radiusearthkm, tumin, xke, j2, j3, j4, j3oj2;
	double startsec, stopsec, startdayofyr, stopdayofyr, jdstart, jdstop;
	int startyear, stopyear, startmon, stopmon, startday, stopday,
		starthr, stophr, startmin, stopmin;
	int cardnumb, numb, j;
	long revnum = 0, elnum = 0;
	char classification, intldesg[11];
	int year = 0;
	int mon, day, hr, minute, nexp, ibexp;

	getgravconst(GRAV_CONST_TYPE, tumin, mu, radiusearthkm, xke, j2, j3, j4, j3oj2);

	elsetrec satrec;
	satrec.error = 0;

	// set the implied decimal points since doing a formated read
	// fixes for bad input data values (missing, ...)
	for (j = 10; j <= 15; j++)
		if (longstr1[j] == ' ')
			longstr1[j] = '_';

	if (longstr1[44] != ' ')
		longstr1[43] = longstr1[44];
	longstr1[44] = '.';
	if (longstr1[7] == ' ')
		longstr1[7] = 'U';
	if (longstr1[9] == ' ')
		longstr1[9] = '.';
	for (j = 45; j <= 49; j++)
		if (longstr1[j] == ' ')
			longstr1[j] = '0';
	if (longstr1[51] == ' ')
		longstr1[51] = '0';
	if (longstr1[53] != ' ')
		longstr1[52] = longstr1[53];
	longstr1[53] = '.';
	longstr2[25] = '.';
	for (j = 26; j <= 32; j++)
		if (longstr2[j] == ' ')
			longstr2[j] = '0';
	if (longstr1[62] == ' ')
		longstr1[62] = '0';
	if (longstr1[68] == ' ')
		longstr1[68] = '0';

	sscanf(longstr1, "%2d %5ld %1c %10s %2d %12lf %11lf %7lf %2d %7lf %2d %2d %6ld ",
		&cardnumb, &satrec.satnum, &classification, intldesg, &satrec.epochyr,
		&satrec.epochdays, &satrec.ndot, &satrec.nddot, &nexp, &satrec.bstar,
		&ibexp, &numb, &elnum);

	if (longstr2[52] == ' ')
		sscanf(longstr2, "%2d %5ld %9lf %9lf %8lf %9lf %9lf %10lf %6ld \n",
			&cardnumb, &satrec.satnum, &satrec.inclo,
			&satrec.nodeo, &satrec.ecco, &satrec.argpo, &satrec.mo, &satrec.no,
			&revnum);
	else
		sscanf(longstr2, "%2d %5ld %9lf %9lf %8lf %9lf %9lf %11lf %6ld \n",
			&cardnumb, &satrec.satnum, &satrec.inclo,
			&satrec.nodeo, &satrec.ecco, &satrec.argpo, &satrec.mo, &satrec.no,
			&revnum);

	// ---- find no, ndot, nddot ----
	satrec.no = satrec.no / xpdotp; //* rad/min
	satrec.nddot = satrec.nddot * pow(10.0, nexp);
	satrec.bstar = satrec.bstar * pow(10.0, ibexp);

	// ---- convert to sgp4 units ----
	satrec.a = pow(satrec.no * tumin, (-2.0 / 3.0));
	satrec.ndot = satrec.ndot / (xpdotp * 1440.0);  //* ? * minperday
	satrec.nddot = satrec.nddot / (xpdotp * 1440.0 * 1440);

	// ---- find standard orbital elements ----
	satrec.inclo = satrec.inclo * deg2rad;
	satrec.nodeo = satrec.nodeo * deg2rad;
	satrec.argpo = satrec.argpo * deg2rad;
	satrec.mo = satrec.mo * deg2rad;

	satrec.alta = satrec.a * (1.0 + satrec.ecco) - 1.0;
	satrec.altp = satrec.a * (1.0 - satrec.ecco) - 1.0;

	// ----------------------------------------------------------------
	// find sgp4epoch time of element set
	// remember that sgp4 uses units of days from 0 jan 1950 (sgp4epoch)
	// and minutes from the epoch (time)
	// ----------------------------------------------------------------

	// ---------------- temp fix for years from 1957-2056 -------------------
	// --------- correct fix will occur when year is 4-digit in tle ---------
	if (satrec.epochyr < 57)
		year = satrec.epochyr + 2000;
	else
		year = satrec.epochyr + 1900;

	days2mdhms(year, satrec.epochdays, mon, day, hr, minute, sec);
	jday(year, mon, day, hr, minute, sec, satrec.jdsatepoch);

	// ---------------- initialize the orbit at sgp4epoch -------------------
	sgp4init(GRAV_CONST_TYPE, OPS_MODE, satrec.satnum, satrec.jdsatepoch - 2433281.5, satrec.bstar,
		satrec.ecco, satrec.argpo, satrec.inclo, satrec.mo, satrec.no,
		satrec.nodeo, satrec);

	return satrec;
}



int RSOManager::filter_invalid_TLEs()
{
	list<rg_Point3D> coordOfRSOs;
	auto itForTLEData = m_TLEFileInfos.begin();

	int numFilteredRSOs = 0;
	while (itForTLEData != m_TLEFileInfos.end())
	{
		double secFromSatEpochToPredictionStartTime = (m_startMomentOfPredictionTimeWindow.Date() - (*itForTLEData).SGP4_06Info.jdsatepoch) * 1440;

		double coordArray[3];
		double velocityArray[3];

		bool sgp4Result = sgp4(GRAV_CONST_TYPE, (*itForTLEData).SGP4_06Info, secFromSatEpochToPredictionStartTime, coordArray, velocityArray);
		if (sgp4Result == true)
		{
			rg_Point3D coord(coordArray[0], coordArray[1], coordArray[2]);
			bool isOverlapped = false;
			for (auto& coordOfRSO : coordOfRSOs)
			{
				float distance = coord.distance(coordOfRSO);
				if (distance < OVERLAP_THRESHOLD)
				{
					isOverlapped = true;
					break;
				}
			}

			if (isOverlapped == false)
			{
				coordOfRSOs.push_back(coord);
				itForTLEData++;
			}
			else
			{
				//if (PRINT_PROGRESS_LEVEL >= 1)
				//	cout << "RSO " << (*itForTLEData).SGP4_80Info.Orbit().SatId() << "(" << (*itForTLEData).SGP4_80Info.Name() << ") is overlapped to other RSO" << endl;

				itForTLEData = m_TLEFileInfos.erase(itForTLEData);
			}
		}
		else
		{
			//if (PRINT_PROGRESS_LEVEL >= 1)
			//	cout << "RSO " << (*itForTLEData).SGP4_80Info.Orbit().SatId() << "(" << (*itForTLEData).SGP4_80Info.Name() << ") is not fit for SGP4" << endl;
			itForTLEData = m_TLEFileInfos.erase(itForTLEData);
			++numFilteredRSOs;
		}
	}

	return numFilteredRSOs;
}




MinimalRSO* RSOManager::find_RSO_that_has_eccentricity_similar_to_given(const double& targetEccentricity)
{
	MinimalRSO* ballWithClosestEccentricity = nullptr;
	double minEccentricityDifference = DBL_MAX;
	for (auto& ball : m_RSOs)
	{
		double eccentricity = ball.get_satellite()->Orbit().Eccentricity();
		double difference = abs(eccentricity - targetEccentricity);
		if (ball.is_SGP4_available() && (difference < minEccentricityDifference))
		{
			ballWithClosestEccentricity = &ball;
			minEccentricityDifference = difference;
		}
	}

	return ballWithClosestEccentricity;
}



list<array<MinimalRSO*, 2>> RSOManager::find_danger_close_pairs(const double& threshold)
{
	list<array<MinimalRSO*, 2>> dangerClosePairs;
	for (list<MinimalRSO>::iterator it1 = m_RSOs.begin(); it1 != m_RSOs.end(); it1++)
	{
		for (list<MinimalRSO>::iterator it2 = next(it1); it2 != m_RSOs.end(); it2++)
		{
			float distance = (*it1).get_coord().distance((*it2).get_coord());
			if (distance < threshold)
			{
				dangerClosePairs.push_back({ &(*it1), &(*it2) });
			}
		}
	}
	return dangerClosePairs;
}



void RSOManager::update_RSO_statuses_to_given_moment(const double& givenMoment)
{
	for (auto& rso : m_RSOs)
		rso.update_status_to_given_moment(givenMoment);
}



void RSOManager::save_RSO_infos(const string& filePath)
{
	ofstream RSOInfoFile(filePath);
	RSOInfoFile << "% RSO Information - Total "<<m_RSOs.size()<<" objects\n%\n";
	RSOInfoFile << "% CatalogID\tCatalogName\n";
	for (auto& rso : m_RSOs)
	{
		RSOInfoFile << setfill('0') << setw(5) << stoi(rso.get_satellite()->Orbit().SatId());
		RSOInfoFile << "\t" <<rso.get_satellite()->Orbit().SatName() << "\n";
	}
	RSOInfoFile.close();
}




tm convert_seconds_to_tmStruct(const cJulian& epoch, const double& targetTime)
{
	cJulian julianTime(epoch);
	julianTime.AddSec(targetTime);

	time_t timet = julianTime.ToTime();
	tm gmt;
#ifdef _WIN32
	gmtime_s(&gmt, &timet);
#else
	gmtime_r(&timet, &gmt);
#endif
	gmt.tm_year += 1900;
	gmt.tm_mon += 1;

	return gmt;
}



tm RSOManager::convert_given_moment_to_tm(const double& givenMoment) const
{
	return convert_seconds_to_tmStruct(m_startMomentOfPredictionTimeWindow, givenMoment);
}



string make_time_string(tm moment)
{
	string monthStr;
	switch (moment.tm_mon)
	{
	case 0:
		monthStr = string("Jan.");
		break;
	case 1:
		monthStr = string("Feb.");
		break;
	case 2:
		monthStr = string("Mar.");
		break;
	case 3:
		monthStr = string("Apr.");
		break;
	case 4:
		monthStr = string("May.");
		break;
	case 5:
		monthStr = string("Jun.");
		break;
	case 6:
		monthStr = string("Jul.");
		break;
	case 7:
		monthStr = string("Aug.");
		break;
	case 8:
		monthStr = string("Sep.");
		break;
	case 9:
		monthStr = string("Oct.");
		break;
	case 10:
		monthStr = string("Nov.");
		break;
	case 11:
		monthStr = string("Dec.");
		break;
	default:
		break;
	}

	string timeStr = to_string(moment.tm_hour) += string(":") += to_string(moment.tm_min) += string(":") += to_string(moment.tm_sec) += string(", ") += monthStr += string(" ") += to_string(moment.tm_mday) += string(", ") += to_string(moment.tm_year + 1900);
	return timeStr;
}




pair<double, double> RSOManager::find_time_of_closest_approach_for_RSO_pair(int primaryID, int secondaryID, int targetTime, double searchInterval, double resolution)
{
	double startTime = targetTime - searchInterval;
	double endTime = targetTime + searchInterval;

	MinimalRSO* primary = m_mapFromIDToRSO.at(primaryID);
	MinimalRSO* secondary = m_mapFromIDToRSO.at(secondaryID);

	double TCA = DBL_MAX;
	double minDistance = DBL_MAX;

	double currTime = startTime;
	while (currTime <= endTime)
	{
		rg_Point3D primaryCoord = primary->calculate_coord_at_given_moment(currTime);
		rg_Point3D secondaryCoord = secondary->calculate_coord_at_given_moment(currTime);
		double distance = primaryCoord.distance(secondaryCoord);

		if (distance < minDistance)
		{
			TCA = currTime;
			minDistance = distance;
		}

		currTime += resolution;
	}

	return make_pair(TCA, minDistance);
}



list<MinimalRSO*> RSOManager::find_object_of_interests()
{
	list<MinimalRSO*> OOIs;

	for (auto& OOIID : m_objectOfInterestIDs)
	{
		MinimalRSO* OOI = nullptr;
		auto itForOOI = m_mapFromIDToRSO.find(OOIID);
		if (itForOOI != m_mapFromIDToRSO.end())
			OOI = (*itForOOI).second;

		OOIs.push_back(OOI);
	}

	return OOIs;
}



double RSOManager::calculate_OOI_distance()
{
	if (m_objectOfInterestIDs.size() == 2)
	{
		list<MinimalRSO*> OOIs = find_object_of_interests();

		MinimalRSO* primary = OOIs.front();
		MinimalRSO* secondary = OOIs.back();

		double distance = 0;

		if(primary != nullptr && secondary != nullptr)
			distance = primary->get_coord().distance(secondary->get_coord());

		return distance;
	}
	else
		return 0;
}




Circle3D RSOManager::calculate_circle_of_OOIs()
{
	//Note : NEED TO BE UPDATED - 210220

	Circle3D cotangentCircle;

	if (m_objectOfInterestIDs.size() == 3)
	{
		list<MinimalRSO*> OOIs = find_object_of_interests();

		array<rg_Point3D, 3> coords;
		
		int index = 0;
		for (auto& OOI : OOIs)
		{
			coords.at(index++) = OOI->get_coord();
		}

		cotangentCircle = evaluate_cotangent_circle(coords);
	}

	return cotangentCircle;
}



Circle3D RSOManager::evaluate_cotangent_circle(const array<rg_Point3D, 3>& inputPts) const
{
	rg_Point3D vec1 = inputPts.at(0) - inputPts.at(1);
	rg_Point3D vec2 = inputPts.at(2) - inputPts.at(1);


	//////////////////////////////////////////////////////////////////////////
	//Cotangent circle solution from https://stackoverflow.com/questions/13977354/build-circle-from-3-points-in-3d-space-implementation-in-c-or-c
	//////////////////////////////////////////////////////////////////////////

	// triangle "edges"
	const rg_Point3D t = inputPts.at(1) - inputPts.at(0);
	const rg_Point3D u = inputPts.at(2) - inputPts.at(0);
	const rg_Point3D v = inputPts.at(2) - inputPts.at(1);

	// triangle normal
	const rg_Point3D w = t.crossProduct(u);
	const double wsl = w.squaredMagnitude();//w.getSqrLength();

	if (wsl < 10e-14)
	{
		cout << "Error: too small wsl: " << wsl << endl;
		//return false; // area of the triangle is too small (you may additionally check the points for colinearity if you are paranoid)
	}

	// helpers
	const double iwsl2 = 1.0 / (2.0 * wsl);
	const double tt = t.squaredMagnitude();
	const double uu = u.squaredMagnitude();

	// result circle
	rg_Point3D circleCenter_3D = inputPts.at(0) + (u * tt * (u * v) - t * uu * (t * v)) * iwsl2;
	double   radius = sqrt(tt * uu * (v.squaredMagnitude()) * iwsl2 * 0.5);
	rg_Point3D zVec = (inputPts.at(0) - circleCenter_3D).crossProduct(inputPts.at(2) - circleCenter_3D).getUnitVector();

	Circle3D cotangentCircle(circleCenter_3D, radius, zVec);
	return cotangentCircle;
}



void RSOManager::load_PPDB(const string& filePath)
{
	std::ifstream fin;
	fin.open(filePath);
	//fin.open("D:\\LAB\\source\\repos\\vdrc\\COOPViewer\\COOPViewer\\result\\PPDB.txt");
	
	

	int ID = 0;

	if (fin.is_open())
	{
		string line;
		while (getline(fin, line))
		{
			if (line.size() > 0)
			{
				istringstream iss(line);
				string token;
				iss >> token;
				if (token.compare("%") != 0)
				{
					TCAReport entity;
					entity.ID = ++ID;
					entity.primaryID = stoi(token);
					iss >> entity.secondaryID >> entity.distanceOfClosestApproach >> entity.timeOfClosestApproach >> entity.closeApproachEnteringTime >> entity.closeApproachLeavingTime >> entity.year >> entity.mon >> entity.day >> entity.hour >> entity.min >> entity.sec;
					m_PPDBInfos.push_back(entity);
				}
			}
			else
				break;
		}
	}
	fin.close();
}



void RSOManager::load_TPDB(const string& filePath)
{
	std::ifstream fin;
	fin.open(filePath);

	int ID = 0;

	if (fin.is_open())
	{
		string line;
		while (getline(fin, line))
		{
			if (line.size() > 0)
			{
				istringstream iss(line);
				string token;
				iss >> token;
				if (token.compare("%") != 0)
				{
					TPDBReport entity;
					entity.ID = ++ID;
					entity.primaryID = stoi(token);
					iss >> entity.secondaryID >> entity.tertiaryID >> entity.minRadius >> entity.time >> entity.year >> entity.mon >> entity.day >> entity.hour >> entity.min >> entity.sec;
					m_TPDBInfos.push_back(entity);
				}
			}
			else
				break;
		}
	}
	fin.close();
}




string RSOManager::make_time_string(const double& givenMoment)
{
	tm timeInTM = convert_given_moment_to_tm(givenMoment);

	string monthStr;
	switch (timeInTM.tm_mon)
	{
	case 1:
		monthStr = string("Jan.");
		break;
	case 2:
		monthStr = string("Feb.");
		break;
	case 3:
		monthStr = string("Mar.");
		break;
	case 4:
		monthStr = string("Apr.");
		break;
	case 5:
		monthStr = string("May.");
		break;
	case 6:
		monthStr = string("Jun.");
		break;
	case 7:
		monthStr = string("Jul.");
		break;
	case 8:
		monthStr = string("Aug.");
		break;
	case 9:
		monthStr = string("Sep.");
		break;
	case 10:
		monthStr = string("Oct.");
		break;
	case 11:
		monthStr = string("Nov.");
		break;
	case 12:
		monthStr = string("Dec.");
		break;
	default:
		break;
	}

	string timeStr = monthStr += string(" ") += to_string(timeInTM.tm_mday) += string(", ") += to_string(timeInTM.tm_year) += string(" ") += to_string(timeInTM.tm_hour) += string(":") += to_string(timeInTM.tm_min) += string(":") += to_string(timeInTM.tm_sec);
	return timeStr;
}


string RSOManager::translate_to_window_path(const QString& QfilePath)
{
	string filePath = QfilePath.toLocal8Bit();

	size_t i = filePath.find('/');
	while (i != string::npos)
	{
		string part1 = filePath.substr(0, i);
		string part2 = filePath.substr(i + 1);
		filePath = part1 + R"(\)" + part2; // Use "\\\\" instead of R"(\\)" if your compiler doesn't support C++11's raw string literals
		i = filePath.find('/', i + 1);
	}
	return filePath;
}