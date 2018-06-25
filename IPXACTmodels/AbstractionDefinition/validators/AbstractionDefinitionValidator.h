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

#include <QString>
#include <QVector>
#include <QSet>
#include "../TransactionalPort.h"
#include "../WirePort.h"

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
    AbstractionDefinitionValidator(LibraryInterface* library,
        QSharedPointer<ExpressionParser> expressionParser);

	//! The destructor.
	virtual ~AbstractionDefinitionValidator();

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

    // Disable copying.
    AbstractionDefinitionValidator(AbstractionDefinitionValidator const& rhs);
    AbstractionDefinitionValidator& operator=(AbstractionDefinitionValidator const& rhs);

	/*!
	 *  Validates the given PortAbstraction.
	 *
	 *      @param [in] port			The PortAbstraction to validate.		
	 *      @param [in] ports			Collection of ports within the abstraction definition.
     *      @param [in] busDefinition   The bus definition used by the abstraction definition.
	 */
    bool isValidPortAbstraction(QSharedPointer<PortAbstraction> port,
        QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports,
        QSharedPointer<const BusDefinition> busDefinition) const;

	/*!
	*   Finds possible errors in a PortAbstraction and creates a list of them.
	 *
	 *      @param [in] errors			List of found errors.
	 *      @param [in] port			The PortAbstraction whose errors to find.
	 *      @param [in] ports			Collection of ports within the abstraction definition.
     *      @param [in] busDefinition   Bus definition used by the abstraction definition.
	 */
	void findErrorsInPortAbstraction(QVector<QString>& errors,
		QSharedPointer<PortAbstraction> port, 
        QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports,
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
