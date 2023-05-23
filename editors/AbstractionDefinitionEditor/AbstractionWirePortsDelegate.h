//-----------------------------------------------------------------------------
// File: AbstractionWirePortsDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 27.6.2011
//
// Description:
// The delegate that provides editors for logical port properties in Abstraction Definition.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONWIREPORTSDELEGATE_H
#define ABSTRACTIONWIREPORTSDELEGATE_H

#include "AbstractionPortsDelegate.h"

//-----------------------------------------------------------------------------
//! The delegate that provides editors for logical port properties in Abstraction Definition.
//-----------------------------------------------------------------------------
class AbstractionWirePortsDelegate : public AbstractionPortsDelegate
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] libraryAccess   Interface to the library.
     *      @param [in] parent          The owner of this instance
     */
    AbstractionWirePortsDelegate(LibraryInterface* libraryAccess, QObject *parent);
	
	/*!
     *  The destructor.
     */
	virtual ~AbstractionWirePortsDelegate() = default;

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

    //! No copying. No assignment.
    AbstractionWirePortsDelegate(const AbstractionWirePortsDelegate& other) = delete;
    AbstractionWirePortsDelegate& operator=(const AbstractionWirePortsDelegate& other) = delete;

protected:

    /*!
     *  Get the list of available qualifier values.
     *
     *      @return The list of available qualifier values.
     */
    virtual QStringList getQualifierList() const;

    /*!
     *  Check if the editor used in the selected index column is a line editor.
     *
     *      @param [in] indexColumn     The selected index column.
     *
     *      @return True, if the editor used in the selected column is a line editor, false otherwise.
     */
    virtual bool editorIsLineEditor(int indexColumn) const;

    /*!
     *  Check if the editor used in the selected index column is a combo box editor.
     *
     *      @param [in] indexColumn     The selected index column.
     *
     *      @return True, if the editor used in the selected column is a combo box editor, false otherwise.
     */
    virtual bool editorIsComboBox(int indexColumn) const;
	
private:

    /*!
     *  Get the list of exclusive enumerations.
     *
     *      @return List of exclusive enumerations.
     */
    virtual QStringList getExclusiveItems() const override final;
};

#endif // ABSTRACTIONWIREPORTSDELEGATE_H
