//-----------------------------------------------------------------------------
// File: FieldSliceEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 09.08.2023
//
// Description:
// Editor to add/remove/edit the mode-elements of a component.
//-----------------------------------------------------------------------------

#include "FieldSliceEditor.h"
#include "FieldSliceColumns.h"

#include <common/widgets/summaryLabel/summarylabel.h>
#include <KactusAPI/include/LibraryInterface.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>


#include <editors/common/ExpressionSet.h>

#include <IPXACTmodels/Component/Component.h>

#include "FieldSliceDelegate.h"

#include <QVBoxLayout>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: FieldSliceEditor::FieldSliceEditor()
//-----------------------------------------------------------------------------
FieldSliceEditor::FieldSliceEditor(QSharedPointer<Component> component, 
	QSharedPointer<Mode> mode, 
	//QSharedPointer<FieldSliceValidator> validator,
	LibraryInterface* handler, 
	QWidget* parent) :
QWidget(parent),
	component_(component),
    view_(this),
    proxy_(this),
    model_(mode, this)
{

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);
	view_.setModel(&proxy_);

// 	const QString compPath = handler->getDirectoryPath(component->getVlnv());
// 	QString defPath = QString("%1/FieldSliceListing.csv").arg(compPath);
// 	view_.setDefaultImFieldExFieldPath(defPath);
// 	view_.setAllowImFieldExField(true);
// 	view_.setItemsDraggable(false);
// 	view_.setSortingEnabled(true);




	auto delegate = new FieldSliceDelegate(component->getMemoryMaps(), component->getAddressSpaces(), this);

	view_.setItemDelegate(delegate);

	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
        &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

   
}

//-----------------------------------------------------------------------------
// Function: FieldSliceEditor::refresh()
//-----------------------------------------------------------------------------
void FieldSliceEditor::refresh()
{
	view_.update();
}
