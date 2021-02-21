#include "COOPViewerWidget.h"
#include "rg_Point3D.h"
#include "RSOManager.h"
#include "MinimalRSO.h"
#include "OrbitShortestLink.h"
#include "OrbitTunnel.h"

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

	if (pMode != nullptr)
	{
		switch (*pMode)
		{
		case COOP_OPERATION_MODE::PPDB:
			draw_PPDB();
			break;
		case COOP_OPERATION_MODE::TPDB:
			draw_TPDB();
			break;
		case COOP_OPERATION_MODE::SPDB:
			draw_SPDB();
			break;
		case COOP_OPERATION_MODE::EVAL_SAFETY:
			draw_eval_safety();
			break;
		default:
			break;
		}
	}
}



void COOPViewerWidget::draw_PPDB()
{
	if (pManager->get_objectOfInterestIDs()->empty() == false)
		draw_line_among_OOIs();
}



void COOPViewerWidget::draw_TPDB()
{
	if (pManager->get_objectOfInterestIDs()->empty() == false)
		draw_line_among_OOIs();
}



void COOPViewerWidget::draw_SPDB()
{
	if (pShortestLink != nullptr)
	{
		const std::list<int> momentNShortestLinkRSOsID = pShortestLink->find_shortest_link_RSOs_ID_in_closest_moment(m_currentTime);
		std::list<rg_Point3D> shortestLinkCoord;


		if (pStarlinkManager != nullptr && pStarlinkManager->get_RSOs().empty() == false)
		{
			for (auto& rso : pStarlinkManager->get_RSOs())
			{
				rg_Point3D& coord = rso.get_coord();
				draw_point(coord / 100, 5, GREEN);
				draw_point(coord / 100, 8, RED);
			}
		}


		for (auto& RSOID : momentNShortestLinkRSOsID)
		{
			MinimalRSO* currRSO = pManager->find_RSO_from_ID(RSOID);
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



void COOPViewerWidget::draw_eval_safety()
{
	if (pOrbitTunnel != nullptr)
	{
		draw_sphere(rg_Point3D(0, 0, 64), 1, RED);
		int index = 0;
		for (const auto& orbit_tunnel : pOrbitTunnel->get_orbit_tunnels())
		{
			rg_Point3D lastCoord = orbit_tunnel.spine.front();
			for (const auto& spineCoord : orbit_tunnel.spine)
			{
				draw_line(lastCoord / 100, spineCoord / 100, 5, BLACK);
				lastCoord = spineCoord;
			}


			std::list<rg_Point3D> tunnelCoord;
			for (const auto& RSOID : orbit_tunnel.tunnelRSOsID)
			{
				MinimalRSO* currRSO = pManager->find_RSO_from_ID(RSOID);
				draw_point(currRSO->get_coord() / 100, 9, RED);
			}
			++index;
			if (index == 2)
				break;
		}
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

void COOPViewerWidget::draw_line_among_OOIs()
{
	list<MinimalRSO*> OOIs = pManager->find_object_of_interests();

	MinimalRSO* lastOOI = nullptr;
	for (auto& OOI : OOIs)
	{
		draw_sphere(OOI->get_coord() / 100, 1, YELLOW);

		if (lastOOI != nullptr && OOI != nullptr)
		{
			draw_line(OOI->get_coord() / 100, lastOOI->get_coord() / 100, 5, RED);
		}

		lastOOI = OOI;
	}
}



void COOPViewerWidget::change_view_to_OOI_direction()
{
	rg_Point3D center;
	
	if (pManager == nullptr)
		return;
	list<MinimalRSO*> OOIs = pManager->find_object_of_interests();
	if (OOIs.empty())
		return;

	MinimalRSO* lastOOI = nullptr;
	for (auto& OOI : OOIs)
	{
		center += OOI->get_coord();
	}

	center = center / OOIs.size();
	center.normalize();
	set_eye_direction(center);
	
}
