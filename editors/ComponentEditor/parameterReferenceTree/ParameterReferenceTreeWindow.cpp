//-----------------------------------------------------------------------------
// File: ParameterReferenceTreeWindow.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.09.2018
//
// Description:
// Constructor for the window containing the parameter reference tree.
//-----------------------------------------------------------------------------

#include "ParameterReferenceTreeWindow.h"

#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/parameterReferenceTree/ParameterReferenceTree.h>

#include <IPXACTmodels/Component/Component.h>

#include <QHBoxLayout>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTreeConstructor::ParameterReferenceTreeConstructor()
//-----------------------------------------------------------------------------
ParameterReferenceTreeWindow::ParameterReferenceTreeWindow(ParameterReferenceTree* parameterReferenceTree,
    QWidget* parent):
QDialog(parent),
referenceTree_(parameterReferenceTree),
expandAllButton_(new QPushButton(QString(tr("Expand All")), this)),
collapseAllButton_(new QPushButton(QString(tr("Collapse All")), this)),
closeButton_(new QPushButton(QString(tr("Close")), this))
{
    setMinimumWidth(600);
    setMinimumHeight(400);

    setupLayout();
    connectItems();

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    resize(width(), height());

    hide();
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTreeWindow::~ParameterReferenceTreeWindow()
//-----------------------------------------------------------------------------
ParameterReferenceTreeWindow::~ParameterReferenceTreeWindow()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTreeWindow::setupLayout()
//-----------------------------------------------------------------------------
void ParameterReferenceTreeWindow::setupLayout()
{
    QHBoxLayout* buttonLayout(new QHBoxLayout());
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(expandAllButton_, 0, Qt::AlignRight);
    buttonLayout->addWidget(collapseAllButton_, 0, Qt::AlignRight);
    buttonLayout->addWidget(closeButton_, 0, Qt::AlignRight);

    QVBoxLayout* overallLayout(new QVBoxLayout());
    overallLayout->addWidget(referenceTree_);
    overallLayout->addLayout(buttonLayout);
    setLayout(overallLayout);
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTreeWindow::connectItems()
//-----------------------------------------------------------------------------
void ParameterReferenceTreeWindow::connectItems()
{
    connect(expandAllButton_, SIGNAL(clicked()), this, SLOT(onExpandAll()), Qt::UniqueConnection);
    connect(collapseAllButton_, SIGNAL(clicked()), this, SLOT(onCollapseAll()), Qt::UniqueConnection);
    connect(closeButton_, SIGNAL(clicked()), this, SLOT(close()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTreeWindow::openReferenceTree()
//-----------------------------------------------------------------------------
void ParameterReferenceTreeWindow::openReferenceTree(QString const& parameterID, QString const& parameterName)
{
    setWindowTitle(QString(tr("References to ")) + parameterName);

    referenceTree_->openReferenceTree(parameterID);

    show();
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTreeWindow::onExpandAll()
//-----------------------------------------------------------------------------
void ParameterReferenceTreeWindow::onExpandAll()
{
    referenceTree_->expandAll();
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTreeWindow::onCollapseAll()
//-----------------------------------------------------------------------------
void ParameterReferenceTreeWindow::onCollapseAll()
{
    referenceTree_->collapseAll();
}
