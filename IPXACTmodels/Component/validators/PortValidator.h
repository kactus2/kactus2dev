//-----------------------------------------------------------------------------
// Port: PortValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 25.11.2015
//
// Description:
// Validator for ipxact:Port.
//-----------------------------------------------------------------------------

#ifndef PortVALIDATOR_H
#define PortVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QVector>
#include <QSharedPointer>

class ExpressionParser;
class Port;
class View;
class Wire;
class WireTypeDef;
class Transactional;
class PortTypeValidator;

//-----------------------------------------------------------------------------
//! Validator for ipxact:Port.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT PortValidator
{
public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] expressionParser    The used expression parser.
     *      @param [in] views               The available views.
	 */
    PortValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<QList<QSharedPointer<View> > > views);

	//! The destructor.
	virtual ~PortValidator();

    /*!
     *  Change the available views.
     *
     *      @param [in] newViews    The new views.
     */
    void componentChange(QSharedPointer<QList<QSharedPointer<View> > > newViews);

    /*!
     *  Validates the given Port.
     *
     *      @param [in] Port           The Port to validate.
     *
     *      @return True, if the Port is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<Port> port) const;

    
    /*!
     *  Check if the name is valid.
     *
     *      @param [in] name    The name to be evaluated.
     *
     *      @return True, if the name is valid, otherwise false.
     */
	bool hasValidName(QString const& name) const;
	
    /*!
     *  Check if the is present is valid.
     *
     *      @param [in] port    The selected port.
     *
     *      @return True, if the is present is valid, otherwise false.
     */
    bool hasValidIsPresent(QSharedPointer<Port> port) const;

    /*!
     *  Check if the arrays are valid.
     *
     *      @param [in] port    The selected port.
     *
     *      @return True, if the arrays are valid, otherwise false.
     */
    bool hasValidArrays(QSharedPointer<Port> port) const;

    /*!
     *  Check if an array value is valid.
     *
     *      @param [in] arrayValue  The selected array value.
     *
     *      @return True, if the array value is valid, otherwise false.
     */
    bool arrayValueIsValid(QString const& arrayValue) const;

    /*!
     *  Check if the port wire is valid.
     *
     *      @param [in] port    The selected port.
     *
     *      @return True, if the port wire is valid, otherwise false.
     */
    bool hasValidWire(QSharedPointer<Port> port) const;
    bool hasValidDefaultValue(QSharedPointer<Port> port) const;

    /*!
     *  Check if the wire is valid.
     *
     *      @param [in] wire    The selected wire.
     *
     *      @return True, if the wire is valid, otherwise false.
     */
    bool wireHasValidDirection(QSharedPointer<Wire> wire) const;

    /*!
     *  Check if the wire port bound is valid.
     *
     *      @param [in] portBound   The selected port bound.
     *
     *      @return True, if the port bound is valid, otherwise false.
     */
    bool portBoundIsValid(QString const& portBound) const;

    /*!
     *  Check if the transactional port is valid.
     *
     *      @param [in] port    The selected port.
     *
     *      @return True, if the transactional port is valid, otherwise false.
     */
    bool hasValidTransactionalPort(QSharedPointer<Port> port) const;

    /*!
     *  Check if a transactional has a valid initiative.
     *
     *      @param [in] transactional   The selected transactional
     *
     *      @return True, if the initiative is valid, false otherwise.
     */
    bool hasValidTransactionalInitiative(QSharedPointer<Transactional> transactional) const;

    /*!
     *  Check if a transactional has a valid kind.
     *
     *      @param [in] transactional   The selected transactional
     *
     *      @return True, if the kind is valid, false otherwise.
     */
    bool hasValidTransactionalKind(QSharedPointer<Transactional> transactional) const;

    /*!
     *  Check if a transactional has a valid bus width.
     *
     *      @param [in] transactional   The selected transactional
     *
     *      @return True, if the bus width is valid, false otherwise.
     */
    bool hasValidTransactionalWidth(QSharedPointer<Transactional> transactional) const;

    /*!
     *  Check if a transactional has a valid value for maximum connections.
     *
     *      @param [in] transactional   The selected transactional
     *
     *      @return True, if the maximum connections value is valid, false otherwise.
     */
    bool hasValidTransactionalMaxConnections(QSharedPointer<Transactional> transactional) const;

    /*!
     *  Check if a transactional has a valid value for minimum connections.
     *
     *      @param [in] transactional   The selected transactional
     *
     *      @return True, if the minimum connections value is valid, false otherwise.
     */
    bool hasValidTransactionalMinConnections(QSharedPointer<Transactional> transactional) const;

    /*!
     *  Check if a transactional has a protocol.
     *
     *      @param [in] transactional   The selected transactional
     *
     *      @return True, if the protocol is valid, false otherwise.
     */
    bool hasValidTransactionalProtocol(QSharedPointer<Transactional> transactional) const;

    /*!
     *  Check if the port has valid type definitions.
     *
     *      @param [in] port    The selected port.
     *
     *      @return True, if the type definitions are valid, false otherwise.
     */
    bool hasValidTypes(QSharedPointer<Port> port) const;

    /*!
     *  Check if the selected type definitions are valid.
     *
     *      @param [in] typeDefinitions     The selected type definitions.
     *
     *      @return True, if the type definitions are valid, false otherwise.
     */
    bool hasValidTypeDefinitions(QSharedPointer<QList<QSharedPointer<WireTypeDef> > > typeDefinitions) const;

    /*!
     *  Finds possible errors in a Port and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] Port   The Port whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<Port> port, QString const& context) const;

    /*!
     *  Get the type definition validator.
     *
     *      @return The validator used for type definitions.
     */
    QSharedPointer<PortTypeValidator> getTypeValidator() const;

private:

	// Disable copying.
	PortValidator(PortValidator const& rhs);
	PortValidator& operator=(PortValidator const& rhs);

    /*!
     *  Finds possible errors in a wire and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] port        The Port whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInWire(QVector<QString>& errors, QSharedPointer<Port> port, QString const& context) const;
    
    /*!
     *  Finds possible errors in a transactional and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] port        The port whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInTransactional(QVector<QString> &errors, QSharedPointer<Port> port,
        QString const& context) const;

    /*!
     *  Find possible errors in the selected type definitions.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] typeDefinitions     The selected type definitions.
     *      @param [in] context             Context to help locate the errors.
     */
    void findErrorsInTypeDefinitions(QVector<QString>& errors,
        QSharedPointer<QList<QSharedPointer<WireTypeDef> > > typeDefinitions, QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! Validator for port type definitions.
    QSharedPointer<PortTypeValidator> typeValidator_;
};

#endif // SYSTEMVERILOGVALIDATOR_H
