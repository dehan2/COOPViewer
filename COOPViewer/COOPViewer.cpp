#include "COOPViewer.h"
#include <direct.h>
#include <fstream>
#include "cJulian.h"
#include <QDate>
#include <QTime>
#include <QTableView>
#include <sstream>
#include <time.h>
#include <sstream>
COOPViewer::COOPViewer(QWidget* parent)
	: QMainWindow(parent), m_simulationTimer(this)
{
    ui.setupUi(this);
	ui.ooiBox->hide();


	add_PPDB_table_header();
	ui.tableView_PPDBData->setModel(&m_PPDBModel);
	ui.tableView_PPDBData->setEditTriggers(QAbstractItemView::NoEditTriggers);
	adjust_PPDB_column_width();

	ui.tableView_PPDBData->resizeRowsToContents();
	ui.tableView_PPDBData->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView_PPDBData->setSelectionMode(QAbstractItemView::SingleSelection);




	add_TPDB_table_header();
	ui.tableView_TPDBData->setModel(&m_TPDBModel);
	ui.tableView_TPDBData->setEditTriggers(QAbstractItemView::NoEditTriggers);
	adjust_TPDB_column_width();

	ui.tableView_TPDBData->resizeRowsToContents();
	ui.tableView_TPDBData->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView_TPDBData->setSelectionMode(QAbstractItemView::SingleSelection);




	time_step_changed();

	connect(&m_simulationTimer, SIGNAL(timeout()), this, SLOT(increase_simulation_time()));
}



void COOPViewer::update_time_info()
{
	tm timeInTM = m_manager.convert_given_moment_to_tm(m_currentTime);

	QDate date;
	date.setDate(timeInTM.tm_year, timeInTM.tm_mon, timeInTM.tm_mday);

	QTime time;
	time.setHMS(timeInTM.tm_hour, timeInTM.tm_min, timeInTM.tm_sec);

	ui.dateTimeEdit_currTime->setDate(date);
	ui.dateTimeEdit_currTime->setTime(time);


	ui.openglWidget->set_current_time(m_currentTime);
	update_status_message();
	update();
}



double COOPViewer::change_time_to_given_moment(const double& givenMoment)
{
	m_currentTime = check_validity_of_given_time(givenMoment);

	update_time_info();

	m_manager.update_RSO_statuses_to_given_moment(m_currentTime);
	
	ui.openglWidget->update();

	return m_currentTime;
}



double COOPViewer::change_time_by_given_increment(const double& givenIncrement)
{
	
	m_currentTime = check_validity_of_given_time(m_currentTime+givenIncrement);

	if (m_mode == COOP_OPERATION_MODE::SPDB && m_currentTime >= 3600)
		m_currentTime = 3600;

	update_time_info();

	m_manager.update_RSO_statuses_to_given_moment(m_currentTime);
	m_starlinkManager.update_RSO_statuses_to_given_moment(m_currentTime);
	ui.openglWidget->update();

	return m_currentTime;
}



double COOPViewer::check_validity_of_given_time(const double& givenTime)
{
	double correctedTime = givenTime;
	if (correctedTime < 0)
		correctedTime = 0;
	if (correctedTime > m_manager.get_prediction_command().predictionTimeWindow)
		correctedTime = m_manager.get_prediction_command().predictionTimeWindow;
	return correctedTime;
}



void COOPViewer::update_status_message()
{
	switch (m_mode)
	{
	case COOP_OPERATION_MODE::PPDB:
		update_status_message_for_PPDB();
		break;
	case COOP_OPERATION_MODE::TPDB:
		update_status_message_for_TPDB();
		break;
	case COOP_OPERATION_MODE::SPDB:
		update_status_message_for_SPDB();
		break;
	case COOP_OPERATION_MODE::EVAL_SAFETY:
		update_status_message_for_eval_safety();
		break;
	default:
		break;
	}
}



void COOPViewer::update_status_message_for_PPDB()
{
	ui.label_printStatus->clear();
	ui.label_summary->clear();
	if (m_manager.get_objectOfInterestIDs()->size() == 2)
	{
		string msgForPPDB = generate_status_message_for_PPDB();
		ui.label_printStatus->setText(QString::fromStdString(msgForPPDB));
	}
	ui.ooiBox->show();
}



