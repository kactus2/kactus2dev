//-----------------------------------------------------------------------------
// File: ModeReference.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 28.7.2023
//
// Description:
// Describes an ipxact:modeRef element.
//-----------------------------------------------------------------------------

#ifndef MODEREFERENCE_H
#define MODEREFERENCE_H

#include <QString>

#include <IPXACTmodels/ipxactmodels_global.h>


class IPXACTMODELS_EXPORT ModeReference
{
public:

	ModeReference();

	ModeReference(ModeReference const& other);

	virtual ~ModeReference() = default;

	QString getReference() const;

	void setReference(QString const& val);

	QString getPriority() const;

	void setPriority(QString const& val);

private:
	QString reference_;

	QString priority_;
};

#endif // MODEREFERENCE_
