#pragma once

#include "MinimalRSO.h"

#include <QString>
#include <list>
#include <string>
#include <array>
#include "cJulian.h"
#include "Circle3D.h"
#include "constForCOOP.h"

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

	list<int> m_objectOfInterestIDs;

	int m_targetCatalogID;
	list<pair<rg_Point3D, tm>> m_wayPoints;

	PredictionCommand m_command;

	list<TCAReport> m_PPDBInfos;
	list<TPDBReport> m_TPDBInfos;

	string m_cwd;
public:
	RSOManager() = default;
	~RSOManager() {	clear();}

	void clear();

	list<MinimalRSO>& get_RSOs() { return m_RSOs; }
	inline const cJulian& get_epoch() { return m_startMomentOfPredictionTimeWindow; }
	const PredictionCommand& get_prediction_command() { return m_command; }
	const list<TCAReport>& get_PPDB_infos() { return m_PPDBInfos; }
	const list<TPDBReport>& get_TPDB_infos() { return m_TPDBInfos; }


	list<int>* get_objectOfInterestIDs() { return &m_objectOfInterestIDs; }

	void read_prediction_command_file(const string& filePath);
	void initialize_RSO_manager(const PredictionCommand& command);

	void load_two_line_element_set_file(const string& filePath, const int& numObjects);

	////////////////////////////////////////ONLY STARLINK////////////////////////////////////////////////
	////////////////////////////////////////ONLY STARLINK////////////////////////////////////////////////
	////////////////////////////////////////ONLY STARLINK////////////////////////////////////////////////

	void read_prediction_command_file_STARLINK(const string& filePath);
	void initialize_RSO_manager_STARLINK(const PredictionCommand& command);

	void load_two_line_element_set_file_STARLINK(const string& filePath, const int& numObjects);
	////////////////////////////////////////ONLY STARLINK////////////////////////////////////////////////
	////////////////////////////////////////ONLY STARLINK////////////////////////////////////////////////
	////////////////////////////////////////ONLY STARLINK////////////////////////////////////////////////

	elsetrec convert_TLE_to_elsetrec(char* longstr1, char* longstr2);
	
	int filter_invalid_TLEs();

	MinimalRSO* find_RSO_from_ID(const int& ID) { return m_mapFromIDToRSO.at(ID); }
	MinimalRSO* find_RSO_that_has_eccentricity_similar_to_given(const double& targetEccentricity);
	list<array<MinimalRSO*, 2>> find_danger_close_pairs(const double& threshold);

	void update_RSO_statuses_to_given_moment(const double& givenMoment);

	void save_RSO_infos(const string& filePath);

	tm convert_given_moment_to_tm(const double& givenMoment) const;

	pair<double, double> find_time_of_closest_approach_for_RSO_pair(int primaryID, int secondaryID, int targetTime, double searchInterval, double resolution);

	list<MinimalRSO*> find_object_of_interests();
	double calculate_OOI_distance();
	Circle3D calculate_circle_of_OOIs();
	Circle3D evaluate_cotangent_circle(const array<rg_Point3D, 3>& inputPts) const;

	void load_PPDB(const string& filePath);
	void load_TPDB(const string& filePath);

	string make_time_string(const double& givenMoment);

	string translate_to_window_path(const QString& QfilePath);
};


