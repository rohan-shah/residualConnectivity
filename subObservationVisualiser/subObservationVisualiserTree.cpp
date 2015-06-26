#include "subObservationVisualiserTree.h"
#include <QEvent>
#include <QKeyEvent>
#include <QTimer>
#include "ZoomGraphicsView.h"
#include <QGraphicsRectItem>
#include <boost/lexical_cast.hpp>
#include <QGraphicsSceneMouseEvent>
#include <QSizePolicy>
namespace discreteGermGrain
{
	subObservationVisualiserTree::subObservationVisualiserTree(const observationTree& inputTree, float graphPointSize, float treePointSize)
		:tree(inputTree), currentLevel(0), currentIndex(0)
	{
		if(tree.nLevels() == 0 || tree.getSampleSize(0) == 0)
		{
			QTimer::singleShot(0, this, SLOT(close()));
		}
		else
		{
			statusBar = new subObservationStatusBar;
			setStatusBar(statusBar);
			
			centralFrame = new QFrame;
			layout = new QHBoxLayout;
			base = new subObservationVisualiserBase(tree.getContext(), graphPointSize);
			treeFrame = new treeVisualiserFrame(inputTree, treePointSize);

			treeFrame->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
			base->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
			
			layout->addWidget(base, 0, 0);//Qt::AlignLeft);
			layout->addWidget(treeFrame, 0, 0);//Qt::AlignRight);
			layout->setContentsMargins(0,0,0,0);
			centralFrame->setLayout(layout);

			setCentralWidget(centralFrame);
			base->installEventFilter(this);

			QObject::connect(base, &subObservationVisualiserBase::positionChanged, this, &subObservationVisualiserTree::graphPositionChanged);
			QObject::connect(treeFrame, &treeVisualiserFrame::positionChanged, this, &subObservationVisualiserTree::treePositionChanged);

			QObject::connect(base, &subObservationVisualiserBase::observationUp, this, &subObservationVisualiserTree::observationUp);
			QObject::connect(base, &subObservationVisualiserBase::observationLeft, this, &subObservationVisualiserTree::observationLeft);
			QObject::connect(base, &subObservationVisualiserBase::observationRight, this, &subObservationVisualiserTree::observationRight);
			QObject::connect(base, &subObservationVisualiserBase::observationDown, this, &subObservationVisualiserTree::observationDown);

			QObject::connect(treeFrame, &treeVisualiserFrame::observationUp, this, &subObservationVisualiserTree::observationUp);
			QObject::connect(treeFrame, &treeVisualiserFrame::observationLeft, this, &subObservationVisualiserTree::observationLeft);
			QObject::connect(treeFrame, &treeVisualiserFrame::observationRight, this, &subObservationVisualiserTree::observationRight);
			QObject::connect(treeFrame, &treeVisualiserFrame::observationDown, this, &subObservationVisualiserTree::observationDown);

			QObject::connect(treeFrame, &treeVisualiserFrame::vertexSelected, this, &subObservationVisualiserTree::treeVertexClicked);

			setObservation();
		}
	}
	void subObservationVisualiserTree::treeVertexClicked(int vertex)
	{
		const observationTree::treeGraphType& treeGraph = tree.getTreeGraph();
		currentIndex = treeGraph[vertex].index;
		currentLevel = treeGraph[vertex].level;
		setObservation();
	}
	void subObservationVisualiserTree::setObservation()
	{
		boost::shared_array<vertexState> expandedState(new vertexState[tree.getContext().nVertices()]);
		tree.expand(expandedState, currentLevel, currentIndex);
		//Putting in dummy values for the last two constructor arguments
		observation obs(tree.getContext(), expandedState);
		base->setObservation(obs);

		const observationTree::treeGraphType& treeGraph = tree.getTreeGraph();
		const std::vector<std::vector<int > >& perLevelIds = tree.getPerLevelVertexIds();
		int currentVertex = perLevelIds[currentLevel][currentIndex];
		double x = treeGraph[currentVertex].x;
		double y = treeGraph[currentVertex].y;
		treeFrame->centreOn(x, y);
		treeFrame->highlightPosition(x, y);
	}
	subObservationVisualiserTree::~subObservationVisualiserTree()
	{
	}
	void subObservationVisualiserTree::observationLeft()
	{
		if(currentIndex > 0)
		{
			currentIndex--;
			setObservation();
		}
	}
	void subObservationVisualiserTree::observationRight()
	{
		if(currentIndex < (int)(tree.getSampleSize(currentLevel)-1))
		{
			currentIndex++;
			setObservation();
		}
	}
	void subObservationVisualiserTree::observationDown()
	{
		const observationTree::treeGraphType& treeGraph = tree.getTreeGraph();
		const std::vector<std::vector<int > >& perLevelIds = tree.getPerLevelVertexIds();
		int currentVertex = perLevelIds[currentLevel][currentIndex];
		observationTree::treeGraphType::out_edge_iterator currentOutEdge, endOutEdge;
		boost::tie(currentOutEdge, endOutEdge) = boost::out_edges(currentVertex, treeGraph);
		double currentX = std::numeric_limits<double>::infinity();
		int minXIndex = -1;
		for(; currentOutEdge != endOutEdge; currentOutEdge++)
		{
			int potentialNewIndex = treeGraph[currentOutEdge->m_target].index;
			double potentialNewX = treeGraph[currentOutEdge->m_target].x;
			if(potentialNewX < currentX)
			{
				currentX = potentialNewX;
				minXIndex = potentialNewIndex;
			}
		}
		if(minXIndex != -1) 
		{
			currentLevel++;
			currentIndex = minXIndex;
			setObservation();
		}
	}
	void subObservationVisualiserTree::observationUp()
	{
		if(currentLevel > 0)
		{
			const observationTree::treeGraphType& treeGraph = tree.getTreeGraph();
			const std::vector<std::vector<int > >& perLevelIds = tree.getPerLevelVertexIds();
			int currentVertex = perLevelIds[currentLevel][currentIndex];
			currentIndex = treeGraph[boost::source(*(boost::in_edges(currentVertex, treeGraph).first), treeGraph)].index;
			currentLevel--;
			setObservation();
		}
	}
	bool subObservationVisualiserTree::eventFilter(QObject* object, QEvent *event)
	{
		if(event->type() == QEvent::KeyPress)
		{
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
			if(keyEvent->key() == Qt::Key_Left && keyEvent->modifiers() & Qt::ShiftModifier)
			{
				observationLeft();
				return true;
			}
			else if(keyEvent->key() == Qt::Key_Right && keyEvent->modifiers() & Qt::ShiftModifier)
			{
				observationRight();
				return true;
			}
		}
		return false;
	}
	void subObservationVisualiserTree::treePositionChanged(double x, double y)
	{
		statusBar->setPosition(x, y);
	}
	void subObservationVisualiserTree::graphPositionChanged(double x, double y)
	{
		statusBar->setPosition(x, y);
	}
}
