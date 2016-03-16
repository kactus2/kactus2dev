//-----------------------------------------------------------------------------
// File: CommonItemsWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.09.2015
//
// Description:
// Writer class for common IP-XACT elements: VLNV, parameters, assertions, vendor extensions and presence.
//-----------------------------------------------------------------------------

#ifndef COMMONITEMSWRITER_H
#define COMMONITEMSWRITER_H

#include <QObject>
#include <QSharedPointer>
#include <QXmlStreamWriter>

class Extendable;
class VendorExtension;
class VLNV;
class Parameter;
class Assertion;
class ConfigurableElementValue;

//-----------------------------------------------------------------------------
//! Writer class for common IP-XACT elements: VLNV, parameters, assertions, vendor extensions and presence.
//-----------------------------------------------------------------------------
class CommonItemsWriter : public QObject
{
    Q_OBJECT
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent   The parent object.
     */
    CommonItemsWriter(QObject* parent = 0);

	//! The destructor.
    ~CommonItemsWriter();

protected:

    /*!
     *  Writes the given VLNV as separate elements into XML.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] VLNV        The VLNV to write.
     */
    void writeVLNVElements(QXmlStreamWriter& writer, VLNV const& vlnv) const;
    
    /*!
     *  Writes the given VLNV as attributes for the current element into XML.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] VLNV        The VLNV to write.
     */
    void writeVLNVAttributes(QXmlStreamWriter& writer, VLNV const& vlnv) const;
    
    /*!
     *  Writes the parameters of a given document into XML.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] parameters  A list of the parameters to be written.
     */
    void writeParameters(QXmlStreamWriter& writer, QSharedPointer<QList<QSharedPointer<Parameter> > > parameters)
        const;

    /*!
     *  Writes the vendor extensions of a given element into XML.
     *
     *      @param [in] writer      The XML writer to use.
     *      @param [in] element     The element whose vendor extensions to write.
     */
    void writeVendorExtensions(QXmlStreamWriter& writer,  QSharedPointer<Extendable> element) const;

    /*!
     *  Write the isPresent element.
     *
     *      @param [in] writer      The XML writer to use.
     *      @param [in] isPresent   The value for the isPresent element.
     */
    void writeIsPresent(QXmlStreamWriter& writer, QString const& isPresent) const;

    /*!
     *  Write the configurable element values.
     *
     *      @param [in] writer                  The XML writer to use.
     *      @param [in] configurableElements    The configurable elemenets.
     */
    void writeConfigurableElementValues(QXmlStreamWriter& writer,
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > configurableElements) const;

private:

	// Disable copying.
    CommonItemsWriter(CommonItemsWriter const& rhs);
    CommonItemsWriter& operator=(CommonItemsWriter const& rhs);

};

#endif // DOCUMENTWRITER_H