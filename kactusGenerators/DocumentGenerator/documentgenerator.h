//-----------------------------------------------------------------------------
// File: DocumentGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 19.09.2011
//
// Description:
// Generates documentation for a component and its associated items.
//-----------------------------------------------------------------------------

#ifndef DOCUMENTGENERATOR_H
#define DOCUMENTGENERATOR_H

#include <kactusGenerators/DocumentGenerator/DocumentationWriter.h>

#include <KactusAPI/include/ExpressionFormatterFactory.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/ComponentParameterFinder.h>

#include <editors/common/DesignWidgetFactory.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Component/Component.h>

#include <QTextStream>
#include <QList>
#include <QSharedPointer>
#include <QStringList>
#include <QWidget>

class AddressBlock;
class Register;
class Field;
class View;

//-----------------------------------------------------------------------------
//! Generates documentation for a component and its associated items.
//-----------------------------------------------------------------------------
class DocumentGenerator : public QObject
{
    Q_OBJECT

public:

    enum DocumentFormat
    {
        HTML,
        MD
    };

    /*!
     *  The constructor.
     *
     *      @param [in] handler                     Library, where the components reside.
     *      @param [in] vlnv                        VLNV of the component or design.
     *      @param [in] designWidgetFactory         Factory for making design widgets.
     *      @param [in] expressionFormatterFactory  Factory for making expression formatters.
     *      @param [in] componentNumber             Component number for current generator
     *      @param [in] parent                      The parent widget of the generator.
     */
    DocumentGenerator(LibraryInterface* handler, const VLNV& vlnv, DesignWidgetFactory* designWidgetFactory,
        ExpressionFormatterFactory* expressionFormatterFactory, int componentNumber, QWidget* parent);
    
    /*!
     *  The destructor
     */
    virtual ~DocumentGenerator();
    
    /*!
     *  Set the document format
     *
     *      @param [in] format          The document file format
     */
    void setFormat(DocumentFormat format);

    /*!
     *  Write the documentation to the selected stream.
     *
     *      @param [in] stream          The stream where to write.
     *      @param [in] targetPath      The path where the document will be placed.
     */
    void writeDocumentation(QTextStream& stream, QString targetPath);

    /*!
     *  Write the document header.
     *
     *      @param [in] stream  The text stream to write the header into.
     */
    void writeHeader(QTextStream& stream);

    /*!
     *  Write the table of contents for the component the generator is working on.
     *
     *      @param [in] componentNumber     The number for this component.
     *      @param [in] stream              The text stream to write the table of contents into.
     */
    void writeTableOfContents(QTextStream& stream);

    /*!
     *  Write the documentation for the given component.
     *
     *      @param [in] stream          The text stream to write the documentation into.
     *      @param [in] targetPath      File path to the file to be written.
     *      @param [in] filesToInclude  List of file names that contain pictures needed to display the document.
     */
    void writeDocumentation(QTextStream& stream, const QString& targetPath, QStringList& filesToInclude);

    /*!
     *  Write the kactus2 attributes of the component.
     *
     *      @param [in] stream              The text stream to write the documentation into.
     *      @param [in] subHeaderNumber     The number that defines the sub-header.
     */
    void writeKactusAttributes(QTextStream& stream, int& subHeaderNumber);

    /*!
     *  Write the parameters of the component.
     *
     *      @param [in] stream              The text stream to write the documentation into.
     *      @param [in] subHeaderNumber     The number that defines the sub-header.
     */
    void writeParameters(QTextStream& stream, int& subHeaderNumber);

    /*!
     *  Write the memory maps of the component.
     *
     *      @param [in] stream              The text stream to write the documentation into.
     *      @param [in] subHeaderNumber     The number that defines the sub header.
     */
    void writeMemoryMaps(QTextStream& stream, int& subHeaderNumber);

    /*!
     *  Write the address blocks of the memory map.
     *
     *      @param [in] addressBlocks       The list of the address blocks.
     *      @param [in] stream              The text stream to write the documentation into.
     *      @param [in] subHeaderNumber     The number that defines the sub header.
     *      @param [in] memoryMapNumber     The number for the memory map, whose address blocks are written.
     */
    void writeAddressBlocks(QList <QSharedPointer <AddressBlock> > addressBlocks, QTextStream& stream,
        int subHeaderNumber, int memoryMapNumber);

