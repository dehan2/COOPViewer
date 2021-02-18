#include "COOPViewerWidget.h"
#include "rg_Point3D.h"
#include "RSOManager.h"
#include "MinimalRSO.h"

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

		if (pManager->get_objectOfInterestIDs()->empty() == false)
			draw_line_among_OOIs();
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

	list<MinimalRSO*> OOIs = pManager->find_object_of_interests();

	MinimalRSO* lastOOI = nullptr;
	for (auto& OOI : OOIs)
	{
		center += OOI->get_coord();
	}

	center = center / OOIs.size();
	center.normalize();
	set_eye_direction(center);
}
