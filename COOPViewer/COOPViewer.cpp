#include "COOPViewer.h"

#include <fstream>

COOPViewer::COOPViewer(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}



void COOPViewer::load_prediction_command()
{
	QString QfilePath = QFileDialog::getOpenFileName(this, tr("Open Prediction Command File"), NULL, tr("Prediction Command file (*.txt)"));
	string filePath = translate_to_window_path(QfilePath);

	m_manager.read_prediction_command_file(filePath);
	update();
	ui.openglWidget->update();
}
