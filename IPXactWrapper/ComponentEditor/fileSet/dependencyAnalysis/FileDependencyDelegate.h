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

class Component;

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
    FileDependencyDelegate(QSharedPointer<Component> component, QObject* parent = 0);

    /*!
     *  Destructor.
     */
    ~FileDependencyDelegate();

    /*!
     *  Creates an editor for the given model index.
     */
    virtual QWidget* createEditor(QWidget *parent, QStyleOptionViewItem const& option,
                                  QModelIndex const& index) const;

    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

    /*!
     *  Returns the size hint for the given model index.
     */
    virtual QSize sizeHint(QStyleOptionViewItem const& option, QModelIndex const& index) const;

private:
    // Disable copying.
    FileDependencyDelegate(FileDependencyDelegate const& rhs);
    FileDependencyDelegate& operator=(FileDependencyDelegate const& rhs);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component being edited.
    QSharedPointer<Component> component_;
};

#endif // FILEDEPENDENCYDELEGATE_H
