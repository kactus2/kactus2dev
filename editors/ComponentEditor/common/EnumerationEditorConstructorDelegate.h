//-----------------------------------------------------------------------------
// File: EnumerationEditorConstructorDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 18.01.2023
//
// Description:
// The delegate that provides enumeration editors.
//-----------------------------------------------------------------------------

#ifndef ENUMERATIONEDITORCONSTRUCTORDELEGATE_H
#define ENUMERATIONEDITORCONSTRUCTORDELEGATE_H

#include <QStyledItemDelegate>
// #include <QSharedPointer>

// class Component;

// #include <QKeyEvent>

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit enumerations.
//-----------------------------------------------------------------------------
class EnumerationEditorConstructorDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	//! The minimum height for editor editing list of interfaces
	static const int LIST_EDITOR_MIN_HEIGHT = 100;

	/*!
     *  The constructor
	 *
	 *      @param [in] parent      The owner of this delegate.
     */
    EnumerationEditorConstructorDelegate(QObject* parent);

    /*!
     *  Set the flag for hiding the check all enumerations check box.
     *
     *      @param [in] newValue    The new flag.
     */
    void setHideCheckAll(bool newValue);

	/*!
     *  The destructor
     */
    virtual ~EnumerationEditorConstructorDelegate() = default;

	/*!
     *  Create a new editor for the given item
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
	 *
	 *      @return The editor to be used to edit the item.
     */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index) const;

	/*!
     *  Set the data for the editor.
	 *
	 *      @param [in] editor  The editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
     */
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*!
     *  Save the data from the editor to the model.
	 *
	 *      @param [in] editor  The editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   Model index identifying the item that's data is to be saved.
     */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

    /*!
     *  Updates the editor geometry.
     *
     *      @param [in] editor  The editor being updated.
     *      @param [in] option  The options used to update the editor.
     *      @param [in] index   The model index being edited.
     */
    virtual void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;

protected:

    /*!
     *  Repositions the editor if there is not enough space under the default position. The editor is then resized
     *  to use the available space.
     *
     *      @param [in] editor  The editor to reposition.
     *      @param [in] option  The style options for the editor.
     *      @param [in] index   The current index.
     */
    void repositionAndResizeEditor(QWidget* editor, QStyleOptionViewItem const& option, QModelIndex const& index) const;

protected slots:

    /*!
     *  Commit the data from the sending editor and close the editor.
     */
    void commitAndCloseEditor();

private slots:

    /*!
     *  Close the editor without committing the changes.
     */
    void onEditorCancel();

private:
	
	//! No copying
	EnumerationEditorConstructorDelegate(const EnumerationEditorConstructorDelegate& other);
    EnumerationEditorConstructorDelegate& operator=(const EnumerationEditorConstructorDelegate& other);

    /*!
     *  Check if the column is used for enumerations.
     *
     *      @param [in] index   The selected index.
     *
     *      @return True, if the column is used for editing enumerations, false otherwise.
     */
    virtual bool isEnumerationEditorColumn(QModelIndex const& index) const = 0;

    /*!
     *  The list of currently selected enumerations in the selected item.
     *
     *      @param [in] index   Index of the selected item.
     *
     *      @return List of currently selected enumerations.
     */
    virtual QStringList getCurrentSelection(QModelIndex const& index) const = 0;

    /*!
     *  Get the list of the available enumerations.
     *
     *      @return List of the available enumerations.
     */
    virtual QStringList getAvailableItems() const = 0;

    /*!
     *  Get the list of exclusive enumerations.
     *
     *      @return List of the exclusvie enumerations.
     */
    virtual QStringList getExclusiveItems() const;

    /*!
     *  Set the selected enumerations to the selected item.
     *
     *      @param [in] index           Model index identifying the item that's data is to be saved.
     *      @param [in] model           Model that contains the data structure where data is to be saved to.
     *      @param [in] selectedItems   List of the selected enumerations.
     */
    virtual void setEnumerationDataToModel(QModelIndex const& index, QAbstractItemModel* model, QStringList const& selectedItems) const = 0;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Flag for hiding the check all enumerations check box.
    bool hideCheckAll_;
};

#endif // ENUMERATIONEDITORCONSTRUCTORDELEGATE_H
