//-----------------------------------------------------------------------------
// File: Enumeration.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.09.2014
//
// Description:
// Enumeration matches the ipxact:enumeration element in IP-XACT.
//-----------------------------------------------------------------------------

#ifndef ENUMERATION_H
#define ENUMERATION_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>

//-----------------------------------------------------------------------------
//! Enumeration matches the ipxact:enumeration element in IP-XACT.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Enumeration 
{
public:
    
    //! The default constructor.
    Enumeration();

    /*!
     *  The copy constructor.
     *
     *      @param [in] other   The enumeration to copy.
     */
    Enumeration(Enumeration const& other);

	//! The destructor.
	~Enumeration();

    /*!
     *  Sets the enumeration value.
     *
     *      @param [in] value   The value to set.
     */
    void setValue(QString const& value);

    /*!
     *  Gets the enumeration value.
     *
     *      @return The enumeration value.
     */
    QString getValue() const;
    
    /*!
     *  Sets the enumeration text to display instead of value.
     *
     *      @param [in] text   The text to set.
     */
    void setText(QString const& text);
    
    /*!
     *  Gets the enumeration text.
     *
     *      @return The enumeration text.
     */
    QString getText() const;
        
    /*!
     *  Sets the clarifying help text.
     *
     *      @param [in] help   The help text to set.
     */
    void setHelp(QString const& help);
        
    /*!
     *  Gets the clarifying help text.
     *
     *      @return The enumeration help text.
     */
    QString getHelp() const;

private:

	// Disable assignment.
	Enumeration& operator=(Enumeration const& rhs);

    //! Value of the enumeration.
	QString value_;

	//! If specified, text is displayed in place of the value. 
	QString text_;

	//! If specified, help is used to clarify the meaning of the enumeration.
	QString help_;
};

#endif // ENUMERATION_H
