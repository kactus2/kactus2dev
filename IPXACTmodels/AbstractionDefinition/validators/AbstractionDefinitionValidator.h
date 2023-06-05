//-----------------------------------------------------------------------------
// AbstractionDefinition: AbstractionDefinitionValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 04.12.2015
//
// Description:
// Validator for ipxact:AbstractionDefinition.
//-----------------------------------------------------------------------------

#ifndef AbstractionDefinitionVALIDATOR_H
#define AbstractionDefinitionVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/common/validators/ProtocolValidator.h>

#include <IPXACTmodels/AbstractionDefinition/TransactionalPort.h>
#include <IPXACTmodels/AbstractionDefinition/WirePort.h>

#include <QString>
#include <QVector>
#include <QSet>

class AbstractionDefinition;
class ExpressionParser;
class LibraryInterface;
class PortAbstraction;
class BusDefinition;

//-----------------------------------------------------------------------------
//! Validator for ipxact:AbstractionDefinition.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AbstractionDefinitionValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 */
    AbstractionDefinitionValidator(LibraryInterface* library, QSharedPointer<ExpressionParser> expressionParser);

    // Disable copying.
    AbstractionDefinitionValidator(AbstractionDefinitionValidator const& rhs) = delete;
    AbstractionDefinitionValidator& operator=(AbstractionDefinitionValidator const& rhs) = delete;

	//! The destructor.
	~AbstractionDefinitionValidator() = default;

    /*!
     *  Validates the given AbstractionDefinition.
     *
     *      @param [in] AbstractionDefinition           The AbstractionDefinition to validate.
     *
     *      @return True, if the AbstractionDefinition is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<AbstractionDefinition> abstractionDefinition) const;

    /*!
     *  Finds possible errors in a AbstractionDefinition and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] AbstractionDefinition   The AbstractionDefinition whose errors to find.
     */
    virtual void findErrorsIn(QVector<QString>& errors,
		QSharedPointer<AbstractionDefinition> abstractionDefinition) const;

