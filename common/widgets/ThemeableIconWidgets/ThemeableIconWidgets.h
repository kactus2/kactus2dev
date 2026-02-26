//-----------------------------------------------------------------------------
// File: ThemeableIconWidgets.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 23.02.2026
//
// Description:
// Widgets/items with icons able to be themed to follow system theme.
//-----------------------------------------------------------------------------

#pragma once

#include <common/KactusUtils.h>

#include <QPushButton>
#include <QAction>
#include <QComboBox>

// -------------------------------------------------
// class ThemeableItemWithIcon - base class
// -------------------------------------------------
class ThemeableItemWithIcon
{
public:

    /*!
     *  The constructor
     *
     *    @param[in] iconPath   The icon path to use.
     */
    explicit ThemeableItemWithIcon(QString const& iconPath);

    virtual ~ThemeableItemWithIcon() = default;
    ThemeableItemWithIcon(ThemeableItemWithIcon const& other) = delete;
    ThemeableItemWithIcon& operator=(ThemeableItemWithIcon const& other) = delete;

    /*!
     *  Changes icon of widget to match system theme.
     *  Widget-specific implementation.
     */
    virtual void styleIconToTheme() = 0;

protected:

    //! Path of the widget icon.
    QString iconPath_;
};

// -------------------------------------------------
// class ThemeablePushButton
// -------------------------------------------------
class ThemeablePushButton : public QPushButton, public ThemeableItemWithIcon
{
    Q_OBJECT
public:

    ThemeablePushButton(QString const& iconPath, QString const& buttonText, QWidget* parent = nullptr);
    virtual ~ThemeablePushButton() = default;

    void styleIconToTheme() override;

private:

};

// -------------------------------------------------
// class ThemeableAction
// -------------------------------------------------
class ThemeableAction : public QAction, public ThemeableItemWithIcon
{
    Q_OBJECT
public:

    ThemeableAction(QString const& iconPath, QString const& tooltip, QWidget* parent = nullptr);
    virtual ~ThemeableAction() = default;

    void styleIconToTheme() override;

private:

};

// -------------------------------------------------
// class ThemeableComboBox
// -------------------------------------------------

// Note: When using this class combobox items must be added and removed using the overriding methods defined
// here. Otherwise icons might get messed up.

class ThemeableComboBox : public QComboBox, public ThemeableItemWithIcon
{
    Q_OBJECT
public:

    ThemeableComboBox(QWidget* parent = nullptr);

    virtual ~ThemeableComboBox() = default;

    /*!
     *  Styles all item icons according to theme (not just one icon)
     */
    void styleIconToTheme() override;

    /*!
     *  Overriding method for adding an item to combobox. Adds new item and styled icon.
     *
     *    @param[in] iconPath     Path of icon to add next to item.
     *    @param[in] text         Item text.
     *    @param[in] userData     Optional extra data to associate with item.
     */
    void addItem(QString const& iconPath, QString const& text, QVariant const& userData = QVariant());

    /*!
     *  Overriding method for removing item from combobox. Removes item and its icon.
     *
     *    @param[in] index   Index of item to remove
     */
    void removeItem(int index);

private:

    //! Combobox item icon paths, indexes correspond to items in combo box
    QStringList itemIconPaths_;
};