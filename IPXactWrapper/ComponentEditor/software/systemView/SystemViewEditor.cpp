//-----------------------------------------------------------------------------
// File: SystemViewEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.5.2012
//
// Description:
// System view editor.
//-----------------------------------------------------------------------------

#include "SystemViewEditor.h"
#include <mainwindow/mainwindow.h>
#include <models/SystemView.h>

#include <common/widgets/viewSelector/viewselector.h>

#include <QApplication>
#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: SystemViewEditor::SystemViewEditor()
//-----------------------------------------------------------------------------
SystemViewEditor::SystemViewEditor(QSharedPointer<Component> component, 
                           QSharedPointer<SystemView> systemView,
                           LibraryInterface* libHandler, 
                           QWidget *parent):
ItemEditor(component, libHandler, parent), 
view_(systemView.data()),
nameEditor_(systemView->getNameGroup(), this, tr("Name and description")),
hierRefEditor_(NULL),
HWViewRefEditor_(NULL),
fileSetRefEditor_(NULL)
{
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
	hierRefEditor_->setTitle(tr("Hierarchy reference"));

	fileSetRefEditor_ = new FileSetRefEditor(component, tr("File set references"), this);
	fileSetRefEditor_->initialize();

	HWViewRefEditor_ = new ViewSelector(ViewSelector::BOTH_HW_VIEWS, component, this);
	HWViewRefEditor_->setFixedWidth(200);

    connect(&nameEditor_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(hierRefEditor_, SIGNAL(vlnvEdited()),
        this, SLOT(onHierRefChange()), Qt::UniqueConnection);
	 connect(fileSetRefEditor_, SIGNAL(contentChanged()),
		 this, SLOT(onFileSetRefChange()), Qt::UniqueConnection);
	 connect(HWViewRefEditor_, SIGNAL(viewSelected(const QString&)),
		 this, SLOT(onHWViewChange(const QString&)), Qt::UniqueConnection);

    refresh();

	 QGroupBox* HWViewGroup = new QGroupBox(tr("HW View"), this);
	 QLabel* HWViewLabel = new QLabel(tr("Used HW view"), HWViewGroup);

	 QHBoxLayout* groupLayout = new QHBoxLayout(HWViewGroup);
	 groupLayout->addWidget(HWViewLabel);
	 groupLayout->addWidget(HWViewRefEditor_);
	 groupLayout->addStretch();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&nameEditor_);
    layout->addWidget(hierRefEditor_);
	 layout->addWidget(HWViewGroup);
	 layout->addWidget(fileSetRefEditor_);
    layout->addStretch();
    layout->setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: SystemViewEditor::~SystemViewEditor()
//-----------------------------------------------------------------------------
SystemViewEditor::~SystemViewEditor() {
}

//-----------------------------------------------------------------------------
// Function: SystemViewEditor::isValid()
//-----------------------------------------------------------------------------
bool SystemViewEditor::isValid() const
{
    // if name group is not valid
    if (!nameEditor_.isValid() || !hierRefEditor_->isValid()) {
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

	 // if there is a HW view selected
	 QString selectedHWView = HWViewRefEditor_->currentText();
	 if (!selectedHWView.isEmpty()) {

		 // if the component does not contain the referenced view.
		 if (!component()->hasView(selectedHWView)) {
			 return false;
		 }
	 }

    return true;
}

void SystemViewEditor::refresh() {
    nameEditor_.refresh();
    hierRefEditor_->setVLNV(view_->getHierarchyRef());
	 HWViewRefEditor_->refresh();
	 HWViewRefEditor_->selectView(view_->getHWViewRef());
	 fileSetRefEditor_->setItems(view_->getFileSetRefs());
}

void SystemViewEditor::onHierRefChange() {
    view_->setHierarchyRef(hierRefEditor_->getVLNV());
    emit contentChanged();
}

void SystemViewEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/systemview.html");
}

void SystemViewEditor::onFileSetRefChange() {
	view_->setFileSetRefs(fileSetRefEditor_->items());
	emit contentChanged();
}

void SystemViewEditor::onHWViewChange( const QString& /*viewName*/ ) {
	view_->setHWViewRef(HWViewRefEditor_->currentText());
	emit contentChanged();
}
