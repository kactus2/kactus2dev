//-----------------------------------------------------------------------------
// File: Kactus2Value.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.5.2014
//
// Description:
// Kactus2 vendor extension for name-value pairs.
//-----------------------------------------------------------------------------

#ifndef KACTUS2VALUE_H
#define KACTUS2VALUE_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/VendorExtension.h>

//-----------------------------------------------------------------------------
// class Kactus2Value.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Kactus2Value : public VendorExtension
{
public:
	/*!
	 *  The constructor.
	 *
	 *      @param [in] parent   The parent object.
	 */
    Kactus2Value(QString name, QString value);

	/*!
	 *  The destructor.
	*/
	virtual ~Kactus2Value();

    /*!
     *  Clones the vendor extension.
     *
     *      @return The clone copy of the vendor extension.
     */
    virtual Kactus2Value* clone() const;

    /*!
     *  Returns a type identifier for the vendor extension.
     *
     *      @return A type identifier of the vendor extension.
     */
    virtual QString type() const;

    /*!
     *  Writes the vendor extension to XML.
     *
     *      @param [in] writer   The writer used for writing the XML.
     */
    virtual void write(QXmlStreamWriter& writer) const;

    /*!
     *  Gets the value of the vendor extension.
     *
     *      @return The stored value.
     */
    QString value() const;

    /*!
     *  Sets the value of the vendor extension.
     *
     *      @param [in] newValue   The value to set.
     */
    void setValue(QString const& newValue);

private:
    //! Disable copying.
    Kactus2Value(Kactus2Value const& other);

	// Disable assignment.
	Kactus2Value& operator=(Kactus2Value const& rhs);
    
	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! The name identifier of the vendor extension.
    QString name_;

    //! The value of the vendor extension.
    QString value_;

};
#endif // KACTUS2VALUE_H
