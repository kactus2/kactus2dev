//-----------------------------------------------------------------------------
// File: businterfacesdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.06.2012
//
// Description:
// The delegate that provides editors for bus interfaces.
//-----------------------------------------------------------------------------

#include "businterfacesdelegate.h"
#include "BusInterfaceColumns.h"

#include <common/widgets/interfaceModeSelector/interfacemodeselector.h>

#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: BusInterfacesDelegate::BusInterfacesDelegate()
//-----------------------------------------------------------------------------
BusInterfacesDelegate::BusInterfacesDelegate(QObject *parent):
MultilineDescriptionDelegate(parent)
{
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesDelegate::~BusInterfacesDelegate()
//-----------------------------------------------------------------------------
BusInterfacesDelegate::~BusInterfacesDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* BusInterfacesDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, 
    QModelIndex const& index) const
{
    if (index.column() == BusInterfaceColumns::NAME_COLUMN)
    {
        QLineEdit* edit = new QLineEdit(parent);
        connect(edit, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return edit;
    }
    else if (index.column() == BusInterfaceColumns::BUSDEF_COLUMN ||
        index.column() == BusInterfaceColumns::ABSDEF_COLUMN) 
    {    		
        Q_ASSERT_X(false, "BusInterfacesDelegate::createEditor()", 
            "Attempting to create editor for non-editable index.");
        return 0;
    }
    else if (index.column() == BusInterfaceColumns::IF_MODE_COLUMN)
    {
        InterfaceModeSelector* selector = new InterfaceModeSelector(parent); 
        connect(selector, SIGNAL(currentIndexChanged(int)), 
            this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return selector;
    }
    else
    {
        return MultilineDescriptionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesDelegate::setEditorData()
//-----------------------------------------------------------------------------
void BusInterfacesDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == BusInterfaceColumns::NAME_COLUMN)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        const QString text = index.data(Qt::DisplayRole).toString();
        edit->setText(text);
    }
    else if (index.column() == BusInterfaceColumns::BUSDEF_COLUMN ||
        index.column() ==  BusInterfaceColumns::ABSDEF_COLUMN)
    {
        Q_ASSERT(false);			
    }
    else if (index.column() == BusInterfaceColumns::IF_MODE_COLUMN)
    {
        InterfaceModeSelector* selector = qobject_cast<InterfaceModeSelector*>(editor);
        Q_ASSERT(selector);

        QString modeName = index.data(Qt::DisplayRole).toString();
        selector->setMode(modeName);
    }
    else
    {
        MultilineDescriptionDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesDelegate::setModelData()
//-----------------------------------------------------------------------------
void BusInterfacesDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, 
    QModelIndex const& index) const
{
    if (index.column() == BusInterfaceColumns::NAME_COLUMN)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        QString text = edit->text();
        model->setData(index, text, Qt::EditRole);
    }
    else if (index.column() == BusInterfaceColumns::BUSDEF_COLUMN || 
        index.column() == BusInterfaceColumns::ABSDEF_COLUMN)
    {
        Q_ASSERT(false);			
    }
    else if (index.column() == BusInterfaceColumns::IF_MODE_COLUMN)
    {
        InterfaceModeSelector* selector = qobject_cast<InterfaceModeSelector*>(editor);
        Q_ASSERT(selector);

        General::InterfaceMode mode = selector->selected();
        QString modeStr = General::interfaceMode2Str(mode);
        model->setData(index, modeStr, Qt::EditRole);
    }	
    else
    {
        MultilineDescriptionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int BusInterfacesDelegate::descriptionColumn() const
{
    return BusInterfaceColumns::DESCRIPTION_COLUMN;
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void BusInterfacesDelegate::commitAndCloseEditor()
{
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	QComboBox* combo = qobject_cast<QComboBox*>(edit);

	if (combo)
    {
		emit commitData(combo);
	}
	else {
		emit commitData(edit);
		emit closeEditor(edit);
	}
}
