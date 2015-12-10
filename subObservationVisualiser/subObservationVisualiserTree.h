#ifndef SUB_OBSERVATION_VISUALISER_TREE_HEADER_GUARD
#define SUB_OBSERVATION_VISUALISER_TREE_HEADER_GUARD
#include <QMainWindow>
#include "Context.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QStatusBar>
#include <QFrame>
#include <QHBoxLayout>
#include "subObservationVisualiserBase.h"
#include "subObservationStatusBar.h"
#include "observationTree.h"
#include "treeVisualiserFrame.h"
namespace residualConnectivity
{
	//If the next state is RESIMULATE, then we resimulate until we observe something that hits the next level, BUT
	class subObservationVisualiserTree : public QMainWindow
	{
		Q_OBJECT
	public:
		subObservationVisualiserTree(const observationTree& tree, float graphPointSize, float treePointSize);
		~subObservationVisualiserTree();
		bool eventFilter(QObject* object, QEvent *event);
	public slots:
		void treePositionChanged(double x, double y);
		void graphPositionChanged(double x, double y);
		void observationLeft();
		void observationRight();
		void observationUp();
		void observationDown();
		void treeVertexClicked(int vertex);
	private:
		void setObservation();
		subObservationStatusBar* statusBar;
		subObservationVisualiserBase* base;
		treeVisualiserFrame* treeFrame;
		QHBoxLayout* layout;
		QFrame* centralFrame;
		const observationTree& tree;
		int currentLevel;
		int currentIndex;
	};
}
#endif
