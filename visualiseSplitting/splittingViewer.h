#ifndef DISCRETE_GERM_GRAIN_VIEWER_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_VIEWER_HEADER_GUARD
#include <QMainWindow>
#include "DiscreteGermGrainObs.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QStatusBar>
#include <QLabel>
#include <boost/random/mersenne_twister.hpp>
#include <boost/shared_ptr.hpp>
namespace discreteGermGrain
{
	class splittingViewer : public QMainWindow
	{
		Q_OBJECT
	public:
		splittingViewer(const DiscreteGermGrainObs& obs, int initialRadius, boost::mt19937& randomSource, float pointSize);
		~splittingViewer();
		bool eventFilter(QObject* object, QEvent *event);
	private:
		//the different graphics bits that have to get added
		void addBackgroundRectangle();
		void addPoints(DiscreteGermGrainSubObs& sub);
		void addLines(DiscreteGermGrainSubObs& sub);
		//void addArticulationPoints(DiscreteGermGrainSubObs& sub);

		void setObservation(DiscreteGermGrainObs& obs, int newRadius);
		DiscreteGermGrainObs originalObs, currentObs;
		QGraphicsScene* graphicsScene;
		QGraphicsView* graphicsView;
		int initialRadius, currentRadius;
		int nSimulationsPerStep;
		boost::mt19937& randomSource;
		QStatusBar* statusBar;
		QLabel* statusLabel;
		float minX, maxX, minY, maxY;
		float pointSize;
	};
}
#endif
