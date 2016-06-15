//-----------------------------------------------------------------------------
// CellSpecification: CellSpecificationValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 10.12.2015
//
// Description:
// Validator for ipxact:CellSpecification.
//-----------------------------------------------------------------------------

#ifndef CELLSPECIFICATIONVALIDATOR_H
#define CELLSPECIFICATIONVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QVector>
#include <QSharedPointer>

class CellSpecification;
//-----------------------------------------------------------------------------
//! Validator for ipxact:CellSpecification.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT CellSpecificationValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 */
    CellSpecificationValidator();

	//! The destructor.
	~CellSpecificationValidator();

    /*!
     *  Validates the given CellSpecification.
     *
     *      @param [in] CellSpecification           The CellSpecification to validate.
     *
     *      @return True, if the CellSpecification is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<CellSpecification> cellSpecification) const;

    /*!
     *  Finds possible errors in a CellSpecification and creates a list of them.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] CellSpecification   The CellSpecification whose errors to find.
     *      @param [in] context             Context to help locate the errors.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<CellSpecification> cellSpecification,
		QString const& context) const;

private:

    // Disable copying.
    CellSpecificationValidator(CellSpecificationValidator const& rhs);
    CellSpecificationValidator& operator=(CellSpecificationValidator const& rhs);

	// Contains possible functions.
	QVector<QString> functions_;
};

#endif // CELLSPECIFICATIONVALIDATOR_H
