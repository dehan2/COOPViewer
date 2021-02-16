#pragma once

#include <QWidget>
#include "ui_COOPWidget.h"

class COOPWidget : public QWidget
{
	Q_OBJECT

public:
	COOPWidget(QWidget *parent = Q_NULLPTR);
	~COOPWidget();

private:
	Ui::COOPWidget ui;
};