void COOPViewer::update_status_message_for_TPDB()
{
	ui.label_printStatus->clear();
	ui.label_summary->clear();
	if (m_manager.get_objectOfInterestIDs()->size() == 3)
	{
		string msgForTPDB = generate_status_message_for_TPDB();
		ui.label_printStatus->setText(QString::fromStdString(msgForTPDB));
	}
	ui.ooiBox->show();
}



void COOPViewer::update_status_message_for_SPDB()
{
	ui.label_printStatus->clear();
	ui.label_summary->clear();
	if (!m_orbitShorestLink.get_shortest_paths().empty())
	{
		string msgForSPDB = generate_status_message_for_SPDB();
		ui.label_printStatus->setText(QString::fromStdString(msgForSPDB));
	}

	/////
	int n_RSOs = m_starlinkManager.get_RSOs().size();
	string summary = "#STARLINKs: ";
	summary += to_string(n_RSOs);
	summary += "\n";
	
	/*summary += "Query elapsed time: ";
	summary += "Done with Web Server in real time";*/
	
	ui.label_summary->setText(QString::fromStdString(summary));
	/////



	ui.ooiBox->hide();
}



void COOPViewer::update_status_message_for_eval_safety()
{
	ui.label_printStatus->clear();
	ui.label_summary->clear();
	if (true)
	{
		string msgForSPDB = generate_status_message_for_eval_safety();
		ui.label_printStatus->setText(QString::fromStdString(msgForSPDB));
	}
	ui.ooiBox->hide();
}



void COOPViewer::add_PPDB_table_header()
{
	m_PPDBModel.setColumnCount(NUM_COL_PPDB);
	m_PPDBModel.setHeaderData(COL_PPDB_PRIMARY, Qt::Horizontal, QObject::tr("Primary"));
	m_PPDBModel.setHeaderData(COL_PPDB_SECONDARY, Qt::Horizontal, QObject::tr("Secondary"));
	m_PPDBModel.setHeaderData(COL_PPDB_DCA, Qt::Horizontal, QObject::tr("DCA(km)"));
	m_PPDBModel.setHeaderData(COL_PPDB_TCA, Qt::Horizontal, QObject::tr("TCA"));
	m_PPDBModel.setHeaderData(COL_PPDB_CASTART, Qt::Horizontal, QObject::tr("CA Start"));
	m_PPDBModel.setHeaderData(COL_PPDB_CAEND, Qt::Horizontal, QObject::tr("CA End"));
}



void COOPViewer::adjust_PPDB_column_width()
{
	ui.tableView_PPDBData->setColumnWidth(COL_PPDB_PRIMARY, 100);
	ui.tableView_PPDBData->setColumnWidth(COL_PPDB_SECONDARY, 100);
	ui.tableView_PPDBData->setColumnWidth(COL_PPDB_DCA, 100);
	ui.tableView_PPDBData->setColumnWidth(COL_PPDB_TCA, 150);
	ui.tableView_PPDBData->setColumnWidth(COL_PPDB_CASTART, 150);
	ui.tableView_PPDBData->setColumnWidth(COL_PPDB_CAEND, 150);
}



