//-----------------------------------------------------------------------------
// File: ThemeableIconWidgets.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 23.02.2026
//
// Description:
// Widgets/items with icons able to be themed to follow system theme.
//-----------------------------------------------------------------------------

#include "ThemeableIconWidgets.h"

ThemeableItemWithIcon::ThemeableItemWithIcon(QString const& iconPath) :
    iconPath_(iconPath)
{

}

ThemeablePushButton::ThemeablePushButton(QString const& iconPath, QString const& buttonText, QWidget* parent) :
    ThemeableItemWithIcon(iconPath),
    QPushButton(QIcon(iconPath), buttonText, parent)
{
    // Theme icon immediately upon construction
    styleIconToTheme();
}

void ThemeablePushButton::styleIconToTheme()
{
    setIcon(KactusUtils::getIconStyledToTheme(iconPath_));
}

ThemeableAction::ThemeableAction(QString const& iconPath, QString const& tooltip, QWidget* parent) :
    ThemeableItemWithIcon(iconPath),
    QAction(QIcon(iconPath), tooltip, parent)
{
    styleIconToTheme();
}

void ThemeableAction::styleIconToTheme()
{
    setIcon(KactusUtils::getIconStyledToTheme(iconPath_));
}

ThemeableComboBox::ThemeableComboBox(QWidget* parent) :
    ThemeableItemWithIcon(QString()),
    QComboBox(parent)
{
    // No call to styleIconToTheme() here, as items with icons are added later
}

void ThemeableComboBox::styleIconToTheme()
{
    for (int i = 0; i < count(); ++i)
    {
        setItemIcon(i, KactusUtils::getIconStyledToTheme(itemIconPaths_.at(i)));
    }
}

void ThemeableComboBox::addItem(QString const& iconPath, QString const& text, QVariant const& userData)
{
    QComboBox::addItem(QIcon(iconPath), text, userData);
    itemIconPaths_.append(iconPath);
}

void ThemeableComboBox::removeItem(int index)
{
    QComboBox::removeItem(index);
    itemIconPaths_.removeAt(index);
}

