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
    hierRefEditor_->setImplementationFilter(true, KactusAttribute::KTS_SW);
	hierRefEditor_->setTitle(tr("Hierarchy reference"));
	hierRefEditor_->setMandatory(false);

	fileSetRefEditor_ = new FileSetRefEditor(component, tr("File set references"), this);
	fileSetRefEditor_->initialize();

	swBuildCommands_ = new SWBuildCommandEditor(component, swView->getSWBuildCommands(), this);

	bspEditor_ = new BSPBuildEditor(swView->getBSPBuildCommand(), component, this);

	// BSP is displayed only for HW components
	if (component->getComponentImplementation() != KactusAttribute::KTS_HW) {
		bspEditor_->hide();
	}
	else {
		bspEditor_->show();
	}

    connect(&nameEditor_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(hierRefEditor_, SIGNAL(vlnvEdited()),
        this, SLOT(onHierRefChange()), Qt::UniqueConnection);
	 connect(fileSetRefEditor_, SIGNAL(contentChanged()),
		 this, SLOT(onFileSetRefChange()), Qt::UniqueConnection);
	 connect(swBuildCommands_, SIGNAL(contentChanged()),
		 this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	 connect(bspEditor_, SIGNAL(contentChanged()),
		 this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    refresh();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&nameEditor_);
    layout->addWidget(hierRefEditor_);
	 layout->addWidget(fileSetRefEditor_);
	 layout->addWidget(swBuildCommands_);
	 layout->addWidget(bspEditor_);
    layout->addStretch();
	layout->setContentsMargins(0, 0, 0, 0);
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
