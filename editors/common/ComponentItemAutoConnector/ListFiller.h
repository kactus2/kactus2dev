//-----------------------------------------------------------------------------
// File: ListFiller.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 12.02.2019
//
// Description:
// Fills a list with desired items.
//-----------------------------------------------------------------------------

#ifndef LISTFILLER_H
#define LISTFILLER_H

#include <QStandardItemModel>

class Component;

//-----------------------------------------------------------------------------
//! Fills a list with desired items.
//-----------------------------------------------------------------------------
class ListFiller
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] firstItem   The first component item to be connected.
     *      @param [in] secondItem  The second component item to be connected.
     *      @param [in] parent      The parent of this widget.
     */
    ListFiller() = default;

    /*!
     *  Destructor.
     */
    virtual ~ListFiller() = default;

    /*!
     *  Populate the selected model with items from the selected component.
     *
     *      @param [in] selectedList            The selected item model.
     *      @param [in] containingComponent     The selected component.
     */
    virtual void initializeList(QStandardItemModel* selectedList, QSharedPointer<Component> containingComponent)
        const = 0;

    // No copying. No assignments.
    ListFiller(ListFiller const& rhs) = delete;
    ListFiller& operator=(ListFiller const& rhs) = delete;
};

//-----------------------------------------------------------------------------

#endif // AUTOCONNECTORLISTFILLER_H
