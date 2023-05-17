//-----------------------------------------------------------------------------
// File: AbstractionTransactionalPortsDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.06.2018
//
// Description:
// The delegate that provides editors for transactional port properties in Abstraction Definition.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONTRANSACTIONALPORTSDELEGATE_H
#define ABSTRACTIONTRANSACTIONALPORTSDELEGATE_H

#include "AbstractionPortsDelegate.h"

//-----------------------------------------------------------------------------
//! The delegate that provides editors for transactional port properties in Abstraction Definition.
//-----------------------------------------------------------------------------
class AbstractionTransactionalPortsDelegate : public AbstractionPortsDelegate
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] libraryAccess   Interface to the library.
     *      @param [in] parent          The owner of this instance
     */
    AbstractionTransactionalPortsDelegate(LibraryInterface* libraryAccess, QObject *parent);

	/*!
     *  The destructor.
     */
	virtual ~AbstractionTransactionalPortsDelegate() = default;

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
    AbstractionTransactionalPortsDelegate(const AbstractionTransactionalPortsDelegate& other) = delete;
    AbstractionTransactionalPortsDelegate& operator=(const AbstractionTransactionalPortsDelegate& other) = delete;
};

#endif // ABSTRACTIONTRANSACTIONALPORTSDELEGATE_H
