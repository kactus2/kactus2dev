//-----------------------------------------------------------------------------
// File: DesignInstantiationEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 26.01.2016
//
// Description:
// Editor for design configuration instantiation.
//-----------------------------------------------------------------------------

#include "DesignInstantiationEditor.h"

#include <common/widgets/nameGroupEditor/namegroupeditor.h>

#include <mainwindow/mainwindow.h>

#include <QApplication>
#include <QScrollArea>
#include <QHBoxLayout>

//-----------------------------------------------------------------------------
// Function: DesignInstantiationEditor::DesignInstantiationEditor()
//-----------------------------------------------------------------------------
DesignInstantiationEditor::DesignInstantiationEditor(QSharedPointer<Component> component, 
    QSharedPointer<DesignInstantiation> instantiation,
    LibraryInterface* libHandler, QWidget* parent):
ItemEditor(component, libHandler, parent), 
    instantiation_(instantiation),
    nameGroupEditor_(new NameGroupEditor(instantiation, this, tr("Design instance name and description"))),
    designEditor_(0)
{
    // find the main window for VLNV editor.
    QWidget* parentW = NULL;
    foreach (QWidget* widget, QApplication::topLevelWidgets())
    {
        MainWindow* mainWnd = dynamic_cast<MainWindow*>(widget);
        if (mainWnd)
        {
            parentW = mainWnd;
            break;
        }
    }

    designEditor_ = new VLNVEditor(VLNV::DESIGN, libHandler, parentW, this);
    designEditor_->setTitle(tr("Design reference"));
    designEditor_->setMandatory(true);
    designEditor_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(nameGroupEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(designEditor_, SIGNAL(vlnvEdited()), this, SLOT(onHierRefChange()), Qt::UniqueConnection);
   
    refresh();

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationEditor::~DesignInstantiationEditor()
//-----------------------------------------------------------------------------
DesignInstantiationEditor::~DesignInstantiationEditor()
{
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationEditor::refresh()
//-----------------------------------------------------------------------------
void DesignInstantiationEditor::refresh()
{
	nameGroupEditor_->refresh();
	designEditor_->setVLNV(*instantiation_->getDesignReference());
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationEditor::onHierRefChange()
//-----------------------------------------------------------------------------
void DesignInstantiationEditor::onHierRefChange()
{
	instantiation_->getDesignReference()->setVLNV(designEditor_->getVLNV());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationEditor::showEvent()
//-----------------------------------------------------------------------------
void DesignInstantiationEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/designInstantiation.html");
}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationEditor::setupLayout()
//-----------------------------------------------------------------------------
void DesignInstantiationEditor::setupLayout()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* editorLayout = new QHBoxLayout();
    editorLayout->addWidget(nameGroupEditor_);
    editorLayout->addWidget(designEditor_, 0, Qt::AlignTop);

    QWidget* topWidget = new QWidget(scrollArea);
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setWidget(topWidget);

    QVBoxLayout* topLayout = new QVBoxLayout(topWidget);
    topLayout->addLayout(editorLayout);
    topLayout->addStretch();
    topLayout->setContentsMargins(0, 0, 0, 0);
}