void COOPViewer::update_PPDB_table()
{
	int currRow = 0;
	m_mapFromPPDBRowToTCAReport.clear();
	m_PPDBModel.clear();
	
	add_PPDB_table_header();
	ui.tableView_PPDBData->setModel(&m_PPDBModel);
	ui.tableView_PPDBData->setEditTriggers(QAbstractItemView::NoEditTriggers);
	adjust_PPDB_column_width();

	for (auto& entity : m_manager.get_PPDB_infos())
	{
		if (m_queryIndex == 1 &&
			(entity.primaryID != ui.q1_id->text().toInt() && entity.secondaryID != ui.q1_id->text().toInt()))
			continue;
		if (m_queryIndex == 1 && entity.distanceOfClosestApproach > ui.q1_distance->text().toDouble())
			continue;
		if (m_queryIndex == 2 && entity.distanceOfClosestApproach > ui.q2_distance->text().toDouble())
			continue;
		QStandardItem* currItem = new QStandardItem(1, NUM_COL_PPDB);
		m_PPDBModel.insertRow(currRow, currItem);

		QModelIndex primaryIndex = m_PPDBModel.index(currRow, COL_PPDB_PRIMARY);
		QModelIndex secondaryIndex = m_PPDBModel.index(currRow, COL_PPDB_SECONDARY);
		
		QModelIndex DCAIndex = m_PPDBModel.index(currRow, COL_PPDB_DCA);
		QModelIndex TCAIndex = m_PPDBModel.index(currRow, COL_PPDB_TCA);
		QModelIndex CAStartIndex = m_PPDBModel.index(currRow, COL_PPDB_CASTART);
		QModelIndex CAEndIndex = m_PPDBModel.index(currRow, COL_PPDB_CAEND);

		string TCAStr = m_manager.make_time_string(entity.timeOfClosestApproach);
		string CAStartStr = m_manager.make_time_string(entity.closeApproachEnteringTime);
		string CAEndStr = m_manager.make_time_string(entity.closeApproachLeavingTime);

		m_PPDBModel.setData(primaryIndex, QString::number(entity.primaryID));
		m_PPDBModel.setData(secondaryIndex, QString::number(entity.secondaryID));
		m_PPDBModel.setData(DCAIndex, QString::number(entity.distanceOfClosestApproach));
		m_PPDBModel.setData(TCAIndex, QString::fromStdString(TCAStr));
		m_PPDBModel.setData(CAStartIndex, QString::fromStdString(CAStartStr));
		m_PPDBModel.setData(CAEndIndex, QString::fromStdString(CAEndStr));

		m_mapFromPPDBRowToTCAReport[currRow] = &entity;

		currRow++;
	}


	//ui.tableView_PPDBData->resizeRowsToContents();
}

void COOPViewer::update_PPDB_n_TPDB_table_Q1()
{
	clock_t start, end; 
	double result; 
	start = clock(); 
	// 수행 시간 측정 시작 /* 수행시간 측정하고자 하는 코드 */ 

	m_queryIndex = 1;
	update_PPDB_table();
	update_TPDB_table();

	end = clock(); //시간 측정 끝 
	result = (double)(end - start); // 결과 출력 
	result = ((result) / CLOCKS_PER_SEC);
	int n_RSOs = m_manager.get_RSOs().size();
	string summary = "#RSOs: ";
	summary += to_string(n_RSOs);
	summary += "\n";
	summary += "Query elapsed time: ";

	std::ostringstream out;
	out.precision(3);
	out << std::fixed << result;

	summary += out.str();
	summary += "sec";
	ui.label_summary->setText(QString::fromStdString(summary));
}

void COOPViewer::update_PPDB_n_TPDB_table_Q2()
{
	clock_t start, end;
	double result;
	start = clock();
	// 수행 시간 측정 시작 /* 수행시간 측정하고자 하는 코드 */ 

	m_queryIndex = 2;
	update_PPDB_table();
	update_TPDB_table();

	end = clock(); //시간 측정 끝 
	result = (double)(end - start); // 결과 출력 
	result = ((result) / CLOCKS_PER_SEC);
	int n_RSOs = m_manager.get_RSOs().size();
	string summary = "#RSOs: ";
	summary += to_string(n_RSOs);
	summary += "\n";
	summary += "Query elapsed time: ";

	std::ostringstream out;
	out.precision(3);
	out << std::fixed << result;

	summary += out.str();
	summary += "sec";
	ui.label_summary->setText(QString::fromStdString(summary));
}



void COOPViewer::add_TPDB_table_header()
{
	m_TPDBModel.setColumnCount(5);
	m_TPDBModel.setHeaderData(0, Qt::Horizontal, QObject::tr("Primary"));
	m_TPDBModel.setHeaderData(1, Qt::Horizontal, QObject::tr("Secondary"));
	m_TPDBModel.setHeaderData(2, Qt::Horizontal, QObject::tr("Tertiary"));
	m_TPDBModel.setHeaderData(3, Qt::Horizontal, QObject::tr("Radius(km)"));
	m_TPDBModel.setHeaderData(4, Qt::Horizontal, QObject::tr("Time"));
}



