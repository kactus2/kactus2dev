//-----------------------------------------------------------------------------
// File: VerilogWriterFactory.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 26.01.2017
//
// Description:
// Creates writes for generating Verilog.
//-----------------------------------------------------------------------------

#include "VerilogWriterFactory.h"

#include <library/LibraryManager/libraryinterface.h>

#include <Plugins/VerilogGenerator/common/WriterGroup.h>
#include <Plugins/VerilogGenerator/CommentWriter/CommentWriter.h>
#include <Plugins/VerilogGenerator/ComponentVerilogWriter/ComponentVerilogWriter.h>
#include <Plugins/VerilogGenerator/ComponentInstanceVerilogWriter/ComponentInstanceVerilogWriter.h>
#include <Plugins/common/PortSorter/InterfaceDirectionNameSorter.h>
#include <Plugins/VerilogGenerator/VerilogHeaderWriter/VerilogHeaderWriter.h>
#include <Plugins/VerilogGenerator/VerilogWireWriter/VerilogWireWriter.h>
#include <Plugins/VerilogGenerator/VerilogAssignmentWriter/VerilogAssignmentWriter.h>
#include <Plugins/VerilogGenerator/PortVerilogWriter/VerilogTopDefaultWriter.h>
#include <Plugins/VerilogGenerator/VerilogInterconnectionWriter/VerilogInterconnectionWriter.h>

#include <Plugins/VerilogImport/VerilogSyntax.h>
#include <Plugins/common/HDLParser/MetaDesign.h>

#include <Plugins/PluginSystem/GeneratorPlugin/FileOutput.h>
#include <Plugins/PluginSystem/GeneratorPlugin/GeneratorConfiguration.h>

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::VerilogWriterFactory()
//-----------------------------------------------------------------------------
VerilogWriterFactory::VerilogWriterFactory(LibraryInterface* library, GenerationSettings* settings,
    QString const& kactusVersion, QString const& generatorVersion) :
