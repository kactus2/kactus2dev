//-----------------------------------------------------------------------------
// File: PortAbstractionValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 03.09.2024
//
// Description:
// Validator for port abstractions.
//-----------------------------------------------------------------------------

#ifndef PORTABSTRACTIONVALIDATOR_H
#define PORTABSTRACTIONVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/common/validators/ProtocolValidator.h>
#include <IPXACTmodels/AbstractionDefinition/validators/PacketValidator.h>

class PortAbstraction;
class WirePort;
class WireAbstraction;
class TransactionalPort;
class AbstractionDefinition;
class BusDefinition;
class ExpressionParser;
class LibraryInterface;

class IPXACTMODELS_EXPORT PortAbstractionValidator
{

public:

    /*!
     *	The constructor.
     *  
     *    @param [in] library              Library interface to use.
     *    @param [in] expressionParser     Expression parser to use.
     */
    PortAbstractionValidator(LibraryInterface* library, QSharedPointer<ExpressionParser> expressionParser);
    
    virtual ~PortAbstractionValidator() = default;
    
    //! No copying or assignment.
    PortAbstractionValidator(PortAbstractionValidator const& other) = delete;
    PortAbstractionValidator& operator=(PortAbstractionValidator const& other) = delete;

    /*!
     *  Validates the given PortAbstraction.
     *
     *    @param [in] port			        The PortAbstraction to validate.
     *    @param [in] abstractionDefinition   The abstraction definition being validated.
     *    @param [in] busDefinition           The bus definition used by the abstraction definition.
     */
    bool validate(QSharedPointer<PortAbstraction> port,
        QSharedPointer<AbstractionDefinition > abstractionDefinition,
        QSharedPointer<const BusDefinition> busDefinition) const;

    /*!
     *   Finds possible errors in a PortAbstraction and creates a list of them.
     *
     *    @param [in] errors			        List of found errors.
     *    @param [in] port			        The PortAbstraction whose errors to find.
     *    @param [in] abstractionDefinition   The abstraction definition.
     *    @param [in] busDefinition           Bus definition used by the abstraction definition.
     */
    void findErrorsIn(QStringList& errors,
        QSharedPointer<PortAbstraction> port,
        QSharedPointer<AbstractionDefinition> abstractionDefinition,
        QSharedPointer<const BusDefinition> busDefinition) const;

    bool hasValidName(QSharedPointer<PortAbstraction> portAbstraction) const;

    /*!
     *	Checks the validity of the default value of a wire port.
     *
     *    @param [in] wirePort    The wire port to check.
     *
     * 	    @return True, if the default value is valid, otherwise false.
     */
    bool wireHasValidDefaultValue(QSharedPointer<WireAbstraction> wireAbstraction) const;

