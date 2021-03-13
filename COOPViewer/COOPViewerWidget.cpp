#include "COOPViewerWidget.h"
#include "rg_Point3D.h"
#include "RSOManager.h"
#include "MinimalRSO.h"
#include "OrbitShortestLink.h"
#include "OrbitTunnel.h"
#include "OrbitClosestNeighbor.h"

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

	//draw_sphere(rg_Point3D(0, 0, 0), 64, BLUE);
	draw_texture_sphere(rg_Point3D(0, 0, 0), 64, BLUE);




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
		case COOP_OPERATION_MODE::CLOSESTNEIGHBORS:
			draw_CN();
			break;
		default:
			break;
		}
	}
}



void COOPViewerWidget::draw_total_RSOs()
{
	if (pManager != nullptr && pManager->get_RSOs().empty() == false)
	{
		for (auto& rso : pManager->get_RSOs())
		{
			rg_Point3D& coord = rso.get_coord();
			draw_point(coord / 100, 5, GREEN);
		}
	}
}

void COOPViewerWidget::draw_PPDB()
{
	draw_total_RSOs();
	if (pManager->get_objectOfInterestIDs()->empty() == false)
		draw_line_among_OOIs();
}



void COOPViewerWidget::draw_TPDB()
{
	draw_total_RSOs();
	if (pManager->get_objectOfInterestIDs()->empty() == false)
		draw_circle_among_OOIs();
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

void COOPViewerWidget::draw_CN()
{
	draw_total_RSOs();
	if (pOrbitClosestNeighbor != nullptr)
	{
		const std::list<int> momentNClosestNeighborRSOsID = pOrbitClosestNeighbor->find_closest_neighbor_RSOs_ID_in_closest_moment(m_currentTime);
		std::list<rg_Point3D> closestNeighborCoords;

		for (auto& RSOID : momentNClosestNeighborRSOsID)
		{
			MinimalRSO* currRSO = pManager->find_RSO_from_ID(RSOID);
			closestNeighborCoords.push_back(currRSO->get_coord());
		}

		rg_Point3D targetRSO = pManager->find_RSO_from_ID(pOrbitClosestNeighbor->m_momentNClosestNeighborRSOsID.back().targetRSOID)->get_coord();


		draw_sphere(targetRSO / 100, 0.5, BLUE);
		for (auto& rsoCoord : closestNeighborCoords)
		{
			draw_line(targetRSO / 100, rsoCoord / 100, 4, RED);
			draw_sphere(rsoCoord / 100, 0.5, RED);
		}
	}
}




void COOPViewerWidget::draw_eval_safety()
{
	OrbitTunnel* pOrbitTunnel;
	if (m_spaceCenterCode == 1)
		pOrbitTunnel = pOrbitTunnel_N;
	else if (m_spaceCenterCode == 2)
		pOrbitTunnel = pOrbitTunnel_B;
	else if (m_spaceCenterCode == 3)
		pOrbitTunnel = pOrbitTunnel_C;
	else
		pOrbitTunnel = pOrbitTunnel_N;


	if (pOrbitTunnel != nullptr)
	{
		
		if (allRSOs)
		{
			draw_total_RSOs();
		}

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
			if (ciriticRSOs)
			{
				for (const auto& RSOID : orbit_tunnel.tunnelRSOsID)
				{
					MinimalRSO* currRSO = pManager->find_RSO_from_ID(RSOID);
					draw_point(currRSO->get_coord() / 100, 9, RED);
				}
			}
			

			for (const auto& face : orbit_tunnel.boundaryFaces)
			{
				const list< list<rg_Point3D> >& vertices = face.boundary_vertices();
				const list< rg_Point3D >& verticesOfOuterLoop = vertices.front();

				list<rg_Point3D> verticesWithScaleDown;

				rg_Point3D vtx[3];
				int i_vec = 0;
				for (const auto& vertex : verticesOfOuterLoop)
				{
					if (i_vec < 3)
					{
						verticesWithScaleDown.push_back(vertex / 100);
						vtx[i_vec++] = vertex / 100;
					}
					else
						verticesWithScaleDown.push_back(vertex / 100);
				}

				rg_Point3D vec[2];
				vec[0] = (vtx[1] - vtx[0]);
				vec[1] = (vtx[2] - vtx[1]);
				rg_Point3D normal = vec[0].crossProduct(vec[1]);
				draw_polygon(verticesWithScaleDown, normal.getUnitVector(), GREY, 0.5);
				//draw_polygon2(verticesOfOuterLoop, normal.getUnitVector(), GREY);
			}
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



void COOPViewerWidget::draw_circle_among_OOIs()
{
	if (pManager->get_objectOfInterestIDs()->size() == 3)
	{
		list<MinimalRSO*> OOIs = pManager->find_object_of_interests();

		for (auto& OOI : OOIs)
		{
			draw_sphere(OOI->get_coord() / 100, 1, YELLOW);
		}

		Circle3D cotangentCircle = pManager->calculate_circle_of_OOIs();
		draw_circle(cotangentCircle.getCenter() / 100, cotangentCircle.getRadius() / 100, cotangentCircle.getNormal(), 5, RED);
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

void COOPViewerWidget::change_view_to_target_RSO(rg_Point3D center)
{
	center.normalize();
	set_eye_direction(center);
}


