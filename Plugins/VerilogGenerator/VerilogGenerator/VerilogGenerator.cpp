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

#include <Plugins/VerilogGenerator/common/WriterGroup.h>
#include <Plugins/VerilogGenerator/CommentWriter/CommentWriter.h>
#include <Plugins/VerilogGenerator/ComponentVerilogWriter/ComponentVerilogWriter.h>
#include <Plugins/VerilogGenerator/ComponentInstanceVerilogWriter/ComponentInstanceVerilogWriter.h>
#include <Plugins/common/PortSorter/InterfaceDirectionNameSorter.h>
#include <Plugins/VerilogGenerator/VerilogHeaderWriter/VerilogHeaderWriter.h>
#include <Plugins/VerilogGenerator/VerilogWireWriter/VerilogWireWriter.h>
#include <Plugins/VerilogGenerator/VerilogAssignmentWriter/VerilogAssignmentWriter.h>
#include <Plugins/VerilogGenerator/VerilogInterconnectionWriter/VerilogInterconnectionWriter.h>

#include <Plugins/VerilogImport/VerilogSyntax.h>
#include <Plugins/common/HDLParser/MetaDesign.h>

#include <QDateTime>
#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::VerilogGenerator()
//-----------------------------------------------------------------------------
VerilogGenerator::VerilogGenerator(LibraryInterface* library, bool useInterfaces, bool generateMemory,
    QString const& kactusVersion, QString const& generatorVersion) : QObject(0), 
library_(library),
useInterfaces_(useInterfaces),
generateMemory_(generateMemory),
kactusVersion_(kactusVersion),
generatorVersion_(generatorVersion),
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
// Function: VerilogGenerator::prepareComponent()
//-----------------------------------------------------------------------------
bool VerilogGenerator::prepareComponent(QString const& outputPath, QSharedPointer<MetaInstance> component)
{
    // If we are not generating based on a design, we must parse the existing implementation.
    QString implementation;
    QString postModule;
    QString filePath = outputPath + "/" + component->fileName_;

    if (!selectImplementation(filePath, implementation, postModule))
    {
        // If parser says no-go, we dare do nothing.
        return false;
    }

    QSharedPointer<VerilogDocument> document = initializeComponentWriters(component);
    document->filePath_ = filePath;
    documents_->append(document);

    // Next comes the implementation.
    QSharedPointer<TextBodyWriter> implementationWriter(new TextBodyWriter(implementation));
    document->topWriter_->setImplementation(implementationWriter);

    // Also write any stuff that comes after the actual module.
    QSharedPointer<TextBodyWriter> postModuleWriter(new TextBodyWriter(postModule));
    document->topWriter_->setPostModule(postModuleWriter);

    return true;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::prepareDesign()
//-----------------------------------------------------------------------------
void VerilogGenerator::prepareDesign(QString const& outputPath, QSharedPointer<MetaDesign> design)
{
    QSharedPointer<VerilogDocument> document = initializeComponentWriters(design->topInstance_);
    document->filePath_ = outputPath + "/" + design->topInstance_->fileName_;

    initializeDesignWriters(design, document);

    // Finally, add them to the top writer in desired order.
    addWritersToTopInDesiredOrder(document);

    documents_->append(document);
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
	QTextStream inputStream(&outputFile);
	QString fileContent = inputStream.readAll();

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
// Function: VerilogGenerator::write()
//-----------------------------------------------------------------------------
void VerilogGenerator::write() const
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

        generate(outputStream, document);

        outputFile.close();
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::generate()
//-----------------------------------------------------------------------------
void VerilogGenerator::generate(QTextStream& outputStream, QSharedPointer<VerilogDocument> document) const
{
    document->headerWriter_->write(outputStream, document->filePath_, QDateTime::currentDateTime());
    document->topWriter_->write(outputStream);
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::getDocuments()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<VerilogDocument> > > VerilogGenerator::getDocuments()
{
    return documents_;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::nothingToWrite()
//-----------------------------------------------------------------------------
bool VerilogGenerator::nothingToWrite() const
{
    return documents_->size() < 1;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::initializeComponentWriters()
//-----------------------------------------------------------------------------
QSharedPointer<VerilogDocument> VerilogGenerator::initializeComponentWriters(QSharedPointer<MetaInstance> topComponent)
{
    QSettings settings;
    QString currentUser = settings.value("General/Username").toString();
    QString componentXmlPath = library_->getPath(topComponent->component_->getVlnv());

    QSharedPointer<VerilogDocument> retval(new VerilogDocument);

    retval->headerWriter_ = QSharedPointer<VerilogHeaderWriter>(new VerilogHeaderWriter(topComponent->component_->getVlnv(), 
        componentXmlPath, currentUser, topComponent->component_->getDescription(), kactusVersion_, generatorVersion_));

    retval->topWriter_ = QSharedPointer<ComponentVerilogWriter>(new ComponentVerilogWriter(topComponent, useInterfaces_, generateMemory_));

    retval->instanceWriters_.clear();

    retval->interconnectionWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    retval->wireWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    retval->assignmentWriter_ = QSharedPointer<VerilogAssignmentWriter>(new VerilogAssignmentWriter);

    return retval;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::initializeDesignWriters()
//-----------------------------------------------------------------------------
void VerilogGenerator::initializeDesignWriters(QSharedPointer<MetaDesign> design, QSharedPointer<VerilogDocument> document)
{
    // Create instance writers for the instances, complete with expression parsers and formatters.
    foreach(QSharedPointer<MetaInstance> gi, design->instances_)
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
        QList<QSharedPointer<MetaInterconnection> > usedGic;
        foreach (QSharedPointer<MetaInterconnection> gic, design->interconnections_)
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
        foreach (QSharedPointer<MetaInterconnection> gic, design->interconnections_)
        {
            QMap<QString, QSharedPointer<MetaWire> >::iterator iter = gic->wires_.begin();
            QMap<QString, QSharedPointer<MetaWire> >::iterator end = gic->wires_.end();
            for (;iter != end; ++iter)
            {
                QSharedPointer<MetaWire> gw = *iter;

                document->wireWriters_->add(QSharedPointer<VerilogWireWriter>(new VerilogWireWriter(gw)));
            }
        }
    }

    // Create wire writers for the ad hoc connections as well.
    foreach (QSharedPointer<MetaWire> adHoc, design->adHocWires_)
    {
        document->wireWriters_->add(QSharedPointer<VerilogWireWriter>(new VerilogWireWriter(adHoc)));
    }

    // Create writers for 
    foreach (QSharedPointer<MetaPort> mPort, design->topInstance_->ports_)
    {
        document->assignmentWriter_->addPort(mPort);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::addWritersToTopInDesiredOrder()
//-----------------------------------------------------------------------------
void VerilogGenerator::addWritersToTopInDesiredOrder(QSharedPointer<VerilogDocument> document) const
{
    document->topWriter_->add(document->interconnectionWriters_);

    document->topWriter_->add(document->wireWriters_);

    document->topWriter_->add(document->assignmentWriter_);

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
QSharedPointer<Writer> VerilogGenerator::createHeaderWriterForInstance(QSharedPointer<MetaInstance> instance) const
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