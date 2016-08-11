//-----------------------------------------------------------------------------
// File: VerilogGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Verilog file generator.
//-----------------------------------------------------------------------------

#include "VerilogGenerator.h"

#include <library/LibraryManager/libraryinterface.h>

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/MultipleParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <designEditors/common/TopComponentParameterFinder.h>

#include <IPXACTmodels/common/PortAlignment.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/Model.h>

#include <Plugins/VerilogGenerator/common/WriterGroup.h>
#include <Plugins/VerilogGenerator/CommentWriter/CommentWriter.h>
#include <Plugins/VerilogGenerator/ComponentVerilogWriter/ComponentVerilogWriter.h>
#include <Plugins/VerilogGenerator/ComponentInstanceVerilogWriter/ComponentInstanceVerilogWriter.h>
#include <Plugins/VerilogGenerator/PortSorter/InterfaceDirectionNameSorter.h>
#include <Plugins/VerilogGenerator/VerilogHeaderWriter/VerilogHeaderWriter.h>
#include <Plugins/VerilogGenerator/VerilogWireWriter/VerilogWireWriter.h>
#include <Plugins/VerilogGenerator/VerilogTiedValueWriter/VerilogTiedValueWriter.h>

#include <Plugins/VerilogImport/VerilogSyntax.h>
#include <Plugins/common/HDLParser/HDLDesignParser.h>

