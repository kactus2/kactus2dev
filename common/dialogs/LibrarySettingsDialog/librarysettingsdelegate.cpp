//-----------------------------------------------------------------------------
// File: LibrarySettingsDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 07.02.2013
//
// Description:
// This delegate is used to draw the check marks in the middle of default and active library columns.
//-----------------------------------------------------------------------------

#include "librarysettingsdelegate.h"

#include <common/dialogs/LibrarySettingsDialog/LibrarySettingsColumns.h>

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
	QStyleOptionViewItem viewItemOption(option);

    if (index.column() == LibrarySettingsColumns::DEFAULT ||index.column() == LibrarySettingsColumns::ACTIVE)
	{
        viewItemOption.decorationAlignment = Qt::AlignHCenter | Qt::AlignCenter;
        viewItemOption.decorationPosition = QStyleOptionViewItem::Top;
	}

	QStyledItemDelegate::paint(painter, viewItemOption, index);
}
