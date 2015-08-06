//-----------------------------------------------------------------------------
// File: DesignConfigurationWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.08.2015
//
// Description:
// Writer class for IP-XACT Design configuration element.
//-----------------------------------------------------------------------------

#include "DesignConfigurationWriter.h"

#include "DesignConfiguration.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/ParameterWriter.h>

#include <IPXACTmodels/vlnv.h>
#include <IPXACTmodels/designConfiguration/InterconnectionConfiguration.h>

#include <QMap>
#include <QSharedPointer>

namespace
{
    const QString IPXACT_VENDOR("ipxact:vendor");
    const QString IPXACT_LIBRARY("ipxact:library");
    const QString IPXACT_NAME("ipxact:name");
    const QString IPXACT_VERSION("ipxact:version");

    const QString PLAIN_VENDOR("vendor");
    const QString PLAIN_LIBRARY("library");
    const QString PLAIN_NAME("name");
    const QString PLAIN_VERSION("version");
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::DesignConfigurationWriter()
//-----------------------------------------------------------------------------
DesignConfigurationWriter::DesignConfigurationWriter(QObject* parent /* = 0 */):
QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::~DesignConfigurationWriter()
//-----------------------------------------------------------------------------
DesignConfigurationWriter::~DesignConfigurationWriter()
{

}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeDesignConfiguration()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeDesignConfiguration(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    writeDesignConfigurationStart(writer, designConfiguration);

    writeXMLNameSpace(writer, designConfiguration);

    writeVLNVasElements(writer, designConfiguration);

    writeDescription(writer, designConfiguration);

    writeDesignReference(writer, designConfiguration->getDesignRef());

    writeGeneratorChainConfiguration(writer, designConfiguration);

    writeInterConnectionConfiguration(writer, designConfiguration);

    writeViewConfigurations(writer, designConfiguration);

    writeParameters(writer, designConfiguration);

    writeAssertions(writer, designConfiguration);

    writeVendorExtensions(writer, designConfiguration);

    writer.writeEndElement(); // spirit:designConfiguration
    writer.writeEndDocument();
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeDesignConfigurationStart()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeDesignConfigurationStart(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    writer.writeStartDocument();

    foreach (QString comment, designConfiguration->getTopComments())
    {
        writer.writeComment(comment);
    }

    writer.writeStartElement("ipxact:designConfiguration");
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeXMLNameSpace()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeXMLNameSpace(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    QMap<QString, QString> designConfigAttributes = designConfiguration->getAttributes();

    designConfigAttributes.insert("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    designConfigAttributes.insert("xmlns:ipxact", "http://www.accellera.org/XMLSchema/IPXACT/1685-2014");
    designConfigAttributes.insert("xsi:schemaLocation", "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd");
    designConfigAttributes.insert("xmlns:kactus2", "http://funbase.cs.tut.fi/");

    writeAttributes(writer, designConfigAttributes);

    designConfiguration->setAttributes(designConfigAttributes);
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeAttributes()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeAttributes(QXmlStreamWriter& writer, QMap<QString, QString> attributes) const
{
    for (QMap<QString, QString>::const_iterator attributeIndex = attributes.begin();
        attributeIndex != attributes.end(); ++attributeIndex)
    {
        writer.writeAttribute(attributeIndex.key(), attributeIndex.value());
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeVLNVasElements()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeVLNVasElements(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    VLNV* designConfigurationVLNV = designConfiguration->getVlnv();

    writer.writeTextElement(IPXACT_VENDOR, designConfigurationVLNV->getVendor());
    writer.writeTextElement(IPXACT_LIBRARY, designConfigurationVLNV->getLibrary());
    writer.writeTextElement(IPXACT_NAME, designConfigurationVLNV->getName());
    writer.writeTextElement(IPXACT_VERSION, designConfigurationVLNV->getVersion());
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeDescription()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeDescription(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    if (!designConfiguration->getDescription().isEmpty())
    {
        writer.writeTextElement("ipxact:description", designConfiguration->getDescription());
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeDesignReference()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeDesignReference(QXmlStreamWriter& writer, VLNV& designReference) const
{
    if (designReference.isValid())
    {
        writer.writeEmptyElement("ipxact:designRef");

        writeVLNVAsAttributes(writer, designReference);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeVLNVAsAttributes()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeVLNVAsAttributes(QXmlStreamWriter& writer, VLNV& itemVLNV) const
{
    writer.writeAttribute(PLAIN_VENDOR, itemVLNV.getVendor());
    writer.writeAttribute(PLAIN_LIBRARY, itemVLNV.getLibrary());
    writer.writeAttribute(PLAIN_NAME, itemVLNV.getName());
    writer.writeAttribute(PLAIN_VERSION, itemVLNV.getVersion());
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeGeneratorChainConfiguration()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeGeneratorChainConfiguration(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    foreach (QSharedPointer<ConfigurableVLNVReference> currentChain,designConfiguration->getGeneratorChainConfs())
    {
        writer.writeStartElement("ipxact:generatorChainConfiguration");

        VLNV currentChainVLNV = *currentChain;
        writeVLNVAsAttributes(writer, currentChainVLNV);

        writeConfigurableElementValues(writer, *currentChain->getConfigurableElementValues());

        writer.writeEndElement(); // ipxact:generatorChainConfiguration
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeConfigurableElementValues()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeConfigurableElementValues(QXmlStreamWriter& writer,
    QList<QSharedPointer<ConfigurableElementValue> > configurables) const
{
    if (!configurables.isEmpty())
    {
        writer.writeStartElement("ipxact:configurableElementValues");

        foreach (QSharedPointer<ConfigurableElementValue> configurableElement, configurables)
        {
            writer.writeStartElement("ipxact:configurableElementValue");
            writer.writeAttribute("referenceId", configurableElement->getReferenceId());
            writer.writeCharacters(configurableElement->getConfigurableValue());
            writer.writeEndElement(); // ipxact:configurableElementValue
        }
        writer.writeEndElement(); // ipxact:configurableElementValues
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeInterConnectionConfiguration()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeInterConnectionConfiguration(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    foreach (QSharedPointer<InterconnectionConfiguration> configuration,
        designConfiguration->getInterconnectionConfs())
    {
        writer.writeStartElement("ipxact:interconnectionConfiguration");

        if (!configuration->getIsPresent().isEmpty())
        {
            writer.writeTextElement("ipxact:isPresent", configuration->getIsPresent());
        }

        writer.writeTextElement("ipxact:interconnectionRef", configuration->getInterconnectionReference());

        writeMultipleAbstractorInstances(writer, configuration->getAbstractorInstances());

        writer.writeEndElement(); // ipxact:interconnectionConfiguration
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeMultipleAbstractorInstances()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeMultipleAbstractorInstances(QXmlStreamWriter& writer,
    QList<QSharedPointer<MultipleAbstractorInstances> > multipleAbstractors) const
{
    foreach (QSharedPointer<MultipleAbstractorInstances> multipleAbstractor, multipleAbstractors)
    {
        writer.writeStartElement("ipxact:abstractorInstances");

        if (!multipleAbstractor->getIsPresent().isEmpty())
        {
            writer.writeTextElement("ipxact:isPresent", multipleAbstractor->getIsPresent());
        }

        writeInterfaceReferences(writer, multipleAbstractor->getInterfaceReferences());
        writeAbstractorInstances(writer, multipleAbstractor->getAbstractorInstances());

        writer.writeEndElement(); // ipxact:abstractorInstances
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeInterfaceReferences()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeInterfaceReferences(QXmlStreamWriter& writer,
    QList<QSharedPointer<InterfaceRef> > interfaceReferences) const
{
    foreach (QSharedPointer<InterfaceRef> singleInterface, interfaceReferences)
    {
        writer.writeStartElement("ipxact:interfaceRef");
        
        writer.writeAttribute("componentRef", singleInterface->getComponentRef());
        writer.writeAttribute("busRef", singleInterface->getBusRef());

        if (!singleInterface->getIsPresent().isEmpty())
        {
            writer.writeTextElement("ipxact:isPresent", singleInterface->getIsPresent());
        }

        writer.writeEndElement(); // ipxact:interfaceRef
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeAbstractorInstances()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeAbstractorInstances(QXmlStreamWriter& writer,
    QList<QSharedPointer<AbstractorInstance> > abstractorInstances) const
{
    foreach (QSharedPointer<AbstractorInstance> abstractorInstance, abstractorInstances)
    {
        writer.writeStartElement("ipxact:abstractorInstance");

        writer.writeTextElement("ipxact:instanceName", abstractorInstance->getInstanceName());

        if (!abstractorInstance->getDisplayName().isEmpty())
        {
            writer.writeTextElement("ipxact:displayName", abstractorInstance->getDisplayName());
        }
        if (!abstractorInstance->getDescription().isEmpty())
        {
            writer.writeTextElement("ipxact:description", abstractorInstance->getDescription());
        }

        writer.writeStartElement("ipxact:abstractorRef");

        QSharedPointer<ConfigurableVLNVReference> abstractorRef = abstractorInstance->getAbstractorRef();  

        VLNV abstractorRefVLNV = *abstractorRef;
        writeVLNVAsAttributes(writer, abstractorRefVLNV);

        writeConfigurableElementValues(writer, *abstractorRef->getConfigurableElementValues());

        writer.writeEndElement(); // ipxact:abstractorRef

        writer.writeTextElement("ipxact:viewName", abstractorInstance->getViewName());

        writer.writeEndElement(); // ipxact:abstractorInstance
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeViewConfigurations()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeViewConfigurations(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    foreach (QSharedPointer<ViewConfiguration> configuration, designConfiguration->getViewConfigurations())
    {
        writer.writeStartElement("ipxact:viewConfiguration");

        writer.writeTextElement("ipxact:instanceName", configuration->getInstanceName());

        if (!configuration->getIsPresent().isEmpty())
        {
            writer.writeTextElement("ipxact:isPresent", configuration->getIsPresent());
        }

        writer.writeStartElement("ipxact:view");
        writer.writeAttribute("viewRef", configuration->getViewReference());

        writeConfigurableElementValues(writer, *configuration->getViewConfigurableElements());

        writer.writeEndElement(); // ipxact:view

        writer.writeEndElement(); // ipxact:viewConfiguration
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeParameters()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeParameters(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    if (!designConfiguration->getParameters()->isEmpty())
    {
        writer.writeStartElement("ipxact:parameters");

        ParameterWriter parameterWriter;

        foreach (QSharedPointer<Parameter> currentParameter, *designConfiguration->getParameters())
        {
            parameterWriter.writeParameter(writer, currentParameter);
        }

        writer.writeEndElement(); // ipxact:parameters
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeAssertions()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeAssertions(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    if (!designConfiguration->getAssertions()->isEmpty())
    {
        writer.writeStartElement("ipxact:assertions");

        foreach (QSharedPointer<Assertion> assertion, *designConfiguration->getAssertions())
        {
            writer.writeStartElement("ipxact:assertion");

            NameGroupWriter nameGroupWriter;
            nameGroupWriter.writeNameGroup(writer, assertion);

            writer.writeTextElement("ipxact:assert", assertion->getAssert());

            writer.writeEndElement(); // ipxact:assertion
        }

        writer.writeEndElement(); // ipxact:assertions
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeVendorExtensions()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeVendorExtensions(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    if (!designConfiguration->getVendorExtensions().isEmpty())
    {
        writer.writeStartElement("ipxact:vendorExtensions");

        foreach (QSharedPointer<VendorExtension> extension, designConfiguration->getVendorExtensions())
        {
            extension->write(writer);
        }

        writer.writeEndElement(); // ipxact:vendorExtensions
    }
}
