//-----------------------------------------------------------------------------
// Protocol: ProtocolValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 07.12.2015
//
// Description:
// Validator for ipxact:Protocol.
//-----------------------------------------------------------------------------

#ifndef ProtocolVALIDATOR_H
#define ProtocolVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Protocol.h>

#include <QString>
#include <QVector>

//-----------------------------------------------------------------------------
//! Validator for ipxact:Protocol.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ProtocolValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 */
    ProtocolValidator();

	//! The destructor.
	~ProtocolValidator();

    /*!
     *  Validates the given Protocol.
     *
     *      @param [in] Protocol           The Protocol to validate.
     *
     *      @return True, if the Protocol is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<Protocol> Protocol) const;

    /*!
     *  Finds possible errors in a Protocol and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] Protocol   The Protocol whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    virtual void findErrorsIn(QVector<QString>& errors,
		QSharedPointer<Protocol> Protocol, QString const& contex) const;

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
	ProtocolValidator(ProtocolValidator const& rhs);
	ProtocolValidator& operator=(ProtocolValidator const& rhs);
};

#endif // SYSTEMVERILOGVALIDATOR_H
