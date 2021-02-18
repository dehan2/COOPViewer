#pragma once
#include "VDRCOpenGLWidget.h"
#include <QObject>
#include <QWidget>

class RSOManager;
class OrbitShortestLink;

class COOPViewerWidget : public VDRCOpenGLWidget
{
	Q_OBJECT

private:
	RSOManager* pManager = nullptr;
	OrbitShortestLink* pShortestLink = nullptr;
	double m_currentTime = 0.;

public:
	explicit COOPViewerWidget(QWidget* parent = 0);
	virtual ~COOPViewerWidget();

	void set_manager(RSOManager* manager) { pManager = manager; }
	void set_shortest_links(OrbitShortestLink* shortestLink) { pShortestLink = shortestLink; }

	virtual void draw();

	virtual void mousePressEvent(QMouseEvent* event);
	virtual void process_picking(const int& hits, const GLuint* selectBuff);

<<<<<<< HEAD
	void set_current_time(const double& time);
=======
	void draw_line_among_OOIs();
	void change_view_to_OOI_direction();
>>>>>>> 93c981962ed8f4b646aaf8b22d4191050a2357a3
};

inline void COOPViewerWidget::set_current_time(const double& time)
{
	m_currentTime = time;
}