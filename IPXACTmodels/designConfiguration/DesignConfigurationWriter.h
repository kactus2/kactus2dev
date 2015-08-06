//-----------------------------------------------------------------------------
// File: DesignConfigurationWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.08.2015
//
// Description:
// Writer class for IP-XACT Design configuration element.
//-----------------------------------------------------------------------------

#ifndef DESIGNCONFIGURATIONWRITER_H
#define DESIGNCONFIGURATIONWRITER_H

#include "DesignConfiguration.h"

#include <IPXACTmodels/vlnv.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <QObject>
#include <QXmlStreamWriter>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT Design configuration element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT DesignConfigurationWriter : public QObject
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this constructor.
     */
    DesignConfigurationWriter(QObject* parent = 0);

    //! The destructor.
    ~DesignConfigurationWriter();

    /*!
     *  Writes the given design configuration into xml.
     *
     *      @param [in] writer                  The xml writer to use.
     *      @param [in] designConfiguration     The selected design configuration.
     */
    void writeDesignConfiguration(QXmlStreamWriter& writer,
        QSharedPointer<DesignConfiguration> designConfiguration) const;

private:

    //! No copying allowed.
    DesignConfigurationWriter(DesignConfigurationWriter const& rhs);
    DesignConfigurationWriter& operator=(DesignConfigurationWriter const& rhs);

    /*!
     *  Write the document start elements.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] designConfiguration     The selected design configuration.
     */
    void writeDesignConfigurationStart(QXmlStreamWriter& writer,
        QSharedPointer<DesignConfiguration> designConfiguration) const; 

    /*!
     *  Write the xlm name space.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] designConfiguration     The selected design configuration.
     */
    void writeXMLNameSpace(QXmlStreamWriter& writer, QSharedPointer<DesignConfiguration> designConfiguration) const;

    /*!
     *  Write attributes.
     *
     *      @param [in] writer      The used xml writer.
     *      @param [in] attributes  Key-value pairs to be written as attributes.
     */
    void writeAttributes(QXmlStreamWriter& writer, QMap<QString, QString> attributes) const;

    /*!
     *  Write the vlnv of design configuration as elements.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] designConfiguration     The selected design configuration.
     */
    void writeVLNVasElements(QXmlStreamWriter& writer, QSharedPointer<DesignConfiguration> designConfiguration) const;

    /*!
     *  Write the description.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] designConfiguration     The selected design configuration.
     */
    void writeDescription(QXmlStreamWriter& writer, QSharedPointer<DesignConfiguration> designConfiguration) const;

    /*!
     *  Write the VLNV of the design configuration's referenced design.
     *
     *      @param [in] writer              The used xml writer.
     *      @param [in] designReference     The VLNV of the referenced design.
     */
    void writeDesignReference(QXmlStreamWriter& writer, VLNV& designReference) const;

    /*!
     *  Write the VLNV as attributes.
     *
     *      @param [in] writer      The used xml writer.
     *      @param [in] itemVLNV    The VLNV to be written.
     */
    void writeVLNVAsAttributes(QXmlStreamWriter& writer, VLNV& itemVLNV) const;

    /*!
     *  Write generator chain configuration.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] designConfiguration     The selected design configuration.
     */
    void writeGeneratorChainConfiguration(QXmlStreamWriter& writer,
        QSharedPointer<DesignConfiguration> designConfiguration) const;

    /*!
     *  Write the configurable element values.
     *
     *      @param [in] writer          The used xml writer.
     *      @param [in] configurables   A list of configurable element values.
     */
    void writeConfigurableElementValues(QXmlStreamWriter& writer,
        QList<QSharedPointer<ConfigurableElementValue> > configurables) const;

    /*!
     *  Write the interconnection configurations.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] designConfiguration     The selected design configuration.
     */
    void writeInterConnectionConfiguration(QXmlStreamWriter& writer,
        QSharedPointer<DesignConfiguration> designConfiguration) const;

    /*!
     *  Write the multiple abstractor instances.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] multipleAbstractors     A list of multipleAbastractors.
     */
    void writeMultipleAbstractorInstances(QXmlStreamWriter& writer,
        QList<QSharedPointer<MultipleAbstractorInstances> > multipleAbstractors) const;

    /*!
     *  Write the interface references.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] interfaceReferences     A list of interface references.
     */
    void writeInterfaceReferences(QXmlStreamWriter& writer,
        QList<QSharedPointer<InterfaceRef> > interfaceReferences) const;

    /*!
     *  Write the abstractor instances.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] abstractorInstances     A list of abstractor instances.
     */
    void writeAbstractorInstances(QXmlStreamWriter& writer,
        QList<QSharedPointer<AbstractorInstance> > abstractorInstances) const;

    /*!
     *  Write the view configurations.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] designConfiguration     The selected design configuration.
     */
    void writeViewConfigurations(QXmlStreamWriter& writer,
        QSharedPointer<DesignConfiguration> designConfiguration) const;

    /*!
     *  Write the parameters.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] designConfiguration     The selected design configuration.
     */
    void writeParameters(QXmlStreamWriter& writer, QSharedPointer<DesignConfiguration> designConfiguration) const;

    /*!
     *  Write the assertions.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] designConfiguration     The selected design configuration.
     */
    void writeAssertions(QXmlStreamWriter& writer, QSharedPointer<DesignConfiguration> designConfiguration) const;

    /*!
     *  Write the vendor extensions.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] designConfiguration     The selected design configuration.
     */
    void writeVendorExtensions(QXmlStreamWriter& writer,
        QSharedPointer<DesignConfiguration> designConfiguration) const;
};

#endif // DESIGNCONFIGURATIONWRITER_H
