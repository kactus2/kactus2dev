/* 
 *  	Created on: 14.6.2012
 *      Author: Antti Kamppi
 * 		filename: channelsdelegate.cpp
 *		Project: Kactus 2
 */

#include "channelsdelegate.h"
#include <common/widgets/EnumCollectionEditor/EnumCollectionEditor.h>
#include "channelinterfacemanagerdelegate.h"

#include <QLineEdit>
#include <QString>

ChannelsDelegate::ChannelsDelegate(QSharedPointer<Component> component,
								   QObject *parent):
QStyledItemDelegate(parent),
component_(component) {

	Q_ASSERT(component_);
}

ChannelsDelegate::~ChannelsDelegate() {
}

QWidget* ChannelsDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const {

	switch (index.column()) {
		case ChannelsDelegate::NAME_COLUMN:
		case ChannelsDelegate::DISPLAY_NAME_COLUMN:
		case ChannelsDelegate::DESCRIPTION_COLUMN: {
			QLineEdit* edit = new QLineEdit(parent);
			connect(edit, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return edit;
												   }
		case ChannelsDelegate::INTERFACE_COLUMN: {
			EnumCollectionEditor* editor = new EnumCollectionEditor(parent);
			return editor;
												 }
		default: {
			return QStyledItemDelegate::createEditor(parent, option, index);
				 }
	}
}

void ChannelsDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
	switch (index.column()) {
		case ChannelsDelegate::NAME_COLUMN:
		case ChannelsDelegate::DISPLAY_NAME_COLUMN:
		case ChannelsDelegate::DESCRIPTION_COLUMN:
            {
			    QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			    Q_ASSERT(edit);

			    const QString text = index.model()->data(index, Qt::DisplayRole).toString();
			    edit->setText(text);
			    break;
            }

		case ChannelsDelegate::INTERFACE_COLUMN:
            {
                EnumCollectionEditor* collectionEditor = static_cast<EnumCollectionEditor*>(editor);
                Q_ASSERT(collectionEditor != 0);

                QStringList selectedInterfaces =
                    index.model()->data(index, ChannelsDelegate::USER_DISPLAY_ROLE).toStringList();

                foreach (QString const& name, component_->getBusInterfaceNames())
                {
                    collectionEditor->addItem(name, selectedInterfaces.contains(name));
                }

                break;
            }

		default:
            {
			    QStyledItemDelegate::setEditorData(editor, index);
			    break;
            }
	}
}

void ChannelsDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
	switch (index.column()) {
		case ChannelsDelegate::NAME_COLUMN:
		case ChannelsDelegate::DISPLAY_NAME_COLUMN:
		case ChannelsDelegate::DESCRIPTION_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			QString text = edit->text();
			model->setData(index, text, Qt::EditRole);
			break;
												   }
		case ChannelsDelegate::INTERFACE_COLUMN: {
            EnumCollectionEditor* collectionEditor = static_cast<EnumCollectionEditor*>(editor);
            Q_ASSERT(collectionEditor != 0);

			QStringList interfaces = collectionEditor->getSelectedItems();
			model->setData(index, interfaces, ChannelsDelegate::USER_EDIT_ROLE);

			break;
												 }
		default: {
			QStyledItemDelegate::setModelData(editor, model, index);
			break;
				 }
	}
}

void ChannelsDelegate::commitAndCloseEditor() {
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
