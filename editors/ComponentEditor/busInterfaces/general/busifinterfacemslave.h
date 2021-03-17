//-----------------------------------------------------------------------------
// File: busIfInterfaceMSlave.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Editor to edit mirrored slave details of a bus interface.
//-----------------------------------------------------------------------------

#ifndef BUSIFINTERFACEMSLAVE_H
#define BUSIFINTERFACEMSLAVE_H

#include "busifinterfacemodeeditor.h"

#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <QSharedPointer>
#include <QLineEdit>

class BusInterface;
class Component;
class ExpressionEditor;
class ExpressionParser;
class MirroredSlaveInterface;
class BusInterfaceInterface;

//-----------------------------------------------------------------------------
//! Editor to edit mirrored slave details of a bus interface.
//-----------------------------------------------------------------------------
class BusIfInterfaceMSlave : public BusIfInterfaceModeEditor
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] busInterface        Interface for accessing bus interfaces.
     *      @param [in] busName             Name of the edited bus interface.
	 *      @param [in] parameterFinder     The parameter finder.
     *      @param [in] expressionParser    The expression parser.
	 *      @param [in] parent              The owner of this editor.
	 */
    BusIfInterfaceMSlave(BusInterfaceInterface* busInterface,
        std::string const& busName,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
        QWidget *parent);

	/*!
     *  The destructor.
     */
    virtual ~BusIfInterfaceMSlave() = default;

	/*!
     *  Check for the validity of the edited item.
     *
     *      @return True if item is valid.
     */
	virtual bool isValid() const;

	/*!
     *  Restore the changes made in the editor back to ones in the model.
     *
     */
	virtual void refresh();

	/*!
     *  Get the interface mode of the editor.
	 * 
	 *      @return General::InterfaceMode Specifies the interface mode.
     */
	virtual General::InterfaceMode getInterfaceMode() const;

	/*!
     *  Save the interface mode-specific details to the bus interface.
     */
	virtual void saveModeSpecific();

    /*!
     *  Remove all the references from the expressions contained within this interface.
     */
    void removeReferencesFromExpressions();

private slots:

    /*!
     *  Handler for changes in remap address.
     */
    void onRemapChange();

    /*!
     *  Handler for changes in range.
     */
    void onRangeChange();

signals:

    /*!
     *  Increase the number of references in the selected parameter.
     *
     *      @param [in] id      The id of the selected parameter.
     */
    void increaseReferences(QString const& id);

    /*!
     *  Decrease the number of references in the selected parameter.
     *
     *      @param [in] id      The id of the selected parameter.
     */
    void decreaseReferences(QString const& id);

private:
	
    //! No copying. No assignment.
	BusIfInterfaceMSlave(const BusIfInterfaceMSlave& other);
	BusIfInterfaceMSlave& operator=(const BusIfInterfaceMSlave& other);

    //-----------------------------------------------------------------------------
    //! Data.
    //-----------------------------------------------------------------------------

    //! Editor for the mirrored slave remap address.
    ExpressionEditor* remapEditor_;

    //! Editor for the mirrored slave range.
    ExpressionEditor* rangeEditor_;

    //! The expression parser for calculating the expression.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The parameter finder for searching the component parameters.
    QSharedPointer<ParameterFinder> parameterFinder_;
};

#endif // BUSIFINTERFACEMSLAVE_H
