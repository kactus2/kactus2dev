//-----------------------------------------------------------------------------
// File: Enumeration.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.09.2014
//
// Description:
// Enumeration matches the spirit:enumeration element in IP-XACT.
//-----------------------------------------------------------------------------

#ifndef ENUMERATION_H
#define ENUMERATION_H

#include "ipxactmodels_global.h"

#include <QDomNode>
#include <QString>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! Enumeration matches the spirit:enumeration element in IP-XACT.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Enumeration 
{
public:
    
    //! The default constructor.
    Enumeration();

    /*!
     *  The constructor.
     *
     *      @param [in] enumerationNode   The DOM node describing the enumeration.
     */
    Enumeration(QDomNode const& enumerationNode);

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

    /*!
     *  Writes the element to XML.
     *
     *      @param [in] writer   The writer to use for writing.
     */
    void write(QXmlStreamWriter& writer) const;

private:

	// Disable assignment.
	Enumeration& operator=(Enumeration const& rhs);

    /*!
	 * MANDATORY spirit:enumeration.
	 * Value of the enumeration.
	 */
	QString value_;

	/*!
	 * OPTIONAL spirit:text.
	 * If specified, text is displayed in place of the value. 
	 */
	QString text_;

	/*!
	 * OPTIONAL spirit:help.
	 * If specified, help is used to clarify the meaning of the enumeration.
	 */
	QString help_;
};

#endif // ENUMERATION_H
