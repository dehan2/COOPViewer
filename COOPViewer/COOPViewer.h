#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_COOPViewer.h"

class COOPViewer : public QMainWindow
{
    Q_OBJECT

public:
    COOPViewer(QWidget *parent = Q_NULLPTR);

private:
    Ui::COOPViewerClass ui;
};
