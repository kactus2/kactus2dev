//-----------------------------------------------------------------------------
// File: AutoConnectorConnectionDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 12.02.2019
//
// Description:
// Delegate that provides widgets for editing connected pairings.
//-----------------------------------------------------------------------------

#ifndef AUTOCONNECTORCONNECTIONDELEGATE_H
#define AUTOCONNECTORCONNECTIONDELEGATE_H

#include <QStyledItemDelegate>
#include <QListView>
#include <QComboBox>

class Component;
class TableItemMatcher;

//-----------------------------------------------------------------------------
//! Delegate that provides widgets for editing connected pairings.
//-----------------------------------------------------------------------------
class AutoConnectorConnectionDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] firstItem       The first component item to be connected.
     *      @param [in] secondItem      The second component item to be connected.
     *      @param [in] firstList       View of the first item list.
     *      @param [in] secondList      View of the second item list.
     *      @param [in] itemMatcher     Checks for possible matches between two items.
     *      @param [in] parent          The parent of the object.
	 */
    AutoConnectorConnectionDelegate(QSharedPointer<Component> firstComponent,
        QSharedPointer<Component> secondComponent, QListView* firstList, QListView* secondList,
        TableItemMatcher* itemMatcher, QObject* parent = 0);

	/*!
     *  The destructor.
     */
    ~AutoConnectorConnectionDelegate();

	/*!
     *  Create a new editor for the given item
	 *
	 *      @param [in] parent   Owner for the editor.
	 *      @param [in] option   Contains options for the editor.
	 *      @param [in] index    Model index identifying the item.
	 *
	 *      @return Pointer to the editor to be used to edit the item.
     */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
        QModelIndex const& index) const;

	/*!
     *  Set the data for the editor.
	 *
	 *      @param [in] editor  Pointer to the editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
     */
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*!
     *  Save the data from the editor to the model.
	 *
	 *     @param [in] editor   Pointer to the editor that contains the data to store.
	 *     @param [in] model    Model that contains the data structure where data is to be saved to.
	 *     @param [in] index    Model index identifying the item that's data is to be saved.
     */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

    //! No copying. No assignment.
    AutoConnectorConnectionDelegate(const AutoConnectorConnectionDelegate& other) = delete;
    AutoConnectorConnectionDelegate& operator=(const AutoConnectorConnectionDelegate& other) = delete;

private:

    /*!
     *  Create the selection widget for ports.
     *
     *      @param [in] selectionIndex              Index of the item selector.
     *      @param [in] itemList                    List containing the items of the selected component.
     *      @param [in] comparisonItemColumn        Column of the comparison item.
     *      @param [in] itemComponent               Component containing the items of the selected index.
     *      @param [in] comparisonItemComponent     Component containing the items of the comparison index.
     *      @param [in] parent                      Parent item of the editor.
     *
     *      @return Combobox for component ports.
     */
    QComboBox* createItemSelector(QModelIndex const& selectionIndex, QListView* itemList, int comparisonItemColumn,
        QSharedPointer<Component> itemComponent, QSharedPointer<Component> comparisonItemComponent,
        QWidget* parent) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The first component item to be connected.
    QSharedPointer<Component> firstComponent_;

    //! The second component item to be connected.
    QSharedPointer<Component> secondComponent_;

    //! View of the first item list.
    QListView* firstList_;

    //! View of the second item list.
    QListView* secondList_;

    //! Checks for possible matches between two items.
    TableItemMatcher* itemMatcher_;
};

#endif // AUTOCONNECTORCONNECTIONDELEGATE_H
