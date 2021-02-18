#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_COOPViewer.h"

#include "constForCOOP.h"
#include "RSOManager.h"
#include "OrbitShortestLink.h"
#include <QTimer>
#include <QStandardItemModel>
#include <map>

#include <vector>

using std::vector;

class COOPViewer : public QMainWindow
{
    Q_OBJECT

public:
    COOPViewer(QWidget *parent = Q_NULLPTR);
    RSOManager m_manager;
    
    OrbitShortestLink m_orbitShorestLink;

    double m_currentTime = 0;

    bool m_bOnPlay = false;
    QTimer m_simulationTimer;
    double m_stepSize = 1;

    int milisecPerAnimation = 100;


    vector<int> m_shortest_links;

    QStandardItemModel m_PPDBModel;
    int targetRSOID = -1;

    map<int, const TCAReport*> m_mapFromPPDBRowToTCAReport;


public:
    void update_time_info();
    double change_time_to_given_moment(const double& givenMoment);
    double change_time_by_given_increment(const double& givenIncrement);
    double check_validity_of_given_time(const double& givenTime);

    void update_distance_of_OOI_string();

    void add_PPDB_table_header();
    void adjust_PPDB_column_width();
    void update_PPDB_table();

private:
    Ui::COOPViewerClass ui;

public slots:
    void load_prediction_command();
    void load_PPDB_file();
    void play_simulation();

    
    void load_starlink();
    void load_shortest_link();
    

	void increase_simulation_time();
    void time_step_changed();

    void increase_time_by_step();
    void decrease_time_by_step();

    inline void go_to_start_moment() { change_time_to_given_moment(0); }
    inline void go_to_end_moment() { change_time_to_given_moment(m_manager.get_prediction_command().predictionTimeWindow); }

    void objectOfInterest_changed();
    void PPDB_row_selected(QModelIndex index);
};
