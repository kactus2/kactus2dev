/* 
 *  	Created on: 21.6.2012
 *      Author: Antti Kamppi
 * 		filename: bridgesdelegate.cpp
 *		Project: Kactus 2
 */

#include "bridgesdelegate.h"
#include <common/widgets/interfaceSelector/interfaceselector.h>
#include <models/generaldeclarations.h>

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

BridgesDelegate::BridgesDelegate(QSharedPointer<Component> component, 
								 QObject *parent):
QStyledItemDelegate(parent),
component_(component) {

}

BridgesDelegate::~BridgesDelegate() {
}

QWidget* BridgesDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	switch (index.column()) {
		case BridgesDelegate::MASTER_COLUMN: {
			InterfaceSelector* selector = new InterfaceSelector(component_, parent, General::MASTER);
			return selector;
											 }
		case BridgesDelegate::OPAQUE_COLUMN: 
		default: {
			return QStyledItemDelegate::createEditor(parent, option, index);
				 }
	}
}

void BridgesDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
	switch (index.column()) {
		case BridgesDelegate::MASTER_COLUMN: {
			InterfaceSelector* selector = qobject_cast<InterfaceSelector*>(editor);
			Q_ASSERT(selector);

			selector->refresh();
			selector->selectInterface(index.model()->data(index, Qt::DisplayRole).toString());
			break;
											 }
		case BridgesDelegate::OPAQUE_COLUMN: 
		default: {
			QStyledItemDelegate::setEditorData(editor, index);
				 }
	}
}

void BridgesDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
	switch (index.column()) {
		case BridgesDelegate::MASTER_COLUMN: {
			InterfaceSelector* selector = qobject_cast<InterfaceSelector*>(editor);
			Q_ASSERT(selector);

			QString selected = selector->currentText();
			model->setData(index, selected, Qt::EditRole);
			break;
											 }
		case BridgesDelegate::OPAQUE_COLUMN: 
		default: {
			QStyledItemDelegate::setModelData(editor, model, index);
				 }
	}
}

void BridgesDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {
	QStyleOptionViewItemV4 viewItemOption(option);

	if (index.column() == OPAQUE_COLUMN)
	{
		painter->fillRect(option.rect, Qt::white);

		const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

		QRect newRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
			QSize(option.decorationSize.width() + 5, option.decorationSize.height()),
			QRect(option.rect.x() + textMargin, option.rect.y(),
			option.rect.width() - (2 * textMargin), option.rect.height()));
		viewItemOption.rect = newRect;
	}

	QStyledItemDelegate::paint(painter, viewItemOption, index);
}

bool BridgesDelegate::editorEvent( QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index ) {
	Q_ASSERT(event);
	Q_ASSERT(model);

	// Always reset the ad-hoc group modify flag.
	if (event->type() == QEvent::MouseButtonRelease)
	{
		opaqueGroupModify_ = false;
	}

	// Make sure that the item is checkable.
	Qt::ItemFlags flags = model->flags(index);

	if (!(flags & Qt::ItemIsUserCheckable) || !(flags & Qt::ItemIsEnabled))
	{
		return false;
	}

	// Make sure that we have a check state.
	QVariant value = index.data(Qt::CheckStateRole);

	if (!value.isValid())
	{
		return false;
	}

	Qt::CheckState newState;

	// Handle the mouse button events.
	if (event->type() == QEvent::MouseButtonPress)
	{
		const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

		QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
			option.decorationSize,
			QRect(option.rect.x() + (2 * textMargin), option.rect.y(),
			option.rect.width() - (2 * textMargin),
			option.rect.height()));

		if (!checkRect.contains(static_cast<QMouseEvent*>(event)->pos()))
		{
			return false;
		}

		newState = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked ? Qt::Unchecked : Qt::Checked);
		opaqueGroupModify_ = true;
		opaque = newState;
	}
	else if (event->type() == QEvent::MouseMove)
	{
		if (!opaqueGroupModify_ || static_cast<Qt::CheckState>(value.toInt()) == opaque)
		{
			return false;
		}

		const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

		QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
			option.decorationSize,
			QRect(option.rect.x() + (2 * textMargin), option.rect.y(),
			option.rect.width() - (2 * textMargin),
			option.rect.height()));

		if (!checkRect.contains(static_cast<QMouseEvent*>(event)->pos()))
		{
			return false;
		}

		newState = opaque;
	}
	else if (event->type() == QEvent::KeyPress)
	{
		if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space &&
			static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return model->setData(index, newState, Qt::CheckStateRole);
}
