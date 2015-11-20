//-----------------------------------------------------------------------------
// File: ChannelValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 19.11.2015
//
// Description:
// Validator for ipxact:Channel.
//-----------------------------------------------------------------------------

#ifndef ChannelVALIDATOR_H
#define ChannelVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/Channel.h>

#include <QString>
#include <QVector>

//-----------------------------------------------------------------------------
//! Validator for ipxact:Channel.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ChannelValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 */
    ChannelValidator();

	//! The destructor.
	~ChannelValidator();
    
    /*!
     *  Validates the given Channel.
     *
     *      @param [in] Channel           The Channel to validate.
     *
     *      @return True, if the Channel is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<Channel> channel) const;

    /*!
     *  Finds possible errors in a Channel and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] Channel   The Channel whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<Channel> channel,
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
	ChannelValidator(ChannelValidator const& rhs);
	ChannelValidator& operator=(ChannelValidator const& rhs);
};

#endif // SYSTEMVERILOGVALIDATOR_H
