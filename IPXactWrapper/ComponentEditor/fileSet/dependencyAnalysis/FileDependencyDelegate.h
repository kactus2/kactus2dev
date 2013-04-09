//-----------------------------------------------------------------------------
// File: FileDependencyDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 15.02.2013
//
// Description:
// Delegate used in visualizing the file dependency model.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYDELEGATE_H
#define FILEDEPENDENCYDELEGATE_H

#include <QStyledItemDelegate>
#include <QItemDelegate>

//-----------------------------------------------------------------------------
//! Delegate used in visualizing the file dependency model.
//-----------------------------------------------------------------------------
class FileDependencyDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     */
    FileDependencyDelegate(QObject* parent = 0);

    /*!
     *  Destructor.
     */
    ~FileDependencyDelegate();

    /*!
     *  Returns the size hint for the given model index.
     */
    virtual QSize sizeHint(QStyleOptionViewItem const& option, QModelIndex const& index) const;

protected:
    /*!
     *  Handles the painting of UI elements in the file dependency model.
     *
     *      @param [in] painter  The painter context.
     *      @param [in] option   The painting options.
     *      @param [in] index    The model index of the item to paint.
     */
    virtual void paint(QPainter *painter, QStyleOptionViewItem const& option, QModelIndex const& index) const;

private:
    // Disable copying.
    FileDependencyDelegate(FileDependencyDelegate const& rhs);
    FileDependencyDelegate& operator=(FileDependencyDelegate const& rhs);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
};

#endif // FILEDEPENDENCYDELEGATE_H
