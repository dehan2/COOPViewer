#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_COOPViewer.h"

#include "constForCOOP.h"
#include "RSOManager.h"
#include <QTimer>

class COOPViewer : public QMainWindow
{
    Q_OBJECT

public:
    COOPViewer(QWidget *parent = Q_NULLPTR);
    RSOManager m_manager;

    double m_currentTime = 0;

    bool m_bOnPlay = false;
    QTimer m_simulationTimer;
    double m_simulationSpeed = 1;

private:
    Ui::COOPViewerClass ui;

public slots:
    void load_prediction_command();
    void play_simulation();

    void increase_simulation_time();
    void update_time_info();
};
