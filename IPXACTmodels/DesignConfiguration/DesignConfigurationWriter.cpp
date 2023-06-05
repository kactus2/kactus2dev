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
// Function: DesignConfigurationWriter::writeDesignConfiguration()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeDesignConfiguration(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration)
{
    writer.writeStartDocument();

    Details::writeDesignConfigurationStart(writer, designConfiguration);

    DocumentWriter::writeNamespaceDeclarations(writer, designConfiguration);

    DocumentWriter::writeDocumentNameGroup(writer, designConfiguration);

    Details::writeDesignReference(writer, designConfiguration->getDesignRef());

    Details::writeGeneratorChainConfiguration(writer, designConfiguration);

    Details::writeInterConnectionConfiguration(writer, designConfiguration);

    Details::writeViewConfigurations(writer, designConfiguration);

    if (designConfiguration->getRevision() == Document::Revision::Std14)
    {
        CommonItemsWriter::writeDescription(writer, designConfiguration->getDescription());
    }

    DocumentWriter::writeParameters(writer, designConfiguration);

    DocumentWriter::writeAssertions(writer, designConfiguration);

    CommonItemsWriter::writeVendorExtensions(writer, designConfiguration);

    writer.writeEndElement(); // ipxact:designConfiguration
    writer.writeEndDocument();
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeDesignConfigurationStart()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::Details::writeDesignConfigurationStart(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration)
{
    DocumentWriter::writeTopComments(writer, designConfiguration);
    DocumentWriter::writeXmlProcessingInstructions(writer, designConfiguration);
    writer.writeStartElement(QStringLiteral("ipxact:designConfiguration"));
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeDesignReference()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::Details::writeDesignReference(QXmlStreamWriter& writer, VLNV const& designReference)
{
    if (designReference.isValid())
    {
        writer.writeEmptyElement(QStringLiteral("ipxact:designRef"));

        CommonItemsWriter::writeVLNVAttributes(writer, designReference);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeGeneratorChainConfiguration()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::Details::writeGeneratorChainConfiguration(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration)
{
    for (auto const& currentChain : *designConfiguration->getGeneratorChainConfs())
    {
        writer.writeStartElement(QStringLiteral("ipxact:generatorChainConfiguration"));

        CommonItemsWriter::writeVLNVAttributes(writer, *currentChain);

        CommonItemsWriter::writeConfigurableElementValues(writer, currentChain->getConfigurableElementValues());

        writer.writeEndElement(); // ipxact:generatorChainConfiguration
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeInterConnectionConfiguration()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::Details::writeInterConnectionConfiguration(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration)
{
    auto docRevision = designConfiguration->getRevision();

    for (auto const& configuration : *designConfiguration->getInterconnectionConfs())
    {
        writer.writeStartElement(QStringLiteral("ipxact:interconnectionConfiguration"));

        if (docRevision == Document::Revision::Std14)
        {
            CommonItemsWriter::writeIsPresent(writer, configuration->getIsPresent());
        }

        writer.writeTextElement(QStringLiteral("ipxact:interconnectionRef"), configuration->getInterconnectionReference());

        writeMultipleAbstractorInstances(writer, configuration->getAbstractorInstances(), docRevision);

        if (docRevision == Document::Revision::Std22)
        {
            CommonItemsWriter::writeVendorExtensions(writer, configuration);
        }

        writer.writeEndElement(); // ipxact:interconnectionConfiguration
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeMultipleAbstractorInstances()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::Details::writeMultipleAbstractorInstances(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<MultipleAbstractorInstances> > > multipleAbstractors,
    Document::Revision docRevision)
{
    for (QSharedPointer<MultipleAbstractorInstances> multipleAbstractor : *multipleAbstractors)
    {
        writer.writeStartElement(QStringLiteral("ipxact:abstractorInstances"));

        if (docRevision == Document::Revision::Std14)
        {
            CommonItemsWriter::writeIsPresent(writer, multipleAbstractor->getIsPresent());
        }

        writeInterfaceReferences(writer, multipleAbstractor->getInterfaceReferences(), docRevision);
        writeAbstractorInstances(writer, multipleAbstractor->getAbstractorInstances(), docRevision);

        writer.writeEndElement(); // ipxact:abstractorInstances
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeInterfaceReferences()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::Details::writeInterfaceReferences(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<InterfaceRef> > > interfaceReferences,
    Document::Revision docRevision)
{
    for (QSharedPointer<InterfaceRef> singleInterface : *interfaceReferences)
    {
        writer.writeStartElement(QStringLiteral("ipxact:interfaceRef"));
        
        writer.writeAttribute(QStringLiteral("componentRef"), singleInterface->getComponentRef());
        writer.writeAttribute(QStringLiteral("busRef"), singleInterface->getBusRef());

        if (docRevision == Document::Revision::Std14)
        {
            CommonItemsWriter::writeIsPresent(writer, singleInterface->getIsPresent());
        }
        
        if (docRevision == Document::Revision::Std22)
        {
            CommonItemsWriter::writeVendorExtensions(writer, singleInterface);
        }

        writer.writeEndElement(); // ipxact:interfaceRef
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeAbstractorInstances()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::Details::writeAbstractorInstances(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<AbstractorInstance> > > abstractorInstances,
    Document::Revision docRevision)
{
    for (QSharedPointer<AbstractorInstance> abstractorInstance : *abstractorInstances)
    {
        writer.writeStartElement(QStringLiteral("ipxact:abstractorInstance"));

        writer.writeTextElement(QStringLiteral("ipxact:instanceName"), abstractorInstance->getInstanceName());

        CommonItemsWriter::writeDisplayName(writer, abstractorInstance->getDisplayName());

        if (docRevision == Document::Revision::Std22)
        {
            CommonItemsWriter::writeShortDescription(writer, abstractorInstance->getShortDescription());
        }

        CommonItemsWriter::writeDescription(writer, abstractorInstance->getDescription());

        writer.writeStartElement(QStringLiteral("ipxact:abstractorRef"));

        QSharedPointer<ConfigurableVLNVReference> abstractorRef = abstractorInstance->getAbstractorRef();  

        CommonItemsWriter::writeVLNVAttributes(writer, *abstractorRef);

        CommonItemsWriter::writeConfigurableElementValues(writer, abstractorRef->getConfigurableElementValues());

        writer.writeEndElement(); // ipxact:abstractorRef

        writer.writeTextElement(QStringLiteral("ipxact:viewName"), abstractorInstance->getViewName());

        if (docRevision == Document::Revision::Std22)
        {
            CommonItemsWriter::writeVendorExtensions(writer, abstractorInstance);
        }

        writer.writeEndElement(); // ipxact:abstractorInstance
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeViewConfigurations()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::Details::writeViewConfigurations(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration)
{
    auto docRevision = designConfiguration->getRevision();

    for (QSharedPointer<ViewConfiguration> configuration : *designConfiguration->getViewConfigurations())
    {
        writer.writeStartElement(QStringLiteral("ipxact:viewConfiguration"));

        writer.writeTextElement(QStringLiteral("ipxact:instanceName"), configuration->getInstanceName());

        if (docRevision == Document::Revision::Std14)
        {
            CommonItemsWriter::writeIsPresent(writer, configuration->getIsPresent());
        }

        writer.writeStartElement(QStringLiteral("ipxact:view"));
        writer.writeAttribute(QStringLiteral("viewRef"), configuration->getViewReference());

        CommonItemsWriter::writeConfigurableElementValues(writer, configuration->getViewConfigurableElements());

        writer.writeEndElement(); // ipxact:view

        if (docRevision == Document::Revision::Std22)
        {
            CommonItemsWriter::writeVendorExtensions(writer, configuration);
        }

        writer.writeEndElement(); // ipxact:viewConfiguration
    }
}
