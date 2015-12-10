#ifndef RESIDUAL_CONNECTIVITY_TREE_VISUALISER_GRAPHICS_VIEW_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_TREE_VISUALISER_GRAPHICS_VIEW_HEADER_GUARD
#include <QFrame>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "observationTree.h"
#include <QHBoxLayout>
#include <QGraphicsEllipseItem>
namespace residualConnectivity
{
	class treeVisualiserFrame : public QFrame
	{
		Q_OBJECT
	public:
		treeVisualiserFrame(const observationTree& tree, float pointSize);
		void centreOn(double x, double y);
		void highlightPosition(double x, double y);
		bool eventFilter(QObject* object, QEvent *event);
	signals:
		void observationLeft();
		void observationRight();
		void observationUp();
		void observationDown();
		void vertexSelected(int vertex);
		void positionChanged(double x, double y);
	private:
		double pointSize;
		QHBoxLayout* layout;
		QGraphicsView* graphicsView;
		QGraphicsScene* graphicsScene;
		QGraphicsEllipseItem* highlightItem;
	};
}
#endif
