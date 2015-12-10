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

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <library/LibraryManager/libraryinterface.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <QString>
#include <QVector>
#include <QSet>
#include "../TransactionalPort.h"
#include "../WirePort.h"

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
    AbstractionDefinitionValidator();

	//! The destructor.
	~AbstractionDefinitionValidator();

    /*!
     *  Validates the given AbstractionDefinition.
     *
     *      @param [in] AbstractionDefinition           The AbstractionDefinition to validate.
     *
     *      @return True, if the AbstractionDefinition is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<AbstractionDefinition> abstractionDefinition,
		QSharedPointer<LibraryInterface> library) const;

    /*!
     *  Finds possible errors in a AbstractionDefinition and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] AbstractionDefinition   The AbstractionDefinition whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    virtual void findErrorsIn(QVector<QString>& errors,
		QSharedPointer<AbstractionDefinition> abstractionDefinition, QString const& context,
		QSharedPointer<LibraryInterface> library) const;

private:

	/*!
	 *  Validates the given PortAbstraction.
	 *
	 *      @param [in] port			The PortAbstraction to validate.		
	 *      @param [in] parser			Expression parser.
	 *      @param [in] logicalNames	Logical names of ports of the abstraction definition, where port is.
	 */
	bool isValidPortAbstraction(QSharedPointer<PortAbstraction> port, QSharedPointer<ExpressionParser> parser,
		QSet<QString>& logicalNames) const;

	/*!
	*   Finds possible errors in a PortAbstraction and creates a list of them.
	 *
	 *      @param [in] errors			List of found errors.
	 *      @param [in] port			The PortAbstraction whose errors to find.
	 *      @param [in] context			Context to help locate the errors.
	 *      @param [in] parser			Expression parser.
	 *      @param [in] logicalNames	Logical names of ports of the abstraction definition, where port is.
	 */
	void findErrorsInPortAbstraction(QVector<QString>& errors, QString const& context,
		QSharedPointer<PortAbstraction> port, QSharedPointer<ExpressionParser> parser,
		QSet<QString>& logicalNames) const;

	/*!
	 *  Validates the given TransactionalPort.
	 *
	 *      @param [in] transPort	The TransactionalPort to validate.
	 *      @param [in] parser		Expression parser.
	 */
	bool isValidTransactionalPort(QSharedPointer<TransactionalPort> transPort,
		QSharedPointer<ExpressionParser> parser) const;
	
	/*!
	 *   Finds possible errors in a TransactionalPort and creates a list of them.
	 *
	 *      @param [in] errors      List of found errors.
	 *      @param [in] transPort	The TransactionalPort whose errors to find.
	 *      @param [in] context     Context to help locate the errors.
	 *      @param [in] parser		Expression parser.
	 */
	void findErrorsInTransactionalPort(QVector<QString>& errors, QString const& context,
		QSharedPointer<TransactionalPort> transPort, QSharedPointer<ExpressionParser> parser) const;
	
	/*!
	 *  Validates the given WirePort.
	 *
	 *      @param [in] wirePort			The WirePort to validate.
	 *      @param [in] parser				Expression parser.
	 *      @param [in] logicalNames		Logical names of ports of the abstraction definition, where wirePort is.
	 *
	 *      @return True, if the WirePort is valid IP-XACT, otherwise false.
	 */
	bool isValidWirePort(QSharedPointer<WirePort> wirePort, QSharedPointer<ExpressionParser> parser,
		QSet<QString>& logicalNames) const;
	
	/*!
	 *   Finds possible errors in a WirePort and creates a list of them.
	 *
	 *      @param [in] errors      List of found errors.
	 *      @param [in] wirePort	The WirePort whose errors to find.
	 *      @param [in] context     Context to help locate the errors.
	 *      @param [in] parser		Expression parser.
	 *      @param [in] logicalNames	Logical names of ports of the abstraction definition, where wirePort is.
	 */
	void findErrorsInWirePort(QVector<QString>& errors, QSharedPointer<WirePort> wirePort, QString const& context,
		QSharedPointer<ExpressionParser> parser, QSet<QString>& logicalNames) const;

	/*!
	 *  Validates the constraints of parameter wirePort.
	 *
	 *      @param [in] wirePort		
	 *      @param [in] logicalNames		
	 */
	bool validateConstraints(QSharedPointer<WirePort> wirePort, QSet<QString>& logicalNames) const;

	/*!
	 *   Finds possible errors in constraints of parameter wirePort and creates a list of them.
	 *
	 *      @param [in] errors      List of found errors.
	 *      @param [in] wirePort	The WirePort whose errors to find.
	 *      @param [in] context     Context to help locate the errors.
	 *      @param [in] logicalNames	Logical names of ports of the abstraction definition, where wirePort is.
	 */
	void findErrorsInConstraints(QVector<QString>& errors, QSharedPointer<WirePort> wirePort, QString const& context,
		QSet<QString>& logicalNames) const;

    /*!
     *  Check if the name is valid.
     *
     *      @param [in] name    The name to be evaluated.
     *
     *      @return True, if the name is valid, otherwise false.
     */
	bool hasValidName(QString const& name) const;

	// Disable copying.
	AbstractionDefinitionValidator(AbstractionDefinitionValidator const& rhs);
	AbstractionDefinitionValidator& operator=(AbstractionDefinitionValidator const& rhs);
};

#endif // SYSTEMVERILOGVALIDATOR_H
