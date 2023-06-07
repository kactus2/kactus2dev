//-----------------------------------------------------------------------------
// File: ComponentEditorTreeDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 23.05.2023
//
// Description:
// Component editor navigation tree delegate for custom painting of items.
//-----------------------------------------------------------------------------

#include "ComponentEditorTreeDelegate.h"

#include <QPainter>

#include <QFontMetrics>

//-----------------------------------------------------------------------------
// Function: ComponentEditorTreeDelegate::ComponentEditorTreeDelegate()
//-----------------------------------------------------------------------------
ComponentEditorTreeDelegate::ComponentEditorTreeDelegate(QObject* parent)
    : QItemDelegate(parent)
{
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorTreeDelegate::paint()
//-----------------------------------------------------------------------------
void ComponentEditorTreeDelegate::paint(QPainter* painter,QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    QStyleOptionViewItem optionOverride = option;
    optionOverride.font = qvariant_cast<QFont>(index.data(Qt::FontRole));
    optionOverride.palette.setColor(QPalette::Text, index.data(Qt::ForegroundRole).value<QColor>());
    optionOverride.decorationSize = QSize(option.rect.height() - 2, option.rect.height() - 2);

    bool hasIndicator = index.data(Qt::UserRole).toBool();
    auto indicatorSize = QSize(10, 10);
    QRect indicatorRect;
    if (hasIndicator)
    {
        indicatorRect.setSize(indicatorSize);
    }

    QPixmap decoration = qvariant_cast<QIcon>(index.data(Qt::DecorationRole)).pixmap(optionOverride.decorationSize);
    bool hasDecoration = decoration.isNull() == false;
    QRect decorationRect;
    if (hasDecoration)
    {
        decorationRect.setSize(optionOverride.decorationSize);
    }

    const int MARGIN = 4;

    QString displayText = index.data(Qt::DisplayRole).toString();
    auto metrics = QFontMetrics(optionOverride.font);
    int textWidth = metrics.horizontalAdvance(displayText);

    //! Render the items in the following order: display, decoration, indicator.
    QRect displayRect;
    displayRect.setX(option.rect.x() + MARGIN);
    displayRect.setY(option.rect.y() + 2);
    displayRect.setWidth(qMin(textWidth + 2 * MARGIN, option.rect.width() - indicatorRect.width() - decorationRect.width() - 4 * MARGIN));
    displayRect.setHeight(option.rect.height() - 2);

    decorationRect.moveTo(displayRect.right() + MARGIN/2, (option.rect.y() + (option.rect.height() - optionOverride.decorationSize.height()) / 2 + 1));
    indicatorRect.moveTo(decorationRect.right() + MARGIN/2, (option.rect.y() + (option.rect.height() - indicatorSize.height()) / 2 + 1));

    drawBackground(painter, optionOverride, index);

    drawDisplay(painter, optionOverride, displayRect, displayText);

    if (hasIndicator)
    {
        QPixmap indicator(":icons/common/graphics/traffic-light_gray.png");
        indicator = indicator.scaled(indicatorSize);
        drawDecoration(painter, optionOverride, indicatorRect, indicator);
    }

    if (hasDecoration)
    {
        drawDecoration(painter, optionOverride, decorationRect, decoration);
    }
    
    drawFocus(painter, optionOverride, displayRect);
}
