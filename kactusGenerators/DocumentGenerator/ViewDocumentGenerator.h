//-----------------------------------------------------------------------------
// File: ViewDocumentGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 25.10.2017
//
// Description:
// Generates documentation for the selected view and the instantiations referenced by it.
//-----------------------------------------------------------------------------

#ifndef VIEWDOCUMENTGENERATOR_H
#define VIEWDOCUMENTGENERATOR_H

#include <kactusGenerators/DocumentGenerator/GeneralDocumentGenerator.h>

#include <QTextStream>
#include <QSharedPointer>
#include <QString>

class Component;
class View;
class Parameter;
class ModuleParameter;
class ComponentInstantiation;
class DesignInstantiation;
class DesignConfigurationInstantiation;
class LibraryInterface;
class ExpressionFormatterFactory;
class DesignWidgetFactory;
class ComponentParameterFinder;
class ExpressionFormatter;
class DesignConfiguration;
class Design;
class ConfigurableVLNVReference;
class ConfigurableElementValue;

//-----------------------------------------------------------------------------
//! Generates documentation for the selected view and the instantiations referenced by it.
//-----------------------------------------------------------------------------
class ViewDocumentGenerator : public GeneralDocumentGenerator
{
    Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] libraryHandler          Library, where the components reside.
	 *      @param [in] formatterFactory        Factory for making expression formatters.
     *      @param [in] designWidgetFactory     Factory for making design widgets.
	 */
	ViewDocumentGenerator(LibraryInterface* libraryHandler, ExpressionFormatterFactory* formatterFactory,
        DesignWidgetFactory* designWidgetFactory);
	
	/*!
     *  The destructor.
     */
    virtual ~ViewDocumentGenerator();

    /*!
     *  Set a component to be documented.
     *
     *      @param [in] newComponent        The selected component.
     *      @param [in] newComponentNumber  The header number for the documented component.
     *      @param [in] newPath             New path for document generation.
     */
    void setComponent(QSharedPointer<Component> newComponent, int const& newComponentNumber,
        QString const& newPath);

	/*!
	 *  Write the views of the component.
	 *
	 *      @param [in] stream              The text stream to write the component views.
	 *      @param [in] subHeaderNumber     The number that defines the sub header.
	 *      @param [in] pictureList         List of file names to add the pictures of the referenced designs to.
	 */
	void writeViews(QTextStream& stream, int& subHeaderNumber, QStringList& pictureList);

