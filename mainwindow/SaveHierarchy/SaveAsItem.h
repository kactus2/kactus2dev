//-----------------------------------------------------------------------------
// File: SaveAsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 27.05.2015
//
// Description:
// Tree item for representing a hierarchical save row.
//-----------------------------------------------------------------------------

#ifndef SAVEASITEM_H
#define SAVEASITEM_H

#include <QTreeWidgetItem>

//-----------------------------------------------------------------------------
//! Tree item for representing a hierarchical save row.
//-----------------------------------------------------------------------------
class SaveAsItem : public QTreeWidgetItem
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent   The parent item.
     */
    SaveAsItem(QTreeWidgetItem* parent);
    
    //! The destructor.
    virtual ~SaveAsItem();

    /*!
     *  Sets the data in the item.
     *
     *      @param [in] column      The column whose data to set.
     *      @param [in] role        The data role to set.
     *      @param [in] value       The value to set.
     */
    virtual void setData(int column, int role, QVariant const& value);

private:

    // Disable copying.
    SaveAsItem(SaveAsItem const& rhs);
    SaveAsItem& operator=(SaveAsItem const& rhs);
};

#endif // SAVEASITEM_H
