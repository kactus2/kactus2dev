//-----------------------------------------------------------------------------
// File: SWViewEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.5.2012
//
// Description:
// SW view editor.
//-----------------------------------------------------------------------------

#include "SWViewEditor.h"

#include "swbuildcommandeditor.h"
#include "bspbuildeditor.h"

#include <mainwindow/mainwindow.h>

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>
#include <IPXACTmodels/kactusExtensions/SWView.h>

#include <QApplication>
#include <QScrollArea>
#include <QHBoxLayout>

//-----------------------------------------------------------------------------
// Function: SWViewEditor::SWViewEditor()
//-----------------------------------------------------------------------------
SWViewEditor::SWViewEditor(QSharedPointer<Component> component, 
    QSharedPointer<SWView> swView,
    LibraryInterface* libHandler, 
    QWidget *parent):
ItemEditor(component, libHandler, parent), 
    swView_(swView.data()),
    nameEditor_(swView, this, tr("Name and description")),
    hierRefEditor_(0),
    fileSetRefEditor_(new FileSetRefEditor(component, tr("File set references"), this)),
    swBuildCommands_(new SWBuildCommandEditor(component, swView->getSWBuildCommands(), this)),
    bspEditor_(new BSPBuildEditor(swView->getBSPBuildCommand(), component, this))
{
    // find the main window for VLNV editor
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

    hierRefEditor_ = new VLNVEditor(VLNV::DESIGNCONFIGURATION, libHandler, parentW, this);
    hierRefEditor_->addContentType(VLNV::DESIGN);
    hierRefEditor_->setImplementationFilter(true, KactusAttribute::SW);
    hierRefEditor_->setTitle(tr("Hierarchy reference"));
    hierRefEditor_->setMandatory(true);
    hierRefEditor_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	fileSetRefEditor_->initialize();

	// BSP is displayed only for HW components
	if (component->getImplementation() != KactusAttribute::HW)
    {
		bspEditor_->hide();
	}
	else
    {
		bspEditor_->show();
	}

    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(hierRefEditor_, SIGNAL(vlnvEdited()), this, SLOT(onHierRefChange()), Qt::UniqueConnection);
    connect(fileSetRefEditor_, SIGNAL(contentChanged()), this, SLOT(onFileSetRefChange()), Qt::UniqueConnection);
    connect(swBuildCommands_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(bspEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    refresh();

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: SWViewEditor::~SWViewEditor()
//-----------------------------------------------------------------------------
SWViewEditor::~SWViewEditor()
{
}

//-----------------------------------------------------------------------------
// Function: SWViewEditor::isValid()
//-----------------------------------------------------------------------------
bool SWViewEditor::isValid() const
{
    // if name group is not valid
    if (!nameEditor_.isValid() || !swBuildCommands_->isValid())
    {
        return false;
    }

    // check the file set references that they are to valid file sets.
    foreach (QString const& ref, fileSetRefEditor_->items())
    {
        // if the component does not contain the referenced file set.
        if (!component()->hasFileSet(ref))
        {
            return false;
        }
    }

    if (!swBuildCommands_->isValid())
    {
        return false;
    }

    if (!bspEditor_->isValid())
    {
        return false;
    }

    if (!hierRefEditor_->isValid())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: SWViewEditor::refresh()
//-----------------------------------------------------------------------------
void SWViewEditor::refresh()
{
	nameEditor_.refresh();
	hierRefEditor_->setVLNV(swView_->getHierarchyRef());
	fileSetRefEditor_->setItems(swView_->getFileSetRefs());
	swBuildCommands_->refresh();
	bspEditor_->refresh();
}

//-----------------------------------------------------------------------------
// Function: SWViewEditor::onHierRefChange()
//-----------------------------------------------------------------------------
void SWViewEditor::onHierRefChange()
{
	swView_->setHierarchyRef(hierRefEditor_->getVLNV());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SWViewEditor::showEvent()
//-----------------------------------------------------------------------------
void SWViewEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/swview.html");
}

//-----------------------------------------------------------------------------
// Function: SWViewEditor::onFileSetRefChange()
//-----------------------------------------------------------------------------
void SWViewEditor::onFileSetRefChange()
{
	swView_->setFileSetRefs(fileSetRefEditor_->items());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SWViewEditor::setupLayout()
//-----------------------------------------------------------------------------
void SWViewEditor::setupLayout()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* pageTopLayout = new QHBoxLayout();
    pageTopLayout->addWidget(&nameEditor_);
    pageTopLayout->addWidget(hierRefEditor_, 0, Qt::AlignTop);

    QHBoxLayout* pageMiddleLayout = new QHBoxLayout();
    pageMiddleLayout->addWidget(fileSetRefEditor_);
    pageMiddleLayout->addWidget(swBuildCommands_);

    QWidget* topWidget = new QWidget(scrollArea);
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setWidget(topWidget);

    QVBoxLayout* topLayout = new QVBoxLayout(topWidget);
    topLayout->addLayout(pageTopLayout);
    topLayout->addLayout(pageMiddleLayout);
    topLayout->addWidget(bspEditor_);
    topLayout->addStretch();
    topLayout->setContentsMargins(0, 0, 0, 0);
}