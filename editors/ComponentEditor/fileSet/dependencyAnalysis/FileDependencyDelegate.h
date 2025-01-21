//-----------------------------------------------------------------------------
// File: FileDependencyDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 15.02.2013
//
// Description:
// Delegate used in visualizing the file dependency model.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYDELEGATE_H
#define FILEDEPENDENCYDELEGATE_H

#include <editors/ComponentEditor/common/EnumerationEditorConstructorDelegate.h>

#include <QItemDelegate>

class Component;

//-----------------------------------------------------------------------------
//! Delegate used in visualizing the file dependency model.
//-----------------------------------------------------------------------------
class FileDependencyDelegate : public EnumerationEditorConstructorDelegate
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
     *  Returns the size hint for the given model index.
     */
    virtual QSize sizeHint(QStyleOptionViewItem const& option, QModelIndex const& index) const;

private:
    // Disable copying.
    FileDependencyDelegate(FileDependencyDelegate const& rhs);
    FileDependencyDelegate& operator=(FileDependencyDelegate const& rhs);
    
    /*!
     *  Check if the column is used for enumerations.
     *
     *    @param [in] index   The selected index.
     *
     *    @return True, if the column is used for editing enumerations, false otherwise.
     */
    virtual bool isEnumerationEditorColumn(QModelIndex const& index) const override final;

    /*!
     *  The list of currently selected enumerations in the selected item.
     *
     *    @param [in] index   Index of the selected item.
     *
     *    @return List of currently selected enumerations.
     */
    virtual QStringList getCurrentSelection(QModelIndex const& index) const override final;

    /*!
     *  Get the list of the available enumerations.
     *
     *    @return List of the available enumerations.
     */
    virtual QStringList getAvailableItems() const override final;

    /*!
     *  Set the selected enumerations to the selected item.
     *
     *    @param [in] index           Model index identifying the item that's data is to be saved.
     *    @param [in] model           Model that contains the data structure where data is to be saved to.
     *    @param [in] selectedItems   List of the selected enumerations.
     */
    virtual void setEnumerationDataToModel(QModelIndex const& index, QAbstractItemModel* model, QStringList const& selectedItems) const override final;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component being edited.
    QSharedPointer<Component> component_;
};

#endif // FILEDEPENDENCYDELEGATE_H
