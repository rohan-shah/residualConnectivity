#include "splittingViewer.h"
#include "DiscreteGermGrainSubObs.h"
#include "zoomGraphicsView.h"
#include <QGraphicsItem>
#include <QEvent>
#include <QKeyEvent>
#include "isSingleComponentWithRadius.h"
#include <boost/graph/biconnected_components.hpp>
#include <boost/lexical_cast.hpp>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsTextItem>
namespace discreteGermGrain
{
	bool sortByFirst(Context::vertexPosition const& first, Context::vertexPosition const& second)
	{
		return first.first < second.first;
	}
	bool sortBySecond(Context::vertexPosition const& first, Context::vertexPosition const& second)
	{
		return first.second < second.second;
	}
	splittingViewer::splittingViewer(const DiscreteGermGrainObs& obs, int initialRadius, boost::mt19937& randomSource, float pointSize)
		:originalObs(obs), currentObs(obs), initialRadius(initialRadius), currentRadius(initialRadius), randomSource(randomSource), pointSize(pointSize)
	{
		graphicsScene = new QGraphicsScene();
		graphicsScene->installEventFilter(this);
		graphicsScene->setItemIndexMethod(QGraphicsScene::NoIndex);

		graphicsView = new ZoomGraphicsView(graphicsScene);
		graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		graphicsView->viewport()->installEventFilter(this);
		
		statusBar = new QStatusBar();
		this->statusLabel = new QLabel;
		statusLabel->setText("");
		statusBar->addPermanentWidget(statusLabel);
		setStatusBar(statusBar);

		setCentralWidget(graphicsView);
		
		const std::vector<Context::vertexPosition>& vertexPositions = obs.getContext().getVertexPositions();
		minX = std::min_element(vertexPositions.begin(), vertexPositions.end(), sortByFirst)->first - pointSize;
		maxX = std::max_element(vertexPositions.begin(), vertexPositions.end(), sortByFirst)->first + pointSize;
		minY = std::min_element(vertexPositions.begin(), vertexPositions.end(), sortBySecond)->second - pointSize;
		maxY = std::max_element(vertexPositions.begin(), vertexPositions.end(), sortBySecond)->second + pointSize;

		setObservation(originalObs, currentRadius);
		graphicsView->fitInView(minX, minY, maxX - minX, maxY - minY, Qt::KeepAspectRatio);
	}
	void splittingViewer::addBackgroundRectangle()
	{
		QPen pen(Qt::NoPen);
		QColor grey("grey");
		grey.setAlphaF(0.5);

		QBrush brush;
		brush.setColor(grey);
		brush.setStyle(Qt::SolidPattern);
		QGraphicsRectItem* rect = graphicsScene->addRect(minX, minY, maxX - minX, maxY - minY, pen, brush);
		rect->setZValue(-1);
	}
	void splittingViewer::addLines(DiscreteGermGrainSubObs& sub)
	{
		const vertexState* state = sub.getState();
		const Context::inputGraph& graph = sub.getContext().getGraph();
		const std::vector<Context::vertexPosition>& vertexPositions = sub.getContext().getVertexPositions();

		QPen pen(QColor("black"));
		pen.setWidthF(pointSize/10);
		Context::inputGraph::edge_iterator start, end;
		boost::tie(start, end) = boost::edges(graph);
		while(start != end)
		{
			Context::vertexPosition sourcePosition = vertexPositions[start->m_source], targetPosition = vertexPositions[start->m_target];
			if((state[start->m_source].state & ON_MASK) && (state[start->m_target].state & ON_MASK))
			{
				graphicsScene->addLine(sourcePosition.first, sourcePosition.second, targetPosition.first, targetPosition.second, pen);
			}
			start++;
		}
	}
	void splittingViewer::addPoints(DiscreteGermGrainSubObs& sub)
	{
		std::size_t nVertices = sub.getContext().nVertices();
		const vertexState* state = sub.getState();
		const std::vector<Context::vertexPosition>& vertexPositions = sub.getContext().getVertexPositions();

		QPen unfixedOnPen(QColor("black"));
		unfixedOnPen.setStyle(Qt::NoPen);

		QPen fixedOnPen(QColor("red"));
		fixedOnPen.setStyle(Qt::NoPen);

		QPen unfixedOffPen(QColor("black"));
		unfixedOffPen.setStyle(Qt::SolidLine);
		unfixedOffPen.setWidthF(pointSize/10);

		QPen fixedOffPen(QColor("red"));
		fixedOffPen.setStyle(Qt::SolidLine);
		fixedOffPen.setWidthF(pointSize/10);

		QBrush fixedOnBrush(QColor("red"));
		QBrush fixedOffBrush(QColor("red"));
		QBrush unfixedOffBrush(QColor("black"));
		QBrush unfixedOnBrush(QColor("black"));

		unfixedOffBrush.setStyle(Qt::NoBrush);
		fixedOffBrush.setStyle(Qt::NoBrush);
		for(std::size_t vertexCounter = 0; vertexCounter < nVertices; vertexCounter++)
		{
			Context::vertexPosition currentPosition = vertexPositions[vertexCounter];
			float x = currentPosition.first;
			float y = currentPosition.second;
			QPen pen;
			QBrush brush;
			/*int minDistance = std::numeric_limits<int>::max();
			for(int k = 0; k < allPoints.size(); k++)
			{
				int newDistance;
				if((newDistance = abs(i - allPoints[k].first) + abs(j - allPoints[k].second)) < minDistance) minDistance = newDistance;
			}*/
			if(state[vertexCounter].state & FIXED_ON)
			{
				pen = fixedOnPen;
				brush = fixedOnBrush;
			}
			else if(state[vertexCounter].state & FIXED_OFF)
			{
				pen = fixedOffPen;
				brush = fixedOffBrush;
			}
			else if(state[vertexCounter].state & UNFIXED_ON)
			{
				pen = unfixedOnPen;
				brush = unfixedOnBrush;
			}
			else if(state[vertexCounter].state & UNFIXED_OFF)
			{
				pen = unfixedOffPen;
				brush = unfixedOffBrush;
			}
			graphicsScene->addEllipse(x - pointSize/2, y - pointSize/2, pointSize, pointSize, pen, brush);
		}
	}
	/*void splittingViewer::addArticulationPoints(DiscreteGermGrainSubObs& sub)
	{
		std::set<int> randomVertices;
		std::set<int> subPointVertices;
		std::vector<Config::gridPoint> allPossiblePoints;
		DiscreteGermGrainSubObs::graph_t graph;
		sub.constructRadius1Graph(randomVertices, subPointVertices, allPossiblePoints, graph);

		//get out biconnected components. 
		typedef std::vector<boost::graph_traits<DiscreteGermGrainSubObs::graph_t>::edges_size_type> component_storage_t;
		typedef boost::iterator_property_map<component_storage_t::iterator, boost::property_map<DiscreteGermGrainSubObs::graph_t, boost::edge_index_t>::type> component_map_t;
		
		component_storage_t biconnectedIds(boost::num_edges(graph));
		component_map_t componentMap(biconnectedIds.begin(), boost::get(boost::edge_index, graph));

		std::vector<Config::intType> articulationIndices;
		boost::biconnected_components(graph, componentMap, std::back_inserter(articulationIndices));


		QPen pen("blue");
		pen.setStyle(Qt::NoPen);
		QBrush brush("blue");
		for(std::vector<Config::intType>::iterator i = articulationIndices.begin(); i != articulationIndices.end(); i++)
		{
			Config::gridPoint point = allPossiblePoints[*i];
			graphicsScene->addRect(point.first, point.second, 0.1, 0.1, pen, brush);
		}

		QFont textFont;
		textFont.setPointSize(1);*/

