//-----------------------------------------------------------------------------
// File: SingleCpuEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 15.6.2023
//
// Description:
// Editor to edit a single cpu in a component.
//-----------------------------------------------------------------------------

#include "SingleCpuEditor.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Cpu.h>

#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: SingleCpuEditor::SingleCpuEditor()
//-----------------------------------------------------------------------------
SingleCpuEditor::SingleCpuEditor(QSharedPointer<Component> component,
    QSharedPointer<Cpu> cpu, LibraryInterface* libHandler,
    QSharedPointer<ParameterFinder> parameterFinder, 
    QSharedPointer<ExpressionParser> expressionParser,
    QWidget* parent):
ItemEditor(component, libHandler, parent),
cpu_(cpu),
nameEditor_(cpu_, component->getRevision(), this, tr("CPU name and description")),
detailsEditor_(component, cpu, parameterFinder, expressionParser, this)
{
    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&detailsEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: SingleCpuEditor::refresh()
//-----------------------------------------------------------------------------
void SingleCpuEditor::refresh()
{
    nameEditor_.refresh();
    detailsEditor_.refresh();
}

//-----------------------------------------------------------------------------
// Function: SingleCpuEditor::setupLayout()
//-----------------------------------------------------------------------------
void SingleCpuEditor::setupLayout()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* topWidget = new QWidget(scrollArea);
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setWidget(topWidget);

    auto topLayout = new QGridLayout(topWidget);
    topLayout->addWidget(&nameEditor_, 0, 0, 1, 1);
    topLayout->addWidget(&detailsEditor_, 0, 1, 1, 1);
    topLayout->setContentsMargins(0,0,0,0);
    topLayout->setRowStretch(1, 1);
}

//-----------------------------------------------------------------------------
// Function: SingleCpuEditor::showEvent()
//-----------------------------------------------------------------------------
void SingleCpuEditor::showEvent(QShowEvent * event)
{
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/cpu.html");
}
