//-----------------------------------------------------------------------------
// File: ResetTypesEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.01.2019
//
// Description:
// Editor to add/remove/edit the reset types-elements of a component.
//-----------------------------------------------------------------------------

#include "ResetTypesEditor.h"

#include <IPXACTmodels/Component/Component.h>

#include <common/widgets/summaryLabel/summarylabel.h>

#include <editors/ComponentEditor/resetTypes/ResetTypesDelegate.h>

#include <library/LibraryInterface.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ResetTypesEditor::ResetTypesEditor()
//-----------------------------------------------------------------------------
ResetTypesEditor::ResetTypesEditor(QSharedPointer<Component> component, LibraryInterface* handler,
    ComponentValidator& validator, QWidget* parent):
ItemEditor(component, handler, parent),
view_(this),
proxy_(this),
model_(component, validator, this)
{
    SummaryLabel* summaryLabel = new SummaryLabel(tr("Reset types"), this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
    layout->addWidget(&view_);
    layout->setContentsMargins(0, 0, 0, 0);

    proxy_.setSourceModel(&model_);
    proxy_.setSortCaseSensitivity(Qt::CaseInsensitive);

    view_.setModel(&proxy_);
    view_.resizeColumnsToContents();
    view_.setSortingEnabled(true);
    view_.sortByColumn(0, Qt::AscendingOrder);

    const QString compPath = handler->getDirectoryPath(component->getVlnv());
    QString defPath = QString("%1/cpusListing.csv").arg(compPath);
    view_.setDefaultImportExportPath(defPath);
    view_.setAllowImportExport(true);
    view_.setItemsDraggable(false);
    view_.setItemDelegate(new ResetTypesDelegate(this));

    connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&view_, SIGNAL(addItem(const QModelIndex&)),
        &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
        &model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ResetTypesEditor::~ResetTypesEditor()
//-----------------------------------------------------------------------------
ResetTypesEditor::~ResetTypesEditor()
{

}

//-----------------------------------------------------------------------------
// Function: ResetTypesEditor::refresh()
//-----------------------------------------------------------------------------
void ResetTypesEditor::refresh()
{
	view_.update();
}

//-----------------------------------------------------------------------------
// Function: ResetTypesEditor::showEvent()
//-----------------------------------------------------------------------------
void ResetTypesEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
//	emit helpUrlRequested("componenteditor/cpus.html");
}
