//-----------------------------------------------------------------------------
// TimingConstraint: TimingConstraintValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 09.12.2015
//
// Description:
// Validator for ipxact:TimingConstraint.
//-----------------------------------------------------------------------------

#ifndef TimingConstraintVALIDATOR_H
#define TimingConstraintVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/TimingConstraint.h>

#include <QString>
#include <QVector>
#include <QSet>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Validator for ipxact:TimingConstraint.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT TimingConstraintValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 */
    TimingConstraintValidator();

	//! The destructor.
	~TimingConstraintValidator();

    /*!
     *  Validates the given TimingConstraint.
     *
     *      @param [in] TimingConstraint           The TimingConstraint to validate.
     *
     *      @return True, if the TimingConstraint is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<TimingConstraint> timingConstraint, QSet<QString>& portNames) const;

    /*!
     *  Finds possible errors in a TimingConstraint and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] TimingConstraint   The TimingConstraint whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<TimingConstraint> timingConstraint,
		QString const& contex, QSet<QString>& ports ) const;

private:

	/*!
	 *  Returns true, if value of parameter timingConstraint is valid.
	 *
	 *      @param [in] timingConstraint	The inspected timing constraint.
	 *
	 *      @return See above.
	 */
	bool hasValidValue(QSharedPointer<TimingConstraint> timingConstraint) const;

	/*!
	 *  Returns true, if edge of parameter timingConstraint is valid.
	 *
	 *      @param [in] timingConstraint	The inspected timing constraint.
	 *
	 *      @return See above.
	 */
	bool hasValidEdge(QSharedPointer<TimingConstraint> timingConstraint) const;

	// Disable copying.
	TimingConstraintValidator(TimingConstraintValidator const& rhs);
	TimingConstraintValidator& operator=(TimingConstraintValidator const& rhs);
};

#endif // SYSTEMVERILOGVALIDATOR_H
