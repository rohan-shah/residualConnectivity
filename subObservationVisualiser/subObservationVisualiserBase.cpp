//I want assert always enabled in this particular file
#ifdef NDEBUG
#undef NDEBUG
#include <assert.h>
#define NDEBUG
#endif

#include "subObservationVisualiserBase.h"
#include <QEvent>
#include <QKeyEvent>
#include <QTimer>
#include "ZoomGraphicsView.h"
#include <QGraphicsRectItem>
#include <boost/lexical_cast.hpp>
#include <QGraphicsSceneMouseEvent>
namespace residualConnectivity
{
	bool sortByFirst(context::vertexPosition const& first, context::vertexPosition const& second)
	{
		return first.first < second.first;
	}
	bool sortBySecond(context::vertexPosition const& first, context::vertexPosition const& second)
	{
		return first.second < second.second;
	}
	void subObservationVisualiserBase::initialiseControls()
	{
		graphicsScene = new QGraphicsScene();
		graphicsScene->installEventFilter(this);
		graphicsScene->setItemIndexMethod(QGraphicsScene::NoIndex);

		graphicsView = new ZoomGraphicsView(graphicsScene);
		graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		graphicsView->viewport()->installEventFilter(this);

		const std::vector<context::vertexPosition>& vertexPositions = contextObj.getVertexPositions();
		minX = std::min_element(vertexPositions.begin(), vertexPositions.end(), sortByFirst)->first - pointSize;
		maxX = std::max_element(vertexPositions.begin(), vertexPositions.end(), sortByFirst)->first + pointSize;
		minY = std::min_element(vertexPositions.begin(), vertexPositions.end(), sortBySecond)->second - pointSize;
		maxY = std::max_element(vertexPositions.begin(), vertexPositions.end(), sortBySecond)->second + pointSize;

		layout = new QHBoxLayout;
		layout->addWidget(graphicsView, 1);
		layout->setContentsMargins(0,0,0,0);
		setLayout(layout);
	}
	subObservationVisualiserBase::subObservationVisualiserBase(const context& contextObj, float pointSize)
		:pointSize(pointSize), contextObj(contextObj),standardPointsItem(NULL), standardLinesItem(NULL), backgroundItem(NULL)
	{
		//We don't run this if there are no vertex positions
		if(contextObj.getVertexPositions().size() == 0)
		{
			std::cout << "Specified graph did not have vertex positions, exiting..." << std::endl;
			QTimer::singleShot(0, this, SLOT(close()));
			return;
		}
		initialiseControls();

		QPen pen(Qt::NoPen);
		QColor grey("grey");
		grey.setAlphaF(0.5);
		QBrush brush;
		brush.setColor(grey);
		brush.setStyle(Qt::SolidPattern);
		backgroundItem = new QGraphicsRectItem(minX, minY, maxX - minX, maxY - minY, NULL);
		backgroundItem->setPen(pen);
		backgroundItem->setBrush(brush);
		backgroundItem->setZValue(-1);
		graphicsScene->addItem(backgroundItem);
	}
	void subObservationVisualiserBase::setObservation(const observation& subObs)
	{
		constructGraphics(subObs);
		updateGraphics();
	}
	subObservationVisualiserBase::~subObservationVisualiserBase()
	{
	}
	void subObservationVisualiserBase::updateGraphics()
	{
		QList<QGraphicsItem*> allItems = graphicsScene->items();
		for(QList<QGraphicsItem*>::iterator i = allItems.begin(); i != allItems.end(); i++) 
		{
			if(!(*i)->parentItem()) graphicsScene->removeItem(*i);
		}
		graphicsScene->addItem(backgroundItem);
		graphicsScene->addItem(standardPointsItem);
		graphicsScene->addItem(standardLinesItem);
	}
	void subObservationVisualiserBase::constructGraphics(const observation& subObs)
	{
		QList<QGraphicsItem*> allItems = graphicsScene->items();
		for(QList<QGraphicsItem*>::iterator i = allItems.begin(); i != allItems.end(); i++) 
		{
			if(!(*i)->parentItem()) graphicsScene->removeItem(*i);
		}

		delete standardLinesItem;
		delete standardPointsItem;

		standardPointsItem = new QGraphicsItemGroup(NULL);
		standardLinesItem = new QGraphicsItemGroup(NULL);

		constructStandardLines(subObs);
		constructStandardPoints(subObs);
	}
	void subObservationVisualiserBase::constructStandardPoints(const observation& subObs)
	{
		assert(standardPointsItem);
		std::size_t nVertices = boost::num_vertices(contextObj.getGraph());
		const std::vector<context::vertexPosition>& vertexPositions = contextObj.getVertexPositions();
		const vertexState* state = subObs.getState();

		QPen blackPen(QColor("black"));
		QBrush blackBrush(QColor("black"));

		QPen redPen(QColor("red"));
		QBrush redBrush(QColor("red"));

		QBrush noBrush;
		noBrush.setStyle(Qt::NoBrush);
		
		for(std::size_t vertexCounter = 0; vertexCounter < nVertices; vertexCounter++)
		{
			context::vertexPosition currentPosition = vertexPositions[vertexCounter];
			float x = currentPosition.first;
			float y = currentPosition.second;
			QGraphicsEllipseItem* newItem = new QGraphicsEllipseItem(x - pointSize/2, y - pointSize/2, pointSize, pointSize, standardPointsItem);
			if(state[vertexCounter].state == FIXED_OFF)
			{
				newItem->setBrush(noBrush);
				newItem->setPen(blackPen);
			}
			else if(state[vertexCounter].state == FIXED_ON)
			{
				newItem->setBrush(blackBrush);
				newItem->setPen(blackPen);
			}
			else if(state[vertexCounter].state == UNFIXED_OFF)
			{
				newItem->setBrush(noBrush);
				newItem->setPen(redPen);
			}
			else if(state[vertexCounter].state == UNFIXED_ON)
			{
				newItem->setBrush(redBrush);
				newItem->setPen(redPen);
			}
			else
			{
				throw std::runtime_error("Internal error");
			}
		}
	}
	void subObservationVisualiserBase::constructStandardLines(const observation& subObs)
	{
		assert(standardLinesItem);
		const vertexState* state = subObs.getState();
		const context::inputGraph& graph = contextObj.getGraph();
		const std::vector<context::vertexPosition>& vertexPositions = contextObj.getVertexPositions();

		QPen blackPen(QColor("black"));
		blackPen.setWidthF(pointSize/10);

		QPen redPen(QColor("red"));
		redPen.setWidthF(pointSize/10);

		QVector<qreal> dashPattern;
		dashPattern.push_back(8);
		dashPattern.push_back(8);

		QPen dashedBlackPen(QColor("black"));
		dashedBlackPen.setDashPattern(dashPattern);

		QPen dashedRedPen(QColor("red"));
		dashedRedPen.setDashPattern(dashPattern);

		context::inputGraph::edge_iterator start, end;
		boost::tie(start, end) = boost::edges(graph);

		while(start != end)
		{
			context::vertexPosition sourcePosition = vertexPositions[start->m_source], targetPosition = vertexPositions[start->m_target];
			QGraphicsLineItem* newItem = new QGraphicsLineItem(sourcePosition.first, sourcePosition.second, targetPosition.first, targetPosition.second, standardLinesItem);
			if(state[start->m_source].state == FIXED_ON && state[start->m_target].state == FIXED_ON)
			{
				newItem->setPen(blackPen);
			}
			else if(state[start->m_source].state == FIXED_OFF || state[start->m_target].state == FIXED_OFF)
			{
				newItem->setPen(dashedBlackPen);
			}
			else 
			{
				newItem->setPen(redPen);
			}
			start++;
		}
	}
	bool subObservationVisualiserBase::eventFilter(QObject* object, QEvent *event)
	{
		if(event->type() == QEvent::GraphicsSceneMouseMove && object == graphicsScene)
		{
			QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
			QPointF position = mouseEvent->scenePos();
			emit positionChanged(position.x(), position.y());

			return true;
		}
		else if(event->type() == QEvent::KeyPress)
		{
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
			if(keyEvent->key() == Qt::Key_Left && keyEvent->modifiers() & Qt::ShiftModifier)
			{
				emit observationLeft();
				return true;
			}
			else if(keyEvent->key() == Qt::Key_Right && keyEvent->modifiers() & Qt::ShiftModifier)
			{
				emit observationRight();
				return true;
			}
			else if(keyEvent->key() == Qt::Key_Up && keyEvent->modifiers() & Qt::ShiftModifier)
			{
				emit observationDown();
				return true;
			}
			else if(keyEvent->key() == Qt::Key_Down && keyEvent->modifiers() & Qt::ShiftModifier)
			{
				emit observationUp();
				return true;
			}
		}
		else if(event->type() == QEvent::Leave && object == graphicsScene)
		{
			updateGraphics();
			return true;
		}
		return false;
	}
}
