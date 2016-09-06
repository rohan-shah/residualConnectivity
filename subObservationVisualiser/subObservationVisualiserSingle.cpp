#include "subObservationVisualiserSingle.h"
#include <QEvent>
#include <QKeyEvent>
#include <QTimer>
#include "ZoomGraphicsView.h"
#include <QGraphicsRectItem>
#include <boost/lexical_cast.hpp>
#include <QGraphicsSceneMouseEvent>
namespace residualConnectivity
{
	subObservationVisualiserSingle::subObservationVisualiserSingle(const observationWithContext& obsWithContext, float pointSize)
		:obsWithContext(obsWithContext) 
	{
		statusBar = new subObservationStatusBar;
		setStatusBar(statusBar);
		
		base = new subObservationVisualiserBase(obsWithContext.getContext(), pointSize);
		base->installEventFilter(this);

		setCentralWidget(base);
		QObject::connect(base, &subObservationVisualiserBase::positionChanged, this, &subObservationVisualiserSingle::positionChanged);
		base->setObservation(obsWithContext.getObs());
	}
	void subObservationVisualiserSingle::positionChanged(double x, double y)
	{
		statusBar->setPosition(x, y);
	}
	subObservationVisualiserSingle::~subObservationVisualiserSingle()
	{}
	bool subObservationVisualiserSingle::eventFilter(QObject* object, QEvent *event)
	{
		if(event->type() == QEvent::KeyPress)
		{
			//QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
		}
		return false;
	}
}
