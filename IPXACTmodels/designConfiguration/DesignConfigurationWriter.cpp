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
#include <IPXACTmodels/designConfiguration/InterconnectionConfiguration.h>

#include <QMap>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::DesignConfigurationWriter()
//-----------------------------------------------------------------------------
DesignConfigurationWriter::DesignConfigurationWriter(QObject* parent):
DocumentWriter(parent)
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
    writer.writeStartDocument();

    writeDesignConfigurationStart(writer, designConfiguration);

    writeNamespaceDeclarations(writer);

    writeVLNVElements(writer, designConfiguration->getVlnv());

    writeDescription(writer, designConfiguration);

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
    writer.writeStartElement("ipxact:designConfiguration");
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeDesignReference()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeDesignReference(QXmlStreamWriter& writer, VLNV const& designReference) const
{
    if (designReference.isValid())
    {
        writer.writeEmptyElement("ipxact:designRef");

        writeVLNVAttributes(writer, designReference);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationWriter::writeGeneratorChainConfiguration()
//-----------------------------------------------------------------------------
void DesignConfigurationWriter::writeGeneratorChainConfiguration(QXmlStreamWriter& writer,
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    foreach (QSharedPointer<ConfigurableVLNVReference> currentChain, *designConfiguration->getGeneratorChainConfs())
    {
        writer.writeStartElement("ipxact:generatorChainConfiguration");

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
    foreach (QSharedPointer<InterconnectionConfiguration> configuration,
        *designConfiguration->getInterconnectionConfs())
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
    QSharedPointer<QList<QSharedPointer<MultipleAbstractorInstances> > > multipleAbstractors) const
{
    foreach (QSharedPointer<MultipleAbstractorInstances> multipleAbstractor, *multipleAbstractors)
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
    QSharedPointer<QList<QSharedPointer<InterfaceRef> > > interfaceReferences) const
{
    foreach (QSharedPointer<InterfaceRef> singleInterface, *interfaceReferences)
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
    QSharedPointer<QList<QSharedPointer<AbstractorInstance> > > abstractorInstances) const
{
    foreach (QSharedPointer<AbstractorInstance> abstractorInstance, *abstractorInstances)
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
        writeVLNVAttributes(writer, abstractorRefVLNV);

        writeConfigurableElementValues(writer, abstractorRef->getConfigurableElementValues());

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
    foreach (QSharedPointer<ViewConfiguration> configuration, *designConfiguration->getViewConfigurations())
    {
        writer.writeStartElement("ipxact:viewConfiguration");

        writer.writeTextElement("ipxact:instanceName", configuration->getInstanceName());

        if (!configuration->getIsPresent().isEmpty())
        {
            writer.writeTextElement("ipxact:isPresent", configuration->getIsPresent());
        }

        writer.writeStartElement("ipxact:view");
        writer.writeAttribute("viewRef", configuration->getViewReference());

        writeConfigurableElementValues(writer, configuration->getViewConfigurableElements());

        writer.writeEndElement(); // ipxact:view

        writer.writeEndElement(); // ipxact:viewConfiguration
    }
}
