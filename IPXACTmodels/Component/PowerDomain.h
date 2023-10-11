//-----------------------------------------------------------------------------
// File: PowerDomain.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 4.10.2023
//
// Description:
// Implementation for ipxact:powerDomain element.
//-----------------------------------------------------------------------------

#ifndef POWERDOMAIN_H
#define POWERDOMAIN_H

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/Parameter.h>

#include <QString>
#include <QStringList>

//-----------------------------------------------------------------------------
//! Implementation for ipxact:powerDomain element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT PowerDomain : public NameGroup, public Extendable
{
public:


	/*! The default constructor
	 *
	 * Constructs an empty PowerDomain which is in invalid state.
	*/
	PowerDomain();

	//! Copy constructor
	PowerDomain(const PowerDomain& other);

	//! Assignment operator
	PowerDomain& operator=(const PowerDomain& other);

	//! The destructor
	virtual ~PowerDomain();

 
	void setAlwaysOn(QString const& expression);

	QString getAlwaysOn() const;

	void setSubDomainOf(QString const& domainName);

	QString getSubDomainOf() const;

	void setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > parameters);

	QSharedPointer<QList<QSharedPointer<Parameter> > > getParameters();

private:

	void copyParameters(PowerDomain const& other);

	//! Expression for power domain being always on.
	QString alwaysOn_;

	QString subDomainOf_;

    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_ =
        QSharedPointer<QList<QSharedPointer<Parameter> > >(new QList<QSharedPointer<Parameter> >);
};

#endif // POWERDOMAIN_H
