#pragma once
#include "VDRCOpenGLWidget.h"
#include <QObject>
#include <QWidget>

class RSOManager;

class COOPViewerWidget : public VDRCOpenGLWidget
{
	Q_OBJECT

private:
	RSOManager* pManager = nullptr;

public:
	explicit COOPViewerWidget(QWidget* parent = 0);
	virtual ~COOPViewerWidget();

	void set_manager(RSOManager* manager) { pManager = manager; }

	virtual void draw();

	virtual void mousePressEvent(QMouseEvent* event);
	virtual void process_picking(const int& hits, const GLuint* selectBuff);

	void update();
};
