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

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/DesignConfiguration/InterconnectionConfiguration.h>

#include <QMap>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::DesignConfigurationWriter()
//-----------------------------------------------------------------------------
DesignConfigurationWriter::DesignConfigurationWriter(): DocumentWriter()
{

}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeDesignConfiguration()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeDesignConfiguration(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    writer.writeStartDocument();

    writeDesignConfigurationStart(writer, designConfiguration);

    writeNamespaceDeclarations(writer, designConfiguration);

    writeDocumentNameGroup(writer, designConfiguration);

    writeDesignReference(writer, designConfiguration->getDesignRef());

    writeGeneratorChainConfiguration(writer, designConfiguration);

    writeInterConnectionConfiguration(writer, designConfiguration);

    writeViewConfigurations(writer, designConfiguration);

    writeParameters(writer, designConfiguration);

    writeAssertions(writer, designConfiguration);

    writeVendorExtensions(writer, designConfiguration);

    writer.writeEndElement(); // ipxact:designConfiguration
    writer.writeEndDocument();
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeDesignConfigurationStart()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeDesignConfigurationStart(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    writeTopComments(writer, designConfiguration);
    writeXmlProcessingInstructions(writer, designConfiguration);
    writer.writeStartElement(QStringLiteral("ipxact:designConfiguration"));
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeDesignReference()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeDesignReference(QXmlStreamWriter& writer, VLNV const& designReference) const
{
    if (designReference.isValid())
    {
        writer.writeEmptyElement(QStringLiteral("ipxact:designRef"));

        writeVLNVAttributes(writer, designReference);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeGeneratorChainConfiguration()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeGeneratorChainConfiguration(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    for (auto const& currentChain : *designConfiguration->getGeneratorChainConfs())
    {
        writer.writeStartElement(QStringLiteral("ipxact:generatorChainConfiguration"));

        VLNV currentChainVLNV = *currentChain;
        writeVLNVAttributes(writer, currentChainVLNV);

        writeConfigurableElementValues(writer, currentChain->getConfigurableElementValues());

        writer.writeEndElement(); // ipxact:generatorChainConfiguration
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeInterConnectionConfiguration()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeInterConnectionConfiguration(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    auto docRevision = designConfiguration->getRevision();

    for (auto const& configuration : *designConfiguration->getInterconnectionConfs())
    {
        writer.writeStartElement(QStringLiteral("ipxact:interconnectionConfiguration"));

        if (docRevision == Document::Revision::Std14 && !configuration->getIsPresent().isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:isPresent"), configuration->getIsPresent());
        }

        writer.writeTextElement(QStringLiteral("ipxact:interconnectionRef"), configuration->getInterconnectionReference());

        writeMultipleAbstractorInstances(writer, configuration->getAbstractorInstances(), docRevision);

        if (docRevision == Document::Revision::Std22)
        {
            writeVendorExtensions(writer, configuration);
        }

        writer.writeEndElement(); // ipxact:interconnectionConfiguration
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeMultipleAbstractorInstances()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeMultipleAbstractorInstances(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<MultipleAbstractorInstances> > > multipleAbstractors,
    Document::Revision docRevision) const
{
    for (QSharedPointer<MultipleAbstractorInstances> multipleAbstractor : *multipleAbstractors)
    {
        writer.writeStartElement(QStringLiteral("ipxact:abstractorInstances"));

        if (docRevision == Document::Revision::Std14 && !multipleAbstractor->getIsPresent().isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:isPresent"), multipleAbstractor->getIsPresent());
        }

        writeInterfaceReferences(writer, multipleAbstractor->getInterfaceReferences(), docRevision);
        writeAbstractorInstances(writer, multipleAbstractor->getAbstractorInstances(), docRevision);

        writer.writeEndElement(); // ipxact:abstractorInstances
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeInterfaceReferences()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeInterfaceReferences(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<InterfaceRef> > > interfaceReferences,
    Document::Revision docRevision) const
{
    for (QSharedPointer<InterfaceRef> singleInterface : *interfaceReferences)
    {
        writer.writeStartElement(QStringLiteral("ipxact:interfaceRef"));
        
        writer.writeAttribute(QStringLiteral("componentRef"), singleInterface->getComponentRef());
        writer.writeAttribute(QStringLiteral("busRef"), singleInterface->getBusRef());

        if (docRevision == Document::Revision::Std14 && !singleInterface->getIsPresent().isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:isPresent"), singleInterface->getIsPresent());
        }
        
        if (docRevision == Document::Revision::Std22)
        {
            writeVendorExtensions(writer, singleInterface);
        }

        writer.writeEndElement(); // ipxact:interfaceRef
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeAbstractorInstances()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeAbstractorInstances(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<AbstractorInstance> > > abstractorInstances,
    Document::Revision docRevision) const
{
    for (QSharedPointer<AbstractorInstance> abstractorInstance : *abstractorInstances)
    {
        writer.writeStartElement(QStringLiteral("ipxact:abstractorInstance"));

        writer.writeTextElement(QStringLiteral("ipxact:instanceName"), abstractorInstance->getInstanceName());

        if (!abstractorInstance->getDisplayName().isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:displayName"), abstractorInstance->getDisplayName());
        }

        if (docRevision == Document::Revision::Std22 && !abstractorInstance->getShortDescription().isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:shortDescription"), abstractorInstance->getShortDescription());
        }

        if (!abstractorInstance->getDescription().isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:description"), abstractorInstance->getDescription());
        }

        writer.writeStartElement(QStringLiteral("ipxact:abstractorRef"));

        QSharedPointer<ConfigurableVLNVReference> abstractorRef = abstractorInstance->getAbstractorRef();  

        writeVLNVAttributes(writer, *abstractorRef);

        writeConfigurableElementValues(writer, abstractorRef->getConfigurableElementValues());

        writer.writeEndElement(); // ipxact:abstractorRef

        writer.writeTextElement(QStringLiteral("ipxact:viewName"), abstractorInstance->getViewName());

        if (docRevision == Document::Revision::Std22)
        {
            writeVendorExtensions(writer, abstractorInstance);
        }

        writer.writeEndElement(); // ipxact:abstractorInstance
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeViewConfigurations()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeViewConfigurations(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    auto docRevision = designConfiguration->getRevision();

    for (QSharedPointer<ViewConfiguration> configuration : *designConfiguration->getViewConfigurations())
    {
        writer.writeStartElement(QStringLiteral("ipxact:viewConfiguration"));

        writer.writeTextElement(QStringLiteral("ipxact:instanceName"), configuration->getInstanceName());

        if (docRevision == Document::Revision::Std14 &&
            !configuration->getIsPresent().isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:isPresent"), configuration->getIsPresent());
        }

        writer.writeStartElement(QStringLiteral("ipxact:view"));
        writer.writeAttribute(QStringLiteral("viewRef"), configuration->getViewReference());

        writeConfigurableElementValues(writer, configuration->getViewConfigurableElements());

        writer.writeEndElement(); // ipxact:view

        if (docRevision == Document::Revision::Std22)
        {
            writeVendorExtensions(writer, configuration);
        }

        writer.writeEndElement(); // ipxact:viewConfiguration
    }
}
