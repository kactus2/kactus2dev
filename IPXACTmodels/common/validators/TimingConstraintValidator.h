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

#ifndef TIMINGCONSTRAINTVALIDATOR_H
#define TIMINGCONSTRAINTVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QList>
#include <QString>
#include <QVector>
#include <QSharedPointer>

class PortAbstraction;
class TimingConstraint;
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
	 *      @param [in] TimingConstraint    The TimingConstraint to validate.
	 *      @param [in] ports               Collection ports where the constraint belongs to.
     *
     *      @return True, if the TimingConstraint is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<TimingConstraint> timingConstraint,
		QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const;

    /*!
     *  Finds possible errors in a TimingConstraint and creates a list of them.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] TimingConstraint    The TimingConstraint whose errors to find.
	 *      @param [in] context             Context to help locate the errors.
	 *      @param [in] ports               Collection ports where the constraint belongs to.
     */
    void findErrorsIn(QVector<QString>& errors,
        QSharedPointer<TimingConstraint> timingConstraint,
		QString const& context, QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const;

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

#endif // TIMINGCONSTRAINTVALIDATOR_H
