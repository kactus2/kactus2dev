//-----------------------------------------------------------------------------
// File: EditableTreeSortfilter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.12.2017
//
// Description:
// Sort filter model for editable tree view.
//-----------------------------------------------------------------------------

#ifndef EDITABLETREESORTFILTER_H
#define EDITABLETREESORTFILTER_H

#include <QSortFilterProxyModel>
#include <QObject>

//-----------------------------------------------------------------------------
//! Sort filter model for editable tree view.
//-----------------------------------------------------------------------------
class EditableTreeSortFilter : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    
    /*!
     *  The constructor.
     *
     *      @param [in] parent  Parent object.
     */
    EditableTreeSortFilter(QObject* parent);

    /*!
     *  The destructor.
     */
    virtual ~EditableTreeSortFilter();

    //! No copying. No assignment.
    EditableTreeSortFilter(const EditableTreeSortFilter& other) = delete;
    EditableTreeSortFilter& operator=(const EditableTreeSortFilter& other) = delete;

public slots:

    /*!
     *  Handle the item addition.
     *
     *      @param [in] indexes     Indexes the selected items.
     */
    void onAddItem(QModelIndexList const& indexes);

    /*!
     *  Handle the sub item addition.
     *
     *      @param [in] parentIndexes   Indexes of the selected parent.
     */
    void onAddSubItem(QModelIndexList const& parentIndexes);

    /*!
     *  Handle the item removal.
     *
     *      @param [in] indexes     Indexes of the selected items.
     */
    void onRemoveSelectedRows(QModelIndexList const& indexes);

    /*!
     *  Handle the removal of the sub items of the selected item.
     *
     *      @param [in] parentIndexes   Indexes of the selected items.
     */
    void onRemoveAllSubItemsFromIndexes(QModelIndexList const& parentIndexes);

signals:

    /*!
     *  Add a new item to the selected index position.
     *
     *      @param [in] newItemIndex    The selected index.
     */
    void addItem(QModelIndex const& newItemIndex);

    /*!
     *  Add a new sub item to the selected parent item.
     *
     *      @param [in] parentItemIndex     Index of the selected parent item.
     */
    void addSubItem(QModelIndex const& parentItemIndex);

    /*!
     *  Remove the selected item.
     *
     *      @param [in] itemIndex   Index of the selected item.
     */
    void removeItem(QModelIndex const& itemIndex);

    /*!
     *  Remove all of the sub items contained within the selected item.
     *
     *      @param [in] itemIndex   Index of the selected parent item.
     */
    void removeAllSubItemsFromIndex(QModelIndex const& itemIndex);

private:

    /*!
     *  Change the selected index list To have indexes only from the first column.
     *
     *      @param [in] indexes     The selected index list.
     *
     *      @return List of indexes from the first column.
     */
    QModelIndexList concatIndexesToOneColumn(QModelIndexList const& indexes) const;

    /*!
     *  Removes child indexes that have parent indexes in the selected index list.
     *
     *      @param [in] indexes     The selected index list.
     *
     *      @return List of removable indexes.
     */
    QModelIndexList getRemovableIndexes(QModelIndexList const& indexes) const;

    /*!
     *  Check if the selected index is removable in the selected index list.
     *
     *      @param [in] index       The selected index.
     *      @param [in] indexList   List of the comparison indexes.
     *
     *      @return True, if the selected index is removable, false otherwise.
     */
    bool indexIsRemovable(QModelIndex const& index, QModelIndexList indexList) const;
};

#endif // EDITABLETREESORTFILTER_H
