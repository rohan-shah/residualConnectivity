#include "treeVisualiserFrame.h"
#include "ZoomGraphicsView.h"
#include <QEvent>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
namespace discreteGermGrain
{
	class treeVisualiserVertex : public QGraphicsEllipseItem
	{
	public:
		treeVisualiserVertex(qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent = 0)
			:QGraphicsEllipseItem(x, y, width, height, parent)
		{}
		void setVertexID(int vertexID)
		{
			this->vertexID = vertexID;
		}
		int getVertexID() const
		{
			return vertexID;
		}
	private:
		int vertexID;
	};
	treeVisualiserFrame::treeVisualiserFrame(const observationTree& tree, float pointSize)
		:pointSize(pointSize), highlightItem(NULL)
	{
		graphicsScene = new QGraphicsScene();
		graphicsScene->installEventFilter(this);
		graphicsScene->setItemIndexMethod(QGraphicsScene::NoIndex);

		graphicsView = new ZoomGraphicsView(graphicsScene);
		graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		graphicsView->installEventFilter(this);

		const observationTree::treeGraphType treeGraph = tree.getTreeGraph();
		observationTree::treeGraphType::vertex_iterator currentVertex, endVertex;
		boost::tie(currentVertex, endVertex) = boost::vertices(treeGraph);

		QPen blackPen(QColor("black"));
		blackPen.setStyle(Qt::NoPen);
		QBrush blueBrush(QColor("blue"));
		QBrush redBrush(QColor("red"));
		for(; currentVertex != endVertex; currentVertex++)
		{
			float x = treeGraph[*currentVertex].x;
			float y = treeGraph[*currentVertex].y;
			treeVisualiserVertex* vertexItem = new treeVisualiserVertex(x - pointSize/2, y - pointSize/2, pointSize, pointSize);
			vertexItem->setVertexID((int)*currentVertex);
			if(treeGraph[*currentVertex].potentiallyDisconnected)
			{
				vertexItem->setPen(blackPen);
				vertexItem->setBrush(blueBrush);
			}
			else
			{
				vertexItem->setPen(blackPen);
				vertexItem->setBrush(redBrush);
			}
			vertexItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
			graphicsScene->addItem(vertexItem);
		}

		blackPen.setStyle(Qt::SolidLine);
		blackPen.setWidthF(pointSize/8);
		observationTree::treeGraphType::edge_iterator currentEdge, endEdge;
		boost::tie(currentEdge, endEdge) = boost::edges(treeGraph);
		for(; currentEdge != endEdge; currentEdge++)
		{
			int sourceVertex = (int)boost::source(*currentEdge, treeGraph);
			int targetVertex = (int)boost::target(*currentEdge, treeGraph);
			float sourceX = treeGraph[sourceVertex].x;
			float sourceY = treeGraph[sourceVertex].y;
			float targetX = treeGraph[targetVertex].x;
			float targetY = treeGraph[targetVertex].y;

			QGraphicsLineItem* lineItem = graphicsScene->addLine(sourceX, sourceY, targetX, targetY, blackPen);
			lineItem->setFlag(QGraphicsItem::ItemIsSelectable, false);
		}
		layout = new QHBoxLayout;
		layout->addWidget(graphicsView, 1);
		layout->setContentsMargins(0,0,0,0);
		setLayout(layout);
		graphicsView->fitInView(graphicsView->sceneRect(), Qt::KeepAspectRatioByExpanding);
	}
	void treeVisualiserFrame::highlightPosition(double x, double y)
	{
		if(highlightItem)
		{
			graphicsScene->removeItem(highlightItem);
			delete highlightItem;
		}
		QPen blackPen(QColor("black"));
		blackPen.setStyle(Qt::NoPen);
		QBrush blackBrush(QColor("black"));
		highlightItem = graphicsScene->addEllipse(x - pointSize/2, y - pointSize/2, pointSize, pointSize, blackPen, blackBrush);
		highlightItem->setFlag(QGraphicsItem::ItemIsSelectable, false);
		highlightItem->setZValue(1);
	}
	void treeVisualiserFrame::centreOn(double x, double y)
	{
		graphicsView->centerOn(x, y);
	}
	bool treeVisualiserFrame::eventFilter(QObject* object, QEvent *event)
	{
		if(event->type() == QEvent::GraphicsSceneMouseMove && object == graphicsScene)
		{
			QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
			QPointF position = mouseEvent->scenePos();
			emit positionChanged(position.x(), position.y());
			return true;
		}
		else if(event->type() == QEvent::GraphicsSceneMousePress && object == graphicsScene)
		{
			QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
			QPointF scenePosition = mouseEvent->scenePos();
			QList<QGraphicsItem*> possibleItems = graphicsScene->items(scenePosition.x() - 2*pointSize, scenePosition.y() - 2*pointSize, 4*pointSize, 4*pointSize, Qt::ContainsItemBoundingRect, Qt::DescendingOrder);
			QGraphicsItem* clicked = NULL;
			for(QList<QGraphicsItem*>::iterator i = possibleItems.begin(); i != possibleItems.end(); i++)
			{
				if((*i)->flags() & QGraphicsItem::ItemIsSelectable)
				{
					if(clicked)
					{
						clicked = NULL; break;
					}
					clicked = *i;
				}
			}
			if(clicked) emit vertexSelected(static_cast<treeVisualiserVertex*>(clicked)->getVertexID());
			return true;
		}
		if(event->type() == QEvent::KeyPress)
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
		return false;
	}
}
