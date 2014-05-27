//-----------------------------------------------------------------------------
// File: Kactus2Placeholder.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.5.2014
//
// Description:
// Kactus2 vendor extension for single values.
//-----------------------------------------------------------------------------

#ifndef KACTUS2PLACEHOLDER_H
#define KACTUS2PLACEHOLDER_H

#include <IPXACTmodels/VendorExtension.h>

#include <QMap>

//-----------------------------------------------------------------------------
// class Kactus2Placeholder.
//-----------------------------------------------------------------------------
class Kactus2Placeholder : public VendorExtension
{
public:
	/*!
	 *  The constructor.
	 *
	 *      @param [in] name   The name of the placeholder.
	 */
    Kactus2Placeholder(QString name);

    //! Copy constructor.
    Kactus2Placeholder(Kactus2Placeholder const& other);

	/*!
	 *  The destructor.
	*/
    virtual ~Kactus2Placeholder();

    /*!
     *  Clones the vendor extension.
     *
     *      @return The clone copy of the vendor extension.
     */
    virtual Kactus2Placeholder* clone() const;

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

    void setAttribute(QString const& attributeName, QString const& attributeValue);

    QString getAttributeValue(QString const& attributeName);

private:
	//! Disable assignment.
	Kactus2Placeholder& operator=(Kactus2Placeholder const& rhs);

	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! Name of the placeholder extension.
    QString name_;

    QMap<QString, QString> attributes_;

};
#endif // KACTUS2PLACEHOLDER_H
