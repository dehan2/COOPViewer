#include "COOPViewer.h"

#include <fstream>
#include "cJulian.h"
#include <QDate>
#include <QTime>
#include <QTableView>

COOPViewer::COOPViewer(QWidget* parent)
	: QMainWindow(parent), m_simulationTimer(this)
{
    ui.setupUi(this);

	add_PPDB_table_header();
	ui.tableView_PPDBData->setModel(&m_PPDBModel);
	ui.tableView_PPDBData->setEditTriggers(QAbstractItemView::NoEditTriggers);
	adjust_PPDB_column_width();

	//ui.tableView_PPDBData->resizeColumnsToContents();
	ui.tableView_PPDBData->resizeRowsToContents();
	ui.tableView_PPDBData->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView_PPDBData->setSelectionMode(QAbstractItemView::SingleSelection);

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
	update_distance_of_OOI_string();
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

	update_time_info();

	m_manager.update_RSO_statuses_to_given_moment(m_currentTime);
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



void COOPViewer::update_distance_of_OOI_string()
{
	double distance = m_manager.calculate_OOI_distance();
	string distanceStr = to_string(distance) + string(" km");
	ui.label_distanceOfOOI->setText(QString::fromStdString(distanceStr));
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
	ui.tableView_PPDBData->setColumnWidth(COL_PPDB_TCA, 120);
	ui.tableView_PPDBData->setColumnWidth(COL_PPDB_CASTART, 120);
	ui.tableView_PPDBData->setColumnWidth(COL_PPDB_CAEND, 120);
}



void COOPViewer::update_PPDB_table()
{
	int currRow = 0;
	m_mapFromPPDBRowToTCAReport.clear();

	for (auto& entity : m_manager.get_PPDB_infos())
	{
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

	//ui.tableView_PPDBData->resizeColumnsToContents();
	//ui.tableView_PPDBData->resizeRowsToContents();
}



//////////////////////////////////////////////////////////////////////////
// QT SLOTS
//////////////////////////////////////////////////////////////////////////


void COOPViewer::load_prediction_command()
{
	QString QfilePath = QFileDialog::getOpenFileName(this, tr("Open Prediction Command File"), NULL, tr("Prediction Command file (*.txt)"));
	string filePath = translate_to_window_path(QfilePath);

	m_manager.read_prediction_command_file(filePath);
	ui.openglWidget->set_manager(&m_manager);
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

	m_manager.read_prediction_command_file_STARLINK(filePath);
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

	m_orbitTunnel.load_orbit_tunnel(filePath);
	
	
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



void COOPViewer::objectOfInterest_changed()
{
	list<int>* OOIIDs = m_manager.get_objectOfInterestIDs();
	OOIIDs->clear();

	int primaryID = ui.lineEdit_primaryID->text().toInt();	
	int secondaryID = ui.lineEdit_secondaryID->text().toInt();
	OOIIDs->push_back(primaryID);
	OOIIDs->push_back(secondaryID);

	update_distance_of_OOI_string();
	ui.openglWidget->change_view_to_OOI_direction();

	update();
}



void COOPViewer::PPDB_row_selected(QModelIndex index)
{
	int selectedRow = index.row();
	const TCAReport* selectedEntity = m_mapFromPPDBRowToTCAReport.at(selectedRow);

	double TCA = selectedEntity->timeOfClosestApproach;
	change_time_to_given_moment(TCA);

	ui.lineEdit_primaryID->setText(QString::number(selectedEntity->primaryID));
	ui.lineEdit_secondaryID->setText(QString::number(selectedEntity->secondaryID));
	objectOfInterest_changed();

	update();
}
