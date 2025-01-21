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

class PortAbstractionInterface;
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
     *    @param [in] parametersModel The parameter completer model to use.
     *    @param [in] parameterFinder The parameter finder to use.
     *    @param [in] libraryAccess   Interface to the library.
     *    @param [in] stdRevision     The standard revision used by the abstraction definition.
     *    @param [in] parent          The owner of this instance
     */
    AbstractionTransactionalPortsDelegate(QAbstractItemModel* parametersModel, 
        QSharedPointer<ParameterFinder> parameterFinder, LibraryInterface* libraryAccess, 
        Document::Revision stdRevision, QObject *parent);

	/*!
     *  The destructor.
     */
	virtual ~AbstractionTransactionalPortsDelegate() = default;

	/*!
     *  Create a new editor for the given item
	 *
	 *    @param [in] parent  Owner for the editor.
	 *    @param [in] option  Contains options for the editor.
	 *    @param [in] index   Model index identifying the item.
	 *
	 *    @return The editor to be used to edit the item.
     */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index)
        const;

    //! No copying. No assignment.
    AbstractionTransactionalPortsDelegate(const AbstractionTransactionalPortsDelegate& other) = delete;
    AbstractionTransactionalPortsDelegate& operator=(const AbstractionTransactionalPortsDelegate& other) = delete;

private:

    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *    @param [in] column   The column to check.
     *
     *    @return True, if the cells in the column allow expressions, otherwise false.
     */
    bool columnAcceptsExpression(int column) const final;

};

#endif // ABSTRACTIONTRANSACTIONALPORTSDELEGATE_H
