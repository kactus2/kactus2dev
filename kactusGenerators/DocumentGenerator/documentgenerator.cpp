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

#include <kactusGenerators/DocumentGenerator/MarkdownWriter.h>
#include <kactusGenerators/DocumentGenerator/HtmlWriter.h>

#include <editors/common/DesignWidget.h>
#include <editors/common/DesignWidgetFactory.h>
#include <editors/common/DesignDiagram.h>

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

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <QString>
#include <QDir>
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
    int componentNumber, QWidget* parent) :
    libraryHandler_(handler),
    component_(),
    componentNumber_(componentNumber),
    targetPath_(),
    expressionFormatterFactory_(expressionFormatterFactory),
    childInstances_(),
    parentWidget_(parent),
    expressionFormatter_(),
    designWidgetFactory_(designWidgetFactory),
    writer_(nullptr),
    mdWriter_(nullptr),
    htmlWriter_(nullptr),
    componentFinder_(nullptr),
    currentFormat_(DocumentFormat::MD),
    imagesPath_()
{
    Q_ASSERT(handler);
    Q_ASSERT(parent);

    // parse the model for the component
    component_ = libraryHandler_->getModel(vlnv).dynamicCast<Component>();

    componentFinder_ = QSharedPointer<ComponentParameterFinder>(new ComponentParameterFinder(component_));
    expressionFormatter_ = expressionFormatterFactory_->makeExpressionFormatter(component_);

    if (!component_)
    {
       emit errorMessage("VLNV was not found in the library.");
       return;
    }

    htmlWriter_ = new HtmlWriter(component_, expressionFormatter_, expressionFormatterFactory_,
        libraryHandler_, componentNumber_);

    mdWriter_ = new MarkdownWriter(component_, expressionFormatter_, expressionFormatterFactory_,
        libraryHandler_, componentNumber_);

    writer_ = mdWriter_;
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::~DocumentGenerator()
//-----------------------------------------------------------------------------
DocumentGenerator::~DocumentGenerator()
{
    delete htmlWriter_;
    htmlWriter_ = nullptr;

    delete mdWriter_;
    mdWriter_ = nullptr;

    writer_ = nullptr;
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::parseChildItems()
//-----------------------------------------------------------------------------
void DocumentGenerator::parseChildItems(QList<VLNV>& objects, int& currentComponentNumber)
{
    // ask the component for it's hierarchical references
    QList<VLNV> refs = component_->getHierRefs();
    for (auto const& ref : refs)
    {
        QSharedPointer<Design> design = libraryHandler_->getDesign(ref);
        if (!design)
        {
            continue;
        }

        for (auto const& instance : *design->getComponentInstances())
        {
            if (!objects.contains(*instance->getComponentRef()) &&
                libraryHandler_->getDocumentType(*instance->getComponentRef()) == VLNV::COMPONENT)
            {
                // create a new instance of document generator and add it to child list
                objects.append(*instance->getComponentRef());
                
                currentComponentNumber++;

                QSharedPointer<DocumentGenerator> docGenerator(new DocumentGenerator(libraryHandler_,
                    *instance->getComponentRef(), designWidgetFactory_, expressionFormatterFactory_,
                    currentComponentNumber, parentWidget_));

                docGenerator->setFormat(currentFormat_);
                docGenerator->parseChildItems(objects, currentComponentNumber);

                connect(docGenerator.data(), SIGNAL(errorMessage(const QString&)),
                    this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
                connect(docGenerator.data(), SIGNAL(noticeMessage(const QString&)),
                    this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
    
                childInstances_.append(docGenerator);
            }
        }
    }
}

void DocumentGenerator::setFormat(DocumentFormat format)
{
    if (format == DocumentFormat::HTML)
    {
        writer_ = htmlWriter_;
    }
    else if (format == DocumentFormat::MD)
    {
        writer_ = mdWriter_;
    }

    currentFormat_ = format;
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

    Q_ASSERT(libraryHandler_->contains(component_->getVlnv()));
    Q_ASSERT(libraryHandler_->getDocumentType(component_->getVlnv()) == VLNV::COMPONENT);

    // this function can only be called for the top document generator
    Q_ASSERT(parentWidget_);

    // list of objects that have already been processed to avoid duplicates
    QList<VLNV> objects;
    objects.append(component_->getVlnv());

    // Component number
    int currentComponentNumber = 1;

    parseChildItems(objects, currentComponentNumber);

    targetPath_ = targetPath;

    QFileInfo docInfo(targetPath_);

    imagesPath_ = docInfo.absolutePath() + "/images";
    
    if (QDir imageDir(imagesPath_); !imageDir.exists())
    {
        imageDir.mkdir(imagesPath_);
    }

    writeHeader(stream);

    writer_->writeTableOfContentsHeader(stream);

    writeTableOfContents(stream);

    QStringList pictureList;

    // write the actual documentation for the top component
    writeDocumentation(stream, targetPath_, pictureList);

    writer_->writeEndOfDocument(stream);

    QApplication::restoreOverrideCursor();

    // ask user if he wants to save the generated documentation into object metadata
    QMessageBox::StandardButton button = QMessageBox::question(parentWidget_, 
        tr("Save generated documentation to metadata?"),
        tr("Would you like to save the generated documentation to IP-Xact"
        " metadata?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

    // if the generated file is saved
    if (button == QMessageBox::Yes)
    {
        QString xmlPath = libraryHandler_->getPath(component_->getVlnv());

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

        QString docFileType = "";
        if (currentFormat_ == DocumentFormat::HTML)
        {
            docFileType = "html";
        }
        else if (currentFormat_ == DocumentFormat::MD)
        {
            docFileType = "markdown";
        }

        // create a new file and add it to file set
        QSharedPointer<File> docFile = documentationFileSet->addFile(relativePath, settings);
        Q_ASSERT(docFile);
        docFile->setIncludeFile(false);
        docFile->setDescription(tr("File that contains the documentation "
            "for this component and subcomponents"));

        if (!docFile->matchesFileType(docFileType))
        {
            docFile->getFileTypes()->append(FileType(docFileType));
        }

        // add all created pictures to the file set
        for (auto const& pictureName : pictureList)
        {
            QString relativePicPath = General::getRelativePath(xmlPath, pictureName);
            QString picFileType = "pngImage";

            QSharedPointer<File> picFile = documentationFileSet->addFile(relativePicPath, settings);
            Q_ASSERT(picFile);
            picFile->setIncludeFile(false);
            picFile->setDescription(tr("Preview picture needed by the documentation."));

            if (!picFile->matchesFileType(picFileType))
            {
                picFile->getFileTypes()->append(FileType(picFileType));
            }
        }

        libraryHandler_->writeModelToFile(component_);
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
    for (auto const &generator : childInstances_)
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
    writer_->setImagesPath(imagesPath_);
    targetPath_ = targetPath;

    // write the component header, picture and info
    writer_->writeComponentHeader(stream);

    createComponentPicture(filesToInclude);

    writer_->writeComponentInfo(stream);

    int subHeaderNumber = 1;

    writeKactusAttributes(stream, subHeaderNumber);
    writeParameters(stream, subHeaderNumber);
    writeMemoryMaps(stream, subHeaderNumber);
    writePorts(stream, subHeaderNumber);
    writeInterfaces(stream, subHeaderNumber);
    writeFileSets(stream, subHeaderNumber);
    writeViews(stream, subHeaderNumber, filesToInclude);

    // tell each child to write it's documentation
    for (auto const& generator : childInstances_)
    {
        generator->setImagesPath(imagesPath_);
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
    if (!component_->getMemoryMaps()->isEmpty())
    {
        writer_->writeMemoryMaps(stream, subHeaderNumber);
        ++subHeaderNumber;
    }
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeAddressBlock()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeAddressBlocks(QList<QSharedPointer<AddressBlock> > addressBlocks,
    QTextStream& stream, int subHeaderNumber, int memoryMapNumber)
{
    writer_->writeAddressBlocks(stream, addressBlocks, subHeaderNumber, memoryMapNumber);
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeRegisters()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeRegisters(QList<QSharedPointer<Register> > registers, QTextStream& stream,
    int subHeaderNumber, int memoryMapNumber, int addressBlockNumber, int registerDataNumber)
{
    writer_->writeRegisters(stream, registers, subHeaderNumber, memoryMapNumber,
        addressBlockNumber, registerDataNumber);
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeFields()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeFields(QSharedPointer<Register> currentRegister,
    QTextStream& stream, QList<int> subHeaderNumbers)
{
    writer_->writeFields(stream, currentRegister, subHeaderNumbers);
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

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeViews()
//-----------------------------------------------------------------------------
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
    writer_->writeEndOfDocument(stream);
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::setImagesPath()
//-----------------------------------------------------------------------------
void DocumentGenerator::setImagesPath(QString const& path)
{
    imagesPath_ = path;
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeSingleView()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeSingleView(QTextStream& stream, QSharedPointer<View> view,
    int const& subHeaderNumber, int const& viewNumber, QStringList& pictureList)
{
    QList subHeaderNumbers({ componentNumber_, subHeaderNumber, viewNumber });
    writer_->writeSubHeader(stream, subHeaderNumbers, "View: " + view->name(), 3);

    if (auto const& description = view->description(); !description.isEmpty())
    {
        writer_->writeDescription(stream, description);
    }
    
    int instantiationNumber = 1;

    
    if (QString viewComponentInstantiationRef = view->getComponentInstantiationRef();
        !viewComponentInstantiationRef.isEmpty())
    {
        writeReferencedComponentInstantiation(stream,
            viewComponentInstantiationRef, subHeaderNumber, viewNumber, instantiationNumber);
        instantiationNumber++;
    }

    if (QString viewDesignConfigurationInstantiationRef = view->getDesignConfigurationInstantiationRef();
        !viewDesignConfigurationInstantiationRef.isEmpty())
    {
        writeReferencedDesignConfigurationInstantiation(stream,
           viewDesignConfigurationInstantiationRef, subHeaderNumber, viewNumber, instantiationNumber);
        instantiationNumber++;
    }

    if (QString viewDesignInstantiationRef = view->getDesignInstantiationRef();
        !viewDesignInstantiationRef.isEmpty())
    {
        writeReferencedDesignInstantiation(stream, viewDesignInstantiationRef,
            subHeaderNumber, viewNumber, instantiationNumber);
        instantiationNumber++;
    }

    if (view->isHierarchical())
    {
        writeDesign(stream, view, pictureList);
    }
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeReferencedComponentInstantiation()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeReferencedComponentInstantiation(QTextStream& stream,
    QString const& instantiationReference, int const& subHeaderNumber,
    int const& viewNumber, int const& instantiationNumber)
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
    auto moduleParameters = getModuleParametersAsParameters(instantiation->getModuleParameters());
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

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeReferencedDesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
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
        instantiation, instantiationFormatter, libraryHandler_);
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeReferencedDesignInstantiation()
//-----------------------------------------------------------------------------
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
        libraryHandler_->getModel(*instantiation->getDesignReference());

    if (!designDocument)
    {
        return;
    }

    QSharedPointer<Design> instantiatedDesign = designDocument.dynamicCast<Design>();

    if (instantiatedDesign)
    {
        designFinder->setParameterList(instantiatedDesign->getParameters());

        QString header = QString("Parameters of the referenced design %1:").arg(designVLNV->toString());
        QSharedPointer<ExpressionFormatter> designFormatter(new ExpressionFormatter(designFinder));

        writer_->writeReferencedDesignInstantiation(stream, designVLNV, instantiatedDesign,
            designFormatter, instantiationFormatter);
    }
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::writeDesign()
//-----------------------------------------------------------------------------
void DocumentGenerator::writeDesign(QTextStream& stream, QSharedPointer<View> view, QStringList& pictureList)
{
    QSharedPointer<DesignConfiguration> configuration = getDesignConfiguration(view);
    QSharedPointer<Design> design = getDesign(view, configuration);

    if (view->isHierarchical() && !design)
    {
        QString errorMessage = QString("Design reference %1 does not contain a valid design.").
            arg(view->getDesignInstantiationRef());
        writer_->writeErrorMessage(stream, errorMessage);
        return;
    }

    QString designPicPath = imagesPath_
        + (imagesPath_.isEmpty() ? QStringLiteral("") : QStringLiteral("/"))
        + component_->getVlnv().toString(".")
        + QStringLiteral(".")
        + view->name()
        + QStringLiteral(".png");

    createDesignPicture(pictureList, view->name(), designPicPath);

    QString designDiagramTitle = QString("Diagram of design %1:").arg(design->getVlnv().toString());
    QString designDiagramAltText = QString("View: %1 preview picture").arg(view->name());
    QString relativePicPath = imagesPath_.split("/").back()
        + "/"
        + component_->getVlnv().toString(".")
        + QStringLiteral(".")
        + view->name()
        + QStringLiteral(".png");

    writer_->writeDiagram(stream, designDiagramTitle, relativePicPath, designDiagramAltText);
    writer_->writeDesignInstances(stream, design, configuration);
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::createDesignPicture()
//-----------------------------------------------------------------------------
void DocumentGenerator::createDesignPicture(QStringList& pictureList,
    QString const& viewName, QString const& designPicPath)
{
    DesignWidget* designWidget(designWidgetFactory_->makeHWDesignWidget());

    designWidget->hide();
    designWidget->setDesign(component_->getVlnv(), viewName);

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

//-----------------------------------------------------------------------------
// Function: documentgenerator::getDesignInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<DesignInstantiation> DocumentGenerator::getDesignInstantiation(
    QString const& designReference) const
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

//-----------------------------------------------------------------------------
// Function: documentgenerator::getComponentInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstantiation> DocumentGenerator::getComponentInstantiation(
    QString const& instantiationReference) const
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

//-----------------------------------------------------------------------------
// Function: documentgenerator::getModuleParametersAsParameters()
//-----------------------------------------------------------------------------
ParameterList DocumentGenerator::getModuleParametersAsParameters(
    QSharedPointer<QList<QSharedPointer<ModuleParameter> > > moduleParameters) const
{
    ParameterList newModuleParameters(
        new QList<QSharedPointer<Parameter> >());

    for (QSharedPointer<Parameter> parameter : *moduleParameters)
    {
        newModuleParameters->append(parameter);
    }

    return newModuleParameters;
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::getDesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
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
// Function: documentgenerator::getDesignConfiguration()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfiguration> DocumentGenerator::getDesignConfiguration(QSharedPointer<View> view) const
{
    QSharedPointer<DesignConfiguration> designConf(nullptr);

    QSharedPointer<DesignConfigurationInstantiation> configurationInstantiation =
        getDesignConfigurationInstantiation(view->getDesignConfigurationInstantiationRef());
    
    if (!configurationInstantiation)
    {
        return designConf;
    }

    QSharedPointer<ConfigurableVLNVReference> configurationVLNV =
        configurationInstantiation->getDesignConfigurationReference();
    if (configurationVLNV)
    {
        QSharedPointer<Document> configurationDocument =
            libraryHandler_->getModel(*configurationVLNV.data());
        if (configurationDocument && configurationVLNV->getType() == VLNV::DESIGNCONFIGURATION)
        {
            designConf = configurationDocument.dynamicCast<DesignConfiguration>();
        }
    }

    return designConf;
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::getDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> DocumentGenerator::getDesign(QSharedPointer<View> view,
    QSharedPointer<DesignConfiguration> configuration) const
{
    QSharedPointer<ConfigurableVLNVReference> designVLNV(nullptr);
    if (!view->getDesignInstantiationRef().isEmpty())
    {
        QSharedPointer<DesignInstantiation> designInstantiation =
            getDesignInstantiation(view->getDesignInstantiationRef());
        if (designInstantiation)
        {
            designVLNV = designInstantiation->getDesignReference();
        }
    }

    if (!designVLNV && configuration)
    {
        VLNV referenceVLNV = configuration->getDesignRef();
        if (referenceVLNV.isValid() && referenceVLNV.getType() == VLNV::DESIGN)
        {
            designVLNV = QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(referenceVLNV));
        }
    }

    if (designVLNV)
    {
        QSharedPointer<Document> designDocument = libraryHandler_->getModel(*designVLNV.data());
        if (designDocument && designVLNV->getType() == VLNV::DESIGN)
        {
            QSharedPointer<Design> design = designDocument.dynamicCast<Design>();
            return design;
        }
    }

    return QSharedPointer<Design>();
}

//-----------------------------------------------------------------------------
// Function: documentgenerator::createComponentPicture()
//-----------------------------------------------------------------------------
void DocumentGenerator::createComponentPicture(QStringList& pictureList)
{
    ComponentPreviewBox compBox(libraryHandler_);
    compBox.hide();
    compBox.setComponent(component_);

    QString compPicPath = imagesPath_
        + "/"
        + component_->getVlnv().toString(".")
        + ".png";

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
