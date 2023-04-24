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

#include <KactusAPI/include/utils.h>
#include <KactusAPI/include/ListParameterFinder.h>
#include <KactusAPI/include/MultipleParameterFinder.h>

#include <common/widgets/componentPreviewBox/ComponentPreviewBox.h>

#include <kactusGenerators/DocumentGenerator/ViewDocumentGenerator.h>
#include <kactusGenerators/DocumentGenerator/DocumentGeneratorHTML.h>
#include <kactusGenerators/DocumentGenerator/MarkdownWriter.h>
#include <kactusGenerators/DocumentGenerator/HtmlWriter.h>

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
    QWidget* parent) :
    GeneralDocumentGenerator(handler, expressionFormatterFactory, parent),
    expressionFormatterFactory_(expressionFormatterFactory),
    childInstances_(),
    parentWidget_(parent),
    expressionFormatter_(),
    component_(),
    componentNumber_(1),
    libraryHandler_(handler),
    componentFinder_(nullptr),
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
    component_ = getLibraryHandler()->getModel(vlnv).dynamicCast<Component>();

    componentFinder_ = QSharedPointer<ComponentParameterFinder>(new ComponentParameterFinder(component_));

    if (!component_)
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
    component_ = libComp.staticCast<Component>();

    componentFinder_ = QSharedPointer<ComponentParameterFinder>(new ComponentParameterFinder(component_));

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
    QList<VLNV> refs = component_->getHierRefs();
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

