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

#include <QStyledItemDelegate>

class LibraryInterface;
class Component;
class View;

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit bus interface abstraction types.
//-----------------------------------------------------------------------------
class AbstractionTypesDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] component       Component containing the abstraction types.
     *      @param [in] library         VLNV library interface.
     *      @param [in] parentWindow    The main window.
     *      @param [in] parent          The parent object.
     */
    AbstractionTypesDelegate(QSharedPointer<Component> component, LibraryInterface* library, QWidget* parentWindow,
        QObject* parent = 0);

	/*!
     *  The destructor.
     */
    virtual ~AbstractionTypesDelegate();

	/*!
     *  Create a new editor for the given item
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
	 *
	 *      @return The editor to be used to edit the item.
     */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index)
        const;

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
     *  Set a new component.
     *
     *      @param [in] newComponent    The new component.
     */
    void setComponent(QSharedPointer<Component> newComponent);

    /*!
     *  Update the geometry of the editor in the selected index.
     *
     *      @param [in] editor  The contained editor.
     *      @param [in] option  Style options for the area containing the editor.
     *      @param [in] index   The selected index.
     */
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
        const QModelIndex &index) const;

private:

    // No copying. No assignment.
    AbstractionTypesDelegate(AbstractionTypesDelegate const& rhs);
    AbstractionTypesDelegate& operator=(AbstractionTypesDelegate const& rhs);

    /*!
     *  Create the VLNV editor for abstraction definition references.
     *
     *      @param [in] parent  Parent item for the editor.
     *
     *      @return The created VLNV editor.
     */
    QWidget* createVLNVEditor(QWidget* parent) const;

    /*!
     *  Reposition and resize the selected editor.
     *
     *      @param [in] editor  The selected editor.
     *      @param [in] option  Style options for the area containing the selected editor.
     */
    void repositionAndResizeEditor(QWidget* editor, QStyleOptionViewItem const& option) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of all the referable views.
    QSharedPointer<QList<QSharedPointer<View> > > availableViews_;

    //! The VLNV library interface.
    LibraryInterface* library_;

    //! Main window.
    QWidget* parentWindow_;
};

#endif // ABSTRACTIONTYPESDELEGATE_H
