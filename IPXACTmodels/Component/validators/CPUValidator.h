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

#include <IPXACTmodels/Component/Cpu.h>

#include <QString>
#include <QVector>

//-----------------------------------------------------------------------------
//! Validator for ipxact:CPU.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT CPUValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 */
    CPUValidator();

	//! The destructor.
	~CPUValidator();
    
    /*!
     *  Validates the given CPU.
     *
     *      @param [in] CPU           The CPU to validate.
     *
     *      @return True, if the CPU is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<Cpu> CPU) const;

    /*!
     *  Finds possible errors in a CPU and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] CPU   The CPU whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<Cpu> CPU,
		QString const& context) const;

private:

    /*!
     *  Check if the name is valid.
     *
     *      @param [in] name    The name to be evaluated.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QString const& name) const;

	// Disable copying.
	CPUValidator(CPUValidator const& rhs);
	CPUValidator& operator=(CPUValidator const& rhs);
};

#endif // SYSTEMVERILOGVALIDATOR_H
