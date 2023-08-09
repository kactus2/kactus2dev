//-----------------------------------------------------------------------------
// File: SingleModeEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 08.08.2023
//
// Description:
// Editor to edit a single mode in a component.
//-----------------------------------------------------------------------------

#include "SingleModeEditor.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Mode.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <editors/ComponentEditor/memoryMaps/ExpressionProxyModel.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>

#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: SingleModeEditor::SingleModeEditor()
//-----------------------------------------------------------------------------
SingleModeEditor::SingleModeEditor(QSharedPointer<Component> component,
    QSharedPointer<Mode> Mode,
    LibraryInterface* libHandler,
    ExpressionSet expressions,
    QWidget* parent) :
    ItemEditor(component, libHandler, parent),
    Mode_(Mode),
    nameEditor_(Mode_, component->getRevision(), this, tr("Mode name and description"))
{
    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: SingleModeEditor::refresh()
//-----------------------------------------------------------------------------
void SingleModeEditor::refresh()
{
    nameEditor_.refresh();
}

//-----------------------------------------------------------------------------
// Function: SingleModeEditor::setupLayout()
//-----------------------------------------------------------------------------
void SingleModeEditor::setupLayout()
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
    topLayout->setContentsMargins(0,0,0,0);
    topLayout->setRowStretch(1, 1);
}

//-----------------------------------------------------------------------------
// Function: SingleModeEditor::showEvent()
//-----------------------------------------------------------------------------
void SingleModeEditor::showEvent(QShowEvent * event)
{
    QWidget::showEvent(event);

    emit helpUrlRequested("componenteditor/mode2022.html");
}
