//-----------------------------------------------------------------------------
// File: VerilogGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Verilog file generator.
//-----------------------------------------------------------------------------

#include "VerilogGenerator.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>

#include <Plugins/VerilogGenerator/common/WriterGroup.h>
#include <Plugins/VerilogGenerator/CommentWriter/CommentWriter.h>
#include <Plugins/VerilogGenerator/ComponentVerilogWriter/ComponentVerilogWriter.h>
#include <Plugins/VerilogGenerator/ComponentInstanceVerilogWriter/ComponentInstanceVerilogWriter.h>
#include <Plugins/common/PortSorter/InterfaceDirectionNameSorter.h>
#include <Plugins/VerilogGenerator/VerilogHeaderWriter/VerilogHeaderWriter.h>
#include <Plugins/VerilogGenerator/VerilogWireWriter/VerilogWireWriter.h>
#include <Plugins/VerilogGenerator/VerilogTiedValueWriter/VerilogTiedValueWriter.h>
#include <Plugins/VerilogGenerator/VerilogInterconnectionWriter/VerilogInterconnectionWriter.h>

#include <Plugins/VerilogImport/VerilogSyntax.h>
#include <Plugins/common/HDLParser/HDLDesignParser.h>

#include <QDateTime>
#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::VerilogGenerator()
//-----------------------------------------------------------------------------
VerilogGenerator::VerilogGenerator(LibraryInterface* library, bool useInterfaces, bool generateMemory) : QObject(0), 
library_(library),
useInterfaces_(useInterfaces),
generateMemory_(generateMemory),
sorter_(new InterfaceDirectionNameSorter()),
documents_(new QList<QSharedPointer<VerilogDocument> >)
{
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::~VerilogGenerator()
//-----------------------------------------------------------------------------
VerilogGenerator::~VerilogGenerator()
{
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::parseComponent()
//-----------------------------------------------------------------------------
void VerilogGenerator::parseComponent(QString const& outputPath, QSharedPointer<GenerationComponent> gc)
{
    // If we are not generating based on a design, we must parse the existing implementation.
    QString implementation;
    QString postModule;
    QString filePath = outputPath + "/" + gc->fileName_;

    if (!selectImplementation(filePath, implementation, postModule))
    {
        // If parser says no-go, we dare do nothing.
        return;
    }

    QSharedPointer<VerilogDocument> document = initializeWriters(gc);
    document->filePath_ = filePath;
    documents_->append(document);

    // Next comes the implementation.
    QSharedPointer<TextBodyWriter> implementationWriter(new TextBodyWriter(implementation));
    document->topWriter_->setImplementation(implementationWriter);

    // Also write any stuff that comes after the actual module.
    QSharedPointer<TextBodyWriter> postModuleWriter(new TextBodyWriter(postModule));
    document->topWriter_->setPostModule(postModuleWriter);
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::parseDesign()
//-----------------------------------------------------------------------------
void VerilogGenerator::parseDesign(QString const& outputPath, QList<QSharedPointer<GenerationDesign> >& designs)
{
    foreach (QSharedPointer<GenerationDesign> design, designs)
    {
        QSharedPointer<VerilogDocument> document = initializeWriters(design->topComponent_);
        document->filePath_ = outputPath + "/" + design->topComponent_->fileName_;

        createDesignWriters(design, document);

        // Finally, add them to the top writer in desired order.
        addWritersToTopInDesiredOrder(document);

        documents_->append(document);
    }
}

void VerilogGenerator::createDesignWriters(QSharedPointer<GenerationDesign> design, QSharedPointer<VerilogDocument> document)
{
    // Create instance writers for the instances, complete with expression parsers and formatters.
    foreach(QSharedPointer<GenerationInstance> gi, design->instances_)
    {
        QSharedPointer<ComponentInstance> instance = gi->componentInstance_;

        QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter(new ComponentInstanceVerilogWriter(
            gi, sorter_, useInterfaces_));

        document->instanceWriters_.append(instanceWriter);

        document->instanceHeaderWriters_.insert(instanceWriter, createHeaderWriterForInstance(gi));
    }

    if (useInterfaces_)
    {
        // Create interconnection writers for the interconnections
        QList<QSharedPointer<GenerationInterconnection> > usedGic;
        foreach (QSharedPointer<GenerationInterconnection> gic, design->interConnections_)
        {
            if (usedGic.contains(gic))
            {
                continue;
            }

            document->interconnectionWriters_->add(QSharedPointer<VerilogInterconnectionWriter>
                (new VerilogInterconnectionWriter(gic)));

            usedGic.append(gic);
        }
    }
    else
    {
        // Create wire writers for the interconnections
        QList<QSharedPointer<GenerationInterconnection> > usedGic;
        foreach (QSharedPointer<GenerationInterconnection> gic, design->interConnections_)
        {
            if (usedGic.contains(gic))
            {
                continue;
            }

            QMap<QString, QSharedPointer<GenerationWire> >::iterator iter = gic->wires_.begin();
            QMap<QString, QSharedPointer<GenerationWire> >::iterator end = gic->wires_.end();
            for (;iter != end; ++iter)
            {
                QSharedPointer<GenerationWire> gw = *iter;

                document->wireWriters_->add(QSharedPointer<VerilogWireWriter>(new VerilogWireWriter(gw)));
            }

            usedGic.append(gic);
        }
    }

    // Create wire writers for the ad hoc connections as well.
    QList<QSharedPointer<GenerationWire> > usedWire;
    foreach (QSharedPointer<GenerationAdHoc> adHoc, design->adHocs_)
    {
        QSharedPointer<GenerationWire> gw = adHoc->wire;

        if (usedWire.contains(gw))
        {
            continue;
        }

        document->wireWriters_->add(QSharedPointer<VerilogWireWriter>(new VerilogWireWriter(gw)));

        usedWire.append(gw);
    }

    // Create tied value writers for hierarchical tied values,
    QMap<QString,QString>::iterator iter2 = design->portTiedValues_.begin();
    QMap<QString,QString>::iterator end2 = design->portTiedValues_.end();
    for (;iter2 != end2; ++iter2)
    {
        document->tiedValueWriter_->addPortTiedValue(iter2.key(), iter2.value());
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::selectImplementation()
//-----------------------------------------------------------------------------
bool VerilogGenerator::selectImplementation(QString const& outputPath, QString& implementation,
	QString& postModule) const
{
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
		emit reportError(tr("The output file exists, but could not open it for reading. The path: %1").arg(outputPath));
		return false;
	}

	// Read the content.
	QTextStream outputStream(&outputFile);
	QString fileContent = outputStream.readAll();

	// We do not support multiple modules in the same file.
	if (fileContent.count(VerilogSyntax::MODULE_KEY_WORD) > 1)
	{
		emit reportError(tr("There was more than one module headers in the output file!"));
		return false;
	}

    // Find the module header.
    QPair<int,int> headerPosition = VerilogSyntax::findModuleDeclaration(fileContent);
    int moduleDeclarationBeginIndex = headerPosition.first;
    int moduleDeclarationLength = headerPosition.second;

    // Must have it to proceed.
	if (moduleDeclarationBeginIndex == -1)
	{
		emit reportError(tr("Could not find module header start from the output file."));
		return false;
	}

    // Must have it to proceed.
	if (moduleDeclarationLength == -1)
	{
		emit reportError(tr("Could not find module header end from the output file!"));
		return false;
	}

	// The end of the override tag line is the beginning of the implementation.
	int implementationStart = fileContent.indexOf(VerilogSyntax::TAG_OVERRIDE);

	if (implementationStart == -1)
	{
		// If does not exist, the end of the header is the beginning of the implementation.
		implementationStart = moduleDeclarationBeginIndex + moduleDeclarationLength + 3;
	}
	else
	{
		// Else we should seek the position where tag line ends.
		implementationStart += VerilogSyntax::TAG_OVERRIDE.size() + 1;
	}

	// The end of the module is the end of the implementation.
	int implementationEnd = fileContent.indexOf(VerilogSyntax::MODULE_END);

	// The module must end some where.
	if (implementationEnd == -1)
	{
		emit reportError(tr("Could not find module end from the output file!"));
		return false;
	}

	// Rip the implementation once detected.
	int implementationLength = implementationEnd - implementationStart;
	implementation = fileContent.mid(implementationStart,implementationLength);

	// Then take all the text that comes after the module, just in case.
	int postStart = implementationEnd + 9;
	postModule = fileContent.mid(postStart);

	// Also trim away extra white space.
	postModule = postModule.trimmed();

	// The destructor shall close the file. All done here.
	return true;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::generate()
//-----------------------------------------------------------------------------
void VerilogGenerator::generate(QString const& generatorVersion,
    QString const& kactusVersion) const
{
    if (nothingToWrite())
	{
		emit reportError(tr("Nothing to write"));
        return;
    }

    foreach(QSharedPointer<VerilogDocument> document, *documents_)
    {
        QFile outputFile(document->filePath_); 
        if (!outputFile.open(QIODevice::WriteOnly))
        {
            emit reportError(tr("Could not open output file for writing: %1").arg(document->filePath_));
            return;
        }

        QTextStream outputStream(&outputFile);

        document->headerWriter_->write(outputStream, document->filePath_, generatorVersion, kactusVersion,
            QDateTime::currentDateTime());
        document->topWriter_->write(outputStream);

        outputFile.close();
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::initializeWriters()
//-----------------------------------------------------------------------------
QSharedPointer<VerilogDocument> VerilogGenerator::initializeWriters(QSharedPointer<GenerationComponent> topComponent)
{
    QSettings settings;
    QString currentUser = settings.value("General/Username").toString();
    QString componentXmlPath = library_->getPath(topComponent->component->getVlnv());

    QSharedPointer<VerilogDocument> retval(new VerilogDocument);

    retval->headerWriter_ = QSharedPointer<VerilogHeaderWriter>(new VerilogHeaderWriter(topComponent->component->getVlnv(), 
        componentXmlPath, currentUser, topComponent->component->getDescription()));

    retval->topWriter_ = QSharedPointer<ComponentVerilogWriter>(new ComponentVerilogWriter(topComponent, useInterfaces_, generateMemory_));

    retval->instanceWriters_.clear();

    retval->interconnectionWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    retval->wireWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    retval->tiedValueWriter_ = QSharedPointer<VerilogTiedValueWriter>(new VerilogTiedValueWriter);

    return retval;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::nothingToWrite()
//-----------------------------------------------------------------------------
bool VerilogGenerator::nothingToWrite() const
{
    return documents_->size() < 1;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::addWritersToTopInDesiredOrder()
//-----------------------------------------------------------------------------
void VerilogGenerator::addWritersToTopInDesiredOrder(QSharedPointer<VerilogDocument> document) const
{
    document->topWriter_->add(document->interconnectionWriters_);

    document->topWriter_->add(document->wireWriters_);

    document->topWriter_->add(document->tiedValueWriter_);

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
// Function: VerilogGenerator::createHeaderWriterForInstance()
//-----------------------------------------------------------------------------
QSharedPointer<Writer> VerilogGenerator::createHeaderWriterForInstance(QSharedPointer<GenerationInstance> instance) const
{
    QString header = instance->componentInstance_->getDescription();
    if (!header.isEmpty())
    {
        header.append("\n");
    }

    header.append("IP-XACT VLNV: " + instance->component->component->getVlnv().toString());

    QSharedPointer<CommentWriter> headerWriter(new CommentWriter(header));
    headerWriter->setIndent(4);

    return headerWriter;
}