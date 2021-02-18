#include "COOPViewer.h"

#include <fstream>
#include "cJulian.h"
#include <QDate>
#include <QTime>

COOPViewer::COOPViewer(QWidget* parent)
	: QMainWindow(parent), m_simulationTimer(this)
{
    ui.setupUi(this);

	connect(&m_simulationTimer, SIGNAL(timeout()), this, SLOT(increase_simulation_time()));
}



void COOPViewer::update_time_info()
{
	tm timeInTM = m_manager.convert_given_moment_to_tm(m_currentTime);

	QDate date;
	date.setDate(timeInTM.tm_year, timeInTM.tm_mon + 1, timeInTM.tm_mday);

	QTime time;
	time.setHMS(timeInTM.tm_hour, timeInTM.tm_min, timeInTM.tm_sec);

	ui.dateTimeEdit_currTime->setDate(date);
	ui.dateTimeEdit_currTime->setTime(time);

	ui.openglWidget->set_current_time(m_currentTime);
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
	


	//m_shortest_links = { 44238, 47145, 47164, 47175, 47181, 47144, 47128, 46137, 47147, 44968, 45737, 46774, 44928, 47372, 47381 };
	/////TODO
	//ui.openglWidget->set_shortest_links(m_shortest_links); -> 
	/////TODO
	ui.openglWidget->set_shortest_links(&m_orbitShorestLink);
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