private:

	/*!
	 *  Validates the given PortAbstraction.
	 *
	 *      @param [in] port			        The PortAbstraction to validate.		
	 *      @param [in] abstractionDefinition   The abstraction definition being validated.
     *      @param [in] busDefinition           The bus definition used by the abstraction definition.
	 */
    bool isValidPortAbstraction(QSharedPointer<PortAbstraction> port,
        QSharedPointer<AbstractionDefinition > abstractionDefinition,
        QSharedPointer<const BusDefinition> busDefinition) const;

	/*!
	*   Finds possible errors in a PortAbstraction and creates a list of them.
	 *
	 *      @param [in] errors			        List of found errors.
	 *      @param [in] port			        The PortAbstraction whose errors to find.
	 *      @param [in] abstractionDefinition   The abstraction definition.
     *      @param [in] busDefinition           Bus definition used by the abstraction definition.
	 */
	void findErrorsInPortAbstraction(QVector<QString>& errors,
		QSharedPointer<PortAbstraction> port, 
        QSharedPointer<AbstractionDefinition> abstractionDefinition,
        QSharedPointer<const BusDefinition> busDefinition) const;

	/*!
	 *  Validates the given TransactionalPort.
	 *
	 *      @param [in] transPort	The TransactionalPort to validate.
	 */
	bool isValidTransactionalPort(QSharedPointer<TransactionalPort> transPort) const;
	
	/*!
	 *   Finds possible errors in a TransactionalPort and creates a list of them.
	 *
	 *      @param [in] errors      List of found errors.
	 *      @param [in] transPort	The TransactionalPort whose errors to find.
	 *      @param [in] context     Context to help locate the errors.
	 */
	void findErrorsInTransactionalPort(QVector<QString>& errors, QString const& context,
		QSharedPointer<TransactionalPort> transPort) const;

	/*!
	 *  Validates the given WirePort.
	 *
	 *      @param [in] wirePort			The WirePort to validate.
	 *      @param [in] ports				Collection of ports within the abstraction definition.
	 *
	 *      @return True, if the WirePort is valid IP-XACT, otherwise false.
	 */
	bool isValidWirePort(QSharedPointer<WirePort> wirePort,
        QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const;
    
	/*!
	 *   Finds possible errors in a WirePort and creates a list of them.
	 *
	 *      @param [in] errors      List of found errors.
	 *      @param [in] wirePort	The WirePort whose errors to find.
	 *      @param [in] context     Context to help locate the errors.
	 *      @param [in] ports		Collection of ports within the abstraction definition.
	 */
	void findErrorsInWirePort(QVector<QString>& errors, QSharedPointer<WirePort> wirePort,
        QString const& context, 
        QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const;

	/*!
	 *  Validates the constraints of parameter wirePort.
	 *
	 *      @param [in] wirePort		The WirePort to validate.
	 *      @param [in] ports			Collection of ports within the abstraction definition.
	 */
	bool validateConstraints(QSharedPointer<WirePort> wirePort,
        QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const;

	/*!
	 *   Finds possible errors in constraints of parameter wirePort and creates a list of them.
	 *
	 *      @param [in] errors      List of found errors.
	 *      @param [in] wirePort	The WirePort whose errors to find.
	 *      @param [in] context     Context to help locate the errors.
	 *      @param [in] ports		Collection of ports within the abstraction definition.
	 */
	void findErrorsInConstraints(QVector<QString>& errors, QSharedPointer<WirePort> wirePort, 
        QString const& context,	QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const;

    /*!
     *  Check if the name is valid.
     *
     *      @param [in] name    The name to be evaluated.
     *
     *      @return True, if the name is valid, otherwise false.
     */
	bool hasValidName(QString const& name) const;

    /*!
     *  Get the bus definition used by the selected abstraction definition
     *
     *      @param [in] abstraction     The selected abstraction definition.
     *
     *      @return The bus definition used by the selected abstraction definition.
     */
    QSharedPointer<const BusDefinition> getBusDefinition(QSharedPointer<AbstractionDefinition> abstraction) const;

    /*!
     *  Find errors in the selected port system group.
     *
     *      @param [in] errors                      List of found errors.
     *      @param [in] systemGroup                 The selected system group.
     *      @param [in] context                     Context to help locate the errors.
     *      @param [in] availableSystemNames        List of the available system names.
     *      @param [in] busDefinitionIdentifier     Identifier for the bus definition containing the system names.
     *
     *      @return 
     */
    void findErrorsInSystemGroup(QVector<QString>& errors, QString const& systemGroup, QString const& context,
        QStringList const& availableSystemNames, QString const& busDefinitionIdentifier) const;

    /*!
     *  Check if the extended port elements of the selected abstraction definition are valid.
     *
     *      @param [in] abstraction     The selected abstraction definition.
     *
     *      @return True, if the extend ports are valid, false otherwise.
     */
    bool hasValidExtendPorts(QSharedPointer<AbstractionDefinition> abstraction) const;

    /*!
     *  Check if the wire of the selected extended abstraction port is valid.
     *
     *      @param [in] abstractionPort     The selected abstraction port.
     *      @param [in] extendPort          The extend abstraction port.
     *
     *      @return True, if the extended wire is valid, false otherwise.
     */
    bool extendWirePortIsValid(QSharedPointer<PortAbstraction> abstractionPort,
        QSharedPointer<PortAbstraction> extendPort) const;

    /*!
     *  Check if the transactional of the selected extended abstraction port is valid.
     *
     *      @param [in] abstractionPort     The selected abstraction port.
     *      @param [in] extendPort          The extend abstraction port.
     *
     *      @return True, if the extended transactional is valid, false otherwise.
     */
    bool extendTransactionalPortAbstractionIsValid(QSharedPointer<PortAbstraction> abstractionPort,
        QSharedPointer<PortAbstraction> extendPort) const;

    /*!
     *  Check if the extended transactional port is valid.
     *
     *      @param [in] port        The selected transactional port.
     *      @param [in] extendPort  The extend transactional port.
     *
     *      @return True, if the extended transactional port is valid, false otherwise.
     */
    bool extendTransactionalPortIsValid(QSharedPointer<TransactionalPort> port,
        QSharedPointer<TransactionalPort> extendPort) const;

    /*!
     *  Get the extended port abstraction matching the selected port abstraction.
     *
     *      @param [in] logicalPort         The selected abstraction port.
     *      @param [in] extendAbstraction   The extend abstraction definition.
     *
     *      @return The extend port matching the selected port.
     */
    QSharedPointer<PortAbstraction> getMatchingExtendPort(QSharedPointer<PortAbstraction> logicalPort,
        QSharedPointer<AbstractionDefinition> extendAbstraction) const;

    /*!
     *  Check if the extend is valid.
     *
     *      @param [in] abstraction     The selected abstraction definition.
     *
     *      @return True, if the extend is valid, false otherwise.
     */
    bool hasValidExtend(QSharedPointer<AbstractionDefinition> abstraction) const;

    /*!
     *  Check if the contained bus type defines the bus type of the extended abstraction definition.
     *
     *      @param [in] abstraction     The selected abstraction definition.
     *
     *      @return True, if the bus type defines the extend abstraction definition bus type, false otherwise.
     */
    bool busTypeDefinesExtendedAbstractionBusType(QSharedPointer<AbstractionDefinition> abstraction) const;

    /*!
     *  Check if the selected bus type is extended from the other bus type.
     *
     *      @param [in] busType         The selected bus type.
     *      @param [in] extendBustype   The target bus type.
     *
     *      @return True, if the bus type is extended from the other bus type, false otherwise.
     */
    bool busTypeIsExtendedFromExtendBusType(QSharedPointer<const BusDefinition> busType,
        QSharedPointer<const BusDefinition> extendBustype) const;

    /*!
     *  Get the extend abstraction definition.
     *
     *      @param [in] abstraction     The selected abstraction definition.
     *
     *      @return The extend abstraction definition.
     */
    QSharedPointer<AbstractionDefinition> getExtendedAbstractionDefinition(
        QSharedPointer<AbstractionDefinition> abstraction) const;

    /*!
     *  Get the bus type extended bus definition.
     *
     *      @param [in] busType     The selected bus type.
     *
     *      @return The extend bus definition of the selected bus type.
     */
    QSharedPointer<const BusDefinition> getBusTypeExtend(QSharedPointer<const BusDefinition> busType) const;

    /*!
     *   Find possible errors in extend of the selected abstraction definition and create a list of them.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] context         Context to help locate the errors.
     *      @param [in] abstraction     The selected abstraction definition.
     */
    void findErrorsInExtend(QVector<QString>& errors, QString const& context,
        QSharedPointer<AbstractionDefinition> abstraction) const;

    /*!
     *  Find errors in extended port abstractions of the selected abstraction definition.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] context         Context to help locate the errors.
     *      @param [in] abstraction     The selected abstraction definition.
     */
    void findErrorsInExtendPorts(QVector<QString>& errors, QString const& context,
        QSharedPointer<AbstractionDefinition> abstraction) const;

    /*!
     *  Find errors in wire abstraction of the selected extend port abstraction.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] context     Context to help locate the errors.
     *      @param [in] port        The selected abstraction port.
     *      @param [in] extendPort  The extend port.
     */
    void findErrorsInExtendWireAbstraction(QVector<QString>& errors, QString const& context,
        QSharedPointer<PortAbstraction> port, QSharedPointer<PortAbstraction> extendPort) const;

    /*!
     *  Find errors in the selected extend wire port.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] context     Context to help locate the errors.
     *      @param [in] port        The selected wire port.
     *      @param [in] extendPort  The extend port.
     *      @param [in] portName    Name of the port.
     *      @param [in] modeName    Name of the mode.
     */
    void findErrorsInExtendWirePort(QVector<QString>& errors, QString const& context,
        QSharedPointer<WirePort> port, QSharedPointer<WirePort> extendPort, QString const& portName,
        QString const& modeName) const;

    /*!
     *  Find errors in transactional abstraction of the selected extend port abstraction.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] context     Context to help locate the errors.
     *      @param [in] port        The selected abstraction port.
     *      @param [in] extendPort  The extend port.
     */
    void findErrorsInExtendTransactionalAbstraction(QVector<QString>& errors, QString const& context,
        QSharedPointer<PortAbstraction> port, QSharedPointer<PortAbstraction> extendPort) const;

    /*!
     *  Find errors in transactional abstraction of the selected extend port abstraction.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] context     Context to help locate the errors.
     *      @param [in] port        The selected transactional port.
     *      @param [in] extendPort  The extend port.
     */
    void findErrorsInExtendTransactionalPort(QVector<QString>& errors, QString const& context,
        QSharedPointer<TransactionalPort> port, QSharedPointer<TransactionalPort> extendPort) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The IP-XACT library available.
    LibraryInterface* library_;

    //! Parser for expressions in abstraction definition.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! Validator for abstraction definition parameters.
    ParameterValidator parameterValidator_;

    //! Validator for protocols within transactional ports.
    ProtocolValidator protocolValidator_;
};

#endif // SYSTEMVERILOGVALIDATOR_H
