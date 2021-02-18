#pragma once
#include "VDRCOpenGLWidget.h"
#include <QObject>
#include <QWidget>

class RSOManager;
class OrbitShortestLink;
class OrbitTunnel;
class COOPViewerWidget : public VDRCOpenGLWidget
{
	Q_OBJECT

private:
	RSOManager* pManager = nullptr;
	
	OrbitShortestLink* pShortestLink = nullptr;
	double m_currentTime = 0.;

	OrbitTunnel* pOrbitTunnel = nullptr;

public:
	explicit COOPViewerWidget(QWidget* parent = 0);
	virtual ~COOPViewerWidget();

	void set_manager(RSOManager* manager) { pManager = manager; }
	void set_shortest_links(OrbitShortestLink* shortestLink) { pShortestLink = shortestLink; }
	void set_orbit_tunnel(OrbitTunnel* orbitTunnel) {	pOrbitTunnel = orbitTunnel;	}

	virtual void draw();

	virtual void mousePressEvent(QMouseEvent* event);
	virtual void process_picking(const int& hits, const GLuint* selectBuff);


	void set_current_time(const double& time);
	void draw_line_among_OOIs();
	void change_view_to_OOI_direction();

};

inline void COOPViewerWidget::set_current_time(const double& time)
{
	m_currentTime = time;
}