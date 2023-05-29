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

#include <IPXACTmodels/common/VendorExtension.h>

//-----------------------------------------------------------------------------
// class Kactus2Value.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Kactus2Value : public VendorExtension
{
public:
	/*!
	 *  The constructor.
	 *
     *      @param [in] name        Corresponds name of XML element.
     *      @param [in] value       Corresponds the value of XML element.
	 */
    Kactus2Value(std::string const& name, std::string const& value);

	/*!
	 *  The destructor.
	*/
	virtual ~Kactus2Value();

    //! Disable copying.
    Kactus2Value(Kactus2Value const& other) = delete;

    // Disable assignment.
    Kactus2Value& operator=(Kactus2Value const& rhs) = delete;

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
    std::string type() const final;

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
    std::string value() const;

    /*!
     *  Sets the value of the vendor extension.
     *
     *      @param [in] newValue   The value to set.
     */
    void setValue(std::string const& newValue);

private:

    
	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! The name identifier of the vendor extension.
    std::string name_;

    //! The value of the vendor extension.
    std::string value_;

};
#endif // KACTUS2VALUE_H
