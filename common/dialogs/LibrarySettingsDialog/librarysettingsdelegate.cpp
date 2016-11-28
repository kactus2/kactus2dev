/* 
 *	Created on: 7.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	librarysettingsdelegate.cpp
 * 	Project:	Kactus 2
*/
#include "librarysettingsdelegate.h"

#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QEvent>

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDelegate::LibrarySettingsDelegate()
//-----------------------------------------------------------------------------
LibrarySettingsDelegate::LibrarySettingsDelegate(QObject *parent): QStyledItemDelegate(parent)
{
}

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDelegate::~LibrarySettingsDelegate()
//-----------------------------------------------------------------------------
LibrarySettingsDelegate::~LibrarySettingsDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDelegate::paint()
//-----------------------------------------------------------------------------
void LibrarySettingsDelegate::paint(QPainter* painter, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
	QStyleOptionViewItemV4 viewItemOption(option);

	if (index.column() == DEF_COLUMN ||index.column() == ACTIVE_COL)
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

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDelegate::editorEvent()
//-----------------------------------------------------------------------------
bool LibrarySettingsDelegate::editorEvent(QEvent* event, QAbstractItemModel* model,
    QStyleOptionViewItem const& option, QModelIndex const& index)
{
	Q_ASSERT(event);
	Q_ASSERT(model);

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

        if (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked)
        {
            newState = Qt::Unchecked;
        }
        else
        {
            newState = Qt::Checked;
        }

         model->setData(index, newState, Qt::CheckStateRole);
	}
	else
    {
		return false;
	}

	return QStyledItemDelegate::editorEvent(event, model, option, index);
}
