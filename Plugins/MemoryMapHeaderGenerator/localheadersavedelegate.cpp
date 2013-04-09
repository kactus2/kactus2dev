/* 
 *	Created on:	9.4.2013
 *	Author:		Antti Kamppi
 *	File name:	localheadersavedelegate.cpp
 *	Project:		Kactus 2
*/

#include "localheadersavedelegate.h"
#include "localheadersavemodel.h"
#include <common/widgets/viewSelector/viewselector.h>

LocalHeaderSaveDelegate::LocalHeaderSaveDelegate(QSharedPointer<Component> component,
	QObject *parent):
QStyledItemDelegate(parent),
component_(component) {

	Q_ASSERT(component_);
}

LocalHeaderSaveDelegate::~LocalHeaderSaveDelegate() {
}

QWidget* LocalHeaderSaveDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	switch (index.column()) {
	case LocalHeaderSaveModel::SW_VIEW_NAME: {

		// view selector is used to select among the SW views of the component.
		ViewSelector* viewSelector = new ViewSelector(ViewSelector::SW_VIEWS, component_, parent, false);
		viewSelector->refresh();
		viewSelector->addItem(tr("all"));
		viewSelector->setEditable(true);
		return viewSelector;
														  }
	default: {
		return QStyledItemDelegate::createEditor(parent, option, index);
				}
	}
}

void LocalHeaderSaveDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
	switch (index.column()) {
	case LocalHeaderSaveModel::SW_VIEW_NAME: {
		ViewSelector* viewSelector = qobject_cast<ViewSelector*>(editor);
		Q_ASSERT(viewSelector);

		// fetch the view name from the model and set it as selected
		QString viewName = index.model()->data(index, Qt::DisplayRole).toString();
		viewSelector->selectView(viewName);
		break;
														  }
	default: {
		QStyledItemDelegate::setEditorData(editor, index);
		break;
				}
	}
}

void LocalHeaderSaveDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
	switch (index.column()) {
	case LocalHeaderSaveModel::SW_VIEW_NAME: {
		ViewSelector* viewSelector = qobject_cast<ViewSelector*>(editor);
		Q_ASSERT(viewSelector);

		QString selectedView = viewSelector->currentText();
		model->setData(index, selectedView, Qt::EditRole);
		break;
														  }
	default: {
		QStyledItemDelegate::setModelData(editor, model, index);
		break;
				}
	}
}
