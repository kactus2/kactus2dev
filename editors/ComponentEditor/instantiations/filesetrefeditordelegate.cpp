//-----------------------------------------------------------------------------
// File: filesetrefeditordelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.12.2011
//
// Description:
// Delegate that manages the editors to select a file set in a component.
//-----------------------------------------------------------------------------

#include "filesetrefeditordelegate.h"

#include <IPXACTmodels/Component/Component.h>

#include <QComboBox>
#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: filesetrefeditordelegate::FileSetRefEditorDelegate()
//-----------------------------------------------------------------------------
FileSetRefEditorDelegate::FileSetRefEditorDelegate( QObject *parent, QSharedPointer<Component> component ):
ComboDelegate(parent), 
component_(component)
{
	Q_ASSERT(component_);
}

//-----------------------------------------------------------------------------
// Function: filesetrefeditordelegate::~FileSetRefEditorDelegate()
//-----------------------------------------------------------------------------
FileSetRefEditorDelegate::~FileSetRefEditorDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: filesetrefeditordelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* FileSetRefEditorDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
	Q_ASSERT(component_);
	QComboBox* combo = qobject_cast<QComboBox*>(ComboDelegate::createEditor(parent, option, index));
	Q_ASSERT(combo);

	QStringList filesetNames = component_->getFileSetNames();
	combo->addItems(filesetNames);
	return combo;
}
