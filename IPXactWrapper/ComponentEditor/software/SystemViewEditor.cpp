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

#include <QApplication>

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
hierRefEditor_(NULL)
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

    connect(&nameEditor_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(hierRefEditor_, SIGNAL(vlnvEdited()),
        this, SLOT(onHierRefChange()), Qt::UniqueConnection);

    hierRefEditor_->setTitle(tr("Hierarchy reference"));

    refresh();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&nameEditor_);
    layout->addWidget(hierRefEditor_);
    layout->addStretch();
    layout->setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: SystemViewEditor::~SystemViewEditor()
//-----------------------------------------------------------------------------
SystemViewEditor::~SystemViewEditor()
{

}

//-----------------------------------------------------------------------------
// Function: SystemViewEditor::isValid()
//-----------------------------------------------------------------------------
bool SystemViewEditor::isValid() const
{
    // if name group is not valid
    if (!nameEditor_.isValid() || !hierRefEditor_->isValid())
        return false;

    return true;
}

void SystemViewEditor::refresh() {
    nameEditor_.refresh();
    hierRefEditor_->setVLNV(view_->getHierarchyRef());
}

void SystemViewEditor::onHierRefChange() {
    view_->setHierarchyRef(hierRefEditor_->getVLNV());
    emit contentChanged();
}

void SystemViewEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/systemview.html");
}
