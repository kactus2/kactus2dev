//-----------------------------------------------------------------------------
// Port: PortValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 25.11.2015
//
// Description:
// Validator for ipxact:Port.
//-----------------------------------------------------------------------------

#ifndef PortVALIDATOR_H
#define PortVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/View.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <QString>
#include <QVector>

//-----------------------------------------------------------------------------
//! Validator for ipxact:Port.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT PortValidator
{
public:

	/*!
	 *  The constructor.
	 *
	 */
    PortValidator();

	//! The destructor.
	~PortValidator();

    /*!
     *  Validates the given Port.
     *
     *      @param [in] Port           The Port to validate.
     *
     *      @return True, if the Port is valid IP-XACT, otherwise false.
     */
    virtual bool validate(QSharedPointer<Port> port, QSharedPointer<QList<QSharedPointer<View> > > views) const;

    /*!
     *  Finds possible errors in a Port and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] Port   The Port whose errors to find.
     *      @param [in] context     Context to help locate the errors.
     */
    virtual void findErrorsIn(QVector<QString>& errors,
		QSharedPointer<Port> Port, QString const& contex,
		QSharedPointer<QList<QSharedPointer<View> > > views) const;

private:

    /*!
     *  Check if the name is valid.
     *
     *      @param [in] name    The name to be evaluated.
     *
     *      @return True, if the name is valid, otherwise false.
     */
	bool hasValidName(QString const& name) const;
	
    /*!
     *  Check if an env identifier is valid.
     *
     *      @param [in] name    The env identifier to be evaluated.
     *
     *      @return True, if the env identifier is valid, otherwise false.
     */
	bool isValidEnvId( QString const& envId ) const;
	
    /*!
     *  Check if the referenced view exists within the provided collection.
     *
	 *      @param [in] views		The collection of potentially referenced views.
	 *      @param [in] viewRef		The reference which view that must be found.
     *
     *      @return True, if the env identifier is valid, otherwise false.
     */
	bool referencedViewExists(QSharedPointer<QList<QSharedPointer<View> > > views, QString viewRef) const;

	// Disable copying.
	PortValidator(PortValidator const& rhs);
	PortValidator& operator=(PortValidator const& rhs);
};

#endif // SYSTEMVERILOGVALIDATOR_H
