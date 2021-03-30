//-----------------------------------------------------------------------------
// File: filesetrefeditordelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 09.12.2011
//
// Description:
// Delegate that manages the editors to select a file set in a component.
//-----------------------------------------------------------------------------

#include "filesetrefeditordelegate.h"

#include <IPXACTmodels/Component/Component.h>

#include <editors/ComponentEditor/fileSet/interfaces/FileSetInterface.h>

#include <QComboBox>
#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: filesetrefeditordelegate::FileSetRefEditorDelegate()
//-----------------------------------------------------------------------------
FileSetRefEditorDelegate::FileSetRefEditorDelegate(QObject *parent, FileSetInterface* fileSetInterface):
ComboDelegate(parent),
fileSetInterface_(fileSetInterface)
{

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
	QComboBox* combo = qobject_cast<QComboBox*>(ComboDelegate::createEditor(parent, option, index));
	Q_ASSERT(combo);

    QStringList filesetNames;
    for (auto setName : fileSetInterface_->getItemNames())
    {
        filesetNames.append(QString::fromStdString(setName));
    }

    combo->addItems(filesetNames);
	return combo;
}