    /*!
     *  Write the registers of the address block.
     *
     *      @param [in] registers           The list of registers.
     *      @param [in] stream              The text stream to write the documentation into.
     *      @param [in] subHeaderNumber     The number that defines the sub header.
     *      @param [in] memoryMapNumber     The number that defines the memory map.
     *      @param [in] addressBlockNumber  The number that defines the address block.
     */
    void writeRegisters(QList <QSharedPointer <Register> > registers, QTextStream& stream, int subHeaderNumber,
        int memoryMapNumber, int addressBlockNumber);

    /*!
     *  Write the fields of the register.
     *
     *      @param [in] currentRegister     The register, whose fields are to be written.
     *      @param [in] stream              The text stream to write the documentation into.
     */
    void writeFields(QSharedPointer <Register> currentRegister, QTextStream& stream);

    /*!
     *  Write the ports of the component.
     *
     *      @param [in] stream              The text stream to write the documentation into.
     *      @param [in] subHeaderNumber     The number that defines the sub header.
     */
    void writePorts(QTextStream& stream, int& subHeaderNumber);

    /*!
     *  Write the interfaces of the component.
     *
     *      @param [in] stream              The text stream to write the documentation into.
     *      @param [in] subHeaderNumber     The number that defines the sub header.
     */
    void writeInterfaces(QTextStream& stream, int& subHeaderNumber);

    /*!
     *  Write the file sets of the component.
     *
     *      @param [in] stream              The text stream to write the documentation into.
     *      @param [in] subHeaderNumber     The number that defines the sub header.
     */
    void writeFileSets(QTextStream& stream, int& subHeaderNumber);

    /*!
     *  Write the views of the component.
     *
     *      @param [in] stream              The text stream to write the component views.
     *      @param [in] subHeaderNumber     The number that defines the sub header.
     *      @param [in] pictureList         List of file names to add the pictures of the referenced designs to.
     */
    void writeViews(QTextStream& stream, int& subHeaderNumber, QStringList& pictureList);
    
    /*!
     *  Write the end of the document, if the format requires it.
     *
     *      @param [in] stream  The text stream to write the documentation into.
     */
    void writeEndOfDocument(QTextStream& stream);

signals:

    /*!
     *  Print an error message to the user.
     *
     *      @param [in] errorMessage    Selected error message.
     */
    void errorMessage(const QString& errorMessage);

    /*!
     *  Print a notification to the user.
     *
     *      @param [in] noticeMessage   Selected notification.
     */
    void noticeMessage(const QString& noticeMessage);

private:
    //! No copying. No assignment
    DocumentGenerator(const DocumentGenerator& other);
    DocumentGenerator& operator=(const DocumentGenerator& other);

    /*!
     *  Write the selected view.
     *
     *      @param [in] stream              The text stream to write the view.
     *      @param [in] view                The selected view.
     *      @param [in] subHeaderNumber     Number for the sub header.
     *      @param [in] viewNumber          Number defining the selected view.
     *      @param [in] pictureList         List of file names to add the pictures of the referenced designs to.
     */
    void writeSingleView(QTextStream& stream, QSharedPointer<View> view,
        int const& subHeaderNumber, int const& viewNumber, QStringList& pictureList);

    /*!
     *  Write the referenced component instantiation.
     *
     *      @param [in] stream                  Text stream to write the component instantiation.
     *      @param [in] instantiationReference  Name of the selected component instantiation.
     *      @param [in] subHeaderNumber         Number for the sub header.
     *      @param [in] viewNumber              Number defining the selected view.
     *      @param [in] instantiationNumber     Number defining the component instantiation.
     */
    void writeReferencedComponentInstantiation(QTextStream& stream, QString const& instantiationReference,
        int const& subHeaderNumber, int const& viewNumber, int const& instantiationNumber);