private:

	//! No copying. No assignment.
	ViewDocumentGenerator(const ViewDocumentGenerator& other);
	ViewDocumentGenerator& operator=(const ViewDocumentGenerator& other);

    /*!
     *  Write the selected view.
     *
     *      @param [in] stream              The text stream to write the view.
     *      @param [in] view                The selected view.
     *      @param [in] subHeaderNumber     Number for the sub header.
     *      @param [in] viewNumber          Number defining the selected view.
     *      @param [in] pictureList         List of file names to add the pictures of the referenced designs to.
     */
    void writeSingleView(QTextStream& stream, QSharedPointer<View> view, int const& subHeaderNumber,
        int const& viewNumber, QStringList& pictureList);

    /*!
     *  Write the referenced component instantiation.
     *
     *      @param [in] instantiationTabs       Component instantiation indentation in the file.
     *      @param [in] instantiationItemTabs   Instantiation item indentation in the file.
     *      @param [in] stream                  Text stream to write the component instantiation.
     *      @param [in] instantiationReference  Name of the selected component instantiation.
     *      @param [in] subHeaderNumber         Number for the sub header.
     *      @param [in] viewNumber              Number defining the selected view.
     *      @param [in] instantiationNumber     Number defining the component instantiation.
     */
    void writeReferencedComponentInstantiation(QString const& instantiationTabs,
        QString const& instantiationItemTabs, QTextStream& stream, QString const& instantiationReference,
        int const& subHeaderNumber, int const& viewNumber, int const& instantiationNumber);

    /*!
     *  Get the referenced component instantiation.
     *
     *      @param [in] instantiationReference  Name of the selected component instantiation.
     *
     *      @return Component instantiation containing the selected name.
     */
    QSharedPointer<ComponentInstantiation> getComponentInstantiation(QString const& instantiationReference) const;

    /*!
     *  Write the selected error message to the documentation and send it forward.
     *
     *      @param [in] stream          Text stream to write the error message.
     *      @param [in] documentTabs    Error message indentation in the file.
     *      @param [in] errorMsg        The selected error message.
     */
    void writeErrorMessage(QTextStream& stream, QString const& documentTabs, QString const& errorMsg);

    /*!
     *  Get the selected module parameters as a list of parameters.
     *
     *      @param [in] moduleParameters    The selected module parameters.
     *
     *      @return The module parameters as a list of parameters
     */
    QSharedPointer<QList<QSharedPointer<Parameter> > > getModuleParametersAsParameters(
        QSharedPointer<QList<QSharedPointer<ModuleParameter> > > moduleParameters);

    /*!
     *  Write the implementation details of the selected component instantiation.
     *
     *      @param [in] stream                  Text stream to write the selected implementation details.
     *      @param [in] instantiationTabs       General implementation details indentation in the file.
     *      @param [in] instantiationItemTabs   Singular item indentation in the file.
     *      @param [in] instantiation           The selected component instantiation.
     */
    void writeImplementationDetails(QTextStream& stream, QString const& instantiationTabs,
        QString const& instantiationItemTabs, QSharedPointer<ComponentInstantiation> instantiation);

    /*!
     *  Write the file set references contained within the selected component instantiation.
     *
     *      @param [in] stream                  Text stream to write the selected file set references.
     *      @param [in] instantiationTabs       General file sets indentation in the file.
     *      @param [in] instantiationItemTabs   Singular file set indentation in the file.
     *      @param [in] instantiation           The selected component instantiation.
     */
    void writeFileSetReferences(QTextStream& stream, QString const& instantiationTabs,
        QString const& instantiationItemTabs, QSharedPointer<ComponentInstantiation> instantiation);

    /*!
     *  Write the file build commands of the selected component instantiation.
     *
     *      @param [in] stream                  Text stream to write the selected file build commands.
     *      @param [in] instantiationTabs       General file build command indentation in the file.
     *      @param [in] instantiation           The selected component instantiation.
     *      @param [in] instantiationFormatter  Expression formatter for the component instantiation.
     */
    void writeFileBuildCommands(QTextStream& stream, QString const& instantiationTabs,
        QSharedPointer<ComponentInstantiation> instantiation, ExpressionFormatter* instantiationFormatter);

    /*!
     *  Write the selected parameters as a table.
     *
     *      @param [in] stream                  Text stream to write the selected parameters.
     *      @param [in] tableTooltip            Tooltip for the parameter table.
     *      @param [in] tableHeading            Header for the parameter table.
     *      @param [in] parameterTabs           Parameter indentation in the file.
     *      @param [in] parameters              The selected parameters.
     *      @param [in] instantiationFormatter  Expression formatter for the selected parameters.
     */
    void writeParameters(QTextStream&stream, QString const& tableTooltip, QString const& tableHeading,
        QString const& parameterTabs, QSharedPointer<QList<QSharedPointer<Parameter> > > parameters,
        ExpressionFormatter* instantiationFormatter);

    /*!
     *  Write the referenced design configuration instantiation.
     *
     *      @param [in] instantiationTabs       Design configuration instantiation indentation in the file.
     *      @param [in] stream                  Text stream to write the component instantiation.
     *      @param [in] configurationReference  Name of the selected design configuration instantiation.
     *      @param [in] subHeaderNumber         Number for the sub header.
     *      @param [in] viewNumber              Number defining the selected view.
     *      @param [in] instantiationNumber     Number defining the component instantiation.
     */
    void writereferencedDesignConfigurationInstantiation(QString const& instantiationTabs, QTextStream& stream,
        QString const& configurationReference, int const& subHeaderNumber, int const& viewNumber,
        int const& instantiationNumber);

    /*!
     *  Get the selected design configuration instantiation.
     *
     *      @param [in] instantiationReference  Name of the selected design configuration instantiation.
     */
    QSharedPointer<DesignConfigurationInstantiation> getDesignConfigurationInstantiation(
        QString const& instantiationReference) const;

    /*!
     *  Write the document contained within the selected VLNV.
     *
     *      @param [in] stream                  Text stream to write the document.
     *      @param [in] documentType            Type of the document.
     *      @param [in] instantiationItemTabs   Document indentation in the file.
     *      @param [in] vlnvReference           VLNV of the selected document.
     */
    void writeDocumentReference(QTextStream& stream, QString const& documentType,
        QString const& instantiationItemTabs, QSharedPointer<ConfigurableVLNVReference> vlnvReference);

    /*!
     *  Write the selected configurable element values.
     *
     *      @param [in] stream                  Text stream to write the document.
     *      @param [in] instantiationsItemTabs  Configurable element indentation in the file.
     *      @param [in] vlnvReference           Configurable VLNV containing the selected configurable element
     *                                          values.
     *      @param [in] instantiationFormatter  Expression formatter for the configurable element values.
     */
    void writeConfigurableElementValues(QTextStream& stream, QString const& instantiationsItemTabs,
        QSharedPointer<ConfigurableVLNVReference> vlnvReference, ExpressionFormatter* instantiationFormatter);

    /*!
     *  Write the referenced design instantiation.
     *
     *      @param [in] instantiationTabs       Design instantiation indentation in the file.
     *      @param [in] stream                  Text stream to write the component instantiation.
     *      @param [in] designReference         Name of the selected design instantiation.
     *      @param [in] subHeaderNumber         Number for the sub header.
     *      @param [in] viewNumber              Number defining the selected view.
     *      @param [in] instantiationNumber     Number defining the component instantiation.
     */
    void writeReferencedDesignInstantiation(QString const& instantiationTabs, QTextStream& stream,
        QString const& designReference, int const& subHeaderNumber, int const& viewNumber,
        int const& instantiationNumber);

    /*!
     *  Get the selected design instantiation.
     *
     *      @param [in] designReference     Name of the selected design instantiation.
     */
    QSharedPointer<DesignInstantiation> getDesignInstantiation(QString const& designReference) const;

    /*!
     *  Write the design diagram referenced by the design instantiation or the design configuration.
     *
     *      @param [in] stream          Text stream to write the design diagram.
     *      @param [in] viewTabs        Design diagram indentation in the file.
     *      @param [in] view            View containing the design and design configuration references.
     *      @param [in] pictureList     List of file names to add the pictures of the referenced designs to.
     */
    void writeDesign(QTextStream& stream, QString const& viewTabs, QSharedPointer<View> view,
        QStringList& pictureList);

    /*!
     *  Get the design configuration referenced by the selected view.
     *
     *      @param [in] view    The selected view.
     *
     *      @return The design configuration referenced by the selected view.
     */
    QSharedPointer<DesignConfiguration> getDesignConfiguration(QSharedPointer<View> view) const;

    /*!
     *  Get the design referenced by the selected view.
     *
     *      @param [in] view    The selected view.
     *
     *      @return The design referenced by the selected view.
     */
    QSharedPointer<Design> getDesign(QSharedPointer<View> view, QSharedPointer<DesignConfiguration> configuration)
        const;

	/*!
     *  Create a picture for the design
	 *
     *      @param [in] pictureList     List of file names to add the path of the created picture to.
	 *      @param [in] viewName        Name of the view to create the design picture for.
	*/
	void createDesignPicture(QStringList& pictureList, const QString& viewName);

    /*!
     *  Write the component instances contained within the selected design.
     *
     *      @param [in] stream          Text stream to write the component instances.
     *      @param [in] design          The selected design.
     *      @param [in] configuration   The selected design configuration.
     *      @param [in] viewTabs        Component instance indentation in the file.
     */
    void writeDesignInstances(QTextStream& stream, QSharedPointer<Design> design,
        QSharedPointer<DesignConfiguration> configuration, QString const& viewTabs);

    /*!
     *  Write the selected description.
     *
     *      @param [in] stream              Text stream to write the description.
     *      @param [in] descriptionTabs     Description indentation in the file.
     *      @param [in] description         The selected description text.
     */
    void writeDescription(QTextStream& stream, QString const& descriptionTabs, QString const& description);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Factory for constructing design widgets used to create a picture of the design diagram.
    DesignWidgetFactory* designWidgetFactory_;

    //! Parameter finder for the documented component.
    QSharedPointer<ComponentParameterFinder> componentFinder_;
};

#endif // VIEWDOCUMENTGENERATOR_H
