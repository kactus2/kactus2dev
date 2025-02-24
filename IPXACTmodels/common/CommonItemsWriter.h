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

#include "Document.h"

#include <QSharedPointer>
#include <QXmlStreamWriter>

class Extendable;
class VendorExtension;
class VLNV;
class Parameter;
class Choice;
class Assertion;
class ConfigurableElementValue;
class NameGroup;
class PartSelect;
class ModeReference;
class FileSetRef;

//-----------------------------------------------------------------------------
//! Writer class for common IP-XACT elements: VLNV, parameters, assertions, vendor extensions and presence.
//-----------------------------------------------------------------------------
class CommonItemsWriter
{
public:

    virtual ~CommonItemsWriter() = default;

    /*!
     *  Writes the given VLNV as separate elements into XML.
     *
     *    @param [in] writer      The used XML writer.
     *    @param [in] VLNV        The VLNV to write.
     */
    static void writeVLNVElements(QXmlStreamWriter& writer, VLNV const& vlnv);
    
    /*!
     *  Writes the given VLNV as attributes for the current element into XML.
     *
     *    @param [in] writer      The used XML writer.
     *    @param [in] VLNV        The VLNV to write.
     */
    static void writeVLNVAttributes(QXmlStreamWriter& writer, VLNV const& vlnv);

    /*!
     *  Writes the description for the current element into XML.
     *
     *    @param [in] writer      The used XML writer.
     *    @param [in] VLNV        The VLNV to write.
     */
    static void writeDisplayName(QXmlStreamWriter& writer, QString const& displayName);

    /*!
     *  Writes the description for the current element into XML.
     *
     *    @param [in] writer      The used XML writer.
     *    @param [in] VLNV        The VLNV to write.
     */
    static void writeDescription(QXmlStreamWriter& writer, QString const& description);

    /*!
     *  Writes the short description for the current element into XML.
     *
     *    @param [in] writer      The used XML writer.
     *    @param [in] VLNV        The VLNV to write.
     */
    static void writeShortDescription(QXmlStreamWriter& writer, QString const& shortDescription);

    /*!
     *  Writes the short description for the current element into XML.
     *
     *    @param [in] writer      The used XML writer.
     *    @param [in] VLNV        The VLNV to write.
     */
    static void writeChoices(QXmlStreamWriter& writer, QSharedPointer<QList<QSharedPointer<Choice> > > choices);

    /*!
     *  Writes the parameters of a given document into XML.
     *
     *    @param [in] writer      The used XML writer.
     *    @param [in] parameters  A list of the parameters to be written.
     *    @param [in] docRevision The document std revision.
     */
    static void writeParameters(QXmlStreamWriter& writer, 
        QSharedPointer<QList<QSharedPointer<Parameter> > > parameters,
        Document::Revision docRevision = Document::Revision::Std14);

    /*!
     *  Writes the vendor extensions of a given element into XML.
     *
     *    @param [in] writer      The XML writer to use.
     *    @param [in] element     The element whose vendor extensions to write.
     */
    static void writeVendorExtensions(QXmlStreamWriter& writer,  QSharedPointer<Extendable> element);

    /*! Reads the attributes from the QMap and uses writer to write them. The
     *  element to write the attributes to must be opened by the writer before
     *  calling this function.
     *
     *  @param [in] writer      A QXmlStreamWriter instance that has been set up to write a document.
     *  @param [in] attributes  The attributes to be written.
     */
    static void writeAttributes(QXmlStreamWriter& writer, QMap<QString, QString> const& attributes);

    /*!
     *  Write the isPresent element.
     *
     *    @param [in] writer      The XML writer to use.
     *    @param [in] isPresent   The value for the isPresent element.
     */
    static void writeIsPresent(QXmlStreamWriter& writer, QString const& isPresent);

    /*!
     *  Write the configurable element values.
     *
     *    @param [in] writer                  The XML writer to use.
     *    @param [in] configurableElements    The configurable elements.
     */
    static void writeConfigurableElementValues(QXmlStreamWriter& writer,
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > configurableElements);

    /*!
     * Writes an XML element, if the provided value is not empty.
     *
     *    @param [in] writer        The XML writer to use.
     *    @param [in] elementName   The name of the element.
     *    @param [in] value         The value of the element.
     *
     *    @return 
     */
    static void writeNonEmptyElement(QXmlStreamWriter& writer, QString const& elementName, QString const& value);

    static void writeNonEmptyElement(QXmlStreamWriter& writer, QString const& elementName, std::string const& value);

    /*!
     * Writes an XML attribute, if the provided value is not empty.
     *
     *    @param [in] writer        The XML writer to use.
     *    @param [in] elementName   The name of the attribute.
     *    @param [in] value         The value of the attribute.
     */
    static void writeNonEmptyAttribute(QXmlStreamWriter& writer, QString const& attributeName, QString const& value);
    
    static void writeNonEmptyAttribute(QXmlStreamWriter& writer, QString const& attributeName, std::string const& value);

    /*!
     *  Write a part select.
     *
     *    @param [in] writer      The used XML writer.
     *    @param [in] partSelect  The selected part select.
     */
    static void writePartSelect(QXmlStreamWriter& writer, QSharedPointer<PartSelect> partSelect);

    static void writeRange(QXmlStreamWriter& writer, Range const& range);

    /*!
     *	Write mode references.
     *  
     *    @param [in] writer          The used XML writer.
     *    @param [in] modeReferences  The mode references to write.
     */
    static void writeModeReferences(QXmlStreamWriter& writer, 
        QSharedPointer<QList<QSharedPointer<ModeReference> > > modeReferences);

    /*!
     *	Write file set references.
     *  
     *    @param [in] writer          The used XML writer.
     *    @param [in] fileSetRefs     The file set references to write.
     *    @param [in] docRevision     The IP-XACT standard revision in use.
     */
    static void writeFileSetReferences(QXmlStreamWriter& writer,
        QSharedPointer<QList<QSharedPointer<FileSetRef> > > fileSetRefs,
        Document::Revision docRevision);
};

#endif // DOCUMENTWRITER_H