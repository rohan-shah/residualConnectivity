#ifndef RESIDUAL_CONNECTIVITY_ZOOM_GRAPHICS_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_ZOOM_GRAPHICS_HEADER_GUARD
#include <QGraphicsView>
namespace residualConnectivity
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