    /*!
	 *  Validates the given WirePort.
	 *
	 *    @param [in] wirePort			The WirePort to validate.
	 *    @param [in] ports				Collection of ports within the abstraction definition.
	 *
	 *    @return True, if the WirePort is valid IP-XACT, otherwise false.
	 */
    bool isValidWirePort(QSharedPointer<WirePort> wirePort,
        QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const;

    /*!
     *	Checks the validity of the width of a wire port.
     *
     *    @param [in] wirePort    The wire port to check.
     *
     * 	    @return True, if the width is valid, otherwise false.
     */
    bool wireHasValidWidth(QSharedPointer<WirePort> wirePort) const;

    /*!
     *  Validates the given TransactionalPort.
     *
     *    @param [in] transPort	The TransactionalPort to validate.
     */
    bool isValidTransactionalPort(QSharedPointer<TransactionalPort> transPort) const;

    /*!
     *	Checks the validity of the bus width of a transactional port.
     *  
     *    @param [in] transPort     The transactional port to check.
     *	    
     * 	    @return True, if the bus width is valid, otherwise false.
     */
    bool transactionalHasValidBusWidth(QSharedPointer<TransactionalPort> transPort) const;

    /*!
     *	Validates the given port qualifier.
     *
     *    @param [in] qualifier	        The qualifier to check.
     *    @param [in] documentRevision	The IP-XACT standard revision of the document being checked.
     *
     * 		@return True, if port qualifier is valid, otherwise false.
     */
    bool isValidPortQualifier(QSharedPointer<Qualifier> qualifier, Document::Revision documentRevision) const;

private:

    /*!
     *  Validates the constraints of parameter wirePort.
     *
     *    @param [in] wirePort		The WirePort to validate.
     *    @param [in] ports			Collection of ports within the abstraction definition.
     */
    bool validateConstraints(QSharedPointer<WirePort> wirePort,
        QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const;
    
    /*!
     *	Check if port packets are valid.
     *
     *    @param [in] port	        The port whose packets are checked.
     *
     * 		@return True if valid, otherwise false.
     */
    bool hasValidPortPackets(QSharedPointer<PortAbstraction> port)
        const;

    /*!
     *   Finds possible errors in constraints of parameter wirePort and creates a list of them.
     *
     *    @param [in] errors      List of found errors.
     *    @param [in] wirePort	The WirePort whose errors to find.
     *    @param [in] context     Context to help locate the errors.
     *    @param [in] ports		Collection of ports within the abstraction definition.
     */
    void findErrorsInConstraints(QStringList& errors, QSharedPointer<WirePort> wirePort,
        QString const& context, QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const;

    /*!
     *   Finds possible errors in a WirePort and creates a list of them.
     *
     *    @param [in] errors      List of found errors.
     *    @param [in] wirePort	The WirePort whose errors to find.
     *    @param [in] context     Context to help locate the errors.
     *    @param [in] ports		Collection of ports within the abstraction definition.
     */
    void findErrorsInWirePort(QStringList& errors, QSharedPointer<WirePort> wirePort, QString const& context,
        QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const;


    /*!
     *   Finds possible errors in a TransactionalPort and creates a list of them.
     *
     *    @param [in] errors      List of found errors.
     *    @param [in] transPort	The TransactionalPort whose errors to find.
     *    @param [in] context     Context to help locate the errors.
     */
    void findErrorsInTransactionalPort(QStringList& errors, QString const& context,
        QSharedPointer<TransactionalPort> transPort) const;

    /*!
     *	Finds possible errors in a qualifier of a port and creates a list of them.
     *
     *    @param [in] errors	            List of found errors.
     *    @param [in] context	            Context to help locate the errors.
     *    @param [in] abstraction     	The abstraction definition being checked.
     */
    void findErrorsInPortQualifier(QStringList& errors, QSharedPointer<Qualifier> qualifier, QString const& context,
        QSharedPointer<AbstractionDefinition> abstraction) const;


    /*!
     *  Find errors in the selected port system group.
     *
     *    @param [in] errors                      List of found errors.
     *    @param [in] systemGroup                 The selected system group.
     *    @param [in] context                     Context to help locate the errors.
     *    @param [in] availableSystemNames        List of the available system names.
     *    @param [in] busDefinitionIdentifier     Identifier for the bus definition containing the system names.
     *
     *    @return
     */
    void findErrorsInSystemGroup(QVector<QString>& errors, QString const& systemGroup, QString const& context,
        QStringList const& availableSystemNames, QString const& busDefinitionIdentifier) const;

    /*!
     *	Find errors in the packets of the selected port.
     *
     *    @param [in] errors	    List of found errors.
     *    @param [in] context	    Context to help locate the errors.
     *    @param [in] port	    The port whose packets are checked.
     *    @param [in] revision	The IP-XACT revision of the document.
     */
    void findErrorsInPortPackets(QVector<QString>& errors, QString const& context,
        QSharedPointer<PortAbstraction> port, Document::Revision revision) const;

    /*!
     *	Checks if the given expression is valid.
     *  
     *    @param [in] expression     The expression to check.
     *	    
     * 	    @return True, if the expression is valid, otherwise false.
     */
    bool isValidExpression(QString const& expression) const;

    QSharedPointer<ExpressionParser> expressionParser_;

    LibraryInterface* library_;

    ProtocolValidator protocolValidator_;

    PacketValidator packetValidator_;
};

#endif // PORTABSTRACTIONVALIDATOR_H