#include <QDateTime>
#include <QFileInfo>
#include "../../common/HDLParser/HDLComponentParser.h"

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::VerilogGenerator()
//-----------------------------------------------------------------------------
VerilogGenerator::VerilogGenerator(LibraryInterface* library): QObject(0), 
library_(library),
headerWriter_(0),
topWriter_(0), 
topComponent_(),
design_(),
wireWriters_(),
tiedValueWriter_(),
instanceWriters_(),
sorter_(new InterfaceDirectionNameSorter())
{

}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::~VerilogGenerator()
//-----------------------------------------------------------------------------
VerilogGenerator::~VerilogGenerator()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::parse()
//-----------------------------------------------------------------------------
void VerilogGenerator::parse(QSharedPointer<Component> component, QSharedPointer<View> topComponentView, 
	QString const& outputPath /*= QString("")*/, QSharedPointer<Design> design /*= QSharedPointer<Design>()*/,
	QSharedPointer<DesignConfiguration> designConf /*= QSharedPointer<DesignConfiguration>()*/ )
{
    topComponent_ = component;
    topComponentView_ = topComponentView;
    design_ = design;
	designConf_ = designConf;

    initializeWriters();

    if (design_ && designConf_)
    {
		// Parse the design, using the provided configuration and the top component.
		HDLDesignParser parser(library_,component,topComponentView,design,designConf);
		parser.parseDesign();

		// Create instance writers for the instances, complete with expression parsers and formatters.
		foreach(QSharedPointer<GenerationInstance> gi, parser.instances_)
		{
			QSharedPointer<ComponentInstance> instance = gi->componentInstance_;

			QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter(new ComponentInstanceVerilogWriter(
				gi, sorter_,
				parser.createParserForComponent(gi->component, gi->activeView_, instance->getInstanceName()),
				parser.createFormatterForComponent(gi->component, gi->activeView_, instance->getInstanceName())
				));


			instanceWriters_.insert(instance->getInstanceName(), instanceWriter);
		}

		// Create wire writers for the interconnections
		QList<QSharedPointer<GenerationInterconnection> > usedGic;
		foreach (QSharedPointer<GenerationInterconnection> gic, parser.interConnections_)
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

				if (gw->ports.size() > 1)
				{
					wireWriters_->add(QSharedPointer<VerilogWireWriter>(new VerilogWireWriter(gw)));
				}
			}

			usedGic.append(gic);
		}

		// Create wire writers for the ad hoc connections as well.
		QList<QSharedPointer<GenerationWire> > usedWire;
		foreach (QSharedPointer<GenerationAdHoc> adHoc, parser.adHocs_)
		{
			QSharedPointer<GenerationWire> gw = adHoc->wire;

			if (usedWire.contains(gw))
			{
				continue;
			}

			if (gw->ports.size() > 1)
			{
				wireWriters_->add(QSharedPointer<VerilogWireWriter>(new VerilogWireWriter(gw)));
			}

			usedWire.append(gw);
		}

		// Create tied value writers for hierarchical tied values,
		QMap<QString,QString>::iterator iter2 = parser.portTiedValues_.begin();
		QMap<QString,QString>::iterator end2 = parser.portTiedValues_.end();
		for (;iter2 != end2; ++iter2)
		{
			tiedValueWriter_->addPortTiedValue(iter2.key(), iter2.value());
		}

		// Finally, add them to the top writer in desired order.
		addWritersToTopInDesiredOrder();       
    }
	else
	{
		// If we are not generating based on a design, we must parse the existing implementation.
		QString implementation;
		QString postModule;

		if (!selectImplementation(outputPath, implementation, postModule))
		{
			// If parser says no-go, we dare do nothing.
			return;
		}

		// Next comes the implementation.
		QSharedPointer<TextBodyWriter> implementationWriter(new TextBodyWriter(implementation));
		topWriter_->setImplementation(implementationWriter);

		// Also write any stuff that comes after the actual module.
		QSharedPointer<TextBodyWriter> postModuleWriter(new TextBodyWriter(postModule));
		topWriter_->setPostModule(postModuleWriter);
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

	// Find the beginning of the module header.
	int moduleDeclarationBeginIndex = fileContent.indexOf(VerilogSyntax::MODULE_KEY_WORD);

	// Must have it to proceed.
	if (moduleDeclarationBeginIndex == -1)
	{
		emit reportError(tr("Could not find module header start from the output file."));
		return false;
	}

	// Find the ending of the module header.
	int moduleDeclarationEndIndex = fileContent.indexOf(");",moduleDeclarationBeginIndex);

	// Must have it to proceed.
	if (moduleDeclarationEndIndex == -1)
	{
		emit reportError(tr("Could not find module header end from the output file!"));
		return false;
	}

	// The end of the override tag line is the beginning of the implementation.
	int implementationStart = fileContent.indexOf(VerilogSyntax::TAG_OVERRIDE);

	if (implementationStart == -1)
	{
		// If does not exist, the end of the header is the beginning of the implementation.
		implementationStart = moduleDeclarationEndIndex + 3;
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
void VerilogGenerator::generate(QString const& outputPath, QString const& generatorVersion /*= ""*/,
	QString const& kactusVersion /*= ""*/) const
{
    if (nothingToWrite())
	{
		emit reportError(tr("Nothing to write"));
        return;
    }

	QFile outputFile(outputPath); 
    if (!outputFile.open(QIODevice::WriteOnly))
	{
		emit reportError(tr("Could not open output file for writing: %1").arg(outputPath));
        return;
    }

    QTextStream outputStream(&outputFile);

    QString fileName = QFileInfo(outputPath).fileName();

    headerWriter_->write(outputStream, fileName, generatorVersion, kactusVersion,
		topComponent_->getDescription(), QDateTime::currentDateTime());
    topWriter_->write(outputStream);

    outputFile.close();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::initializeWriters()
//-----------------------------------------------------------------------------
void VerilogGenerator::initializeWriters()
{
    QSettings settings;
    QString currentUser = settings.value("General/Username").toString();
    QString componentXmlPath = library_->getPath(topComponent_->getVlnv());

    headerWriter_ = QSharedPointer<VerilogHeaderWriter>(new VerilogHeaderWriter(topComponent_->getVlnv(), 
        componentXmlPath, currentUser));

	QSharedPointer<ComponentParameterFinder> parameterFinder(new ComponentParameterFinder(topComponent_));
	parameterFinder->setActiveView(topComponentView_);

	QSharedPointer<ExpressionParser> parser = QSharedPointer<IPXactSystemVerilogParser>(new IPXactSystemVerilogParser(parameterFinder));
	QSharedPointer<ExpressionFormatter> formatter = QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(parameterFinder));

    HDLComponentParser pars(topComponent_,topComponentView_,sorter_,formatter);

    topWriter_ = QSharedPointer<ComponentVerilogWriter>(new ComponentVerilogWriter(pars.parseComponent(), formatter));

    instanceWriters_.clear();

    wireWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    tiedValueWriter_ = QSharedPointer<VerilogTiedValueWriter>(new VerilogTiedValueWriter(formatter));
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::nothingToWrite()
//-----------------------------------------------------------------------------
bool VerilogGenerator::nothingToWrite() const
{
    return topWriter_ == 0;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::addWritersToTopInDesiredOrder()
//-----------------------------------------------------------------------------
void VerilogGenerator::addWritersToTopInDesiredOrder() const
{
    topWriter_->add(wireWriters_);

    topWriter_->add(tiedValueWriter_);

    foreach(QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter, instanceWriters_)
    {
        QString instanceName = instanceWriters_.key(instanceWriter);

        QSharedPointer<WriterGroup> instanceGroup(new WriterGroup);
        instanceGroup->add(createHeaderWriterForInstance(instanceName));
        instanceGroup->add(instanceWriter);

        topWriter_->add(instanceGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::createHeaderWriterForInstance()
//-----------------------------------------------------------------------------
QSharedPointer<Writer> VerilogGenerator::createHeaderWriterForInstance(QString const& instanceName) const
{
    QString header = design_->getHWInstanceDescription(instanceName);
    if (!header.isEmpty())
    {
        header.append("\n");
    }

    QString instanceVLNV = design_->getHWComponentVLNV(instanceName).toString();
    header.append("IP-XACT VLNV: " + instanceVLNV);

    QSharedPointer<CommentWriter> headerWriter(new CommentWriter(header));
    headerWriter->setIndent(4);

    return headerWriter;
}