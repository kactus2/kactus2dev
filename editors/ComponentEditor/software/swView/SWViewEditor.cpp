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
#include <mainwindow/mainwindow.h>
#include "swbuildcommandeditor.h"
#include "bspbuildeditor.h"
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <IPXACTmodels/SWView.h>
#include <QApplication>
#include <QScrollArea>

//-----------------------------------------------------------------------------
// Function: SWViewEditor::SWViewEditor()
//-----------------------------------------------------------------------------
SWViewEditor::SWViewEditor(QSharedPointer<Component> component, 
						   QSharedPointer<SWView> swView,
                           LibraryInterface* libHandler, 
						   QWidget *parent):
ItemEditor(component, libHandler, parent), 
view_(swView.data()),
nameEditor_(swView->getNameGroup(), this, tr("Name and description")),
hierRefEditor_(NULL),
fileSetRefEditor_(NULL),
swBuildCommands_(NULL),
bspEditor_(NULL) {

	// find the main window for VLNV editor
	QWidget* parentW = NULL;
	foreach (QWidget* widget, QApplication::topLevelWidgets()) {
		MainWindow* mainWnd = dynamic_cast<MainWindow*>(widget);
		if (mainWnd) {
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

	fileSetRefEditor_ = new FileSetRefEditor(component, tr("File set references"), this);
	fileSetRefEditor_->initialize();

	swBuildCommands_ = new SWBuildCommandEditor(component, swView->getSWBuildCommands(), this);

	bspEditor_ = new BSPBuildEditor(swView->getBSPBuildCommand(), component, this);

	// BSP is displayed only for HW components
	if (component->getComponentImplementation() != KactusAttribute::HW) {
		bspEditor_->hide();
	}
	else {
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
SWViewEditor::~SWViewEditor() {
}

//-----------------------------------------------------------------------------
// Function: SWViewEditor::isValid()
//-----------------------------------------------------------------------------
bool SWViewEditor::isValid() const
{
    // if name group is not valid
    if (!nameEditor_.isValid() || !swBuildCommands_->isValid()) {
        return false;
	 }

	 // check the file set references that they are to valid file sets.
	 QStringList fileSetRefs = fileSetRefEditor_->items();
	 foreach (QString ref, fileSetRefs) {

		 // if the component does not contain the referenced file set.
		 if (!component()->hasFileSet(ref)) {
			 return false;
		 }
	 }

	 if (!swBuildCommands_->isValid()) {
		 return false;
	 }

	 if (!bspEditor_->isValid()) {
		 return false;
	 }

     if (!hierRefEditor_->isValid())
     {
         return false;
     }

    return true;
}

void SWViewEditor::refresh() {
	nameEditor_.refresh();
	hierRefEditor_->setVLNV(view_->getHierarchyRef());
	fileSetRefEditor_->setItems(view_->getFileSetRefs());
	swBuildCommands_->refresh();
	bspEditor_->refresh();
}

void SWViewEditor::onHierRefChange() {
	view_->setHierarchyRef(hierRefEditor_->getVLNV());
	emit contentChanged();
}

void SWViewEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/swview.html");
}

void SWViewEditor::onFileSetRefChange() {
	view_->setFileSetRefs(fileSetRefEditor_->items());
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