//-----------------------------------------------------------------------------
// File: ViewDocumentGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 25.10.2017
//
// Description:
// Generates documentation for the selected view and the instantiations referenced by it.
//-----------------------------------------------------------------------------

#include "ViewDocumentGenerator.h"

#include <kactusGenerators/DocumentGenerator/DocumentGeneratorHTML.h>

#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/MultipleParameterFinder.h>
#include <editors/ComponentEditor/common/ListParameterFinder.h>

#include <editors/common/DesignWidgetFactory.h>
#include <editors/common/DesignWidget.h>
#include <editors/common/DesignDiagram.h>

#include <library/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::ViewDocumentGenerator()
//-----------------------------------------------------------------------------
ViewDocumentGenerator::ViewDocumentGenerator(LibraryInterface* libraryHandler,
    ExpressionFormatterFactory* formatterFactory, DesignWidgetFactory* designWidgetFactory):
GeneralDocumentGenerator(libraryHandler, formatterFactory),
designWidgetFactory_(designWidgetFactory),
componentFinder_(0)
{

}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::~ViewDocumentGenerator()
//-----------------------------------------------------------------------------
ViewDocumentGenerator::~ViewDocumentGenerator()
{

}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::setComponent()
//-----------------------------------------------------------------------------
void ViewDocumentGenerator::setComponent(QSharedPointer<Component> newComponent, int const& newComponentNumber,
    QString const& newPath)
{
    GeneralDocumentGenerator::setComponent(newComponent);
    setNumber(newComponentNumber);
    setTargetPath(newPath);

    componentFinder_ = QSharedPointer<ComponentParameterFinder>(new ComponentParameterFinder(getComponent()));
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::writeViews()
//-----------------------------------------------------------------------------
void ViewDocumentGenerator::writeViews( QTextStream& stream, int& subHeaderNumber, QStringList& pictureList )
{
    QSharedPointer<Component> component = getComponent();
	if (component->hasViews())
    {
        stream << "\t\t\t<h2><a id=\"" << component->getVlnv().toString() << ".views\">" << myNumber() <<
            "." << subHeaderNumber << " Views</a></h2>" << endl;
		
		int viewNumber = 1;
		foreach (QSharedPointer<View> view, *component->getViews())
        {
            writeSingleView(stream, view, subHeaderNumber, viewNumber, pictureList);
			++viewNumber;
		}
		++subHeaderNumber;
	}
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::writeSingleView()
//-----------------------------------------------------------------------------
void ViewDocumentGenerator::writeSingleView(QTextStream& stream, QSharedPointer<View> view,
    int const& subHeaderNumber, int const& viewNumber, QStringList& pictureList)
{
    QString viewTabs = "\t\t\t";

    stream << viewTabs << "<h3>" << myNumber() << "." << subHeaderNumber << "." << viewNumber <<
        " View: " << view->name() << "</h3>" << endl;

    stream << viewTabs << "<p>" << endl;
    writeDescription(stream, viewTabs, view->description());
    stream << viewTabs << "</p>" << endl;

    QString instantiationTabs = viewTabs + QString("\t");
    QString instantiationItemTabs = instantiationTabs + ("\t");
    int instantiationNumber = 1;
    if (!view->getComponentInstantiationRef().isEmpty())
    {
        writeReferencedComponentInstantiation(instantiationTabs, instantiationItemTabs, stream,
            view->getComponentInstantiationRef(), subHeaderNumber, viewNumber, instantiationNumber);
        instantiationNumber++;
    }

    if (!view->getDesignConfigurationInstantiationRef().isEmpty())
    {
        writereferencedDesignConfigurationInstantiation(instantiationTabs, stream,
            view->getDesignConfigurationInstantiationRef(), subHeaderNumber, viewNumber, instantiationNumber);
        instantiationNumber++;
    }

    if (!view->getDesignInstantiationRef().isEmpty())
    {
        writeReferencedDesignInstantiation(instantiationTabs, stream, view->getDesignInstantiationRef(),
            subHeaderNumber, viewNumber, instantiationNumber);
        instantiationNumber++;
    }

    if (view->isHierarchical())
    {
        writeDesign(stream, viewTabs, view, pictureList);
    }
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::writeReferencedComponentInstantiation()
//-----------------------------------------------------------------------------
void ViewDocumentGenerator::writeReferencedComponentInstantiation(QString const& instantiationTabs,
    QString const& instantiationItemTabs, QTextStream& stream, QString const& instantiationReference,
    int const& subHeaderNumber, int const& viewNumber, int const& instantiationNumber)
{
    stream << instantiationTabs << "<h4>" << myNumber() << "." << subHeaderNumber << "." << viewNumber << "." <<
        instantiationNumber << " Component instantiation: " << instantiationReference << "</h4>" << endl;

    QSharedPointer<ComponentInstantiation> instantiation = getComponentInstantiation(instantiationReference);
    if (!instantiation)
    {
        QString errorMessage(tr("Referenced component instantiation %1 was not found.").
            arg(instantiationReference));
        writeErrorMessage(stream, instantiationTabs, errorMessage);
        return;
    }

    writeDescription(stream, instantiationTabs, instantiation->description());

    QSharedPointer<ListParameterFinder> parameterFinder(new ListParameterFinder());
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters = instantiation->getParameters();
    parameterFinder->setParameterList(parameters);

    QSharedPointer<ListParameterFinder> moduleParameterFinder(new ListParameterFinder());
    QSharedPointer<QList<QSharedPointer<Parameter> > > moduleParameters =
        getModuleParametersAsParameters(instantiation->getModuleParameters());
    moduleParameterFinder->setParameterList(moduleParameters);

    QSharedPointer<MultipleParameterFinder> instantiationParameterFinder(new MultipleParameterFinder());
    instantiationParameterFinder->addFinder(componentFinder_);
    instantiationParameterFinder->addFinder(parameterFinder);
    instantiationParameterFinder->addFinder(moduleParameterFinder);

    ExpressionFormatter* instantiationFormatter = new ExpressionFormatter(instantiationParameterFinder);

    QString moduleParameterToolTip = QString("Module parameters of component instantiation ") +
        instantiation->name();
    QString parameterToolTip = QString("Parameters of component instantiation ") + instantiation->name();

    writeImplementationDetails(stream, instantiationTabs, instantiationItemTabs, instantiation);
    writeFileSetReferences(stream, instantiationTabs, instantiationItemTabs, instantiation);
    writeFileBuildCommands(stream, instantiationTabs, instantiation, instantiationFormatter);
    writeParameters(stream, QString("Module parameters:"), moduleParameterToolTip, instantiationTabs,
        moduleParameters, instantiationFormatter);
    writeParameters(stream, QString("Parameters:"), parameterToolTip, instantiationTabs, parameters,
        instantiationFormatter);
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::getComponentInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstantiation> ViewDocumentGenerator::getComponentInstantiation(
    QString const& instantiationReference) const
{
    foreach (QSharedPointer<ComponentInstantiation> instantiation, *getComponent()->getComponentInstantiations())
    {
        if (instantiation->name() == instantiationReference)
        {
            return instantiation;
        }
    }

    return QSharedPointer<ComponentInstantiation>(0);
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::getModuleParametersAsParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > ViewDocumentGenerator::getModuleParametersAsParameters(
    QSharedPointer<QList<QSharedPointer<ModuleParameter> > > moduleParameters)
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > newModuleParameters(
        new QList<QSharedPointer<Parameter> >());
    foreach (QSharedPointer<Parameter> parameter, *moduleParameters)
    {
        newModuleParameters->append(parameter);
    }

    return newModuleParameters;
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::writeImplementationDetails()
//-----------------------------------------------------------------------------
void ViewDocumentGenerator::writeImplementationDetails(QTextStream& stream, QString const& instantiationTabs,
    QString const& instantiationItemTabs, QSharedPointer<ComponentInstantiation> instantiation)
{
    QString language = instantiation->getLanguage();
    QString library = instantiation->getLibraryName();
    QString package = instantiation->getPackageName();
    QString module = instantiation->getModuleName();
    QString architecture = instantiation->getArchitectureName();
    QString configuration = instantiation->getConfigurationName();

    if (!language.isEmpty() || !library.isEmpty() || !package.isEmpty() || !module.isEmpty() ||
        !architecture.isEmpty() || !configuration.isEmpty())
    {
        stream << instantiationTabs << "<p>" << endl;

        if (!language.isEmpty())
        {
            stream << instantiationItemTabs << DocumentGeneratorHTML::indent() << "<strong>Language: </strong>" <<
                language;

            if (instantiation->isLanguageStrict())
            {
                stream << " <strong>strict</strong>";
            }

            stream << "<br>" << endl;
        }
        if (!library.isEmpty())
        {
            stream << instantiationItemTabs << DocumentGeneratorHTML::indent() << "<strong>Library: </strong>" <<
                library << "<br>" << endl;
        }
        if (!package.isEmpty())
        {
            stream << instantiationItemTabs << DocumentGeneratorHTML::indent() << "<strong>Package: </strong>" <<
                package << "<br>" << endl;
        }
        if (!module.isEmpty())
        {
            stream << instantiationItemTabs << DocumentGeneratorHTML::indent() << "<strong>Module name: </strong>" <<
                module << "<br>" << endl;
        }
        if (!architecture.isEmpty())
        {
            stream << instantiationItemTabs << DocumentGeneratorHTML::indent() <<
                "<strong>Architecture: </strong>" << architecture << "<br>" << endl;
        }
        if (!configuration.isEmpty())
        {
            stream << instantiationItemTabs << DocumentGeneratorHTML::indent() <<
                "<strong>Configuration: </strong>" << configuration << "<br>" << endl;
        }

        stream << instantiationTabs << "</p>" << endl;
    }
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::writeFileSetReferences()
//-----------------------------------------------------------------------------
void ViewDocumentGenerator::writeFileSetReferences(QTextStream& stream, QString const& instantiationTabs,
    QString const& instantiationItemTabs, QSharedPointer<ComponentInstantiation> instantiation)
{
    QStringList fileSetRefs = *instantiation->getFileSetReferences().data();
    if (!fileSetRefs.isEmpty())
    {
        stream << instantiationTabs << "<p>" << endl;
        stream << instantiationItemTabs << "File sets contained in this component instantiation: </strong>" <<
            endl;
        stream << instantiationTabs << "</p>" << endl;

        stream << instantiationTabs << "<ul>" << endl;

        foreach (QString fileSetRef, fileSetRefs)
        {
            stream << instantiationItemTabs << "<li><a href=\"#" << getComponent()->getVlnv().toString() <<
                ".fileSet." << fileSetRef << "\">" << fileSetRef << "</a></li>" << endl;
        }

        stream << instantiationTabs << "</ul>" << endl;
    }
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::writeFileBuildCommands()
//-----------------------------------------------------------------------------
void ViewDocumentGenerator::writeFileBuildCommands(QTextStream& stream, QString const& instantiationTabs,
    QSharedPointer<ComponentInstantiation> instantiation, ExpressionFormatter* instantiationFormatter)
{
    if (!instantiation->getDefaultFileBuilders()->isEmpty())
    {
        stream << instantiationTabs << "<p>Default file build commands:</p>" << endl;

        GeneralDocumentGenerator::writeFileBuildCommands(stream, instantiationTabs,
            instantiation->getDefaultFileBuilders(), instantiationFormatter);
    }
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::writeParameters()
//-----------------------------------------------------------------------------
void ViewDocumentGenerator::writeParameters(QTextStream&stream, QString const& tableTooltip,
    QString const& tableHeading, QString const& parameterTabs,
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters, ExpressionFormatter* instantiationFormatter)
{
    if (parameters)
    {
        if (!parameters->isEmpty())
        {
            stream << parameterTabs << "<p>" << tableTooltip << "</p>" << endl;
            GeneralDocumentGenerator::writeParameters(
                stream, tableHeading, parameterTabs, parameters, instantiationFormatter);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::writereferencedDesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
void ViewDocumentGenerator::writereferencedDesignConfigurationInstantiation(QString const& instantiationTabs,
    QTextStream& stream, QString const& configurationReference, int const& subHeaderNumber, int const& viewNumber,
    int const& instantiationNumber)
{
    stream << instantiationTabs << "<h4>" << myNumber() << "." << subHeaderNumber << "." << viewNumber << "." <<
        instantiationNumber << " Design configuration instantiation: " << configurationReference << "</h4>" <<
        endl;

    QSharedPointer<DesignConfigurationInstantiation> instantiation =
        getDesignConfigurationInstantiation(configurationReference);
    if (!instantiation)
    {
        QString errorMessage(tr("Referenced design configuration instantiation %1 was not found.").
            arg(configurationReference));
        writeErrorMessage(stream, instantiationTabs, errorMessage);
        return;
    }

    writeDescription(stream, instantiationTabs, instantiation->description());

    QSharedPointer<ListParameterFinder> configurationInstantiationParameterFinder(new ListParameterFinder());
    configurationInstantiationParameterFinder->setParameterList(instantiation->getParameters());

    QSharedPointer<ListParameterFinder> configurationFinder(new ListParameterFinder());

    QSharedPointer<MultipleParameterFinder> instantiationParameterFinder(new MultipleParameterFinder());
    instantiationParameterFinder->addFinder(componentFinder_);
    instantiationParameterFinder->addFinder(configurationInstantiationParameterFinder);
    instantiationParameterFinder->addFinder(configurationFinder);

    ExpressionFormatter* instantiationFormatter(new ExpressionFormatter(instantiationParameterFinder));

    QString parameterToolTip = QString("Parameters of design configuration instantiation %1.").
        arg(instantiation->name());

    QSharedPointer<ConfigurableVLNVReference> configurationVLNV = instantiation->getDesignConfigurationReference();

    writeDocumentReference(stream, QString("Design configuration"), instantiationTabs, configurationVLNV);

    if (configurationVLNV)
    {
        QSharedPointer<Document> configurationDocument = getLibraryHandler()->getModel(*configurationVLNV.data());
        if (configurationDocument)
        {
            QSharedPointer<DesignConfiguration> configuration =
                configurationDocument.dynamicCast<DesignConfiguration>();
            if (configuration)
            {
                configurationFinder->setParameterList(configuration->getParameters());

                QString header = QString("Parameters of the referenced design configuration %1:").
                    arg(configurationVLNV->toString());
                ExpressionFormatter* configurationFormatter(new ExpressionFormatter(configurationFinder));

                writeParameters(stream, header, QString("Design configuration parameters"), instantiationTabs,
                    configuration->getParameters(), configurationFormatter);
                writeConfigurableElementValues(stream, instantiationTabs,
                    instantiation->getDesignConfigurationReference(), instantiationFormatter);
            }
        }
    }

    writeParameters(stream, QString("Design configuration instantiation parameters:"), parameterToolTip,
        instantiationTabs, instantiation->getParameters(), instantiationFormatter);
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::getDesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfigurationInstantiation> ViewDocumentGenerator::getDesignConfigurationInstantiation(
    QString const& instantiationReference) const
{
    foreach (QSharedPointer<DesignConfigurationInstantiation> instantiation,
        *getComponent()->getDesignConfigurationInstantiations())
    {
        if (instantiation->name() == instantiationReference)
        {
            return instantiation;
        }
    }

    return QSharedPointer<DesignConfigurationInstantiation>();
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::writeDocumentReference()
//-----------------------------------------------------------------------------
void ViewDocumentGenerator::writeDocumentReference(QTextStream& stream, QString const& documentType,
    QString const& instantiationItemTabs, QSharedPointer<ConfigurableVLNVReference> vlnvReference)
{
    if (vlnvReference)
    {
        if (!getLibraryHandler()->getModelReadOnly(*vlnvReference.data()))
        {
            QString errorMsg(tr("VLNV: %1 was not found in library.").arg(vlnvReference->toString()));
            writeErrorMessage(stream, instantiationItemTabs, errorMsg);
        }
        else
        {
            QString documentReferenceTabs = instantiationItemTabs + "\t";
            stream << instantiationItemTabs << "<p>" << endl;
            stream << documentReferenceTabs << DocumentGeneratorHTML::indent() <<
                "<strong>" << documentType << ": </strong>" << vlnvReference->toString() << "<br>" << endl;

            QFileInfo vlnvXMLInfo(getLibraryHandler()->getPath(*vlnvReference.data()));
            QString relativeXmlPath = General::getRelativePath(getTargetPath(), vlnvXMLInfo.absoluteFilePath());

            stream << documentReferenceTabs <<  DocumentGeneratorHTML::indent() << "<strong>" <<
                "IP-Xact file: </strong><a href=\"" << relativeXmlPath << "\">" << vlnvXMLInfo.fileName() <<
                "</a><br>" << endl;
            stream << instantiationItemTabs << "</p>" << endl;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::writeConfigurableElementValues()
//-----------------------------------------------------------------------------
void ViewDocumentGenerator::writeConfigurableElementValues(QTextStream& stream,
    QString const& instantiationsItemTabs, QSharedPointer<ConfigurableVLNVReference> vlnvReference,
    ExpressionFormatter* instantiationFormatter)
{
    if (vlnvReference && vlnvReference->getConfigurableElementValues() &&
        !vlnvReference->getConfigurableElementValues()->isEmpty())
    {
        stream << instantiationsItemTabs << "<p>Configurable element values:</p>" << endl;

        QStringList paramHeaders;
        paramHeaders.append(QStringLiteral("Name"));
        paramHeaders.append(QStringLiteral("Value"));
        writeTableElement(paramHeaders, QString("Configurable element values"), stream, instantiationsItemTabs);

        QString elementTableTabs = instantiationsItemTabs + "\t";
        QString elementRowTabs = elementTableTabs + "\t";
        foreach (QSharedPointer<ConfigurableElementValue> element, *vlnvReference->getConfigurableElementValues())
        {
            stream << elementTableTabs << "<tr>" << endl;
            stream << elementRowTabs << "<td>" <<
                instantiationFormatter->formatReferringExpression(element->getReferenceId()) << "</td>" << endl;
            stream << elementRowTabs << "<td>" <<
                instantiationFormatter->formatReferringExpression(element->getConfigurableValue()) << "</td>" <<
                endl;
            stream << elementTableTabs << "</tr>" << endl;
        }

        stream << instantiationsItemTabs << "</table>" << endl;
    }
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::writeReferencedDesignInstantiation()
//-----------------------------------------------------------------------------
void ViewDocumentGenerator::writeReferencedDesignInstantiation(QString const& instantiationTabs,
    QTextStream& stream, QString const& designReference, int const& subHeaderNumber, int const& viewNumber,
    int const& instantiationNumber)
{
    stream << instantiationTabs << "<h4>" << myNumber() << "." << subHeaderNumber << "." << viewNumber << "." <<
        instantiationNumber << " Design instantiation: " << designReference << "</h4>" << endl;

    QSharedPointer<DesignInstantiation> instantiation = getDesignInstantiation(designReference);
    if (!instantiation)
    {
        QString errorMessage(tr("Referenced design instantiation %1 was not found.").arg(designReference));
        writeErrorMessage(stream, instantiationTabs, errorMessage);
        return;
    }

    writeDescription(stream, instantiationTabs, instantiation->description());

    QSharedPointer<ListParameterFinder> designFinder(new ListParameterFinder());
    QSharedPointer<MultipleParameterFinder> instantiationParameterFinder(new MultipleParameterFinder());
    instantiationParameterFinder->addFinder(componentFinder_);
    instantiationParameterFinder->addFinder(designFinder);

    ExpressionFormatter* instantiationFormatter(new ExpressionFormatter(instantiationParameterFinder));

    QSharedPointer<ConfigurableVLNVReference> designVLNV = instantiation->getDesignReference();
    writeDocumentReference(stream, QString("Design"), instantiationTabs, designVLNV);

    if (designVLNV)
    {
        QSharedPointer<Document> designDocument =
            getLibraryHandler()->getModel(*instantiation->getDesignReference().data());
        if (designDocument)
        {
            QSharedPointer<Design> instantiatedDesign = designDocument.dynamicCast<Design>();
            if (instantiatedDesign)
            {
                designFinder->setParameterList(instantiatedDesign->getParameters());

                QString header = QString("Parameters of the referenced design %1:").arg(designVLNV->toString());
                ExpressionFormatter* designFormatter(new ExpressionFormatter(designFinder));

                writeParameters(stream, header, QString("Design parameters"), instantiationTabs,
                    instantiatedDesign->getParameters(), designFormatter);

                writeConfigurableElementValues(stream, instantiationTabs, designVLNV, instantiationFormatter);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::getDesignInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<DesignInstantiation> ViewDocumentGenerator::getDesignInstantiation(QString const& designReference)
    const
{
    foreach (QSharedPointer<DesignInstantiation> instantiation, *getComponent()->getDesignInstantiations())
    {
        if (instantiation->name() == designReference)
        {
            return instantiation;
        }
    }

    return QSharedPointer<DesignInstantiation>();
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::writeErrorMessage()
//-----------------------------------------------------------------------------
void ViewDocumentGenerator::writeErrorMessage(QTextStream& stream, QString const& documentTabs,
    QString const& errorMsg)
{
    stream << documentTabs << "<p>" << endl;
    stream << documentTabs << "<strong><font color=red>" << errorMsg << "</font></strong><br>" << endl;
    stream << documentTabs << "</p>" << endl;
    emit errorMessage(errorMsg);
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::writeDesign()
//-----------------------------------------------------------------------------
void ViewDocumentGenerator::writeDesign(QTextStream& stream, QString const& viewTabs, QSharedPointer<View> view,
    QStringList& pictureList)
{
    stream << viewTabs << "<br>" << endl;

    QSharedPointer<DesignConfiguration> configuration = getDesignConfiguration(view);
    QSharedPointer<Design> design = getDesign(view, configuration);
    if (view->isHierarchical() && !design)
    {
        QString errorMessage = QString("Design reference %1 does not contain a valid design.").
            arg(view->getDesignInstantiationRef());
        writeErrorMessage(stream, viewTabs, errorMessage);
        return;
    }

    createDesignPicture(pictureList, view->name());

    QString diagramHeadline = QString("Diagram of design %1:").arg(design->getVlnv().toString());
    stream << viewTabs << diagramHeadline << "<br>" << endl;
    stream << viewTabs  << "<img src=\"" << getComponent()->getVlnv().toString(".") << "." << view->name() <<
        ".png\" alt=\"" << "View: " << view->name() << " preview picture\"><br>" << endl;

    writeDesignInstances(stream, design, configuration, viewTabs);
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::getDesignConfiguration()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfiguration> ViewDocumentGenerator::getDesignConfiguration(QSharedPointer<View> view) const
{
    QSharedPointer<DesignConfigurationInstantiation> configurationInstantiation =
        getDesignConfigurationInstantiation(view->getDesignConfigurationInstantiationRef());
    if (configurationInstantiation)
    {
        QSharedPointer<ConfigurableVLNVReference> configurationVLNV =
            configurationInstantiation->getDesignConfigurationReference();
        if (configurationVLNV)
        {
            QSharedPointer<Document> configurationDocument =
                getLibraryHandler()->getModel(*configurationVLNV.data());
            if (configurationDocument && configurationVLNV->getType() == VLNV::DESIGNCONFIGURATION)
            {
                QSharedPointer<DesignConfiguration> configuration =
                    configurationDocument.dynamicCast<DesignConfiguration>();
                return configuration;
            }
        }
    }

    return QSharedPointer<DesignConfiguration>();
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::getDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> ViewDocumentGenerator::getDesign(QSharedPointer<View> view,
    QSharedPointer<DesignConfiguration> configuration) const
{
    QSharedPointer<ConfigurableVLNVReference> designVLNV(0);
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
        QSharedPointer<Document> designDocument = getLibraryHandler()->getModel(*designVLNV.data());
        if (designDocument && designVLNV->getType() ==  VLNV::DESIGN)
        {
            QSharedPointer<Design> design = designDocument.dynamicCast<Design>();
            return design;
        }
    }

    return QSharedPointer<Design>();
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::writeDesignInstances()
//-----------------------------------------------------------------------------
void ViewDocumentGenerator::writeDesignInstances(QTextStream& stream, QSharedPointer<Design> design,
    QSharedPointer<DesignConfiguration> configuration, QString const& viewTabs)
{
    if (design->getComponentInstances()->isEmpty())
    {
        return;
    }

    QString instanceTitle = QString("Component instances within design %1").arg(design->getVlnv().toString());

    stream << viewTabs << "<br>" << endl;
    stream << viewTabs << instanceTitle << ":<br>" << endl;

    QStringList instanceHeaders;
    instanceHeaders.append("Instance name");
    instanceHeaders.append("Component type");
    instanceHeaders.append("Configurable values");
    instanceHeaders.append("Active view");
    instanceHeaders.append("Description");
    writeTableElement(instanceHeaders, instanceTitle + QString("."), stream, viewTabs);

    QString tableTabs = viewTabs + "\t";
    QString tableRowTabs = tableTabs + "\t";
    QString elementRowTabs = tableRowTabs + "\t";

    foreach (QSharedPointer<ComponentInstance> instance, *design->getComponentInstances())
    {
        stream << tableTabs << "<tr>" << endl;
        stream << tableRowTabs << "<td>" << instance->getInstanceName() << "</td>" << endl;

        stream << tableRowTabs << "<td><a href=\"#" << instance->getComponentRef()->toString(":") << "\">" <<
            instance->getComponentRef()->toString(" - ") << "</a></td>" << endl;

        stream << tableRowTabs << "<td>" << endl;

        VLNV componentVLNV = *instance->getComponentRef();

        QSharedPointer<Document> libComp = getLibraryHandler()->getModel(componentVLNV);
        QSharedPointer<Component> component = libComp.staticCast<Component>();

        ExpressionFormatter* equationFormatter = createDesignInstanceFormatter(design, component);

        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > >  confElements =
            instance->getConfigurableElementValues();

        foreach(QSharedPointer<ConfigurableElementValue> configurableElement, *confElements)
        {
            QString configurableElementID = configurableElement->getReferenceId();
            QString configurableElementName = equationFormatter->formatReferringExpression(configurableElementID);

            stream << elementRowTabs;
            if (configurableElementID == configurableElementName)
            {
                stream << "<font color=red>Unknown</font>";
            }
            else
            {
                stream << configurableElementName;
            }

            stream << " = " 
                << equationFormatter->formatReferringExpression(configurableElement->getConfigurableValue());

            if (configurableElement != confElements->last())
            {
                stream << "<br>";
            }
            stream << endl;
        }

        stream << tableRowTabs << "</td>" << endl;

        stream << tableRowTabs << "<td>";
        if (configuration && configuration->getDesignRef() == design->getVlnv())
        {
            stream << configuration->getActiveView(instance->getInstanceName());
        }
        stream << "</td>" << endl;

        stream << tableRowTabs << "<td>" << instance->getDescription() << "</td>" << endl;
        stream << tableTabs << "</tr>" << endl;
    }

    stream << viewTabs << "</table>" << endl; 
}

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::createDesignPicture()
//-----------------------------------------------------------------------------
void ViewDocumentGenerator::createDesignPicture( QStringList& pictureList, const QString& viewName )
{
    DesignWidget* designWidget (designWidgetFactory_->makeHWDesignWidget());

    QSharedPointer<Component> component = getComponent();

    designWidget->hide();
    designWidget->setDesign(component->getVlnv(), viewName);

	QFileInfo htmlInfo(getTargetPath());
	QString designPicPath = htmlInfo.absolutePath(); 
	designPicPath += "/";
	designPicPath += component->getVlnv().toString(".");
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

//-----------------------------------------------------------------------------
// Function: ViewDocumentGenerator::writeDescription()
//-----------------------------------------------------------------------------
void ViewDocumentGenerator::writeDescription(QTextStream& stream, QString const& descriptionTabs,
    QString const& description)
{
    if (!description.isEmpty())
    {
        stream << descriptionTabs << DocumentGeneratorHTML::indent() << "<strong>Description: </strong>" <<
            description << "<br>" << endl;
    }
}
