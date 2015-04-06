#include "subObservationStatusBar.h"
#include <sstream>
namespace discreteGermGrain
{
	subObservationStatusBar::subObservationStatusBar(std::string text)
		:text(text)
	{
		frame = new QFrame;
		addPermanentWidget(frame, 1);

		this->positionLabel = new QLabel;
		positionLabel->setText("");
		this->reducedLabel = new QLabel;
		reducedLabel->setText("");
		QLabel* textLabel = new QLabel;
		textLabel->setText(QString::fromStdString(text));

		layout = new QHBoxLayout;
		layout->addWidget(positionLabel, 1, Qt::AlignLeft);
		layout->addWidget(textLabel, 1, Qt::AlignCenter);
		layout->addWidget(reducedLabel, 0, Qt::AlignRight);
		layout->setContentsMargins(0,0,0,0);
		frame->setLayout(layout);
	}
	void subObservationStatusBar::setPosition(double x, double y)
	{
		std::stringstream ss;
		ss << "(" << x << ", " << y << ")";
		positionLabel->setText(QString::fromStdString(ss.str()));
	}
}
