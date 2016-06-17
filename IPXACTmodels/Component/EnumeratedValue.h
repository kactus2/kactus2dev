//-----------------------------------------------------------------------------
// File: EnumeratedValue.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Describes the ipxact:enumeratedValue element of a field.
//-----------------------------------------------------------------------------

#ifndef ENUMERATEDVALUE_H
#define ENUMERATEDVALUE_H

#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/NameGroup.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>

//-----------------------------------------------------------------------------
//! Describes the ipxact:enumeratedValue element of a field.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT EnumeratedValue : public NameGroup, public Extendable
{

public:

	/*! 
     *  Matches the values of ipxact:usage.
	 */
	enum EnumeratedUsage
    {
		READ = 0,
		WRITE,
		READWRITE,	// default value
        UNKNOWNUSAGE
	};

	/*!
	 *  The constructor.
	 *
	 *      @param [in] enumerationName     Name of the enumerated value.
	 *      @param [in] value               The enumerated value.
	 */
	EnumeratedValue(QString const& enumerationName = QString(), QString const& value = QString());

	//! Copy constructor
	EnumeratedValue(const EnumeratedValue& other);

	//! Assignment operator
	EnumeratedValue& operator=(const EnumeratedValue& other);

    //! The destructor
    virtual ~EnumeratedValue();

    /*!
     *  Get the usage.
     *
     *      @return     The usage.
     */
    EnumeratedUsage getUsage() const;

    /*!
     *  Set the usage.
     *
     *      @param [in] usage   The new usage value.
     */
    void setUsage(EnumeratedUsage usage);

    /*!
     *  Get the value of the enumeratedValue.
     *
     *      @return The value.
     */
	QString getValue() const;

    /*!
     *  Set the value of the enumeratedValue.
     *
     *      @param [in] value   The new value.
     */
	void setValue(QString const& value);

    /*!
     *  Static function that translates enumeratedUsage to QString.
     *
     *      @param [in] usage   The usage to be translated to QString
     *
     *      @return String form of the usage.
     */
    static QString usage2Str(EnumeratedValue::EnumeratedUsage usage);

    /*!
     *  Static function that translates QString to enumeratedUsage.
     *
     *      @param [in] usage           QString that is translated into enumeratedUsage.
     *      @param [in] defaultValue    The value that is returned if the string doesn't match any usage.
     *
     *      @return EnumeratedValue that matches the string of defaultValue.
     */
    static EnumeratedValue::EnumeratedUsage str2Usage(const QString& usage, EnumeratedUsage defaultValue);

private:

    //! Defines the software access condition.
    EnumeratedUsage usage_;

    //! The value of the enumeratedValue
    QString value_;
};

Q_DECLARE_METATYPE(QSharedPointer<EnumeratedValue>);

#endif // ENUMERATEDVALUE_H