    /*!
     *  Write the referenced design configuration instantiation.
     *
     *      @param [in] stream                  Text stream to write the component instantiation.
     *      @param [in] configurationReference  Name of the selected design configuration instantiation.
     *      @param [in] subHeaderNumber         Number for the sub header.
     *      @param [in] viewNumber              Number defining the selected view.
     *      @param [in] instantiationNumber     Number defining the component instantiation.
     */
    void writeReferencedDesignConfigurationInstantiation(QTextStream& stream,
        QString const& configurationReference, int const& subHeaderNumber, int const& viewNumber,
        int const& instantiationNumber);
    /*!
     *  Write the referenced design instantiation.
     *
     *      @param [in] stream                  Text stream to write the component instantiation.
     *      @param [in] designReference         Name of the selected design instantiation.
     *      @param [in] subHeaderNumber         Number for the sub header.
     *      @param [in] viewNumber              Number defining the selected view.
     *      @param [in] instantiationNumber     Number defining the component instantiation.
     */
    void writeReferencedDesignInstantiation(QTextStream& stream, QString const& designReference,
        int const& subHeaderNumber, int const& viewNumber, int const& instantiationNumber);

    /*!
        *  Write the design diagram referenced by the design instantiation or the design configuration.
        *
        *      @param [in] stream          Text stream to write the design diagram.
        *      @param [in] viewTabs        Design diagram indentation in the file.
        *      @param [in] view            View containing the design and design configuration references.
        *      @param [in] pictureList     List of file names to add the pictures of the referenced designs to.
        */
    void writeDesign(QTextStream& stream, QSharedPointer<View> view, QStringList& pictureList);

    /*!
    *  Create a picture for the design
    *
    *      @param [in] pictureList     List of file names to add the path of the created picture to.
    *      @param [in] viewName        Name of the view to create the design picture for.
    */
    void createDesignPicture(QStringList& pictureList, QString const& viewName, QString const& designPicPath);

    /*!
     *  Get the selected design instantiation.
     *
     *      @param [in] designReference     Name of the selected design instantiation.
     *
     *      @return The design instantiation 
     */
    QSharedPointer<DesignInstantiation> getDesignInstantiation(QString const& designReference) const;

    /*!
     *  Get the referenced component instantiation.
     *
     *      @param [in] instantiationReference  Name of the selected component instantiation.
     *
     *      @return Component instantiation containing the selected name.
     */
    QSharedPointer<ComponentInstantiation> getComponentInstantiation(QString const& instantiationReference) const;

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
     *  Get the selected design configuration instantiation.
     *
     *      @param [in] instantiationReference  Name of the selected design configuration instantiation.
     * 
     *      @return The selected design configuration instantiation
     */
    QSharedPointer<DesignConfigurationInstantiation> getDesignConfigurationInstantiation(
        QString const& instantiationReference) const;

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
     *  Parse the child items for the document generator.
     *
     *      @param [in] objects     List of objects that have already been parsed so there won't be duplicate
     *                              components in the generated document.
     */
    void parseChildItems(QList<VLNV>& objects, int& currentComponentNumber);

    /*!
     *  Create a picture for the component.
     *
     *      @param [in] pictureList     List of file names to add the path of the created picture to.
     */
    void createComponentPicture(QStringList& pictureList);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! Pointer to the instance that manages the library.
    LibraryInterface* libraryHandler_;

    //! Pointer to the component whose documentation this generator writes.
    QSharedPointer<Component> component_;

    //! The running number that used for writing the numbered headers.
    int componentNumber_;

    //! The file path to the documentation file being written.
    QString targetPath_;

    //! The factory for creating expression formatters.
    ExpressionFormatterFactory* expressionFormatterFactory_;
    
    //! List contains pointers to the document generators under this generator.
    QList<QSharedPointer<DocumentGenerator> > childInstances_;

    //! Pointer to the instance that owns the top document generator.
    QWidget* parentWidget_;

    //! The expression formatter, used to change parameter IDs into names.
    ExpressionFormatter* expressionFormatter_;

    //! Factory for constructing design widgets used to create a picture of the design diagram.
    DesignWidgetFactory* designWidgetFactory_;

    //! The active writer
    DocumentationWriter* writer_;

    //! Parameter finder for the documented component.
    QSharedPointer<ComponentParameterFinder> componentFinder_;

    //! The current document format
    DocumentFormat currentFormat_;

    //! Writer for markdown
    DocumentationWriter* mdWriter_;

    //! Writer for HTML
    DocumentationWriter* htmlWriter_;
};

#endif // DOCUMENTGENERATOR_H
