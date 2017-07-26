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

#include <library/LibraryInterface.h>

#include <Plugins/VerilogGenerator/CommentWriter/CommentWriter.h>
#include <Plugins/common/PortSorter/InterfaceDirectionNameSorter.h>
#include <Plugins/VerilogGenerator/VerilogWireWriter/VerilogWireWriter.h>
#include <Plugins/VerilogGenerator/VerilogAssignmentWriter/VerilogAssignmentWriter.h>
#include <Plugins/VerilogGenerator/PortVerilogWriter/VerilogTopDefaultWriter.h>

#include <Plugins/VerilogImport/VerilogSyntax.h>

#include <Plugins/PluginSystem/GeneratorPlugin/GenerationControl.h>

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::VerilogWriterFactory()
//-----------------------------------------------------------------------------
VerilogWriterFactory::VerilogWriterFactory(LibraryInterface* library,
    MessagePasser* messages, GenerationSettings* settings,
    QString const& kactusVersion, QString const& generatorVersion) :
    library_(library),
    messages_(messages),
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
QSharedPointer<GenerationOutput> VerilogWriterFactory::prepareComponent(QString const& outputPath,
    QSharedPointer<MetaComponent> component)
{
    // If we are not generating based on a design, we must parse the existing implementation.
    QString fileName = component->getModuleName() + ".v";
    QString filePath = outputPath + "/" + fileName;

    QSharedPointer<VerilogDocument> document = QSharedPointer<VerilogDocument>(new VerilogDocument);

    document->fileName_ = fileName;
    document->vlnv_ = component->getComponent()->getVlnv().toString();

    // Try to find an implementation from the file.
    if (!readImplementation(document, filePath))
    {
        return QSharedPointer<VerilogDocument>();
    }

    // Create writers for module header etc.
    initializeComponentWriters(document, component);

    // Create writers.
    QSharedPointer<TextBodyWriter> implementationWriter(new TextBodyWriter(document->implementation_));
    document->topWriter_->setImplementation(implementationWriter);

    QSharedPointer<TextBodyWriter> postModuleWriter(new TextBodyWriter(document->postModule_));
    document->topWriter_->setPostModule(postModuleWriter);

    return document;
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::prepareDesign()
//-----------------------------------------------------------------------------
QList<QSharedPointer<GenerationOutput> > VerilogWriterFactory::prepareDesign(QList<QSharedPointer<MetaDesign> >& designs)
{
    QList<QSharedPointer<GenerationOutput> > retval;

    foreach(QSharedPointer<MetaDesign> mDesign, designs)
    {
        QSharedPointer<VerilogDocument> document = QSharedPointer<VerilogDocument>(new VerilogDocument);
        initializeComponentWriters(document, mDesign->getTopInstance());
        document->fileName_ = mDesign->getTopInstance()->getModuleName() + ".v";
        document->vlnv_ = mDesign->getTopInstance()->getComponent()->getVlnv().toString();

        initializeDesignWriters(document, mDesign);

        // Finally, add them to the top writer in desired order.
        addWritersToTopInDesiredOrder(document);
        retval.append(document);
    }

    return retval;
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::getLanguage()
//-----------------------------------------------------------------------------
QString VerilogWriterFactory::getLanguage() const
{
    return "Verilog";
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::getSaveToFileset()
//-----------------------------------------------------------------------------
bool VerilogWriterFactory::getSaveToFileset() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::getGroupIdentifier()
//-----------------------------------------------------------------------------
QString VerilogWriterFactory::getGroupIdentifier() const
{
    return QStringLiteral("generatedFiles");
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::readImplementation()
//-----------------------------------------------------------------------------
bool VerilogWriterFactory::readImplementation(QSharedPointer<VerilogDocument> document, QString const& outputPath)
{
    // This will contain the found implementation, if success.
    QString implementation;
    // This may contain the text after the module definition, if success.
    QString postModule;
    // This will contain the error message, if not success.
    QString error;

    // Check if the output file already exists.
    QFile outputFile(outputPath); 

    // If it does not, there is nothing do here.
    if (!outputFile.exists())
    {
        return true;
    }

    // Must be able to open it for reading.
    if (!outputFile.open(QIODevice::ReadOnly))
    {
        messages_->errorMessage(QObject::tr("File %1: The output file exists, but could not open it for reading. The path: %2").
            arg(document->fileName_, outputPath));
        return false;
    }

    // Read the content.
    QTextStream inputStream(&outputFile);
    QString fileContent = inputStream.readAll();
    // Remove carriage return.
    fileContent.remove('\r');

    // The document sub class is supposed to know how to find its implementation.
    if (!document->selectImplementation(fileContent, implementation, postModule, error))
    {
        messages_->errorMessage(QObject::tr("File %1: %2").arg(document->fileName_, error));

        return false;
    }

    document->implementation_ = implementation;
    document->postModule_ = postModule;

    return true;
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::initializeComponentWriters()
//-----------------------------------------------------------------------------
void VerilogWriterFactory::initializeComponentWriters(QSharedPointer<VerilogDocument> document,
    QSharedPointer<MetaComponent> component)
{
    QSettings settings;
    QString currentUser = settings.value("General/Username").toString();
    QString componentXmlPath = library_->getPath(component->getComponent()->getVlnv());

    document->headerWriter_ = QSharedPointer<VerilogHeaderWriter>(new VerilogHeaderWriter(component->getComponent()->getVlnv(), 
        componentXmlPath, currentUser, component->getComponent()->getDescription(), kactusVersion_, generatorVersion_));

    document->topWriter_ = QSharedPointer<ComponentVerilogWriter>(new ComponentVerilogWriter
        (component, settings_->generateInterfaces_));
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::initializeDesignWriters()
//-----------------------------------------------------------------------------
void VerilogWriterFactory::initializeDesignWriters(QSharedPointer<VerilogDocument> document, QSharedPointer<MetaDesign> design)
{
    document->instanceWriters_.clear();

    document->interconnectionWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    document->connectionWireWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    document->adHocWireWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    document->portWireWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    document->topAssignmentWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    document->topDefaultWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    document->instanceAssignmentWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    // Comment for top for assignments.
    if (design->getTopInstance()->getPorts()->size() > 0)
    {
        QSharedPointer<CommentWriter> topAssignmentHeaderWriter(
            new CommentWriter("Assignments for the ports of the encompassing component:"));
        topAssignmentHeaderWriter->setIndent(4);
        document->topAssignmentWriters_->add(topAssignmentHeaderWriter);
    }

    // Create assignments fort top ports.
    foreach (QSharedPointer<MetaPort> mPort, *design->getTopInstance()->getPorts())
    {
        if (mPort->downAssignments_.size() > 0)
        {
            // Create a writer for each assignment of the port.
            foreach (QSharedPointer<MetaPortAssignment> mpa, mPort->downAssignments_)
            {
                // Current policy dictates that hierarchical inout ports are directly connected to an instance port.
                // As an exception, it could have a default value assignment.
                if (mPort->port_->getDirection() != DirectionTypes::INOUT || !mpa->wire_)
                {
                    QSharedPointer<VerilogAssignmentWriter> topAssignment = QSharedPointer<VerilogAssignmentWriter>
                        (new VerilogAssignmentWriter(mPort->port_->name(), mpa, mPort, true));
                    document->topAssignmentWriters_->add(topAssignment);
                }
            }
        }
        else
        {
            QSharedPointer<VerilogTopDefaultWriter> topDefault = QSharedPointer<VerilogTopDefaultWriter>
                (new VerilogTopDefaultWriter(mPort));
            document->topDefaultWriters_->add(topDefault);
        }
    }

    // Create instance writers for the instances.
    foreach(QSharedPointer<MetaInstance> mInstance, *design->getInstances())
    {
        QSharedPointer<ComponentInstance> instance = mInstance->getComponentInstance();

        QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter(new ComponentInstanceVerilogWriter(
            mInstance, sorter_, settings_->generateInterfaces_));

        document->instanceWriters_.append(instanceWriter);

        document->instanceHeaderWriters_.insert(instanceWriter, createHeaderWriterForInstance(mInstance));

        // Comment for instance assignments.
        if (mInstance->getPorts()->size() > 0)
        {
            QSharedPointer<CommentWriter> portWireHeaderWriter(
                new CommentWriter(instance->getInstanceName() + " port wires:"));
            portWireHeaderWriter->setIndent(4);
            document->portWireWriters_->add(portWireHeaderWriter);

            QSharedPointer<CommentWriter> assignmentHeaderWriter(
                new CommentWriter(instance->getInstanceName() + " assignments:"));
            assignmentHeaderWriter->setIndent(4);
            document->instanceAssignmentWriters_->add(assignmentHeaderWriter);
        }

        // Create writers for instance ports, wires, and assignments.
        foreach (QSharedPointer<MetaPort> mPort, *mInstance->getPorts())
        {
            if (mPort->upAssignments_.size() < 1)
            {
                continue;
            }

            // Current policy dictates that instance inout ports are directly connected to wire or hierarchical port.
            if (mPort->port_->getDirection() == DirectionTypes::INOUT)
            {
                continue;
            }

            // Determine the name for the connected element: Name of the instance plus port.
            QString physName = VerilogSyntax::legalizeName(instance->getInstanceName() + "_" +
                mPort->port_->name());

            // Create a wire for it.
            document->portWireWriters_->add(QSharedPointer<VerilogWireWriter>(new VerilogWireWriter
                (physName, mPort->vectorBounds_)));

            // Then create assignments, that connect it to logical wires.
            foreach (QSharedPointer<MetaPortAssignment> mpa, mPort->upAssignments_)
            {
                QSharedPointer<VerilogAssignmentWriter> instanceAssignment = QSharedPointer<VerilogAssignmentWriter>
                    (new VerilogAssignmentWriter(physName, mpa, mPort, false));
                document->instanceAssignmentWriters_->add(instanceAssignment);
            }
        }
    }

    // Create wire writers for the interconnections
    foreach (QSharedPointer<MetaInterconnection> mInterconnect, *design->getInterconnections())
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
            // Current policy dictates that hierarchical inout ports are directly connected to an instance port.
            bool hierInout = false;
            foreach (QSharedPointer<MetaPort> mPort, gw->hierPorts_)
            {
                if (mPort->port_->getDirection() == DirectionTypes::INOUT)
                {
                    hierInout = true;
                    break;
                }
            }

            if (!hierInout)
            {
                document->connectionWireWriters_->
                    add(QSharedPointer<VerilogWireWriter>(new VerilogWireWriter
                    (VerilogSyntax::legalizeName(gw->name_), gw->bounds_)));
            }
        }
    }

    // Create wire writers for the ad-hoc connections
    if (design->getAdHocWires()->size() > 0)
    {
        QSharedPointer<CommentWriter> adHocWireHeaderWriter(
            new CommentWriter("Ad-hoc wires:"));
        adHocWireHeaderWriter->setIndent(4);
       document->adHocWireWriters_->add(adHocWireHeaderWriter);
    }

    foreach (QSharedPointer<MetaWire> adHoc, *design->getAdHocWires())
    {
        // Current policy dictates that hierarchical in out ports are directly connected to an instance port.
        bool hierInout = false;
        foreach (QSharedPointer<MetaPort> mPort, adHoc->hierPorts_)
        {
            if (mPort->port_->getDirection() == DirectionTypes::INOUT)
            {
                hierInout = true;
                break;
            }
        }

        // Thus, if an ad-hoc wire is connected to any hierarchical inout ports, it is no written.
        if (!hierInout)
        {
            document->adHocWireWriters_->
                add(QSharedPointer<VerilogWireWriter>(new VerilogWireWriter(
                VerilogSyntax::legalizeName(adHoc->name_), adHoc->bounds_)));
        }
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
    QString header = instance->getComponentInstance()->getDescription();
    if (!header.isEmpty())
    {
        header.append("\n");
    }

    header.append("IP-XACT VLNV: " + instance->getComponent()->getVlnv().toString());

    QSharedPointer<CommentWriter> headerWriter(new CommentWriter(header));
    headerWriter->setIndent(4);

    return headerWriter;
}