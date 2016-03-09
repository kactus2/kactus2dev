//-----------------------------------------------------------------------------
// File: CPUValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 18.11.2015
//
// Description:
// Validator for ipxact:CPU.
//-----------------------------------------------------------------------------

#ifndef CPUVALIDATOR_H
#define CPUVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QList>
#include <QSharedPointer>
#include <QString>
#include <QVector>

class AddressSpace;
class Cpu;
class ExpressionParser;
class ParameterValidator2014;
//-----------------------------------------------------------------------------
//! Validator for ipxact:CPU.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT CPUValidator
{
public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] parameterValidator  Validator for cpu parameters.
     *      @param [in] expressionParser    Parser for expressions in cpu.
     *      @param [in] addressSpaces       The available address spaces that may be referenced.
	 */
    CPUValidator(QSharedPointer<ParameterValidator2014> parameterValidator,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces);

	//! The destructor.
	virtual ~CPUValidator();
    
    /*!
     *  Change the available address spaces.
     *
     *      @param [in] newAddressSpaces    The new address spaces.
     */
    void componentChange(QSharedPointer<QList<QSharedPointer<AddressSpace> > > newAddressSpaces);

    /*!
     *  Validates the given CPU.
     *
     *      @param [in] cpu           The CPU to validate.
     *
     *      @return True, if the CPU is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<Cpu> cpu) const;

    /*!
     *  Checks if the address space references of a CPU are valid.
     *
     *      @param [in] cpu   The CPU whose address space references to check.
     *
     *      @return True, if the references are valid, otherwise false.
     */
    bool hasValidAddressSpaceReferences(QSharedPointer<Cpu> cpu) const;

    /*!
     *  Finds possible errors in a CPU and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] cpu         The CPU whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<Cpu> cpu, QString const& context) const;

private:

    // Disable copying.
    CPUValidator(CPUValidator const& rhs);
    CPUValidator& operator=(CPUValidator const& rhs);

    /*!
     *  Check if the name is valid.
     *
     *      @param [in] name    The name to be evaluated.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QString const& name) const;

    /*!
     *  Checks if the given reference to address space is valid.
     *
     *      @param [in] reference   The name reference for the address space.
     *
     *      @return True, if the reference is valid, otherwise false.
     */
    bool isValidAddressSpaceReference(QString const& reference) const;

    //! Validator for cpu parameters.
    QSharedPointer<ParameterValidator2014> parameterValidator_;

    //! Parser for expressions in cpu.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The available address spaces for referencing.
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces_;
};

#endif // SYSTEMVERILOGVALIDATOR_H