void COOPViewer::adjust_TPDB_column_width()
{
	ui.tableView_TPDBData->setColumnWidth(0, 100);
	ui.tableView_TPDBData->setColumnWidth(1, 100);
	ui.tableView_TPDBData->setColumnWidth(2, 100);
	ui.tableView_TPDBData->setColumnWidth(3, 100);
	ui.tableView_TPDBData->setColumnWidth(4, 150);
}



void COOPViewer::update_TPDB_table()
{
	int currRow = 0;
	m_mapFromTPDBRowToReport.clear();

	m_TPDBModel.clear();

	add_TPDB_table_header();
	ui.tableView_TPDBData->setModel(&m_TPDBModel);
	ui.tableView_TPDBData->setEditTriggers(QAbstractItemView::NoEditTriggers);
	adjust_TPDB_column_width();




	for (auto& entity : m_manager.get_TPDB_infos())
	{
		if (m_queryIndex == 1 &&
			(entity.primaryID != ui.q1_id->text().toInt() && entity.secondaryID != ui.q1_id->text().toInt() && entity.tertiaryID != ui.q1_id->text().toInt()))
			continue;
		if (m_queryIndex == 1 && entity.minRadius > ui.q1_distance->text().toDouble())
			continue;
		if (m_queryIndex == 2 && entity.minRadius > ui.q2_distance->text().toDouble())
			continue;


		QStandardItem* currItem = new QStandardItem(1, 5);
		m_TPDBModel.insertRow(currRow, currItem);

		QModelIndex primaryIndex = m_TPDBModel.index(currRow, 0);
		QModelIndex secondaryIndex = m_TPDBModel.index(currRow, 1);
		QModelIndex tertiaryIndex = m_TPDBModel.index(currRow, 2);
		QModelIndex radiusIndex = m_TPDBModel.index(currRow, 3);
		QModelIndex timeIndex = m_TPDBModel.index(currRow, 4);


		string timeStr = m_manager.make_time_string(entity.time);

		m_TPDBModel.setData(primaryIndex, QString::number(entity.primaryID));
		m_TPDBModel.setData(secondaryIndex, QString::number(entity.secondaryID));
		m_TPDBModel.setData(tertiaryIndex, QString::number(entity.tertiaryID));
		m_TPDBModel.setData(radiusIndex, QString::number(entity.minRadius));
		m_TPDBModel.setData(timeIndex, QString::fromStdString(timeStr));

		m_mapFromTPDBRowToReport[currRow] = &entity;

		currRow++;
	}
}


void COOPViewer::modify_objects_of_interest(const list<int>& OOIs)
{
	list<int>* OOIIDs = m_manager.get_objectOfInterestIDs();
	OOIIDs->clear();

	for (auto& OOI : OOIs)
	{
		OOIIDs->push_back(OOI);
	}

	update_status_message();
	ui.openglWidget->change_view_to_OOI_direction();
}



string COOPViewer::generate_status_message_for_PPDB()
{
	stringstream message;

	const list<int>* OOIs = m_manager.get_objectOfInterestIDs();
	int primaryID = OOIs->front();
	int secondaryID = OOIs->back();
	double distance = m_manager.calculate_OOI_distance();

	message << "Primary: " << primaryID << ", Secondary: " << secondaryID << ", distance: " <<distance<< "km\n";

	return message.str();
}



string COOPViewer::generate_status_message_for_TPDB()
{
	stringstream message;

	const list<int>* OOIs = m_manager.get_objectOfInterestIDs();
	double radius = m_manager.calculate_circle_of_OOI_radius();

	auto it = OOIs->begin();

	message << "Primary: " << (*it++) << ", Secondary: " << (*it++) << ", Tertiary: "<< (*it++)<<", radius: " << radius<<"km\n";

	return message.str();
}



