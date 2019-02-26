//-----------------------------------------------------------------------------
// File: TableItemMatcher.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 25.02.2019
//
// Description:
// Automatically defines possible matches between two items.
//-----------------------------------------------------------------------------

#ifndef TABLEITEMMATCHER_H
#define TABLEITEMMATCHER_H

#include <QTableWidget>
#include <QModelIndex>
//#include <QListView>

#include <QSharedPointer>

class Component;

//-----------------------------------------------------------------------------
//! Automatically defines possible matches between two items.
//-----------------------------------------------------------------------------
class TableItemMatcher
{
public:

    /*!
     *  The constructor.
     */
    TableItemMatcher() = default;

    /*!
     *  Destructor.
     */
    virtual ~TableItemMatcher() = default;

    /*!
     *  Check if the item of the selected source index can be connected to the item of the target index.
     *
     *      @param [in] sourceIndex         Index of the source item.
     *      @param [in] targetIndex         Index of the target item.
     *      @param [in] sourceComponent     Component containing the source item.
     *      @param [in] targetComponent     Component containing the target item.
     *
     *      @return True, if the source item can be connected with the target item, false otherwise.
     */
    bool canDrop(QModelIndex const& sourceIndex, QModelIndex const& targetIndex,
        QSharedPointer<Component> sourceComponent, QSharedPointer<Component> targetComponent) const;

    // No copying. No assignments.
    TableItemMatcher(TableItemMatcher const& rhs) = delete;
    TableItemMatcher& operator=(TableItemMatcher const& rhs) = delete;

private:

    /*!
     *  Check if the selected items can be connected.
     *
     *      @param [in] firstItem           The selected item from the first component.
     *      @param [in] firstComponent      Component containing the first item.
     *      @param [in] secondItem          The selected item from the second component.
     *      @param [in] secondComponent     Component containing the second item.
     *
     *      @return True, if the items can be connected, false otherwise.
     */
    virtual bool itemsCanBeConnected(QString const& firstItem, QSharedPointer<Component> firstComponent,
        QString const& secondItem, QSharedPointer<Component> secondComponent) const = 0;
};

//-----------------------------------------------------------------------------

#endif // TABLEITEMMATCHER_H
