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



void COOPViewer::load_prediction_command()
{
	QString QfilePath = QFileDialog::getOpenFileName(this, tr("Open Prediction Command File"), NULL, tr("Prediction Command file (*.txt)"));
	string filePath = translate_to_window_path(QfilePath);

	m_manager.read_prediction_command_file(filePath);
	ui.openglWidget->set_manager(&m_manager);
	update();
	ui.openglWidget->update();
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



void COOPViewer::increase_simulation_time()
{
	m_currentTime += m_simulationSpeed;
	//update Time
}

void COOPViewer::update_time_info()
{
	cJulian julianTime = m_manager.get_epoch();
	julianTime.AddSec(m_currentTime);

	QDate date;
	date.setDate(julianTime)
}
