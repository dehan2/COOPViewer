#pragma once
#include "VDRCOpenGLWidget.h"
#include "constForCOOP.h"
#include <QObject>
#include <QWidget>

class RSOManager;
class OrbitShortestLink;
class OrbitTunnel;
class OrbitClosestNeighbor;
class COOPViewer;

class COOPViewerWidget : public VDRCOpenGLWidget
{
	Q_OBJECT

private:
	RSOManager* pManager = nullptr;
	
	OrbitShortestLink* pShortestLink = nullptr;
	OrbitTunnel* pOrbitTunnel_B = nullptr;
	OrbitTunnel* pOrbitTunnel_C = nullptr;
	OrbitTunnel* pOrbitTunnel_N = nullptr;
	OrbitClosestNeighbor* pOrbitClosestNeighbor = nullptr;

	COOP_OPERATION_MODE* pMode = nullptr;
	double m_currentTime = 0.;
	



public:
	bool allRSOs = true;
	bool ciriticRSOs = false;
	int m_spaceCenterCode = 0;

	RSOManager* pStarlinkManager = nullptr;
	explicit COOPViewerWidget(QWidget* parent = 0);
	virtual ~COOPViewerWidget();

	void set_manager(RSOManager* manager) { pManager = manager; }
	void set_mode(COOP_OPERATION_MODE* mode) { pMode = mode; }
	void set_shortest_links(OrbitShortestLink* shortestLink) { pShortestLink = shortestLink; }
	
	
	inline void set_orbit_tunnel_B(OrbitTunnel* orbitTunnel) { pOrbitTunnel_B = orbitTunnel; }
	inline void set_orbit_tunnel_C(OrbitTunnel* orbitTunnel) { pOrbitTunnel_C = orbitTunnel; }
	inline void set_orbit_tunnel_N(OrbitTunnel* orbitTunnel) { pOrbitTunnel_N = orbitTunnel; }
	inline void set_orbit_closest_neighbor(OrbitClosestNeighbor* orbitClosestNeighbor) {
		pOrbitClosestNeighbor = orbitClosestNeighbor;
	}

	virtual void draw();

	void draw_total_RSOs();
	void draw_PPDB();
	void draw_TPDB();
	void draw_SPDB();
	void draw_CN();
	void draw_eval_safety();
	

	virtual void mousePressEvent(QMouseEvent* event);
	virtual void process_picking(const int& hits, const GLuint* selectBuff);


	void set_current_time(const double& time);
	void draw_line_among_OOIs();
	void draw_circle_among_OOIs();
	void change_view_to_OOI_direction();
	void change_view_to_target_RSO(rg_Point3D center);


};

inline void COOPViewerWidget::set_current_time(const double& time)
{
	m_currentTime = time;
}