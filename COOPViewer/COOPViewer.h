#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_COOPViewer.h"

#include "constForCOOP.h"
#include "RSOManager.h"
#include "OrbitShortestLink.h"
#include "OrbitTunnel.h"
#include "OrbitClosestNeighbor.h"
#include <QTimer>
#include <QStandardItemModel>
#include <map>
#include <string>
#include <vector>
#include <list>

using std::list;
using std::vector;

class COOPViewer : public QMainWindow
{
    Q_OBJECT

public:
    COOPViewer(QWidget *parent = Q_NULLPTR);
    RSOManager m_manager;
    RSOManager m_starlinkManager;
    OrbitShortestLink m_orbitShorestLink;
    OrbitClosestNeighbor m_orbitClosestNeighbor;
    
    
    OrbitTunnel m_orbitTunnel_B;
    OrbitTunnel m_orbitTunnel_C;
    OrbitTunnel m_orbitTunnel_N;

    double m_currentTime = 0;

    bool m_bOnPlay = false;
    QTimer m_simulationTimer;
    double m_stepSize = 1;

    int milisecPerAnimation = 100;


    QStandardItemModel m_PPDBModel;
    QStandardItemModel m_TPDBModel;
    int targetRSOID = -1;

    map<int, const TCAReport*> m_mapFromPPDBRowToTCAReport;
    map<int, const TPDBReport*> m_mapFromTPDBRowToReport;

    COOP_OPERATION_MODE m_mode = COOP_OPERATION_MODE::PPDB;

    int m_queryIndex = 0;

public:
    void update_time_info();
    double change_time_to_given_moment(const double& givenMoment);
    double change_time_by_given_increment(const double& givenIncrement);
    double check_validity_of_given_time(const double& givenTime);

    void update_status_message();
    void update_status_message_for_PPDB();
    void update_status_message_for_TPDB();
    void update_status_message_for_SPDB();
    void update_status_message_for_eval_safety();
    void update_status_message_for_CN();

    void add_PPDB_table_header();
    void adjust_PPDB_column_width();
    void update_PPDB_table();

    

	void add_TPDB_table_header();
	void adjust_TPDB_column_width();
	void update_TPDB_table();



    void modify_objects_of_interest(const list<int>& OOIs);
    
    string generate_status_message_for_PPDB();
    string generate_status_message_for_TPDB();
    string generate_status_message_for_SPDB();
    string generate_status_message_for_eval_safety();
    string generate_status_message_for_CN();

    void change_mode_selection();

private:
    Ui::COOPViewerClass ui;

public slots:
    void load_prediction_command();
    void load_PPDB_file();
    void load_TPDB_file();
    void play_simulation();

    
    void load_starlink();
    void load_shortest_link();
    
    void load_orbit_tunnel();

	void increase_simulation_time();
    void time_step_changed();

    void increase_time_by_step();
    void decrease_time_by_step();

    inline void go_to_start_moment() { change_time_to_given_moment(0); }
    inline void go_to_end_moment() { change_time_to_given_moment(m_manager.get_prediction_command().predictionTimeWindow); }

	void update_PPDB_selection_in_table(QModelIndex selectedRow);
	void update_TPDB_selection_in_table(QModelIndex selectedRow);

    void mode_selection_changed();

    void space_center_selection_changed_Q4();

    void update_PPDB_n_TPDB_table_Q1();
    void update_PPDB_n_TPDB_table_Q2();

    void on_all_RSOs();
    void on_criticial_RSOs();

};
