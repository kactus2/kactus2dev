//-----------------------------------------------------------------------------
// File: FileDependencyGraphWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.03.2013
//
// Description:
// Widget which encapsulates the dependency graph view and a scroll bar
// for scrolling the dependencies.
//-----------------------------------------------------------------------------

#include "FileDependencyGraphWidget.h"

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::FileDependencyGraphWidget()
//-----------------------------------------------------------------------------
FileDependencyGraphWidget::FileDependencyGraphWidget(QWidget* parent)
    : QWidget(parent),
      view_(this),
      scrollBar_(Qt::Horizontal, parent),
      barLayout_(0),
      infoLabel_(this)
{
    view_.setContentsMargins(0, 0, 0, 0);
    scrollBar_.setContentsMargins(0, 0, 0, 0);

    barLayout_ = new QHBoxLayout();
    barLayout_->addWidget(&infoLabel_);
    barLayout_->addWidget(&scrollBar_);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->addWidget(&view_, 1);
    layout->addLayout(barLayout_);
    layout->setContentsMargins(0, 0, 0, 0);

    connect(&view_, SIGNAL(graphColumnScollMaximumChanged(int)),
            this, SLOT(onGraphColumnScrollMaximumChanged(int)), Qt::UniqueConnection);
    connect(&view_, SIGNAL(dependencyColumnPositionChanged(int)),
            this, SLOT(onDependencyColumnPositionChanged(int)), Qt::UniqueConnection);
    connect(&view_, SIGNAL(warningMessage(const QString&)),
            this, SLOT(showWarningMessage(const QString&)), Qt::UniqueConnection);

    connect(&scrollBar_, SIGNAL(valueChanged(int)),
            &view_, SLOT(setGraphColumnScrollIndex(int)), Qt::UniqueConnection);

    infoLabel_.setTextFormat(Qt::RichText);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::~FileDependencyGraphWidget()
//-----------------------------------------------------------------------------
FileDependencyGraphWidget::~FileDependencyGraphWidget()
{

}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::getView()
//-----------------------------------------------------------------------------
FileDependencyGraphView& FileDependencyGraphWidget::getView()
{
    return view_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::onGraphColumnCountChanged()
//-----------------------------------------------------------------------------
void FileDependencyGraphWidget::onGraphColumnScrollMaximumChanged(int maximum)
{
    scrollBar_.setEnabled(maximum >= 0);
    scrollBar_.setMaximum(maximum);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::onDependencyColumnPositionChanged()
//-----------------------------------------------------------------------------
void FileDependencyGraphWidget::onDependencyColumnPositionChanged(int pos)
{
    infoLabel_.setFixedWidth(pos);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::showWarningMessage()
//-----------------------------------------------------------------------------
void FileDependencyGraphWidget::showWarningMessage(QString const& message)
{
//     QString prefix = "";
// 
//     if (message != "")
//     {
//         prefix += "<img src=\":/icons/common/graphics/exclamation.png\">";
//     }
    
    infoLabel_.setText("<font color=\"red\">" + message + "</font>");
}
