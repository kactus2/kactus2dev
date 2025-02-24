//-----------------------------------------------------------------------------
// File: ValueFormatter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.12.2014
//
// Description:
// Formatter for numeric values.
//-----------------------------------------------------------------------------

#ifndef VALUEFORMATTER_H
#define VALUEFORMATTER_H

#include <QString>

#include <IPXACTmodels/ipxactmodels_global.h>

class IPXACTMODELS_EXPORT ValueFormatter 
{
public:

	//! The constructor.
	ValueFormatter();

	//! The destructor.
	virtual ~ValueFormatter() = default;

    /*!
     *  Formats the given value.
     *
     *    @param [in] value   The value to format.
     *    @param [in] base    The base to use in formatting.
     *
     *    @return The formatted value.
     */
    virtual QString format(QString const& value, int base) const;

private:

	// Disable copying.
	ValueFormatter(ValueFormatter const& rhs);
	ValueFormatter& operator=(ValueFormatter const& rhs);

    /*!
     *  Format the given array.
     *
     *    @param [in] arrayValue  The array to be formatted.
     *    @param [in] base        The base to use in formatting.
     */
    QString formatArray(QString const& arrayValue, int& base) const;
};

#endif // VALUEFORMATTER_H
