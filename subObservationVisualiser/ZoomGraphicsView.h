#ifndef DISCRETE_GERM_GRAIN_ZOOM_GRAPHICS_HEADER_GUARD
#define DISCRETE_GREM_GRAIN_ZOOM_GRAPHICS_HEADER_GUARD
#include <QGraphicsView>
namespace discreteGermGrain
{
	class ZoomGraphicsView : public QGraphicsView
	{
		Q_OBJECT
	public:
		ZoomGraphicsView(QGraphicsScene* scene);
	protected:
		void wheelEvent(QWheelEvent* event);
		void keyPressEvent(QKeyEvent* event);
	};
}
#endif