library_(library),
settings_(settings),
kactusVersion_(kactusVersion),
generatorVersion_(generatorVersion),
sorter_(new InterfaceDirectionNameSorter())
{
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::~VerilogWriterFactory()
//-----------------------------------------------------------------------------
VerilogWriterFactory::~VerilogWriterFactory()
{
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::prepareComponent()
//-----------------------------------------------------------------------------
QSharedPointer<GenerationFile> VerilogWriterFactory::prepareComponent(QString const& outputPath,
    QSharedPointer<HDLComponentParser> component)
{
    // If we are not generating based on a design, we must parse the existing implementation.
    QString implementation;
    QString postModule;
    QString fileName = component->moduleName_ + ".v";
    QString filePath = outputPath + "/" + fileName;
    QString error;

    if (!VerilogSyntax::readImplementation(filePath, implementation, postModule, error))
    {
        //emit reportError(error); TODO: error
        // If parser says no-go, we dare do nothing.
        return QSharedPointer<VerilogDocument>::QSharedPointer();
    }

    QSharedPointer<VerilogDocument> document = initializeComponentWriters(component);
    document->fileName_ = fileName;
    document->vlnv_ = component->component_->getVlnv().toString();

    // Next comes the implementation.
    QSharedPointer<TextBodyWriter> implementationWriter(new TextBodyWriter(implementation));
    document->topWriter_->setImplementation(implementationWriter);

    // Also write any stuff that comes after the actual module.
    QSharedPointer<TextBodyWriter> postModuleWriter(new TextBodyWriter(postModule));
    document->topWriter_->setPostModule(postModuleWriter);

    return document;
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::prepareDesign()
//-----------------------------------------------------------------------------
QSharedPointer<GenerationFile> VerilogWriterFactory::prepareDesign(QSharedPointer<MetaDesign> design)
{
    QSharedPointer<VerilogDocument> document = initializeComponentWriters(design->topInstance_);
    document->fileName_ = design->topInstance_->moduleName_ + ".v";
    document->vlnv_ = design->topInstance_->component_->getVlnv().toString();

    initializeDesignWriters(design, document);

    // Finally, add them to the top writer in desired order.
    addWritersToTopInDesiredOrder(document);

    return document;
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::getLanguage()
//-----------------------------------------------------------------------------
QString VerilogWriterFactory::getLanguage() const
{
    return "Verilog";
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::initializeComponentWriters()
//-----------------------------------------------------------------------------
QSharedPointer<VerilogDocument> VerilogWriterFactory::initializeComponentWriters(QSharedPointer<MetaInstance> topComponent)
{
    QSettings settings;
    QString currentUser = settings.value("General/Username").toString();
    QString componentXmlPath = library_->getPath(topComponent->component_->getVlnv());

    QSharedPointer<VerilogDocument> retval(new VerilogDocument);

    retval->headerWriter_ = QSharedPointer<VerilogHeaderWriter>(new VerilogHeaderWriter(topComponent->component_->getVlnv(), 
        componentXmlPath, currentUser, topComponent->component_->getDescription(), kactusVersion_, generatorVersion_));

    retval->topWriter_ = QSharedPointer<ComponentVerilogWriter>(new ComponentVerilogWriter
        (topComponent, settings_->generateInterfaces_));

    retval->instanceWriters_.clear();

    retval->interconnectionWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    retval->connectionWireWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    retval->adHocWireWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    retval->portWireWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    retval->topAssignmentWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    retval->topDefaultWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    retval->instanceAssignmentWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    return retval;
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::initializeDesignWriters()
//-----------------------------------------------------------------------------
void VerilogWriterFactory::initializeDesignWriters(QSharedPointer<MetaDesign> design, QSharedPointer<VerilogDocument> document)
{
    if (design->topInstance_->ports_.size() > 0)
    {
        QSharedPointer<CommentWriter> topAssignmentHeaderWriter(
            new CommentWriter("Assignments for the ports of the encompassing component:"));
        topAssignmentHeaderWriter->setIndent(4);
        document->topAssignmentWriters_->add(topAssignmentHeaderWriter);
    }

    // Create writers for 
    foreach (QSharedPointer<MetaPort> mPort, design->topInstance_->ports_)
    {
        if (mPort->downAssignments_.size() > 0)
        {
            foreach (QSharedPointer<MetaPortAssignment> mpa, mPort->downAssignments_)
            {
                QSharedPointer<VerilogAssignmentWriter> topAssignment = QSharedPointer<VerilogAssignmentWriter>
                    (new VerilogAssignmentWriter(mPort->port_->name(), mpa, mPort->port_->getDirection(), true));
                document->topAssignmentWriters_->add(topAssignment);
            }
        }
        else
        {
            QSharedPointer<VerilogTopDefaultWriter> topDefault = QSharedPointer<VerilogTopDefaultWriter>
                (new VerilogTopDefaultWriter(mPort));
            document->topDefaultWriters_->add(topDefault);
        }
    }

    // Create instance writers for the instances, complete with expression parsers and formatters.
    foreach(QSharedPointer<MetaInstance> mInstance, design->instances_)
    {
        QSharedPointer<ComponentInstance> instance = mInstance->componentInstance_;

        QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter(new ComponentInstanceVerilogWriter(
            mInstance, sorter_, settings_->generateInterfaces_));

        document->instanceWriters_.append(instanceWriter);

        document->instanceHeaderWriters_.insert(instanceWriter, createHeaderWriterForInstance(mInstance));

        if (mInstance->ports_.size() > 0)
        {
            QSharedPointer<CommentWriter> portWireHeaderWriter(
                new CommentWriter(mInstance->componentInstance_->getInstanceName() + " port wires:"));
            portWireHeaderWriter->setIndent(4);
            document->portWireWriters_->add(portWireHeaderWriter);

            QSharedPointer<CommentWriter> assignmentHeaderWriter(
                new CommentWriter(mInstance->componentInstance_->getInstanceName() + " assignments:"));
            assignmentHeaderWriter->setIndent(4);
            document->instanceAssignmentWriters_->add(assignmentHeaderWriter);
        }

        foreach (QSharedPointer<MetaPort> mPort, mInstance->ports_)
        {
            if (mPort->upAssignments_.size() < 1)
            {
                continue;
            }

            QString physName = mInstance->componentInstance_->getInstanceName() + "_" +
                mPort->port_->name();

            document->portWireWriters_->add(QSharedPointer<VerilogWireWriter>(
                new VerilogWireWriter(physName, mPort->vectorBounds_)));

            foreach (QSharedPointer<MetaPortAssignment> mpa, mPort->upAssignments_)
            {
                QSharedPointer<VerilogAssignmentWriter> instanceAssignment = QSharedPointer<VerilogAssignmentWriter>
                    (new VerilogAssignmentWriter(physName, mpa, mPort->port_->getDirection(), false));
                document->instanceAssignmentWriters_->add(instanceAssignment);
            }
        }
    }

    // Create wire writers for the interconnections
    foreach (QSharedPointer<MetaInterconnection> mInterconnect, design->interconnections_)
    {
        if (mInterconnect->wires_.size() > 0)
        {
            QSharedPointer<CommentWriter> connectionWireHeaderWriter(
                new CommentWriter(mInterconnect->name_ + " wires:"));
            connectionWireHeaderWriter->setIndent(4);
            document->connectionWireWriters_->add(connectionWireHeaderWriter);
        }

        foreach (QSharedPointer<MetaWire> gw, mInterconnect->wires_)
        {
            document->connectionWireWriters_->add(QSharedPointer<VerilogWireWriter>(new VerilogWireWriter(gw->name_, gw->bounds_)));
        }
    }

    if (design->adHocWires_.size() > 0)
    {
        QSharedPointer<CommentWriter> adHocWireHeaderWriter(
            new CommentWriter("Ad-hoc wires:"));
        adHocWireHeaderWriter->setIndent(4);
       document->adHocWireWriters_->add(adHocWireHeaderWriter);
    }

    // Create wire writers for the ad hoc connections as well.
    foreach (QSharedPointer<MetaWire> adHoc, design->adHocWires_)
    {
        document->adHocWireWriters_->add(QSharedPointer<VerilogWireWriter>(new VerilogWireWriter(adHoc->name_, adHoc->bounds_)));
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::addWritersToTopInDesiredOrder()
//-----------------------------------------------------------------------------
void VerilogWriterFactory::addWritersToTopInDesiredOrder(QSharedPointer<VerilogDocument> document) const
{
    document->topWriter_->add(document->interconnectionWriters_);

    document->topWriter_->add(document->connectionWireWriters_);

    document->topWriter_->add(document->adHocWireWriters_);

    document->topWriter_->add(document->portWireWriters_);

    document->topWriter_->add(document->topAssignmentWriters_);

    document->topWriter_->add(document->topDefaultWriters_);

    document->topWriter_->add(document->instanceAssignmentWriters_);

    foreach(QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter, document->instanceWriters_)
    {
        QSharedPointer<Writer> headerWriter = document->instanceHeaderWriters_[instanceWriter];

        QSharedPointer<WriterGroup> instanceGroup(new WriterGroup);
        instanceGroup->add(headerWriter);
        instanceGroup->add(instanceWriter);

        document->topWriter_->add(instanceGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::createHeaderWriterForInstance()
//-----------------------------------------------------------------------------
QSharedPointer<Writer> VerilogWriterFactory::createHeaderWriterForInstance(QSharedPointer<MetaInstance> instance) const
{
    QString header = instance->componentInstance_->getDescription();
    if (!header.isEmpty())
    {
        header.append("\n");
    }

    header.append("IP-XACT VLNV: " + instance->component_->getVlnv().toString());

    QSharedPointer<CommentWriter> headerWriter(new CommentWriter(header));
    headerWriter->setIndent(4);

    return headerWriter;
}