		//also add point numbers
		/*for(int i = 0; i < allPossiblePoints.size(); i++)
		{
			QString text(boost::lexical_cast<std::string>(i).c_str());
			QGraphicsSimpleTextItem* textItem = new QGraphicsSimpleTextItem;
			textItem->setFont(textFont);
			textItem->setText(text);
			textItem->setScale(0.2);

			textItem->setPos(allPossiblePoints[i].first, allPossiblePoints[i].second);

			graphicsScene->addItem(textItem);
		}*/
	//}
	void splittingViewer::setObservation(DiscreteGermGrainObs& obs, int newRadius)
	{
		std::stringstream ss;
		ss << "Radius " << newRadius;
		statusLabel->setText(QString::fromStdString(ss.str()));
		this->currentObs = obs;
		this->currentRadius = newRadius;
		
		
		DiscreteGermGrainSubObs sub = obs.getSubObservation(newRadius);

		//delete all existing graphics items
		QList<QGraphicsItem*> allItems = graphicsScene->items();
		for(QList<QGraphicsItem*>::iterator i = allItems.begin(); i != allItems.end(); i++) delete *i;
		
		//draw the background rectangle
		addBackgroundRectangle();
		//put in all the points of the observation
		addPoints(sub);
		//put in the lines
		addLines(sub);
		
		/*if(newRadius == 1)
		{
			addArticulationPoints(sub);
		}*/
	}
	bool splittingViewer::eventFilter(QObject*, QEvent *event)
	{
		if(event->type() == QEvent::KeyPress)
		{
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
			if((keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) && currentRadius > 0)
			{
				/*DiscreteGermGrainSubObs sub = currentObs.getSubObservation(currentRadius);
				float outputProbability = 0;
				std::vector<int> componentIDs;
				DiscreteGermGrainObs outputObservation(sub.getContext());
				sub.getSingleComponentObservation(currentRadius - 1, randomSource, componentIDs, outputObservation, outputProbability);
				setObservation(outputObservation, currentRadius-1);*/
			}
			if(keyEvent->key() == Qt::Key_N)
			{
				setObservation(originalObs, initialRadius);
			}
		}
		return false;
	}
	splittingViewer::~splittingViewer()
	{
		delete statusLabel;
		delete statusBar;
		delete graphicsView;
		delete graphicsScene;
	}
}
