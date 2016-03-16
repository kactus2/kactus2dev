//-----------------------------------------------------------------------------
// File: documentgenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 19.09.2011
//
// Description:
// Generates a HTML document.
//-----------------------------------------------------------------------------

#include "documentgenerator.h"

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/common/FileBuilder.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/BuildCommand.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/MemoryMap.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <common/utils.h>
#include <common/widgets/componentPreviewBox/ComponentPreviewBox.h>

#include <designEditors/common/DesignWidget.h>
#include <designEditors/common/DesignDiagram.h>

#include <QFileDialog>
#include <QString>
#include <QFile>
#include <QDateTime>
#include <QSettings>
#include <QVariant>
#include <QFileInfo>
#include <QMap>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QRectF>
#include <QPointF>
#include <QBrush>
#include <QSharedPointer>

namespace
{
    static const QString SPACE = QString("&nbsp;");
    static const QString INDENT = QString("&nbsp;&nbsp;&nbsp;");
    static const QString TABLE = QString("<table frame=\"box\" rules=\"all\""
        " border=\"1\" cellPadding=\"3\" title=\"");

    static const QString ENCODING = QString("<meta http-equiv=\"Content-Type\""
        "content=\"text/html; charset=utf-8\">");

    static const QString DOCTYPE = QString("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" "
        "\"http://www.w3.org/TR/html4/strict.dtd\">");

