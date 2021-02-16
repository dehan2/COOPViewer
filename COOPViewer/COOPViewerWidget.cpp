#include "COOPViewerWidget.h"
#include "rg_Point3D.h"
#include "RSOManager.h"
#include "ResidentSpaceObject.h"

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
			draw_sphere(coord / 100, 1, GREEN);
		}
	}
}



void COOPViewerWidget::mousePressEvent(QMouseEvent* event)
{

}

void COOPViewerWidget::process_picking(const int& hits, const GLuint* selectBuff)
{

}

void COOPViewerWidget::update()
{
	draw();
}
