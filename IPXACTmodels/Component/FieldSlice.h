//-----------------------------------------------------------------------------
// File: FieldSlice.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 18.8.2023
//
// Description:
// Describes the ipxact:fieldSlice element.
//-----------------------------------------------------------------------------

#ifndef FIELDSLICE_H
#define FIELDSLICE_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/NameGroup.h>

#include <IPXACTmodels/common/Range.h>

#include <IPXACTmodels/Component/FieldReference.h>

#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QList>

//-----------------------------------------------------------------------------
//! Equals the ipxact:FieldSlice element in IP-Xact specification.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT FieldSlice : public NameGroup, public FieldReference, public Range
{

public:

	using List = QSharedPointer<QList<QSharedPointer<FieldSlice> > >;

	//! The default constructor.
	explicit FieldSlice(QString const& name = QString());

	//! Copy constructor
	FieldSlice(const FieldSlice& other);

	//! Assignment operator
	FieldSlice& operator=(const FieldSlice& other);

	//! The destructor
	virtual ~FieldSlice();


};

#endif /* FIELDSLICE_H */
