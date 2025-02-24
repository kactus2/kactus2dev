//-----------------------------------------------------------------------------
// File: AbstractionTypesDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.01.2018
//
// Description:
// The delegate that provides editors to edit bus interface abstraction types.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONTYPESDELEGATE_H
#define ABSTRACTIONTYPESDELEGATE_H

#include <editors/ComponentEditor/common/EnumerationEditorConstructorDelegate.h>

class LibraryInterface;
class Component;
class View;

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit bus interface abstraction types.
//-----------------------------------------------------------------------------
class AbstractionTypesDelegate : public EnumerationEditorConstructorDelegate
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] component       Component containing the abstraction types.
     *    @param [in] library         VLNV library interface.
     *    @param [in] parentWindow    The main window.
     *    @param [in] parent          The parent object.
     */
    AbstractionTypesDelegate(QSharedPointer<Component> component, LibraryInterface* library, QWidget* parentWindow,
        QObject* parent = 0);

	/*!
     *  The destructor.
     */
    virtual ~AbstractionTypesDelegate() = default;

    // No copying. No assignment.
    AbstractionTypesDelegate(AbstractionTypesDelegate const& rhs) = delete;
    AbstractionTypesDelegate& operator=(AbstractionTypesDelegate const& rhs) = delete;

	/*!
     *  Create a new editor for the given item
	 *
	 *    @param [in] parent  Owner for the editor.
	 *    @param [in] option  Contains options for the editor.
	 *    @param [in] index   Model index identifying the item.
	 *
	 *    @return The editor to be used to edit the item.
     */
	QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index) const 
        final;

	/*!
     *  Set the data for the editor.
	 *
	 *    @param [in] editor  The editor where the data is to be set.
	 *    @param [in] index   Model index identifying the item that's data is to be set.
	 */
	void setEditorData(QWidget* editor, QModelIndex const& index) const final;

	/*!
     *  Save the data from the editor to the model.
	 *
	 *    @param [in] editor  The editor that contains the data to store.
	 *    @param [in] model   Model that contains the data structure where data is to be saved to.
	 *    @param [in] index   Model index identifying the item that's data is to be saved.
	 */
	void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const final;

    /*!
     *  Set a new component.
     *
     *    @param [in] newComponent    The new component.
     */
    void setComponent(QSharedPointer<Component> newComponent);

private:

    /*!
     *  Create the VLNV editor for abstraction definition references.
     *
     *    @param [in] parent  Parent item for the editor.
     *
     *    @return The created VLNV editor.
     */
    QWidget* createVLNVEditor(QWidget* parent) const;

    /*!
     *  Check if the column is used for enumerations.
     *
     *    @param [in] index   The selected index.
     *
     *    @return True, if the column is used for editing enumerations, false otherwise.
     */
    bool isEnumerationEditorColumn(QModelIndex const& index) const final;

    /*!
     *  The list of currently selected enumerations in the selected item.
     *
     *    @param [in] index   Index of the selected item.
     *
     *    @return List of currently selected enumerations.
     */
    QStringList getCurrentSelection(QModelIndex const& index) const final;

    /*!
     *  Get the list of the available enumerations.
     *
     *    @return List of the available enumerations.
     */
    QStringList getAvailableItems() const final;

    /*!
     *  Set the selected enumerations to the selected item.
     *
     *    @param [in] index           Model index identifying the item that's data is to be saved.
     *    @param [in] model           Model that contains the data structure where data is to be saved to.
     *    @param [in] selectedItems   List of the selected enumerations.
     */
    void setEnumerationDataToModel(QModelIndex const& index, QAbstractItemModel* model, 
        QStringList const& selectedItems) const final;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of all the referable views.
    QSharedPointer<QList<QSharedPointer<View> > > availableViews_;

    //! The VLNV library interface.
    LibraryInterface* library_;

    //! Main window.
    QWidget* parentWindow_;

    //! Containing component.
    QSharedPointer<Component> containingComponent_;
};

#endif // ABSTRACTIONTYPESDELEGATE_H