    static const QString VALID_W3C_STRICT = QString("\t\t<p>\n"
        "\t\t\t<a href=\"http://validator.w3.org/check?uri=referer\">\n"
        "\t\t\t<img src=\"http://www.w3.org/Icons/valid-html401\""
        "alt=\"Valid HTML 4.01 Strict\" height=\"31\""
        "width=\"88\">\n"
        "\t\t\t</a>\n"
        "\t\t</p>");
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::DocumentGenerator()
//-----------------------------------------------------------------------------
DocumentGenerator::DocumentGenerator(LibraryInterface* handler,
                                     const VLNV& vlnv,
                                     DesignWidgetFactory* designWidgetFactory,
                                     ExpressionFormatterFactory* expressionFormatterFactory,
									 QWidget* parent): 
QObject(parent),
handler_(handler),
component_(),
myNumber_(0),
childInstances_(),
targetPath_(), 
parentWidget_(parent),
designWidgetFactory_(designWidgetFactory),
expressionFormatterFactory_(expressionFormatterFactory),
expressionFormatter_()
{
	Q_ASSERT(handler_);
	Q_ASSERT(parent);

    // this function can be called for only the top document generator
    Q_ASSERT(parentWidget_);

    // parse the model for the component
    component_ = handler_->getModel(vlnv).dynamicCast<Component>();
    if (!component_)
    {
        emit errorMessage("VLNV was not found in the library.");
    }
    else
    {
        expressionFormatter_ = expressionFormatterFactory_->makeExpressionFormatter(component_);

        // list of objects that have already been processed to avoid duplicates
        QList<VLNV> objects;
        objects.append(vlnv);

        parseChildItems(objects);
    }
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::DocumentGenerator()
//-----------------------------------------------------------------------------
DocumentGenerator::DocumentGenerator( LibraryInterface* handler, 
									 const VLNV& vlnv,
									 QList<VLNV>& objects,
                                     DesignWidgetFactory* designWidgetFactory,
                                     ExpressionFormatterFactory* expressionFormatterFactory,
									 DocumentGenerator* parent):
QObject(parent),
handler_(handler),
component_(),
myNumber_(0),
childInstances_(),
targetPath_(),
parentWidget_(NULL),
designWidgetFactory_(designWidgetFactory),
expressionFormatterFactory_(expressionFormatterFactory),
expressionFormatter_()
{
	Q_ASSERT(handler_);
	Q_ASSERT(parent);
	Q_ASSERT(handler_->contains(vlnv));
	Q_ASSERT(handler_->getDocumentType(vlnv) == VLNV::COMPONENT);

	connect(this, SIGNAL(errorMessage(const QString&)),
		parent, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(noticeMessage(const QString&)),
		parent, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	QSharedPointer<Document> libComp = handler_->getModel(vlnv);
	component_ = libComp.staticCast<Component>();

    expressionFormatter_ = expressionFormatterFactory_->makeExpressionFormatter(component_);

	parseChildItems(objects);
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::~DocumentGenerator()
//-----------------------------------------------------------------------------
DocumentGenerator::~DocumentGenerator()
{

}

//-----------------------------------------------------------------------------
// Function: documentgenerator::parseChildItems()
//-----------------------------------------------------------------------------
void DocumentGenerator::parseChildItems( QList<VLNV>& objects )
{
	// ask the component for it's hierarchical references
	QList<VLNV> refs = component_->getHierRefs();
	foreach (VLNV ref, refs) {
		QSharedPointer<Design> design = handler_->getDesign(ref);
		if (!design)
			continue;


		foreach (QSharedPointer<ComponentInstance> instance, *design->getComponentInstances())
        {
            if (!objects.contains(*instance->getComponentRef()) && handler_->contains(*instance->getComponentRef()) &&
                handler_->getDocumentType(*instance->getComponentRef()) == VLNV::COMPONENT)
            {
    			// create a new instance of document generator and add it to child list
			    objects.append(*instance->getComponentRef());
			    QSharedPointer<DocumentGenerator> docGenerator(new DocumentGenerator(handler_,
                    *instance->getComponentRef(), objects, designWidgetFactory_, expressionFormatterFactory_, this));
			    childInstances_.append(docGenerator);
            }
		}
	}
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeDocumentation()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeDocumentation(QTextStream& stream, QString targetPath)
{
    if (!component_)
    {
        emit errorMessage("VLNV was not found in the library.");
        return;
    }

	Q_ASSERT(handler_->contains(component_->getVlnv()));
	Q_ASSERT(handler_->getDocumentType(component_->getVlnv()) == VLNV::COMPONENT);

	// this function can only be called for the top document generator
	Q_ASSERT(parentWidget_);

    targetPath_ = targetPath;

    writeHtmlHeader(stream);

	// create a running number to create the numbered headers for table of contents
	unsigned int runningNumber = 0;

	stream << "\t\t<p>" << endl;
	stream << "\t\t<strong>Table of contents</strong><br>" << endl;
	writeTableOfContents(runningNumber, stream);
	stream << "\t\t</p>" << endl;

	QStringList pictureList;

	// write the actual documentation for the top component
	writeDocumentation(stream, targetPath_, pictureList);

    writeEndOfDocument(stream);

	// ask user if he wants to save the generated documentation into object metadata
	QMessageBox::StandardButton button = QMessageBox::question(parentWidget_, 
		tr("Save generated documentation to metadata?"),
		tr("Would you like to save the generated documentation to IP-Xact"
		" metadata?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

	// if the generated file is saved
	if (button == QMessageBox::Yes)
    {
		QString xmlPath = handler_->getPath(component_->getVlnv());

		// get the relative path to add to file set
		QString relativePath = General::getRelativePath(xmlPath, targetPath_);

		QString fileSetName("Documentation");
		QSharedPointer<FileSet> documentationFileSet = component_->getFileSet(fileSetName);

		// if the documentation file set was not found. Create one
		if (!documentationFileSet)
        {
			documentationFileSet = QSharedPointer<FileSet>(new FileSet(fileSetName, QString("documentation")));
            component_->getFileSets()->append(documentationFileSet);
		}

		QSettings settings;

		// create a new file and add it to file set
		QSharedPointer<File> docFile = documentationFileSet->addFile(relativePath, settings);
		Q_ASSERT(docFile);
		docFile->setIncludeFile(false);
		docFile->setDescription(tr("Html file that contains the documentation "
			"for this component and subcomponents"));

		// add all created pictures to the file set
		foreach (QString pictureName, pictureList) {
			QString relativePicPath = General::getRelativePath(xmlPath, pictureName);

			QSharedPointer<File> picFile = documentationFileSet->addFile(relativePicPath, settings);
			Q_ASSERT(picFile);
			picFile->setIncludeFile(false);
			picFile->setDescription(tr("Preview picture needed by the html document."));
		}

		handler_->writeModelToFile(component_);
	}
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeHtmlHeader()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeHtmlHeader(QTextStream& stream)
{
    QSettings settings;

    // write the top of the html document
    stream << DOCTYPE << endl;
    stream << "<html>" << endl;
    stream << "\t<head>" << endl;
    stream << "\t" << ENCODING << endl;
    stream << "\t\t<title>Kactus2 generated documentation for component " <<
        component_->getVlnv().getName() << " " << component_->getVlnv().getVersion() << "</title>" << endl;
    stream << "\t</head>" << endl;
    stream << "\t<body>" << endl;
    stream << "\t\t<h6>This document was generated by Kactus2 on " <<
        QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") << " by user " <<
        settings.value("General/Username").toString() << "</h6>" << endl;
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeTableOfContents()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeTableOfContents(unsigned int& componentNumber, QTextStream& stream )
{
	// increase the running number and save this instance's number
	++componentNumber;
	myNumber_ = componentNumber;

    QString vlnvHeader = "\t\t" + INDENT + "<a href=\"#" + component_->getVlnv().toString();

    stream << "\t\t<a href=\"#" << component_->getVlnv().toString() << "\">" << myNumber_ << ". Component" <<
        SPACE << component_->getVlnv().toString(" - ") << "</a><br>" << endl;

	// subHeader is running number that counts the number of sub headers for component
	int subHeader = 1;

	/*if (component_->hasModelParameters()) {
        stream << vlnvHeader << ".modelParams\">" << myNumber_ << "." << subHeader <<
            ". Model parameters</a><br>" << endl;
		++subHeader;
	}*/

	// component always has kactus parameters
    stream << vlnvHeader << ".kts_params\">" << myNumber_ << "." << subHeader <<
        ". Kactus2 attributes</a><br>" << endl;
	++subHeader;

	if (component_->hasParameters()) {
        stream << vlnvHeader << ".parameters\">" << myNumber_ << "." << subHeader <<
            ". General parameters</a><br>" << endl;
		++subHeader;
	}

    if (!component_->getMemoryMaps()->isEmpty())
    {
        stream << vlnvHeader << ".memoryMaps\">" << myNumber_ << "." << subHeader << ". Memory maps</a><br>" << endl;
        ++subHeader;
    }

	if (component_->hasPorts()) {
        stream << vlnvHeader << ".ports\">" << myNumber_ << "." << subHeader << ". Ports</a><br>" << endl;
		++subHeader;
	}

	if (component_->hasInterfaces()) {
        stream << vlnvHeader << ".interfaces\">" << myNumber_ << "." << subHeader <<
            ". Bus interfaces</a><br>" << endl;
		++subHeader;
	}

	if (component_->hasFileSets()) {
        stream << vlnvHeader << ".fileSets\">" << myNumber_ << "." << subHeader << ". File sets</a><br>" << endl;
		++subHeader;
	}

	if (component_->hasViews()) {
        stream << vlnvHeader << ".views\">" << myNumber_ << "." << subHeader << ". Views</a><br>" << endl;
	}

	// tell each child to write it's table of contents
	foreach (QSharedPointer<DocumentGenerator> generator, childInstances_) {
		generator->writeTableOfContents(componentNumber, stream);
	}
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeDocumentation()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeDocumentation(QTextStream& stream, const QString& targetPath,
                                           QStringList& filesToInclude)
{
	targetPath_ = targetPath;
	
	// write the component header
	stream << "\t\t<h1><a id=\"" << component_->getVlnv().toString() << "\">" << myNumber_ <<
		". Component " << component_->getVlnv().toString(" - ") << "</a></h1>" << endl;

	createComponentPicture(filesToInclude);

	stream << "\t\t<p>" << endl;

	stream << "\t\t<img src=\"" << component_->getVlnv().toString(".") << ".png\""
		" alt=\"" << component_->getVlnv().toString(" - ") << " preview picture\"><br>" << endl;

	// if component has description, write it
	if (!component_->getDescription().isEmpty()) {
		stream << "\t\t<strong>Description:</strong> " << component_->getDescription() 
			<< "<br>" << endl;
	}

	// print relative path to the xml file
	QFileInfo compXmlInfo(handler_->getPath(component_->getVlnv()));
	QString relativeXmlPath = General::getRelativePath(targetPath_, compXmlInfo.absoluteFilePath());
	stream << "\t\t<strong>IP-Xact file: </strong><a href=\"" << 
		relativeXmlPath << "\">" << compXmlInfo.fileName() <<
		"</a><br>" << endl;
	
	stream << "\t\t</p>" << endl;

	int subHeaderNumber = 1;
// 	writeModelParameters(stream, subHeaderNumber);
	writeParameters(stream, subHeaderNumber);
    writeMemoryMaps(stream, subHeaderNumber);
	writePorts(stream, subHeaderNumber);
	writeInterfaces(stream, subHeaderNumber);
	writeFileSets(stream, subHeaderNumber);
	writeViews(stream, subHeaderNumber, filesToInclude);

	// tell each child to write it's documentation
	foreach (QSharedPointer<DocumentGenerator> generator, childInstances_) {
		generator->writeDocumentation(stream, targetPath, filesToInclude);
	}
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeModelParameters()
//-----------------------------------------------------------------------------
/*void DocumentGenerator::writeModelParameters(QTextStream& stream, int& subHeaderNumber)
{
	if (component_->hasModelParameters()) {
		
		writeSubHeader(subHeaderNumber, stream, "Model parameters", "modelParams");

		QStringList paramHeaders;
		paramHeaders.append("Name");
		paramHeaders.append("Data type");
		paramHeaders.append("Default value");
        paramHeaders.append("Array left");
        paramHeaders.append("Array right");
		paramHeaders.append("Description");
		writeTableElement(paramHeaders, "List of model parameters defined for the component", stream);

		foreach (QSharedPointer<ModelParameter> param, *component_->getModelParameters())
        {
			stream << "\t\t\t\t<tr>" << endl;
			stream << "\t\t\t\t\t<td>" << param->name() << "</td>" << endl;
			stream << "\t\t\t\t\t<td>" << param->getDataType() << "</td>" << endl;
			stream << "\t\t\t\t\t<td>" << expressionFormatter_->formatReferringExpression(param->getValue()) <<
                "</td>" << endl;
            stream << "\t\t\t\t\t<td>" << expressionFormatter_->formatReferringExpression(
                param->getArrayLeft()) << "</td>" << endl;
            stream << "\t\t\t\t\t<td>" << expressionFormatter_->formatReferringExpression(
                param->getArrayRight()) << "</td>" << endl;
			stream << "\t\t\t\t\t<td>" << param->description() << "</td>" << endl;
			stream << "\t\t\t\t</tr>" << endl;
		}

		stream << "\t\t\t</table>" << endl;
		++subHeaderNumber;
	}
}*/

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeParameters()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeParameters(QTextStream& stream, int& subHeaderNumber)
{
	writeSubHeader(subHeaderNumber, stream, "Kactus2 attributes", "kts_params");
	
	stream << "\t\t<p>" << endl;
	stream << "\t\t\t<strong>" << INDENT << "Product hierarchy: </strong>" <<
        KactusAttribute::hierarchyToString(component_->getHierarchy()) << "<br>" << endl;
	
	stream << "\t\t\t<strong>" << INDENT << "Component implementation: </strong>" <<
        KactusAttribute::implementationToString(component_->getImplementation()) << "<br>" << endl;

	stream << "\t\t\t<strong>" << INDENT << "Component firmness: </strong>" <<
        KactusAttribute::firmnessToString(component_->getFirmness()) << "<br>" << endl;

	stream << "\t\t</p>" << endl;
	++subHeaderNumber;

	if (component_->hasParameters())
    {
		writeSubHeader(subHeaderNumber, stream, "General parameters", "parameters");

		QStringList paramHeaders;
		paramHeaders.append("Name");
		paramHeaders.append("Value");
        paramHeaders.append("Array left");
        paramHeaders.append("Array right");
		paramHeaders.append("Description");
		writeTableElement(paramHeaders,
			"List of parameters defined for the component", stream);
		QSharedPointer<QList<QSharedPointer<Parameter> > > params = component_->getParameters();
		foreach (QSharedPointer<Parameter> param, *params) {
			stream << "\t\t\t\t<tr>" << endl;
			stream << "\t\t\t\t\t<td>" << param->name() << "</td>" << endl;
			stream << "\t\t\t\t\t<td>" << expressionFormatter_->formatReferringExpression(param->getValue()) <<
                "</td>" << endl;
            stream << "\t\t\t\t\t<td>" << expressionFormatter_->formatReferringExpression(param->getArrayLeft()) 
                << "</td>" << endl;
            stream << "\t\t\t\t\t<td>" << expressionFormatter_->formatReferringExpression(param->getArrayRight())
                << "</td>" << endl;
			stream << "\t\t\t\t\t<td>" << param->description() << "</td>" << endl;
			stream << "\t\t\t\t</tr>" << endl;
		}
		stream << "\t\t\t</table>" << endl;
		++subHeaderNumber;
	}
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeMemoryMaps()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeMemoryMaps(QTextStream& stream, int& subHeaderNumber)
{
    if (!component_->getMemoryMaps()->isEmpty())
    {
        writeSubHeader(subHeaderNumber, stream, "Memory maps", "memoryMaps");

        const QList<QSharedPointer<MemoryMap> > componentMemoryMaps = *component_->getMemoryMaps().data();
        int memoryMapNumber = 1;
        foreach (QSharedPointer<MemoryMap> memoryMap, componentMemoryMaps)
        {
            stream << "\t\t\t<h3><a id=\"" << component_->getVlnv().toString() << ".memoryMap." <<
                memoryMap->name() << "\">" << myNumber_ << "." << subHeaderNumber << "." <<
                memoryMapNumber << " " << memoryMap->name() << "</a></h3>" << endl;

            stream << "\t\t\t<p>" << endl;

            if (!memoryMap->description().isEmpty())
            {
                stream << "\t\t\t" << INDENT << "<strong>Description:</strong> " << memoryMap->description() <<
                    "<br>" << endl;
            }

            stream << "\t\t\t" << INDENT << "<strong>Address unit bits (AUB):</strong> " <<
                memoryMap->getAddressUnitBits() << "<br>" << endl;

            stream << "\t\t\t</p>" << endl;
            
            QList <QSharedPointer <AddressBlock> > addressBlocks;
            foreach (QSharedPointer<MemoryBlockBase> memoryMapItem, *memoryMap->getMemoryBlocks())
            {
                QSharedPointer <AddressBlock> addressItem = memoryMapItem.dynamicCast<AddressBlock>();

                if (addressItem)
                {
                    addressBlocks.append(addressItem);
                }
            }

            writeAddressBlocks(addressBlocks, stream, subHeaderNumber, memoryMapNumber);

            ++memoryMapNumber;
        }

        ++subHeaderNumber;
    }
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeAddressBlock()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeAddressBlocks(QList<QSharedPointer<AddressBlock> > addressBlocks, QTextStream& stream,
    int& subHeaderNumber, int& memoryMapNumber)
{
    if (!addressBlocks.isEmpty())
    {
        int addressBlockNumber = 1;

        foreach (QSharedPointer <AddressBlock> currentAddressBlock, addressBlocks)
        {
            stream << "\t\t\t<h3><a id=\"" << component_->getVlnv().toString() << ".addressBlock." <<
                currentAddressBlock->name() << "\">" << myNumber_ << "." << subHeaderNumber << "." <<
                memoryMapNumber << "." << addressBlockNumber << " " << currentAddressBlock->name() <<
                "</a></h3>" << endl;

            if (!currentAddressBlock->description().isEmpty())
            {
                stream << "\t\t\t<p>" << endl;
                stream << "\t\t\t" << INDENT << "<strong>Description:</strong> " <<
                    currentAddressBlock->description() << "<br>" << endl;
                stream << "\t\t\t</p>" << endl;
            }

            QStringList addressBlockHeaders;
            addressBlockHeaders << "Usage" << "Base address [AUB]" << "Range [AUB]" << "Width [AUB]" << "Access" <<
                "Volatile";
            QString title = "List of values in " + currentAddressBlock->name() + ".";
            writeTableElement(addressBlockHeaders, title, stream);

            stream << "\t\t\t\t<tr>" << endl;
            stream << "\t\t\t\t\t<td>" << General::usage2Str(currentAddressBlock->getUsage()) << "</td>" << endl;
            stream << "\t\t\t\t\t<td>" << expressionFormatter_->formatReferringExpression(
                currentAddressBlock->getBaseAddress()) << "</td>" << endl;
            stream << "\t\t\t\t\t<td>" << expressionFormatter_->formatReferringExpression(
                currentAddressBlock->getRange()) << "</td>" << endl;
            stream << "\t\t\t\t\t<td>" << expressionFormatter_->formatReferringExpression(
                currentAddressBlock->getWidth()) << "</td>" << endl;
            stream << "\t\t\t\t\t<td>" << AccessTypes::access2Str(currentAddressBlock->getAccess()) << "</td>" << endl;
            stream << "\t\t\t\t\t<td>" << currentAddressBlock->getVolatile() << "</td>" << endl;
            stream << "\t\t\t\t</tr>" << endl;
            stream << "\t\t\t</table>" << endl;

            QList <QSharedPointer <Register> > registers;
            foreach (QSharedPointer <RegisterBase> registerModelItem, *currentAddressBlock->getRegisterData())
            {
                QSharedPointer <Register> registerItem = registerModelItem.dynamicCast<Register>();

                if (registerItem)
                {
                    registers.append(registerItem);
                }
            }

            writeRegisters(registers, stream, subHeaderNumber, memoryMapNumber, addressBlockNumber);

            ++addressBlockNumber;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeRegisters()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeRegisters(QList<QSharedPointer<Register> > registers, QTextStream& stream,
    int& subHeaderNumber, int& memoryMapNumber, int& addressBlockNumber)
{
    if (!registers.isEmpty())
    {
        int registerNumber = 1;

        foreach (QSharedPointer<Register> currentRegister, registers)
        {
            stream << "\t\t\t<h3><a id=\"" << component_->getVlnv().toString() << ".register." <<
                currentRegister->name() << "\">" << myNumber_ << "." << subHeaderNumber << "." <<
                memoryMapNumber << "." << addressBlockNumber << "." << registerNumber << " " <<
                currentRegister->name() << "</a></h3>" << endl;

            if (!currentRegister->description().isEmpty())
            {
                stream << "\t\t\t<p>" << endl;
                stream << "\t\t\t" << INDENT << "<strong>Description:</strong> " <<
                    currentRegister->description() << "<br>" << endl;
                stream << "\t\t\t</p>" << endl;
            }

            QStringList registerHeaders;
            registerHeaders << "Offset [AUB]" << "Size [bits]" << "Dimension" << "Volatile" << "Access" <<
                "Reset value" << "Reset mask";
            QString title = "List of values in " + currentRegister->name() + ".";
            writeTableElement(registerHeaders, title, stream);

            stream << "\t\t\t\t<tr>" << endl;
            stream << "\t\t\t\t\t<td>" << expressionFormatter_->formatReferringExpression(
                currentRegister->getAddressOffset()) << "</td>" << endl;
            stream << "\t\t\t\t\t<td>";
            if (!currentRegister->getSize().isEmpty())
            {
                stream << expressionFormatter_->formatReferringExpression(currentRegister->getSize());
            }
            else
            {
                stream << currentRegister->getSize();
            }
            stream << "</td>" << endl;
            stream << "\t\t\t\t\t<td>";
            if (!currentRegister->getDimension().isEmpty())
            {
                stream << expressionFormatter_->formatReferringExpression(currentRegister->getDimension());
            }
            else
            {
                stream << currentRegister->getDimension();
            }
            stream << "</td>" << endl;
            stream << "\t\t\t\t\t<td>" << currentRegister->getVolatile() << "</td>" << endl;
            stream << "\t\t\t\t\t<td>" << AccessTypes::access2Str(currentRegister->getAccess()) << "</td>" << endl;
            stream << "\t\t\t\t</tr>" << endl;
            stream << "\t\t\t</table>" << endl;

            writeFields(currentRegister, stream);

            ++registerNumber;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeFields()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeFields(QSharedPointer<Register> currentRegister, QTextStream& stream)
{
    if (!currentRegister->getFields()->isEmpty())
    {
        stream << "\t\t\t<h4>Register " << currentRegister->name() << " contains the following fields:</h4>" <<
            endl;

        QStringList fieldHeaders;
        fieldHeaders << "Field name" << "Offset [bits]" << "Width [bits]" << "Volatile" << "Access" <<
            "Description";
        QString title = "List of fields contained within register " + currentRegister->name() + ".";
        writeTableElement(fieldHeaders, title, stream);

        foreach (QSharedPointer<Field> currentField, *currentRegister->getFields())
        {
            stream << "\t\t\t\t<tr>" << endl;
            stream << "\t\t\t\t\t<td><a id=\"" << component_->getVlnv().toString() << ".field." <<
                currentField->name() << "\">" << currentField->name() << "</a></td>" << endl;
            stream << "\t\t\t\t\t<td>";
            if (!currentField->getBitOffset().isEmpty())
            {
                stream << expressionFormatter_->formatReferringExpression(currentField->getBitOffset());
            }
            else
            {
                stream << currentField->getBitOffset();
            }
            stream << "</td>" << endl;
            stream << "\t\t\t\t\t<td>";
            if (!currentField->getBitWidth().isEmpty())
            {
                stream << expressionFormatter_->formatReferringExpression(currentField->getBitWidth());
            }
            else
            {
                stream << currentField->getBitWidth();
            }
            stream << "</td>" << endl;
            stream << "\t\t\t\t\t<td>" << currentField->getVolatile().toString()  << "</td>" << endl;
            stream << "\t\t\t\t\t<td>" << AccessTypes::access2Str(currentField->getAccess()) << "</td>" << endl;
            stream << "\t\t\t\t\t<td>" << currentField->description() << "</td>" << endl;
            stream << "\t\t\t\t</tr>" << endl;
        }

        stream << "\t\t\t</table>" << endl;
    }
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writePorts()
//-----------------------------------------------------------------------------
void DocumentGenerator::writePorts(QTextStream& stream, int& subHeaderNumber)
{
	if (component_->hasPorts()) {
		writeSubHeader(subHeaderNumber, stream, "Ports", "ports");

        QString tableTitle = "List of all ports the component has.";
        const QList<QSharedPointer<Port> > ports = *component_->getPorts().data();

        writePortTable(stream, tableTitle, ports);

        ++subHeaderNumber;
	}
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeInterfaces()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeInterfaces(QTextStream& stream, int& subHeaderNumber)
{
	if (component_->hasInterfaces()) {
		
		writeSubHeader(subHeaderNumber, stream, "Bus interfaces", "interfaces");

		int interfaceNumber = 1;
		foreach (QSharedPointer<BusInterface> interface,  *component_->getBusInterfaces())
        {
			stream << "\t\t\t" << "<h3>" << myNumber_ << "." << subHeaderNumber << "." << interfaceNumber++ <<
				" " << interface->name() << "</h3>" << endl;
			
			stream << "\t\t\t<p>" << endl;

			if (!interface->description().isEmpty()) {
				stream << "\t\t\t" << INDENT << "<strong>Description:</strong> " << 
					interface->description() << "<br>" << endl;
			}

			// write the interface mode of the interface
			stream << "\t\t\t" << INDENT << "<strong>Interface mode:</strong> " << 
				General::interfaceMode2Str(interface->getInterfaceMode()) << "<br>" << endl;

			stream << "\t\t\t" << INDENT << "<strong>Ports used in this interface:</strong>" << endl;

			stream << "\t\t\t</p>" << endl;
			
            QString tableTitle ("List of ports contained in interface " + (interface->name()) + ".");
            const QList<QSharedPointer<Port> > ports = component_->getPorts(interface->name());
            writePortTable(stream, tableTitle, ports);
		}
		++subHeaderNumber;
	}
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeFileSets()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeFileSets(QTextStream& stream, int& subHeaderNumber)
{
	if (component_->hasFileSets()) {
		
		writeSubHeader(subHeaderNumber, stream, "File sets", "fileSets");

		const QList<QSharedPointer<FileSet> > fileSets = *component_->getFileSets().data();
		int fileSetNumber = 1;
		foreach (QSharedPointer<FileSet> fileSet, fileSets)
        {
			stream << "\t\t\t" << "<h3><a id=\"" << component_->getVlnv().toString() 
				<< ".fileSet." << fileSet->name() << "\">" << myNumber_ << "." << subHeaderNumber << 
				"." << fileSetNumber <<	" " << fileSet->name() << "</a></h3>" << endl;

			stream << "\t\t\t<p>" << endl;

			if (!fileSet->description().isEmpty())
            {
				stream << "\t\t\t" << INDENT << "<strong>Description:</strong> " << 
					fileSet->description() << "<br>" << endl;
			}

			// write the group identifiers
			QStringList groups = *fileSet->getGroups().data();
			stream << "\t\t\t" << INDENT << "<strong>Identifiers:</strong> ";
			for (int i = 0; i < groups.size(); ++i)
            {
				stream << groups.at(i);

				if (i != groups.size()-1)
					stream << ", ";
			}
			stream << "<br>" << endl;

			// if file set has specified default file builders
			QList<QSharedPointer<FileBuilder> > fileBuilders = *fileSet->getDefaultFileBuilders().data();
			if (!fileBuilders.isEmpty()) {	
				stream << "\t\t\t" << INDENT << "<strong>Default file builders:</strong>" << endl;
				stream << "\t\t\t</p>" << endl;

				QStringList builderHeaders;
				builderHeaders.append("File type");
				builderHeaders.append("Command");
				builderHeaders.append("Flags");
				builderHeaders.append("Replace default flags");
				writeTableElement(builderHeaders, "The build settings for files in this file set.",
					stream);

				foreach (QSharedPointer<FileBuilder> fileBuilder, fileBuilders) {
					stream << "\t\t\t\t<tr>" << endl;
					stream << "\t\t\t\t\t<td>" << fileBuilder->getFileType() << "</td>" << endl;
					stream << "\t\t\t\t\t<td>" << fileBuilder->getCommand() << "</td>" << endl;
					stream << "\t\t\t\t\t<td>" << fileBuilder->getFlags() << "</td>" << endl;
					stream << "\t\t\t\t\t<td>" << 
                        fileBuilder->getReplaceDefaultFlags() << "</td>" << endl;
					stream << "\t\t\t\t</tr>" << endl;
				}
				stream << "\t\t\t</table>" << endl;
			}
			else {
				stream << "\t\t\t</p>" << endl;
			}

            if (!fileSet->getFiles()->isEmpty())
            {
                stream << "\t\t\t" << "<h4>" << INDENT << myNumber_ << "." << subHeaderNumber <<
                    "." << fileSetNumber << ".1 Files</h4>" << endl;

                QStringList fileHeaders;
                fileHeaders.append("File name");
                fileHeaders.append("Logical name");
                fileHeaders.append("Build command");
                fileHeaders.append("Build flags");
                fileHeaders.append("Specified file types");
                fileHeaders.append("Description");
                writeTableElement(fileHeaders, "List of files contained in this file set.", stream);

                // get the files contained in this file set.
                const QList<QSharedPointer<File> > files = *fileSet->getFiles().data();
                foreach (QSharedPointer<File> file, files)
                {
                    writeFile(file, stream);
                }
                stream << "\t\t\t</table>" << endl;
            }
            ++fileSetNumber;
		}
		++subHeaderNumber;
	}
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeViews()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeViews( QTextStream& stream, int& subHeaderNumber, QStringList& pictureList )
{
	if (component_->hasViews())
    {
		stream << "\t\t\t<h2><a id=\"" << component_->getVlnv().toString() << ".views\">"
			<< myNumber_ << "." << subHeaderNumber << " Views</a></h2>" << endl;
		
		int viewNumber = 1;
		foreach (QSharedPointer<View> view, *component_->getViews())
        {
			stream << "\t\t\t<h3>" << myNumber_ << "." << subHeaderNumber <<
				"." << viewNumber << " View: " << view->name() << "</h3>" << endl;

			stream << "\t\t\t<p>" << endl;

			bool isHierarchicalView = view->isHierarchical();
			if (isHierarchicalView)
            {
				createDesignPicture(pictureList, view->name());

				stream << "\t\t\t<img src=\"" << component_->getVlnv().toString(".")
					<< "." << view->name() << ".png\" alt=\"" << "View: " <<
					view->name() << " preview picture\"><br>" << endl;
			}

			stream << "\t\t\t<strong>" << INDENT << "Type: </strong>";
			
			if (isHierarchicalView)
            {
				stream << "hierarchical" << "<br>" << endl;
			}
			else 
            {
				stream << "non-hierarchical" << "<br>" << endl;
			}

			// if view has description
			if (!view->description().isEmpty()) {
				stream << "\t\t\t<strong>Description: </strong>" << view->description() 
					<< "<br>" << endl;
			}

			// if view is hierarchical
			if (isHierarchicalView) {
				stream << "\t\t\t</p>" << endl;

				writeView(view, stream, subHeaderNumber, viewNumber);
			}

			// if is flat view
			else
            {
                if (!view->getComponentInstantiationRef().isEmpty())
                {
                    foreach (QSharedPointer<ComponentInstantiation> instantiation,
                        *component_->getComponentInstantiations())
                    {
                        if (instantiation->name() == view->getComponentInstantiationRef())
                        {
                            QStringList fileSetRefs = *instantiation->getFileSetReferences().data();
                            if (!fileSetRefs.isEmpty())
                            {
                                stream << "\t\t\t<strong>" << INDENT << "File sets contained in this view: </strong>" << endl;

                                stream << "\t\t\t</p>" << endl;

                                stream << "\t\t\t<ul>" << endl;
                                foreach (QString fileSetRef, fileSetRefs)
                                {
                                    stream << "\t\t\t\t<li><a href=\"#" << component_->getVlnv().toString() <<
                                        ".fileSet." << fileSetRef << "\">" << fileSetRef << "</a></li>" << endl;
                                }
                                stream << "\t\t\t</ul>" << endl;
                            }
                            break;
                        }
                    }
                }
			}
			++viewNumber;
		}
		++subHeaderNumber;
	}
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeView()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeView( QSharedPointer<View> view, QTextStream& stream, const int subheaderNumber, 
								  const int viewNumber )
{
    VLNV vlnv;

    if (!view->getDesignConfigurationInstantiationRef().isEmpty())
    {
        foreach (QSharedPointer<DesignConfigurationInstantiation> instantiation,
            *component_->getDesignConfigurationInstantiations())
        {
            if (instantiation->name() == view->getDesignConfigurationInstantiationRef())
            {
                vlnv = *instantiation->getDesignConfigurationReference().data();
                break;
            }
        }
    }

	else if (!view->getDesignInstantiationRef().isEmpty())
	{
        foreach (QSharedPointer<DesignInstantiation> instantiation, *component_->getDesignInstantiations())
        {
            if (instantiation->name() == view->getDesignInstantiationRef())
            {
                vlnv = *instantiation->getDesignReference().data();
                break;
            }
        }
	}

	// if vlnv was not found in library
	if (!handler_->contains(vlnv))
    {
		QString errorMsg(tr("VLNV: %1:%2:%3:%4 was not found in library.").arg(
			vlnv.getVendor()).arg(
			vlnv.getLibrary()).arg(
			vlnv.getName()).arg(
			vlnv.getVersion()));

		stream << "\t\t\t<p>" << endl;
		stream << "\t\t\t<strong>" << errorMsg << "</strong><br>" << endl;
		stream << "\t\t\t</p>" << endl;
		emit errorMessage(errorMsg);
		return;
	}

    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desConf;
    VLNV designVLNV;
    VLNV desConfVLNV;

	QSharedPointer<Document> libComp = handler_->getModel(vlnv);
	
	// if the hierarchical reference was for design configuration
	if (handler_->getDocumentType(vlnv) == VLNV::DESIGNCONFIGURATION) {
		desConf = libComp.staticCast<DesignConfiguration>();
		desConfVLNV = desConf->getVlnv();

		designVLNV = desConf->getDesignRef();
		// if design vlnv was not found in library
		if (!handler_->contains(designVLNV)) {
			QString errorMsg(tr("VLNV: %1:%2:%3:%4 was not found in library.").arg(
				designVLNV.getVendor()).arg(designVLNV.getLibrary()).arg(designVLNV.getName()).arg(
				designVLNV.getVersion()));

			stream << "\t\t\t<p>" << endl;
			stream << "\t\t\t<strong>" << errorMsg << "</strong><br>" << endl;
			stream << "\t\t\t</p>" << endl;
			emit errorMessage(errorMsg);
			return;
		}
		// if the vlnv is not for design
		else if (handler_->getDocumentType(designVLNV) != VLNV::DESIGN) {
			QString errorMsg(tr("VLNV: %1:%2:%3:%4 was not for design.").arg(
				designVLNV.getVendor()).arg(designVLNV.getLibrary()).arg(designVLNV.getName()).arg(
				designVLNV.getVersion()));

			stream << "\t\t\t<p>" << endl;
			stream << "\t\t\t<strong>" << errorMsg << "</strong><br>" << endl;
			stream << "\t\t\t</p>" << endl;
			emit errorMessage(errorMsg);
			return;
		}
		
		libComp = handler_->getModel(designVLNV);
		design = libComp.staticCast<Design>();
		designVLNV = design->getVlnv();
	}

	// if the hierarchical reference was directly for a design
	else if (handler_->getDocumentType(vlnv) == VLNV::DESIGN) {
		libComp = handler_->getModel(vlnv);
		design = libComp.staticCast<Design>();
		designVLNV = design->getVlnv();
	}
	
	// if hierarchical reference was for wrong type
	else {
		QString errorMsg(tr("VLNV: %1:%2:%3:%4 was not for design or design configuration.").arg(
			vlnv.getVendor()).arg(vlnv.getLibrary()).arg(vlnv.getName()).arg(
			vlnv.getVersion()));

		stream << "\t\t\t<p>" << endl;
		stream << "\t\t\t<strong>" << errorMsg << "</strong><br>" << endl;
		stream << "\t\t\t</p>" << endl;
		emit errorMessage(errorMsg);
		return;
	}

	// if design configuration was used
	if (desConf) {
		stream << "\t\t\t<h4>" << myNumber_ << "." << subheaderNumber << "." <<
			viewNumber << ".1 Configuration " << desConfVLNV.toString(" - ") << 
			"</h4>" << endl;
		
		stream << "\t\t\t<p>" << endl;
		// if configuration has a description
		if (!desConf->getDescription().isEmpty()) {
			stream << "\t\t\t<strong>" << INDENT << "Description: </strong>" << desConf->getDescription() <<
				"<br>" << endl;
		}

		// print relative path to the configuration xml file
		QFileInfo confXmlInfo(handler_->getPath(desConfVLNV));
		QString relativeXmlPath = General::getRelativePath(targetPath_, confXmlInfo.absoluteFilePath());
		stream << "\t\t\t<strong>" << INDENT << "IP-Xact file: </strong><a href=\"" << 
			relativeXmlPath << "\">" << confXmlInfo.fileName() <<
			"</a><br>" << endl;
		stream << "\t\t\t<br>" << endl;
		
		stream << "\t\t\t</p>" << endl;
	}

	Q_ASSERT(design);

	stream << "\t\t\t<h4>" << myNumber_ << "." << subheaderNumber << "." <<
		viewNumber << ".2 Design " << designVLNV.toString(" - ") << "</h4>" << endl;
	
	stream << "\t\t\t<p>" << endl;

	// if design contains a description
	if (!design->getDescription().isEmpty()) {
		stream << "\t\t\t<strong>" << INDENT << "Description: </strong>" << design->getDescription() <<
			"<br>" << endl;
	}

	// print relative path to the design xml file
	QFileInfo designXmlInfo(handler_->getPath(designVLNV));
	QString relativeXmlPath = General::getRelativePath(targetPath_, designXmlInfo.absoluteFilePath());
	stream << "\t\t\t<strong>" << INDENT << "IP-Xact file: </strong><a href=\"" << 
		relativeXmlPath << "\">" << designXmlInfo.fileName() <<
		"</a><br>" << endl;
	stream << "\t\t\t<br>" << endl;

	stream << "\t\t\t</p>" << endl;

	QSharedPointer<QList<QSharedPointer<ComponentInstance> > > instances = design->getComponentInstances();
	
	// if design does not contain instances
	if (instances->isEmpty()) {
		return;
	}

	QStringList instanceHeaders;
	instanceHeaders.append("Instance name");
	instanceHeaders.append("Component type");
	instanceHeaders.append("Configurable values");
	instanceHeaders.append("Active view");
	instanceHeaders.append("Description");
	writeTableElement(instanceHeaders, "Component instantiations within this design", stream);

	foreach (QSharedPointer<ComponentInstance> instance, *instances)
    {
		stream << "\t\t\t\t<tr>" << endl;

		stream << "\t\t\t\t\t<td>" << instance->getInstanceName() << "</td>" << endl;
		stream << "\t\t\t\t\t<td><a href=\"#" << instance->getComponentRef()->toString(":") 
			<< "\">" << instance->getComponentRef()->toString(" - ") << "</a></td>" << endl;
		
        VLNV tempVlnv = *instance->getComponentRef();

        QSharedPointer<Document> libComp = handler_->getModel(tempVlnv);
        QSharedPointer<Component> component = libComp.staticCast<Component>();

        ExpressionFormatter* equationFormatter = expressionFormatterFactory_->makeExpressionFormatter(component);

		//QMap<QString, QString> confElements = instance->getConfigurableElementValues();
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > >  confElements = instance->getConfigurableElementValues();

        stream << "\t\t\t\t\t<td>" << endl;
        foreach(QSharedPointer<ConfigurableElementValue> configurableElement, *confElements)
        {
            stream << "\t\t\t\t\t" 
                << equationFormatter->formatReferringExpression(configurableElement->getReferenceId()) 
                << " = " 
                << equationFormatter->formatReferringExpression(configurableElement->getConfigurableValue());

               
            if (configurableElement != confElements->last())
            {
                stream << "<br>";
            }
            stream << endl;
        }

        stream << "\t\t\t\t\t</td>" << endl;


		stream << "\t\t\t\t\t<td>";
		if (desConf) {
			stream << desConf->getActiveView(instance->getInstanceName());
		}
		stream << "</td>" << endl;

		stream << "\t\t\t\t\t<td>" << instance->getDescription() << "</td>" << endl;

		stream << "\t\t\t\t</tr>" << endl;
	}

	stream << "\t\t\t</table>" << endl; 
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeEndOfDocument()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeEndOfDocument(QTextStream& stream)
{
    stream << VALID_W3C_STRICT << endl;
    stream << "\t</body>" << endl;
    stream << "</html>" << endl;
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeSubHeader()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeSubHeader( const int headerNumber, 
									   QTextStream& stream, 
									   const QString& text,
									   const QString& headerID)
{
   stream << "\t\t<h2><a id=\"" << component_->getVlnv().toString() << "." <<
	   headerID << "\">" << myNumber_ << "." << headerNumber << " " << text <<
	   "</a></h2>" << endl;
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writePortTable()
//-----------------------------------------------------------------------------
void DocumentGenerator::writePortTable(QTextStream& stream, QString const& title,
    QList <QSharedPointer <Port> > ports)
{
    QStringList portHeaders;
    portHeaders << "Name" << "Direction" << "Left bound" << "Right bound" << "Port type" <<
        "Type definition" << "Default value" << "Array left" << "Array right" << "Description";
    writeTableElement(portHeaders, title, stream);

    foreach (QSharedPointer<Port> port, ports)
    {
        stream << "\t\t\t\t<tr>" << endl;
        stream << "\t\t\t\t\t<td><a id=\"" << 
            component_->getVlnv().toString() << ".port." << port->name() << 
            "\">" <<  port->name() << "</a></td>" << endl;
        stream << "\t\t\t\t\t<td>" << DirectionTypes::direction2Str(port->getDirection()) << "</td>" << endl;
//         stream << "\t\t\t\t\t<td>" << port->getPortSize() << "</td>" << endl;
        stream << "\t\t\t\t\t<td>" << expressionFormatter_->formatReferringExpression(
            port->getLeftBound()) << "</td>" << endl;
        stream << "\t\t\t\t\t<td>" << expressionFormatter_->formatReferringExpression(
            port->getRightBound()) << "</td>" << endl;

        stream << "\t\t\t\t\t<td>" << port->getTypeName() << "</td>" << endl;
        stream << "\t\t\t\t\t<td>" << port->getTypeDefinition(port->getTypeName()) << "</td>" << endl;
        stream << "\t\t\t\t\t<td>" << expressionFormatter_->formatReferringExpression(port->getDefaultValue())
            << "</td>" << endl;
        stream << "\t\t\t\t\t<td>" << expressionFormatter_->formatReferringExpression(port->getArrayLeft()) 
            << "</td>" << endl;
        stream << "\t\t\t\t\t<td>" << expressionFormatter_->formatReferringExpression(port->getArrayRight())
            << "</td>" << endl;
        stream << "\t\t\t\t\t<td>" << port->description() << "</td>" << endl;
        stream << "\t\t\t\t</tr>" << endl;
    }

    stream << "\t\t\t</table>" << endl;
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeTableElement()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeTableElement( const QStringList& headers, 
										  const QString& title,
										  QTextStream& stream )
{
	stream << "\t\t\t" << TABLE << title << "\">" << endl; 
	stream << "\t\t\t\t<tr>" << endl;
	foreach (QString header, headers) {
		stream << "\t\t\t\t\t<th>" << header << "</th>" << endl;
	}
	stream << "\t\t\t\t</tr>\n";
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeFile()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeFile( QSharedPointer<File> file,
								  QTextStream& stream )
{
	QString relativeFilePath = file->name();
	QString absFilePath = General::getAbsolutePath(handler_->getPath(component_->getVlnv()), relativeFilePath);
	QFileInfo fileInfo(absFilePath);

	// get relative path from html file to the file
	QFileInfo htmlInfo(targetPath_);
	QString pathFromhtmlToFile = General::getRelativePath(targetPath_, absFilePath);

    QSharedPointer<BuildCommand> buildCommand = file->getBuildCommand();

	QStringList fileTypes = *file->getFileTypes().data();

	stream << "\t\t\t\t<tr>" << endl;
	stream << "\t\t\t\t\t<td><a href=\"" << pathFromhtmlToFile << "\">" << 
		fileInfo.fileName() << "</a></td>" << endl;
	stream << "\t\t\t\t\t<td>" << file->getLogicalName() << "</td>" << endl;
	
	stream << "\t\t\t\t\t<td>";
	if (buildCommand)
    {
		stream << buildCommand->getCommand();
	}
	stream << "</td>" << endl;

	stream << "\t\t\t\t\t<td>";
	if (buildCommand)
    {
		stream << buildCommand->getFlags();
	}
	stream << "</td>" << endl;
	
	stream << "\t\t\t\t\t<td>";
	for (int i = 0; i < fileTypes.size(); ++i)
    {
		stream << fileTypes.at(i);
		if (i != fileTypes.size() - 1)
        {
			stream << ",<br>";
		}
	}
	stream << "</td>" << endl;
	
	stream << "\t\t\t\t\t<td>" << file->getDescription() << "</td>" << endl;
	stream << "\t\t\t\t</tr>" << endl;
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::createComponentPicture()
//-----------------------------------------------------------------------------
void DocumentGenerator::createComponentPicture(QStringList& pictureList)
{
	ComponentPreviewBox compBox(handler_);
	compBox.hide();
	compBox.setComponent(component_);

	QFileInfo htmlInfo(targetPath_);
	QString compPicPath = htmlInfo.absolutePath(); 
	compPicPath += "/";
	compPicPath += component_->getVlnv().toString(".");
	compPicPath += ".png";

	QFile compPicFile(compPicPath);

	if (compPicFile.exists())
    {
		compPicFile.remove();
	}

	// get the rect that bounds all items on box
	QRectF boundingRect = compBox.itemsBoundingRect();
	boundingRect.setHeight(boundingRect.height() + 2);
	boundingRect.setWidth(boundingRect.width() + 2);

	// set the size of the picture
	QPixmap compPic(boundingRect.size().toSize());

	// create the picture for the component
	QPainter painter(&compPic);
	painter.fillRect(compPic.rect(), QBrush(Qt::white));
	compBox.scene()->render(&painter, compPic.rect(), boundingRect.toRect());
	if (!compPic.save(&compPicFile, "PNG")) {
		emit errorMessage(tr("Could not save picture %1").arg(compPicPath));
	}
	else {
		pictureList.append(compPicPath);
	}
	
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::createDesignPicture()
//-----------------------------------------------------------------------------
void DocumentGenerator::createDesignPicture( QStringList& pictureList, const QString& viewName )
{
    DesignWidget* designWidget (designWidgetFactory_->makeHWDesignWidget());

    designWidget->hide();
    designWidget->setDesign(component_->getVlnv(), viewName);

	QFileInfo htmlInfo(targetPath_);
	QString designPicPath = htmlInfo.absolutePath(); 
	designPicPath += "/";
	designPicPath += component_->getVlnv().toString(".");
	designPicPath += ".";
	designPicPath += viewName;
	designPicPath += ".png";

	QFile designPicFile(designPicPath);

	if (designPicFile.exists())
    {
		designPicFile.remove();
	}

	// get the rect that bounds all items on box
    QRectF boundingRect = designWidget->getDiagram()->itemsBoundingRect();
	boundingRect.setHeight(boundingRect.height() + 2);
	boundingRect.setWidth(boundingRect.width() + 2);

	// set the size of the picture
	QPixmap designPic(boundingRect.size().toSize());

	// create the picture for the component
	QPainter painter(&designPic);
	painter.fillRect(designPic.rect(), QBrush(Qt::white));
    designWidget->getDiagram()->render(&painter, designPic.rect(), boundingRect.toRect());
	if (!designPic.save(&designPicFile, "PNG"))
    {
		emit errorMessage(tr("Could not save picture %1").arg(designPicPath));
	}
	else
    {
		pictureList.append(designPicPath);
	}
}
