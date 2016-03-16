//-----------------------------------------------------------------------------
// File: RemapStateValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 27.11.2015
//
// Description:
// Validator for the ipxact:remapState.
//-----------------------------------------------------------------------------

#ifndef REMAPSTATEVALIDATOR_H
#define REMAPSTATEVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>

class ExpressionParser;
class RemapState;
class RemapPort;
class Port;
//-----------------------------------------------------------------------------
//! Validator for the ipxact:remapState.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT RemapStateValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] expressionParser    The parser to use for solving expressions.
     *      @param [in] ports               The available component ports.
	 */
    RemapStateValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<QList<QSharedPointer<Port> > > ports);

	//! The destructor.
	~RemapStateValidator();
    
    /*!
     *  Change the available ports.
     *
     *      @param [in] newPorts    The new ports.
     */
    void componentChange(QSharedPointer<QList<QSharedPointer<Port> > > newPorts);

    /*!
     *  Validates the given remap state.
     *
     *      @param [in] remapState  The remap state to validate.
     *
     *      @return True, if the remap state is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<RemapState> remapState) const;

    /*!
     *  Check if the remap state has a valid name.
     *
     *      @param [in] remapState  The selected remap state.
     *
     *      @return True, if the name is valid IP-XACT, otherwise false.
     */
    bool hasValidName(QSharedPointer<RemapState> remapState) const;

    /*!
     *  Check if the remap state has valid remap ports.
     *
     *      @param [in] remapState  The selected remap state.
     *
     *      @return True, if the remap ports are valid IP-XACT, otherwise false.
     */
    bool hasValidRemapPorts(QSharedPointer<RemapState> remapState) const;

    /*!
     *  Locate errors within a remap state.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] remapState  The selected remap state.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<RemapState> remapState, QString const& context)
        const;

private:

	// Disable copying.
	RemapStateValidator(RemapStateValidator const& rhs);
	RemapStateValidator& operator=(RemapStateValidator const& rhs);
    
    /*!
     *  Check if the remap port has a valid port reference.
     *
     *      @param [in] remapPort   The selected remap port.
     *
     *      @return True, if the port reference is valid IP-XACT, otherwise false.
     */
    bool remapPortHasValidPortReference(QSharedPointer<RemapPort> remapPort) const;

    /*!
     *  Check if the remap port has a valid port index.
     *
     *      @param [in] remapPort       The selected remap port.
     *      @param [in] referencedPort  The port referenced by the remap port.
     *
     *      @return True, if the port index is valid IP-XACT, otherwise false.
     */
    bool remapPortHasValidIndex(QSharedPointer<RemapPort> remapPort, QSharedPointer<Port> referencedPort) const;

    /*!
     *  Check if the remap port has a valid value.
     *
     *      @param [in] remapPort   The selected remap port.
     *
     *      @return True, if the value is valid IP-XACT, otherwise false.
     */
    bool remapPortHasValidValue(QSharedPointer<RemapPort> remapPort) const;

    /*!
     *  Find errors within a name.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] remapState  The selected remap state.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<RemapState> remapState, QString const& context)
        const;

    /*!
     *  Find errors within remap ports.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] remapState  The selected remap state.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInRemapPorts(QVector<QString>& errors, QSharedPointer<RemapState> remapState,
        QString const& context) const;

    /*!
     *  Find errors within remap port port reference.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] remapPort   The selected remap port.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsInRemapPortPortReference(QVector<QString>& errors, QSharedPointer<RemapPort> remapPort,
        QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! A list of available remap ports.
    QSharedPointer<QList<QSharedPointer<Port> > > availablePorts_;
};

#endif // REMAPSTATEVALIDATOR_H
