#pragma once

#include "MinimalRSO.h"
#include "constForCOOP.h"

#include <list>
#include <string>
#include <array>
#include "cJulian.h"

using namespace std;

class RSOManager
{
private:
	list<MinimalRSO> m_RSOs;
	
	list<TLEFileInfo> m_TLEFileInfos;

	int m_numSegments;

	map<int, MinimalRSO*> m_mapFromIDToRSO;

	cJulian m_startMomentOfPredictionTimeWindow;

	map<int, double> m_mapFromIDToVelocityUpdatedTime;

	int m_targetCatalogID;
	list<pair<rg_Point3D, tm>> m_wayPoints;

	PredictionCommand m_command;

public:
	RSOManager() = default;
	~RSOManager() {	clear();	};

	void clear();

	list<MinimalRSO>& get_RSOs() { return m_RSOs; }
	inline const cJulian& get_epoch() { return m_startMomentOfPredictionTimeWindow; }

	void read_prediction_command_file(const string& filePath);
	void initialize_RSO_manager(const PredictionCommand& command);

	void load_two_line_element_set_file(const string& filePath, const int& numObjects);
	elsetrec convert_TLE_to_elsetrec(char* longstr1, char* longstr2);
	
	int filter_invalid_TLEs();

	MinimalRSO* find_RSO_from_ID(const int& ID) { return m_mapFromIDToRSO.at(ID); }
	MinimalRSO* find_RSO_that_has_eccentricity_similar_to_given(const double& targetEccentricity);
	list<array<MinimalRSO*, 2>> find_danger_close_pairs(const double& threshold);

	void update_RSO_statuses_to_given_moment(const double& givenMoment);

	void save_RSO_infos(const string& filePath);

	void generate_maneuver_plan(const int& targetCatalogID, const double& rotationAngle, const double& startTime, const double& endTime, const int& numSegments);
	void save_maneuver_plan(const string& filePath, const int& targetCatalogID, const double& rotationAngle, const double& startTime, const double& endTime, const int& numSegments);

	pair<double, double> find_time_of_closest_approach_for_RSO_pair(int primaryID, int secondaryID, int targetTime, double searchInterval, double resolution);
};

