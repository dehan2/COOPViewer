#include "COOPViewerWidget.h"
#include "rg_Point3D.h"
#include "RSOManager.h"
#include "MinimalRSO.h"
#include "OrbitShortestLink.h"

COOPViewerWidget::COOPViewerWidget(QWidget* parent)
	: VDRCOpenGLWidget(parent)
{
	set_eye_distance(200);
}

COOPViewerWidget::~COOPViewerWidget()
{
}


void COOPViewerWidget::draw()
{
	//Drawing scale: 1/100

	draw_sphere(rg_Point3D(0, 0, 0), 64, BLUE);
	

	if (pManager != nullptr && pManager->get_RSOs().empty() == false)
	{
		for (auto& rso : pManager->get_RSOs())
		{
			rg_Point3D& coord = rso.get_coord();
			draw_point(coord / 100, 5, GREEN);
		}
	}


	
	if(pShortestLink != nullptr)
	{
		const std::list<int> momentNShortestLinkRSOsID = pShortestLink->find_shortest_link_RSOs_ID_in_closest_moment(m_currentTime);
		std::list<rg_Point3D> shortestLinkCoord;


		for (auto& rsoID : momentNShortestLinkRSOsID)
		{
			MinimalRSO* currRSO = pManager->find_RSO_from_ID(rsoID);
			shortestLinkCoord.push_back(currRSO->get_coord());
		}

		rg_Point3D srcCoord = pShortestLink->get_src_ground_coordinate().getCenter();
		rg_Point3D destCoord = pShortestLink->get_dest_ground_coordinate().getCenter();
		rg_Point3D& lastCoord = shortestLinkCoord.front();

		draw_sphere(srcCoord / 100, 1, RED);
		draw_line(srcCoord / 100, lastCoord / 100, 5, RED);
		for (auto& rsoCoord : shortestLinkCoord)
		{
			draw_line(lastCoord / 100, rsoCoord / 100, 5, RED);
			lastCoord = rsoCoord;
		}
		draw_line(lastCoord / 100, destCoord / 100, 5, RED);
		draw_sphere(destCoord / 100, 1, PINK);
	}
}



void COOPViewerWidget::mousePressEvent(QMouseEvent* event)
{
	lastPos.setX(event->x());
	lastPos.setY(event->y());

}

void COOPViewerWidget::process_picking(const int& hits, const GLuint* selectBuff)
{

}