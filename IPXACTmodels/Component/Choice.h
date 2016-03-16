//-----------------------------------------------------------------------------
// File: choice.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.10.2010
//
// Description:
// Equals the ipxact:choice element in IP-Xact specification.
// Choice contains the name of the choice and the enumerations defined in it.
//-----------------------------------------------------------------------------

#ifndef CHOICE_H_
#define CHOICE_H_

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/Enumeration.h>

#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QList>

//-----------------------------------------------------------------------------
//! Equals the ipxact:choice element in IP-Xact specification.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Choice
{

public:

	//! The default constructor.
	Choice();

	//! Copy constructor
	Choice(const Choice& other);

	//! Assignment operator
	Choice& operator=(const Choice& other);

	//! The destructor
	virtual ~Choice();

	/*!
     *  Get the name of the choice.
	 *
	 *      @return QString containing the name.
	 */
	QString name() const;

	/*!
     *  Set the name of the choice.
	 *
	 *      @param [in] name QString containing the name.
	 */
	void setName(const QString& name);
    
    /*!
     *  Finds if the choice has an enumeration with the given value.
     *
     *      @param [in] enumerationValue   The enumeration value to search for.
     *
     *      @return True, if the choice has an enumeration with the given value, otherwise false.
     */
    bool hasEnumeration(QString const& enumerationValue) const;

    /*!
     *  Gets the enumeration values of the choice. If a text has been specified for the enumeration,
     *  it is returned instead of the value.
     *
     *      @return The enumeration values.
     */
    QStringList getEnumerationValues() const;

    /*!
     *  Get the enumerations of the choice.
     *
     *      @return The enumerations of the choice.
     */
    QSharedPointer<QList<QSharedPointer<Enumeration> > > enumerations() const;

private:

	//! Name of the Choice element
	QString choiceName_;

	//! List of enumerations attached to the choice.
	QSharedPointer<QList<QSharedPointer<Enumeration> > > enumerations_;

};

#endif /* CHOICE_H_ */