string COOPViewer::generate_status_message_for_SPDB()
{
	stringstream message;

	auto imminentShortestPath = m_orbitShorestLink.find_shortest_path_imminent_to_moment(m_currentTime);
	message << "Path length: " << imminentShortestPath->pathLength<<"\n"
			<< "Start (red ball): SEOUL - Hanyang University" << "\n"
			<< "End (pink ball): San Francisco - Google" << "\n";


	return message.str();
}



string COOPViewer::generate_status_message_for_eval_safety()
{


	/// <summary>
	/// SO WHAT
	/// </summary>
	/// <returns></returns>
	return string();
}




void COOPViewer::change_mode_selection()
{
	ui.radioButton_selectPPDB->setChecked(false);
	ui.radioButton_selectTPDB->setChecked(false);
	ui.radioButton_selectSPDB->setChecked(false);
	ui.radioButton_selectSafetyEval->setChecked(false);

	switch (m_mode)
	{
	case COOP_OPERATION_MODE::PPDB:
		ui.radioButton_selectPPDB->setChecked(true);
		break;
	case COOP_OPERATION_MODE::TPDB:
		ui.radioButton_selectTPDB->setChecked(true);
		break;
	case COOP_OPERATION_MODE::SPDB:
		ui.radioButton_selectSPDB->setChecked(true);
		break;
	case COOP_OPERATION_MODE::EVAL_SAFETY:
		ui.radioButton_selectSafetyEval->setChecked(true);
		break;
	default:
		break;
	}
}




//////////////////////////////////////////////////////////////////////////
// QT SLOTS
//////////////////////////////////////////////////////////////////////////


void COOPViewer::load_prediction_command()
{
	QString QfilePath = QFileDialog::getOpenFileName(this, tr("Open Prediction Command File"), NULL, tr("Prediction Command file (*.coop)"));
	string filePath = translate_to_window_path(QfilePath);

	m_manager.read_prediction_command_file(filePath);
	ui.openglWidget->set_manager(&m_manager);
	ui.openglWidget->set_mode(&m_mode);
	ui.openglWidget->update();
	
	char cwd[1024];
	_getcwd(cwd, sizeof(cwd));
	string s_cwd(cwd);
	m_manager.load_PPDB(s_cwd + string("\\result\\PPDB.txt"));
	update_PPDB_table();
	m_manager.load_TPDB(s_cwd + string("\\result\\TPDB.txt"));
	update_TPDB_table();
	

	m_starlinkManager.read_prediction_command_file(s_cwd + string("\\result\\starlink_PredictionCommand.txt"));
	ui.openglWidget->pStarlinkManager = &m_starlinkManager;
	m_orbitShorestLink.load_orbit_shortest_link(s_cwd + string("\\result\\SPDB.txt"));
	ui.openglWidget->set_shortest_links(&m_orbitShorestLink);


	m_orbitTunnel.load_orbit_tunnel2(s_cwd + string("\\result\\SEDB.txt"));
	ui.openglWidget->set_orbit_tunnel(&m_orbitTunnel);
	ui.openglWidget->update();


	update_time_info();
	update();
}



void COOPViewer::load_PPDB_file()
{
	QString QfilePath = QFileDialog::getOpenFileName(this, tr("Open Prediction Command File"), NULL, tr("Prediction Command file (*.txt)"));
	string filePath = translate_to_window_path(QfilePath);

	m_manager.load_PPDB(filePath);
	update_PPDB_table();

	update();
}



void COOPViewer::load_TPDB_file()
{
	QString QfilePath = QFileDialog::getOpenFileName(this, tr("Open Prediction Command File"), NULL, tr("Prediction Command file (*.txt)"));
	string filePath = translate_to_window_path(QfilePath);

	m_manager.load_TPDB(filePath);
	update_TPDB_table();

	update();
}



void COOPViewer::play_simulation()
{
	if (m_simulationTimer.isActive() == false)
	{
		m_simulationTimer.start(100);
	}
	else
	{
		m_simulationTimer.stop();
	}
}

