//-----------------------------------------------------------------------------
// File: localheadersavedelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.04.2013
//
// Description:
// The delegate class to select the local headers to be generated.
//-----------------------------------------------------------------------------

#include "localheadersavedelegate.h"
#include "localheadersavemodel.h"
#include <common/widgets/viewSelector/viewselector.h>

//-----------------------------------------------------------------------------
// Function: localheadersavedelegate::LocalHeaderSaveDelegate()
//-----------------------------------------------------------------------------
LocalHeaderSaveDelegate::LocalHeaderSaveDelegate(QSharedPointer<Component> component,
	QObject *parent):
QStyledItemDelegate(parent),
component_(component)
{
	Q_ASSERT(component_);
}

//-----------------------------------------------------------------------------
// Function: localheadersavedelegate::~LocalHeaderSaveDelegate()
//-----------------------------------------------------------------------------
LocalHeaderSaveDelegate::~LocalHeaderSaveDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: localheadersavedelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* LocalHeaderSaveDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option,
                                                const QModelIndex& index ) const
{
    if (index.column() == LocalHeaderSaveModel::SW_VIEW_NAME)
    {
        ViewSelector* viewSelector = new ViewSelector(ViewSelector::SW_VIEWS, component_, parent, false);
        viewSelector->refresh();
        viewSelector->addItem(tr("all"));
        viewSelector->setEditable(true);
        return viewSelector;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: localheadersavedelegate::setEditorData()
//-----------------------------------------------------------------------------
void LocalHeaderSaveDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
    if (index.column() == LocalHeaderSaveModel::SW_VIEW_NAME)
    {
		ViewSelector* viewSelector = qobject_cast<ViewSelector*>(editor);
		Q_ASSERT(viewSelector);

		// fetch the view name from the model and set it as selected
		QString viewName = index.model()->data(index, Qt::DisplayRole).toString();
		viewSelector->selectView(viewName);
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: localheadersavedelegate::setModelData()
//-----------------------------------------------------------------------------
void LocalHeaderSaveDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index )
    const
{
    if (index.column() == LocalHeaderSaveModel::SW_VIEW_NAME)
    {
		ViewSelector* viewSelector = qobject_cast<ViewSelector*>(editor);
		Q_ASSERT(viewSelector);

		QString selectedView = viewSelector->currentText();
		model->setData(index, selectedView, Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}
