#include "ZoomGraphicsView.h"
#include <QWheelEvent>
#include <QGraphicsScene>
namespace discreteGermGrain
{
	ZoomGraphicsView::ZoomGraphicsView(QGraphicsScene* scene)
		:QGraphicsView(scene)
	{
		setMouseTracking(true);
	}
	void ZoomGraphicsView::wheelEvent(QWheelEvent* event)
	{
		qreal factor = 1.2;
		if (event->delta() < 0) factor = 1.0 / factor;
		QPointF currentCentre = mapToScene(viewport()->rect()).boundingRect().center();
		QPointF pointBeforeScale = this->mapToScene(event->pos());
		scale(factor, factor);
		QPointF pointAfterScale = this->mapToScene(event->pos());
		this->centerOn(currentCentre + (pointBeforeScale - pointAfterScale));
	}
	void ZoomGraphicsView::keyPressEvent(QKeyEvent* keyEvent)
	{
		qreal factor = 1.2;
		if(keyEvent->key() == Qt::Key_Up && keyEvent->modifiers() & Qt::ShiftModifier)
		{
			scale(1/factor, 1/factor);
		}
		else if(keyEvent->key() == Qt::Key_Down && keyEvent->modifiers() & Qt::ShiftModifier)
		{
			scale(factor, factor);
		}
		else this->QGraphicsView::keyPressEvent(keyEvent);
	}
}
