//-----------------------------------------------------------------------------
// File: ComponentEditorTreeSortProxyModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.08.2013
//
// Description:
// Sorting proxy model for Component Editor tree structure.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORTREEPROXYMODEL_H
#define COMPONENTEDITORTREEPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QStringList>

//-----------------------------------------------------------------------------
//! Sorting proxy for Component Editor tree view. Only supports sorting of views.
//-----------------------------------------------------------------------------
class ComponentEditorTreeProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
	 /*!
	 *  Constructor.
	 *
	 *      @param [in] parent     The parent object.
	 */
    ComponentEditorTreeProxyModel(QObject *parent = 0);

	 /*!
	 *  Destructor.
	 */
    ~ComponentEditorTreeProxyModel();

    /*!
     *  Sets the items to be hidden.
     *
     *      @param [in] hiddenItemNames   The names of the items to hide.
     */
    void setRowVisibility(QStringList hiddenItemNames);

protected:

    /*!
     *  Compares two items to get their order in the sorting. Views are sorted hierarchical first
     *  in alphabetical order followed by non-hierachical views in alphabetical order. All other 
     *  items will be sorted by their index i.e. creation order. 
     *
     *  Note that the corresponding summary view may sort its content into other order.
     *
     *      @param [in] left        The left item in comparison left < right.
     *      @param [in] right       The right item in comparison left < right.
     *
     *      @return True if left precedes right, otherwise false.
     */
    bool lessThan(QModelIndex const& left, QModelIndex const& right) const;

	/*!
	 *  Filters the selected rows.
	 *
	 *      @param [in] source_row      The row to check for filtering.
	 *      @param [in] source_parent   The parent index of the row.
	 *
	 *      @return True, if row passes filters, otherwise false.
	 */
    bool filterAcceptsRow(int sourceRow, QModelIndex const& sourceParent) const;

private:
    //! \brief No copying
    ComponentEditorTreeProxyModel(const ComponentEditorTreeProxyModel& other);
    //! \brief No assignment
    ComponentEditorTreeProxyModel& operator=(const ComponentEditorTreeProxyModel& other);

	/*!
	 *  Checks if item is valid. Invalid items should not be hidden.
	 *
	 *      @param [in] index   Index of item.
	 */
    bool itemIsValidAndCanBeHidden(QModelIndex const& index) const;

	//! A list of all the rows that will be hidden.
	QStringList hiddenItems_;
};

#endif // COMPONENTEDITORTREEPROXYMODEL_H