void DocumentGenerator::setFormat(DocumentFormat format)
{
    if (format == DocumentFormat::HTML)
    {
        writer_ = new HtmlWriter(component_, expressionFormatter_, libraryHandler_, componentNumber_);
    }
    else if (format == DocumentFormat::MD)
    {
        writer_ = new MarkdownWriter(component_, expressionFormatter_, libraryHandler_, componentNumber_);
    }
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeDocumentation()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeDocumentation(QTextStream& stream, QString targetPath)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    if (!component_)
    {
        emit errorMessage("VLNV was not found in the library.");
        return;
    }

    Q_ASSERT(getLibraryHandler()->contains(component_->getVlnv()));
    Q_ASSERT(getLibraryHandler()->getDocumentType(component_->getVlnv()) == VLNV::COMPONENT);

    // this function can only be called for the top document generator
    Q_ASSERT(parentWidget_);

    setTargetPath(targetPath);

    writeHeader(stream);

    // create a running number to create the numbered headers for table of contents
    unsigned int runningNumber = 0;

    stream << "\t\t<p>" << Qt::endl;
    stream << "\t\t<strong>Table of contents</strong><br>" << Qt::endl;
    writeTableOfContents(stream);
    stream << "\t\t</p>" << Qt::endl;

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
        QString xmlPath = getLibraryHandler()->getPath(component_->getVlnv());

        // get the relative path to add to file set
        QString relativePath = General::getRelativePath(xmlPath, getTargetPath());

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
        foreach (QString pictureName, pictureList)
        {
            QString relativePicPath = General::getRelativePath(xmlPath, pictureName);

            QSharedPointer<File> picFile = documentationFileSet->addFile(relativePicPath, settings);
            Q_ASSERT(picFile);
            picFile->setIncludeFile(false);
            picFile->setDescription(tr("Preview picture needed by the html document."));
        }

        getLibraryHandler()->writeModelToFile(component_);
    }
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeHeader()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeHeader(QTextStream& stream)
{
    writer_->writeHeader(stream);
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeTableOfContents()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeTableOfContents(QTextStream& stream)
{
    writer_->setComponentNumber(componentNumber_);
    writer_->writeTableOfContents(stream);

    // tell each child to write it's table of contents
    foreach (QSharedPointer<DocumentGenerator> generator, childInstances_)
    {
        generator->writeTableOfContents(stream);
    }
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeDocumentation()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeDocumentation(QTextStream& stream, const QString& targetPath,
    QStringList& filesToInclude)
{
    writer_->setTargetPath(targetPath);

    QSharedPointer<Component> component = component_;

    // write the component header
    stream << "\t\t<h1><a id=\"" << component->getVlnv().toString() << "\">" << myNumber() << ". Component " <<
        component->getVlnv().toString(" - ") << "</a></h1>" << Qt::endl;

    createComponentPicture(filesToInclude);

    stream << "\t\t<p>" << Qt::endl;

    stream << "\t\t<img src=\"" << component->getVlnv().toString(".") << ".png\" alt=\"" <<
        component->getVlnv().toString(" - ") << " preview picture\"><br>" << Qt::endl;

    // if component has description, write it
    if (!component->getDescription().isEmpty())
    {
        stream << "\t\t<strong>Description:</strong> " << component->getDescription() << "<br>" << Qt::endl;
    }

    // print relative path to the xml file
    QFileInfo compXmlInfo(getLibraryHandler()->getPath(component->getVlnv()));
    QString relativeXmlPath = General::getRelativePath(getTargetPath(), compXmlInfo.absoluteFilePath());
    stream << "\t\t<strong>IP-Xact file: </strong><a href=\"" << 
        relativeXmlPath << "\">" << compXmlInfo.fileName() <<
        "</a><br>" << Qt::endl;
    
    stream << "\t\t</p>" << Qt::endl;

    int subHeaderNumber = 1;

    viewDocumentationGenerator_->setComponent(component, myNumber(), getTargetPath());

    writeKactusAttributes(stream, subHeaderNumber);
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
// Function: documentgenerator::writeKactusAttributes()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeKactusAttributes(QTextStream& stream, int& subHeaderNumber)
{
    writer_->writeKactusAttributes(stream, subHeaderNumber);
    ++subHeaderNumber;
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeParameters()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeParameters(QTextStream& stream, int& subHeaderNumber)
{
    if (component_->hasParameters())
    {
        writer_->writeParameters(stream, subHeaderNumber);
        ++subHeaderNumber;
    }
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeMemoryMaps()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeMemoryMaps(QTextStream& stream, int& subHeaderNumber)
{
    writer_->writeMemoryMaps(stream, subHeaderNumber);
    ++subHeaderNumber;
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeAddressBlock()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeAddressBlocks(QList<QSharedPointer<AddressBlock> > addressBlocks, QTextStream& stream,
    int& subHeaderNumber, int& memoryMapNumber)
{
    writer_->writeAddressBlocks(stream, addressBlocks, subHeaderNumber, memoryMapNumber);
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeRegisters()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeRegisters(QList<QSharedPointer<Register> > registers, QTextStream& stream,
    int& subHeaderNumber, int& memoryMapNumber, int& addressBlockNumber)
{
    writer_->writeRegisters(stream, registers, subHeaderNumber, memoryMapNumber, addressBlockNumber);
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeFields()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeFields(QSharedPointer<Register> currentRegister, QTextStream& stream)
{
    writer_->writeFields(stream, currentRegister);
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writePorts()
//-----------------------------------------------------------------------------
void DocumentGenerator::writePorts(QTextStream& stream, int& subHeaderNumber)
{
    if (component_->hasPorts())
    {
        writer_->writePorts(stream, subHeaderNumber);
        ++subHeaderNumber;
    }
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeInterfaces()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeInterfaces(QTextStream& stream, int& subHeaderNumber)
{
    if (component_->hasInterfaces())
    {
        writer_->writeInterfaces(stream, subHeaderNumber);
        ++subHeaderNumber;
    }
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeFileSets()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeFileSets(QTextStream& stream, int& subHeaderNumber)
{
    if (component_->hasFileSets())
    {
        writer_->writeFileSets(stream, subHeaderNumber);
        ++subHeaderNumber;
    }
}

void DocumentGenerator::writeViews(QTextStream& stream, int& subHeaderNumber, QStringList& pictureList)
{
    if (!component_->hasViews())
    {
        return;
    }

    writer_->writeSubHeader(stream, subHeaderNumber, "Views", "views");

    int viewNumber = 1;

    for (auto const& view : *component_->getViews())
    {
        writeSingleView(stream, view, subHeaderNumber, viewNumber, pictureList);
        ++viewNumber;
    }

    ++subHeaderNumber;
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeEndOfDocument()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeEndOfDocument(QTextStream& stream)
{
    stream << DocumentGeneratorHTML::valid3wStrict() << Qt::endl;
    stream << "\t</body>" << Qt::endl;
    stream << "</html>" << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: GeneralDocumentGenerator::getExpressionFormatterFactory()
//-----------------------------------------------------------------------------
ExpressionFormatterFactory* DocumentGenerator::getExpressionFormatterFactory() const
{
    return expressionFormatterFactory_;
}

//-----------------------------------------------------------------------------
// Function: GeneralDocumentGenerator::createExpressionFormatter()
//-----------------------------------------------------------------------------
ExpressionFormatter* DocumentGenerator::createExpressionFormatter() const
{
    return expressionFormatterFactory_->makeExpressionFormatter(component_);
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeSubHeader()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeSubHeader(const unsigned int& subHeaderNumber, QTextStream& stream, const QString& text,
    const QString& headerID)
{
}

void DocumentGenerator::writeSingleView(QTextStream& stream, QSharedPointer<View> view, int const& subHeaderNumber, int const& viewNumber, QStringList& pictureList)
{
    QList subHeaderNumbers({ componentNumber_, subHeaderNumber, viewNumber });
    writer_->writeSubHeader(stream, subHeaderNumbers, "View: " + view->name(), 3);

    if (auto const& description = view->description(); !description.isEmpty())
    {
        writer_->writeDescription(stream, description);
    }
    
    int instantiationNumber = 1;

    QString viewComponentInstantiationRef = view->getComponentInstantiationRef();
    
    if (!viewComponentInstantiationRef.isEmpty())
    {
        writeReferencedComponentInstantiation(stream,
            viewComponentInstantiationRef, subHeaderNumber, viewNumber, instantiationNumber);
        instantiationNumber++;
    }

    QString viewDesignConfigurationInstantiationRef = view->getDesignConfigurationInstantiationRef();

    if (!viewComponentInstantiationRef.isEmpty())
    {
        writeReferencedDesignConfigurationInstantiation(stream,
           viewDesignConfigurationInstantiationRef, subHeaderNumber, viewNumber, instantiationNumber);
        instantiationNumber++;
    }

    QString viewDesignInstantiationRef = view->getDesignInstantiationRef();

    if (!viewDesignInstantiationRef.isEmpty())
    {
        writeReferencedDesignInstantiation(stream, view->getDesignInstantiationRef(),
            subHeaderNumber, viewNumber, instantiationNumber);
        instantiationNumber++;
    }
}

void DocumentGenerator::writeReferencedComponentInstantiation(QTextStream& stream, QString const& instantiationReference, int const& subHeaderNumber, int const& viewNumber, int const& instantiationNumber)
{
    writer_->writeSubHeader(stream, QList({ componentNumber_, subHeaderNumber, viewNumber, instantiationNumber }),
        "Component instantiation: " + instantiationReference, 4);

    QSharedPointer<ComponentInstantiation> instantiation = getComponentInstantiation(instantiationReference);
    if (!instantiation)
    {
        QString errorMsg(tr("Referenced component instantiation %1 was not found.").
            arg(instantiationReference));
        writer_->writeErrorMessage(stream, errorMsg);
        emit errorMessage(errorMsg);
        return;
    }

    if (auto const& description = instantiation->description(); !description.isEmpty())
    {
        writer_->writeDescription(stream, description);
    }

    QSharedPointer<ListParameterFinder> parameterFinder(new ListParameterFinder());
    ParameterList parameters = instantiation->getParameters();
    parameterFinder->setParameterList(parameters);

    QSharedPointer<ListParameterFinder> moduleParameterFinder(new ListParameterFinder());
    ParameterList moduleParameters =
        getModuleParametersAsParameters(instantiation->getModuleParameters());
    moduleParameterFinder->setParameterList(moduleParameters);

    QSharedPointer<MultipleParameterFinder> instantiationParameterFinder(new MultipleParameterFinder());
    instantiationParameterFinder->addFinder(componentFinder_);
    instantiationParameterFinder->addFinder(parameterFinder);
    instantiationParameterFinder->addFinder(moduleParameterFinder);

    QSharedPointer<ExpressionFormatter> instantiationFormatter(
        new ExpressionFormatter(instantiationParameterFinder));

    QString moduleParameterToolTip = QString("Module parameters of component instantiation ") +
        instantiation->name();
    QString parameterToolTip = QString("Parameters of component instantiation ") + instantiation->name();

    writer_->writeReferencedComponentInstantiation(stream, instantiation, instantiationFormatter,
       moduleParameters, parameters);
}

void DocumentGenerator::writeReferencedDesignConfigurationInstantiation(QTextStream& stream,
    QString const& configurationReference, int const& subHeaderNumber, int const& viewNumber,
    int const& instantiationNumber)
{
    writer_->writeSubHeader(stream, QList({ componentNumber_, subHeaderNumber, viewNumber, instantiationNumber }),
        "Design configuration instantiation: " + configurationReference, 4);

    QSharedPointer<DesignConfigurationInstantiation> instantiation =
        getDesignConfigurationInstantiation(configurationReference);

    if (!instantiation)
    {
        QString errorMessage(tr("Referenced design configuration instantiation %1 was not found.").
            arg(configurationReference));
        writer_->writeErrorMessage(stream, errorMessage);
        return;
    }
    
    if (auto const& description = instantiation->description(); !description.isEmpty())
    {
        writer_->writeDescription(stream, description);
    }

    QSharedPointer<ListParameterFinder> configurationInstantiationParameterFinder(new ListParameterFinder());
    configurationInstantiationParameterFinder->setParameterList(instantiation->getParameters());

    QSharedPointer<ListParameterFinder> configurationFinder(new ListParameterFinder());

    QSharedPointer<MultipleParameterFinder> instantiationParameterFinder(new MultipleParameterFinder());
    instantiationParameterFinder->addFinder(componentFinder_);
    instantiationParameterFinder->addFinder(configurationInstantiationParameterFinder);
    instantiationParameterFinder->addFinder(configurationFinder);

    QSharedPointer<ExpressionFormatter> instantiationFormatter(new ExpressionFormatter(instantiationParameterFinder));

    writer_->writeDocumentReference(stream, QString("Design configuration"),
        instantiation->getDesignConfigurationReference());

    writer_->writeReferencedDesignConfigurationInstantiation(stream, configurationFinder,
        instantiation, instantiationFormatter);
}

void DocumentGenerator::writeReferencedDesignInstantiation(QTextStream& stream,
    QString const& designReference,int const& subHeaderNumber,
    int const& viewNumber, int const& instantiationNumber)
{
    writer_->writeSubHeader(stream, QList({ componentNumber_, subHeaderNumber, viewNumber, instantiationNumber }),
        "Design instantiation: " + designReference, 4);
    
    QSharedPointer<DesignInstantiation> instantiation = getDesignInstantiation(designReference);

    if (!instantiation)
    {
        QString errorMessage(tr("Referenced design configuration instantiation %1 was not found.").
            arg(designReference));
        writer_->writeErrorMessage(stream, errorMessage);
        return;
    }
    
    if (auto const& description = instantiation->description(); !description.isEmpty())
    {
        writer_->writeDescription(stream, description);
    }

    QSharedPointer<ConfigurableVLNVReference> designVLNV = instantiation->getDesignReference();

    writer_->writeDocumentReference(stream, QString("Design"), designVLNV);

    if (!designVLNV)
    {
        return;
    }

    QSharedPointer<ListParameterFinder> designFinder(new ListParameterFinder());
    QSharedPointer<MultipleParameterFinder> instantiationParameterFinder(new MultipleParameterFinder());
    instantiationParameterFinder->addFinder(componentFinder_);
    instantiationParameterFinder->addFinder(designFinder);

    QSharedPointer<ExpressionFormatter> instantiationFormatter(new ExpressionFormatter(instantiationParameterFinder));

    QSharedPointer<Document> designDocument =
        getLibraryHandler()->getModel(*instantiation->getDesignReference());

    if (!designDocument)
    {
        return;
    }

    QSharedPointer<Design> instantiatedDesign = designDocument.dynamicCast<Design>();

    if (instantiatedDesign)
    {
        designFinder->setParameterList(instantiatedDesign->getParameters());

        QString header = QString("Parameters of the referenced design %1:").arg(designVLNV->toString());
        ExpressionFormatter designFormatter(designFinder);

        writer_->writeReferencedDesignInstantiation(stream, designVLNV, instantiatedDesign,
            &designFormatter,instantiationFormatter);
    }
}

QSharedPointer<DesignInstantiation> DocumentGenerator::getDesignInstantiation(QString const& designReference) const
{
    for(auto const& instantiation : *component_->getDesignInstantiations())
    {
        if (instantiation->name() == designReference)
        {
            return instantiation;
        }
    }

    return QSharedPointer<DesignInstantiation>();
}

QSharedPointer<ComponentInstantiation> DocumentGenerator::getComponentInstantiation(QString const& instantiationReference) const
{
    for (auto const& instantiation : *component_->getComponentInstantiations())
    {
        if (instantiation->name() == instantiationReference)
        {
            return instantiation;
        }
    }

    return QSharedPointer<ComponentInstantiation>(nullptr);
}

ParameterList DocumentGenerator::getModuleParametersAsParameters(
    QSharedPointer<QList<QSharedPointer<ModuleParameter> > > moduleParameters)
{
    ParameterList newModuleParameters(
        new QList<QSharedPointer<Parameter> >());

    for (QSharedPointer<Parameter> parameter : *moduleParameters)
    {
        newModuleParameters->append(parameter);
    }

    return newModuleParameters;
}

QSharedPointer<DesignConfigurationInstantiation> DocumentGenerator::getDesignConfigurationInstantiation(
    QString const& instantiationReference) const
{
    for (auto const& instantiation : *component_->getDesignConfigurationInstantiations())
    {
        if (instantiation->name() == instantiationReference)
        {
           return instantiation;
        }
    }

    return QSharedPointer<DesignConfigurationInstantiation>();
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::createComponentPicture()
//-----------------------------------------------------------------------------
void DocumentGenerator::createComponentPicture(QStringList& pictureList)
{
    QSharedPointer<Component> component = component_;

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
