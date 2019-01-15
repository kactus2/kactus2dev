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

#include <common/utils.h>
#include <common/widgets/componentPreviewBox/ComponentPreviewBox.h>

#include <kactusGenerators/DocumentGenerator/ViewDocumentGenerator.h>
#include <kactusGenerators/DocumentGenerator/DocumentGeneratorHTML.h>

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
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/MemoryMap.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

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
#include <QApplication>

//-----------------------------------------------------------------------------
// Function: documentgenerator::DocumentGenerator()
//-----------------------------------------------------------------------------
DocumentGenerator::DocumentGenerator(LibraryInterface* handler, const VLNV& vlnv,
    DesignWidgetFactory* designWidgetFactory, ExpressionFormatterFactory* expressionFormatterFactory,
    QWidget* parent): 
GeneralDocumentGenerator(handler, expressionFormatterFactory, parent),
childInstances_(),
parentWidget_(parent),
expressionFormatter_(),
viewDocumentationGenerator_(new ViewDocumentGenerator(handler, expressionFormatterFactory, designWidgetFactory))
{
	Q_ASSERT(handler);
	Q_ASSERT(parent);

    // this function can be called for only the top document generator
    Q_ASSERT(parentWidget_);

    connect(viewDocumentationGenerator_, SIGNAL(errorMessage(QString const&)),
        this, SIGNAL(errorMessage(QString const&)), Qt::UniqueConnection);
    connect(viewDocumentationGenerator_, SIGNAL(noticeMessage(QString const&)),
        this, SIGNAL(noticeMessage(QString const&)), Qt::UniqueConnection);

    // parse the model for the component

    QSharedPointer<Component> component = getLibraryHandler()->getModel(vlnv).dynamicCast<Component>();
    setComponent(component);

    if (!component)
    {
        emit errorMessage("VLNV was not found in the library.");
    }
    else
    {
        expressionFormatter_ = createExpressionFormatter();

        // list of objects that have already been processed to avoid duplicates
        QList<VLNV> objects;
        objects.append(vlnv);

        parseChildItems(objects);
    }
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::DocumentGenerator()
//-----------------------------------------------------------------------------
DocumentGenerator::DocumentGenerator(LibraryInterface* handler, const VLNV& vlnv, QList<VLNV>& objects,
    ExpressionFormatterFactory* expressionFormatterFactory, ViewDocumentGenerator* viewDocumentationGenerator,
    DocumentGenerator* parent):
GeneralDocumentGenerator(handler, expressionFormatterFactory, parent),
childInstances_(),
parentWidget_(NULL),
expressionFormatter_(),
viewDocumentationGenerator_(viewDocumentationGenerator)
{
    Q_ASSERT(getLibraryHandler());
	Q_ASSERT(parent);
    Q_ASSERT(getLibraryHandler()->contains(vlnv));
    Q_ASSERT(getLibraryHandler()->getDocumentType(vlnv) == VLNV::COMPONENT);

	connect(this, SIGNAL(errorMessage(const QString&)),
		parent, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(noticeMessage(const QString&)),
		parent, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    QSharedPointer<Document> libComp = getLibraryHandler()->getModel(vlnv);
    setComponent(libComp.staticCast<Component>());

    expressionFormatter_ = createExpressionFormatter();

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
    QList<VLNV> refs = getComponent()->getHierRefs();
    foreach (VLNV ref, refs)
    {
        QSharedPointer<Design> design = getLibraryHandler()->getDesign(ref);
		if (!design)
        {
			continue;
        }

		foreach (QSharedPointer<ComponentInstance> instance, *design->getComponentInstances())
        {
            if (!objects.contains(*instance->getComponentRef()) &&
                getLibraryHandler()->contains(*instance->getComponentRef()) &&
                getLibraryHandler()->getDocumentType(*instance->getComponentRef()) == VLNV::COMPONENT)
            {
    			// create a new instance of document generator and add it to child list
			    objects.append(*instance->getComponentRef());
                QSharedPointer<DocumentGenerator> docGenerator(new DocumentGenerator(getLibraryHandler(),
                    *instance->getComponentRef(), objects, getExpressionFormatterFactory(),
                    viewDocumentationGenerator_, this));
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
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    QSharedPointer<Component> component = getComponent();
    if (!component)
    {
        emit errorMessage("VLNV was not found in the library.");
        return;
    }

    Q_ASSERT(getLibraryHandler()->contains(component->getVlnv()));
    Q_ASSERT(getLibraryHandler()->getDocumentType(component->getVlnv()) == VLNV::COMPONENT);

	// this function can only be called for the top document generator
	Q_ASSERT(parentWidget_);

    setTargetPath(targetPath);

    writeHtmlHeader(stream);

	// create a running number to create the numbered headers for table of contents
	unsigned int runningNumber = 0;

	stream << "\t\t<p>" << endl;
	stream << "\t\t<strong>Table of contents</strong><br>" << endl;
	writeTableOfContents(runningNumber, stream);
	stream << "\t\t</p>" << endl;

	QStringList pictureList;

	// write the actual documentation for the top component
    writeDocumentation(stream, getTargetPath(), pictureList);

    writeEndOfDocument(stream);

    QApplication::restoreOverrideCursor();

	// ask user if he wants to save the generated documentation into object metadata
	QMessageBox::StandardButton button = QMessageBox::question(parentWidget_, 
		tr("Save generated documentation to metadata?"),
		tr("Would you like to save the generated documentation to IP-Xact"
		" metadata?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

	// if the generated file is saved
	if (button == QMessageBox::Yes)
    {
        QString xmlPath = getLibraryHandler()->getPath(component->getVlnv());

		// get the relative path to add to file set
        QString relativePath = General::getRelativePath(xmlPath, getTargetPath());

		QString fileSetName("Documentation");
		QSharedPointer<FileSet> documentationFileSet = component->getFileSet(fileSetName);

		// if the documentation file set was not found. Create one
		if (!documentationFileSet)
        {
			documentationFileSet = QSharedPointer<FileSet>(new FileSet(fileSetName, QString("documentation")));
            component->getFileSets()->append(documentationFileSet);
		}

		QSettings settings;

		// create a new file and add it to file set
		QSharedPointer<File> docFile = documentationFileSet->addFile(relativePath, settings);
		Q_ASSERT(docFile);
		docFile->setIncludeFile(false);
		docFile->setDescription(tr("Html file that contains the documentation "
			"for this component and subcomponents"));

		// add all created pictures to the file set
		foreach (QString pictureName, pictureList)
        {
			QString relativePicPath = General::getRelativePath(xmlPath, pictureName);

			QSharedPointer<File> picFile = documentationFileSet->addFile(relativePicPath, settings);
			Q_ASSERT(picFile);
			picFile->setIncludeFile(false);
			picFile->setDescription(tr("Preview picture needed by the html document."));
		}

        getLibraryHandler()->writeModelToFile(component);
	}
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeHtmlHeader()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeHtmlHeader(QTextStream& stream)
{
    QSettings settings;

    // write the top of the html document
    stream << DocumentGeneratorHTML::docType() << endl;
    stream << "<html>" << endl;
    stream << "\t<head>" << endl;
    stream << "\t" << DocumentGeneratorHTML::encoding() << endl;
    stream << "\t\t<title>Kactus2 generated documentation for component " <<
        getComponent()->getVlnv().getName() << " " << getComponent()->getVlnv().getVersion() << "</title>" << endl;
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
    setNumber(componentNumber);

    QSharedPointer<Component> component = getComponent();
    QString vlnvHeader = "\t\t" + DocumentGeneratorHTML::indent() + "<a href=\"#" + component->getVlnv().toString();

    stream << "\t\t<a href=\"#" << component->getVlnv().toString() << "\">" << myNumber() << ". Component" <<
        DocumentGeneratorHTML::space() << component->getVlnv().toString(" - ") << "</a><br>" << endl;

	// subHeader is running number that counts the number of sub headers for component
	int subHeader = 1;


	// component always has kactus parameters
    stream << vlnvHeader << ".kts_params\">" << myNumber() << "." << subHeader << ". Kactus2 attributes</a><br>" <<
        endl;
	++subHeader;

	if (component->hasParameters())
    {
        stream << vlnvHeader << ".parameters\">" << myNumber() << "." << subHeader <<
            ". General parameters</a><br>" << endl;
		++subHeader;
	}

    if (!component->getMemoryMaps()->isEmpty())
    {
        stream << vlnvHeader << ".memoryMaps\">" << myNumber() << "." << subHeader << ". Memory maps</a><br>" <<
            endl;
        ++subHeader;
    }

	if (component->hasPorts())
    {
        stream << vlnvHeader << ".ports\">" << myNumber() << "." << subHeader << ". Ports</a><br>" << endl;
		++subHeader;
	}

	if (component->hasInterfaces())
    {
        stream << vlnvHeader << ".interfaces\">" << myNumber() << "." << subHeader << ". Bus interfaces</a><br>" <<
            endl;
		++subHeader;
	}

	if (component->hasFileSets())
    {
        stream << vlnvHeader << ".fileSets\">" << myNumber() << "." << subHeader << ". File sets</a><br>" << endl;
		++subHeader;
	}

	if (component->hasViews())
    {
        stream << vlnvHeader << ".views\">" << myNumber() << "." << subHeader << ". Views</a><br>" << endl;
	}

	// tell each child to write it's table of contents
	foreach (QSharedPointer<DocumentGenerator> generator, childInstances_)
    {
		generator->writeTableOfContents(componentNumber, stream);
	}
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeDocumentation()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeDocumentation(QTextStream& stream, const QString& targetPath,
    QStringList& filesToInclude)
{
	setTargetPath(targetPath);

    QSharedPointer<Component> component = getComponent();

	// write the component header
	stream << "\t\t<h1><a id=\"" << component->getVlnv().toString() << "\">" << myNumber() << ". Component " <<
        component->getVlnv().toString(" - ") << "</a></h1>" << endl;

	createComponentPicture(filesToInclude);

	stream << "\t\t<p>" << endl;

	stream << "\t\t<img src=\"" << component->getVlnv().toString(".") << ".png\" alt=\"" <<
        component->getVlnv().toString(" - ") << " preview picture\"><br>" << endl;

	// if component has description, write it
	if (!component->getDescription().isEmpty())
    {
		stream << "\t\t<strong>Description:</strong> " << component->getDescription() << "<br>" << endl;
	}

	// print relative path to the xml file
	QFileInfo compXmlInfo(getLibraryHandler()->getPath(component->getVlnv()));
	QString relativeXmlPath = General::getRelativePath(getTargetPath(), compXmlInfo.absoluteFilePath());
	stream << "\t\t<strong>IP-Xact file: </strong><a href=\"" << 
		relativeXmlPath << "\">" << compXmlInfo.fileName() <<
		"</a><br>" << endl;
	
	stream << "\t\t</p>" << endl;

	int subHeaderNumber = 1;

    viewDocumentationGenerator_->setComponent(component, myNumber(), getTargetPath());

	writeParameters(stream, subHeaderNumber);
    writeMemoryMaps(stream, subHeaderNumber);
	writePorts(stream, subHeaderNumber);
	writeInterfaces(stream, subHeaderNumber);
	writeFileSets(stream, subHeaderNumber);
    viewDocumentationGenerator_->writeViews(stream, subHeaderNumber, filesToInclude);

	// tell each child to write it's documentation
	foreach (QSharedPointer<DocumentGenerator> generator, childInstances_)
    {
		generator->writeDocumentation(stream, targetPath, filesToInclude);
	}
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeParameters()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeParameters(QTextStream& stream, int& subHeaderNumber)
{
	writeSubHeader(subHeaderNumber, stream, "Kactus2 attributes", "kts_params");
	
    QSharedPointer<Component> component = getComponent();

	stream << "\t\t<p>" << endl;
	stream << "\t\t\t<strong>" << DocumentGeneratorHTML::indent() << "Product hierarchy: </strong>" <<
        KactusAttribute::hierarchyToString(component->getHierarchy()) << "<br>" << endl;
	
	stream << "\t\t\t<strong>" << DocumentGeneratorHTML::indent() << "Component implementation: </strong>" <<
        KactusAttribute::implementationToString(component->getImplementation()) << "<br>" << endl;

	stream << "\t\t\t<strong>" << DocumentGeneratorHTML::indent() << "Component firmness: </strong>" <<
        KactusAttribute::firmnessToString(component->getFirmness()) << "<br>" << endl;

	stream << "\t\t</p>" << endl;
	++subHeaderNumber;

	if (component->hasParameters())
    {
		writeSubHeader(subHeaderNumber, stream, "General parameters", "parameters");

        QString parameterTabs = QStringLiteral("\t\t\t");
        GeneralDocumentGenerator::writeParameters(stream,
            QStringLiteral("List of parameters defined for the component"), parameterTabs,
            component->getParameters(), expressionFormatter_);
	}
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeMemoryMaps()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeMemoryMaps(QTextStream& stream, int& subHeaderNumber)
{
    QSharedPointer<Component> component = getComponent();
    if (!component->getMemoryMaps()->isEmpty())
    {
        writeSubHeader(subHeaderNumber, stream, "Memory maps", "memoryMaps");

        const QList<QSharedPointer<MemoryMap> > componentMemoryMaps = *component->getMemoryMaps().data();
        int memoryMapNumber = 1;
        foreach (QSharedPointer<MemoryMap> memoryMap, componentMemoryMaps)
        {
            stream << "\t\t\t<h3><a id=\"" << component->getVlnv().toString() << ".memoryMap." <<
                memoryMap->name() << "\">" << myNumber() << "." << subHeaderNumber << "." << memoryMapNumber <<
                " " << memoryMap->name() << "</a></h3>" << endl;

            stream << "\t\t\t<p>" << endl;

            if (!memoryMap->description().isEmpty())
            {
                stream << "\t\t\t" << DocumentGeneratorHTML::indent() << "<strong>Description:</strong> " <<
                    memoryMap->description() << "<br>" << endl;
            }

            stream << "\t\t\t" << DocumentGeneratorHTML::indent() << "<strong>Address unit bits (AUB):</strong> " <<
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
            stream << "\t\t\t<h3><a id=\"" << getComponent()->getVlnv().toString() << ".addressBlock." <<
                currentAddressBlock->name() << "\">" << myNumber() << "." << subHeaderNumber << "." <<
                memoryMapNumber << "." << addressBlockNumber << " " << currentAddressBlock->name() <<
                "</a></h3>" << endl;

            if (!currentAddressBlock->description().isEmpty())
            {
                stream << "\t\t\t<p>" << endl;
                stream << "\t\t\t" << DocumentGeneratorHTML::indent() << "<strong>Description:</strong> " <<
                    currentAddressBlock->description() << "<br>" << endl;
                stream << "\t\t\t</p>" << endl;
            }

            QStringList addressBlockHeaders;
            addressBlockHeaders << "Usage" << "Base address [AUB]" << "Range [AUB]" << "Width [AUB]" << "Access" <<
                "Volatile";
            QString title = "List of values in " + currentAddressBlock->name() + ".";
            writeTableElement(addressBlockHeaders, title, stream, "\t\t\t");

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
            stream << "\t\t\t<h3><a id=\"" << getComponent()->getVlnv().toString() << ".register." <<
                currentRegister->name() << "\">" << myNumber() << "." << subHeaderNumber << "." <<
                memoryMapNumber << "." << addressBlockNumber << "." << registerNumber << " " <<
                currentRegister->name() << "</a></h3>" << endl;

            if (!currentRegister->description().isEmpty())
            {
                stream << "\t\t\t<p>" << endl;
                stream << "\t\t\t" << DocumentGeneratorHTML::indent() << "<strong>Description:</strong> " <<
                    currentRegister->description() << "<br>" << endl;
                stream << "\t\t\t</p>" << endl;
            }

            QStringList registerHeaders;
            registerHeaders << "Offset [AUB]" << "Size [bits]" << "Dimension" << "Volatile" << "Access";
            QString title = "List of values in " + currentRegister->name() + ".";
            writeTableElement(registerHeaders, title, stream, "\t\t\t");

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
            "Resets" << "Description";
        QString title = "List of fields contained within register " + currentRegister->name() + ".";
        writeTableElement(fieldHeaders, title, stream, "\t\t\t");

        foreach (QSharedPointer<Field> currentField, *currentRegister->getFields())
        {
            stream << "\t\t\t\t<tr>" << endl;
            stream << "\t\t\t\t\t<td><a id=\"" << getComponent()->getVlnv().toString() << ".field." <<
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

            QString resetInfo = getFieldResetInfo(currentField);
            stream << "\t\t\t\t\t<td>" << resetInfo << "</td>" << endl;

            stream << "\t\t\t\t\t<td>" << currentField->description() << "</td>" << endl;
            stream << "\t\t\t\t</tr>" << endl;
        }

        stream << "\t\t\t</table>" << endl;
    }
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::getFieldResetInfo()
//-----------------------------------------------------------------------------
QString DocumentGenerator::getFieldResetInfo(QSharedPointer<Field> field) const
{
    QString resetInfo = "";

    for (auto singleRest : *field->getResets())
    {
        if (singleRest != field->getResets()->first())
        {
            resetInfo.append("<br>");
        }

        QString resetTypeReference = singleRest->getResetTypeReference();
        if (resetTypeReference.isEmpty())
        {
            resetTypeReference = QLatin1String("HARD");
        }

        QString resetValue = expressionFormatter_->formatReferringExpression(singleRest->getResetValue());

        resetInfo.append(resetTypeReference + " : " + resetValue);
    }

    return resetInfo;
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writePorts()
//-----------------------------------------------------------------------------
void DocumentGenerator::writePorts(QTextStream& stream, int& subHeaderNumber)
{
	if (getComponent()->hasPorts())
    {
		writeSubHeader(subHeaderNumber, stream, "Ports", "ports");

        QString tableTitle = "List of all ports the component has.";
        const QList<QSharedPointer<Port> > ports = *getComponent()->getPorts().data();

        writePortTable(stream, tableTitle, ports);

        ++subHeaderNumber;
	}
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeInterfaces()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeInterfaces(QTextStream& stream, int& subHeaderNumber)
{
    QSharedPointer<Component> component = getComponent();
	if (component->hasInterfaces())
    {
		writeSubHeader(subHeaderNumber, stream, "Bus interfaces", "interfaces");

		int interfaceNumber = 1;
		foreach (QSharedPointer<BusInterface> interface,  *component->getBusInterfaces())
        {
			stream << "\t\t\t" << "<h3>" << myNumber() << "." << subHeaderNumber << "." << interfaceNumber++ <<
				" " << interface->name() << "</h3>" << endl;
			
			stream << "\t\t\t<p>" << endl;

			if (!interface->description().isEmpty())
            {
				stream << "\t\t\t" << DocumentGeneratorHTML::indent() << "<strong>Description:</strong> " << 
					interface->description() << "<br>" << endl;
			}

			// write the interface mode of the interface
			stream << "\t\t\t" << DocumentGeneratorHTML::indent() << "<strong>Interface mode:</strong> " << 
				General::interfaceMode2Str(interface->getInterfaceMode()) << "<br>" << endl;

            stream << "\t\t\t" << DocumentGeneratorHTML::indent() <<
                "<strong>Ports used in this interface:</strong>";

            const QList<QSharedPointer<Port> > ports = component->getPortsMappedInInterface(interface->name());
            if (!ports.isEmpty())
            {
                stream << endl << "\t\t\t</p>" << endl;

                QString tableTitle ("List of ports contained in interface " + (interface->name()) + ".");
                writePortTable(stream, tableTitle, ports);
            }
            else
            {
                stream << " None" << endl;
            }
		}
		++subHeaderNumber;
	}
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeFileSets()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeFileSets(QTextStream& stream, int& subHeaderNumber)
{
    QSharedPointer<Component> component = getComponent();
	if (component->hasFileSets())
    {
		writeSubHeader(subHeaderNumber, stream, "File sets", "fileSets");

		const QList<QSharedPointer<FileSet> > fileSets = *component->getFileSets().data();
		int fileSetNumber = 1;
		foreach (QSharedPointer<FileSet> fileSet, fileSets)
        {
			stream << "\t\t\t" << "<h3><a id=\"" << component->getVlnv().toString() << ".fileSet." <<
                fileSet->name() << "\">" << myNumber() << "." << subHeaderNumber << "." << fileSetNumber <<	" " <<
                fileSet->name() << "</a></h3>" << endl;

			stream << "\t\t\t<p>" << endl;

			if (!fileSet->description().isEmpty())
            {
				stream << "\t\t\t" << DocumentGeneratorHTML::indent() << "<strong>Description:</strong> " << 
					fileSet->description() << "<br>" << endl;
			}

			// write the group identifiers
			QStringList groups = *fileSet->getGroups().data();
			stream << "\t\t\t" << DocumentGeneratorHTML::indent() << "<strong>Identifiers:</strong> ";
			for (int i = 0; i < groups.size(); ++i)
            {
				stream << groups.at(i);

				if (i != groups.size()-1)
                {
					stream << ", ";
                }
			}
			stream << "<br>" << endl;

			// if file set has specified default file builders
            QSharedPointer<QList<QSharedPointer<FileBuilder> > > fileBuilders = fileSet->getDefaultFileBuilders();
            if (!fileBuilders->isEmpty())
            {
				stream << "\t\t\t" << DocumentGeneratorHTML::indent() << "<strong>Default file builders:</strong>" <<
                    endl;
				stream << "\t\t\t</p>" << endl;

                writeFileBuildCommands(stream, QString("\t\t\t"), fileBuilders, expressionFormatter_);
			}
			else
            {
				stream << "\t\t\t</p>" << endl;
			}

            if (!fileSet->getFiles()->isEmpty())
            {
                stream << "\t\t\t" << "<h4>" << DocumentGeneratorHTML::indent() << myNumber() << "." <<
                    subHeaderNumber << "." << fileSetNumber << ".1 Files</h4>" << endl;

                QStringList fileHeaders;
                fileHeaders.append("File name");
                fileHeaders.append("Logical name");
                fileHeaders.append("Build command");
                fileHeaders.append("Build flags");
                fileHeaders.append("Specified file types");
                fileHeaders.append("Description");
                writeTableElement(fileHeaders, "List of files contained in this file set.", stream, "\t\t\t");

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
// Function: documentgenerator::writeEndOfDocument()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeEndOfDocument(QTextStream& stream)
{
    stream << DocumentGeneratorHTML::valid3wStrict() << endl;
    stream << "\t</body>" << endl;
    stream << "</html>" << endl;
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeSubHeader()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeSubHeader( const int headerNumber, QTextStream& stream, const QString& text,
    const QString& headerID)
{
   stream << "\t\t<h2><a id=\"" << getComponent()->getVlnv().toString() << "." << headerID << "\">" <<
       myNumber() << "." << headerNumber << " " << text << "</a></h2>" << endl;
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writePortTable()
//-----------------------------------------------------------------------------
void DocumentGenerator::writePortTable(QTextStream& stream, QString const& title,
    QList <QSharedPointer <Port> > ports)
{
    QStringList portHeaders;
    portHeaders << "Name" << "Direction" << "Left bound" << "Right bound" << "Port type" << "Type definition" <<
        "Default value" << "Array left" << "Array right" << "Description";
    writeTableElement(portHeaders, title, stream, "\t\t\t");

    foreach (QSharedPointer<Port> port, ports)
    {
        stream << "\t\t\t\t<tr>" << endl;
        stream << "\t\t\t\t\t<td><a id=\"" << getComponent()->getVlnv().toString() << ".port." << port->name() << 
            "\">" <<  port->name() << "</a></td>" << endl;
        stream << "\t\t\t\t\t<td>" << DirectionTypes::direction2Str(port->getDirection()) << "</td>" << endl;
        stream << "\t\t\t\t\t<td>" << expressionFormatter_->formatReferringExpression(port->getLeftBound()) <<
            "</td>" << endl;
        stream << "\t\t\t\t\t<td>" << expressionFormatter_->formatReferringExpression(port->getRightBound()) <<
            "</td>" << endl;

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
// Function: documentgenerator::writeFile()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeFile( QSharedPointer<File> file, QTextStream& stream )
{
	QString relativeFilePath = file->name();
	QString absFilePath =
        General::getAbsolutePath(getLibraryHandler()->getPath(getComponent()->getVlnv()), relativeFilePath);
	QFileInfo fileInfo(absFilePath);

	// get relative path from html file to the file
	QFileInfo htmlInfo(getTargetPath());
	QString pathFromhtmlToFile = General::getRelativePath(getTargetPath(), absFilePath);

    QSharedPointer<BuildCommand> buildCommand = file->getBuildCommand();

	QStringList fileTypes = *file->getFileTypes();

	stream << "\t\t\t\t<tr>" << endl;
	stream << "\t\t\t\t\t<td><a href=\"" << pathFromhtmlToFile << "\">" << fileInfo.fileName() << "</a></td>" <<
        endl;
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
    QSharedPointer<Component> component = getComponent();

	ComponentPreviewBox compBox(getLibraryHandler());
	compBox.hide();
	compBox.setComponent(component);

	QFileInfo htmlInfo(getTargetPath());
	QString compPicPath = htmlInfo.absolutePath(); 
	compPicPath += "/";
	compPicPath += component->getVlnv().toString(".");
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
	if (!compPic.save(&compPicFile, "PNG"))
    {
		emit errorMessage(tr("Could not save picture %1").arg(compPicPath));
	}
	else
    {
		pictureList.append(compPicPath);
	}
}