void COOPViewer::load_starlink()
{
	QString QfilePath = QFileDialog::getOpenFileName(this, tr("Open Prediction Command File"), NULL, tr("Prediction Command file (*.txt)"));
	string filePath = translate_to_window_path(QfilePath);

	m_manager.read_prediction_command_file(filePath);
	//m_manager.read_prediction_command_file_STARLINK(filePath);
	ui.openglWidget->set_manager(&m_manager);
	ui.openglWidget->update();

	update_time_info();
	update();
}

void COOPViewer::load_shortest_link()
{
	
	QString QfilePath = QFileDialog::getOpenFileName(this, tr("Open Prediction Command File"), NULL, tr("Prediction Command file (*.txt)"));
	string filePath = translate_to_window_path(QfilePath);
	m_orbitShorestLink.load_orbit_shortest_link(filePath);
	
	ui.openglWidget->set_shortest_links(&m_orbitShorestLink);
	ui.openglWidget->update();

	update_time_info();
	update();
}

void COOPViewer::load_orbit_tunnel()
{
	QString QfilePath = QFileDialog::getOpenFileName(this, tr("Open Prediction Command File"), NULL, tr("Prediction Command file (*.txt)"));
	string filePath = translate_to_window_path(QfilePath);

	//m_orbitTunnel.load_orbit_tunnel(filePath);

	m_orbitTunnel.load_orbit_tunnel2(filePath);
	ui.openglWidget->set_orbit_tunnel(&m_orbitTunnel);
	ui.openglWidget->update();

	update_time_info();
	update();

}



void COOPViewer::increase_simulation_time()
{
	change_time_by_given_increment(m_stepSize / 10);
	update();
}




void COOPViewer::time_step_changed()
{
	double sec = ui.doubleSpinBox_stepSize_sec->value();
	int min = ui.spinBox_stepSize_min->value();
	int hour = ui.spinBox_stepSize_hour->value();

	m_stepSize = sec + min * 60 + hour * 3600;
}



void COOPViewer::increase_time_by_step()
{
	change_time_by_given_increment(m_stepSize);
}



void COOPViewer::decrease_time_by_step()
{
	change_time_by_given_increment(-m_stepSize);
}



void COOPViewer::update_PPDB_selection_in_table(QModelIndex selectedRow)
{
	m_mode = COOP_OPERATION_MODE::PPDB;
	change_mode_selection();

	const TCAReport* selectedEntity = m_mapFromPPDBRowToTCAReport.at(selectedRow.row());

	double TCA = selectedEntity->timeOfClosestApproach;
	change_time_to_given_moment(TCA);

	list<int> OOIs;
	OOIs.push_back(selectedEntity->primaryID);
	OOIs.push_back(selectedEntity->secondaryID);
	modify_objects_of_interest(OOIs);

	update();
}



void COOPViewer::update_TPDB_selection_in_table(QModelIndex selectedRow)
{
	m_mode = COOP_OPERATION_MODE::TPDB;
	change_mode_selection();

	const TPDBReport* selectedEntity = m_mapFromTPDBRowToReport.at(selectedRow.row());

	double time = selectedEntity->time;
	change_time_to_given_moment(time);

	list<int> OOIs;
	OOIs.push_back(selectedEntity->primaryID);
	OOIs.push_back(selectedEntity->secondaryID);
	OOIs.push_back(selectedEntity->tertiaryID);
	modify_objects_of_interest(OOIs);

	update();
}



void COOPViewer::mode_selection_changed()
{
	if (ui.radioButton_selectPPDB->isChecked())
	{
		m_mode = COOP_OPERATION_MODE::PPDB;

	}
	else if (ui.radioButton_selectTPDB->isChecked())
	{
		m_mode = COOP_OPERATION_MODE::TPDB;

	}

	else if (ui.radioButton_selectSPDB->isChecked())
	{
		m_mode = COOP_OPERATION_MODE::SPDB;

	}

	else if (ui.radioButton_selectSafetyEval->isChecked())
	{
		m_mode = COOP_OPERATION_MODE::EVAL_SAFETY;

	}

	update_status_message();
	ui.openglWidget->change_view_to_OOI_direction();
	ui.openglWidget->update();
}
