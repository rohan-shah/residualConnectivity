#ifndef ZOOM_GRAPHICS_HEADER_GUARD
#define ZOOM_GRAPHICS_HEADER_GUARD
#include <QGraphicsView>
namespace residualConnectivity
{
	class ZoomGraphicsView : public QGraphicsView
	{
	public:
		ZoomGraphicsView(QGraphicsScene* scene);
	protected:
		void wheelEvent(QWheelEvent* event);
	};
}
#endif