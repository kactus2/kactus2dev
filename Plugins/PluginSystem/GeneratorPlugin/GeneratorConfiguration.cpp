//-----------------------------------------------------------------------------
// File: GeneratorConfiguration.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 26.01.2017
//
// Description:
// Container class for generator configuration.
//-----------------------------------------------------------------------------

#include "GeneratorConfiguration.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <Plugins/common/HDLParser/HDLComponentParser.h>
#include <Plugins/common/HDLParser/MetaDesign.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <QFileInfo>
#include <QRegularExpression>
#include <QDateTime>

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::GeneratorConfiguration()
//-----------------------------------------------------------------------------
GeneratorConfiguration::GeneratorConfiguration(LibraryInterface* library, IWriterFactory* factory,
    GenerationTuple input, GenerationSettings* settings) :
	library_(library), factory_(factory), input_(input), settings_(settings), isDesign_(input.design != 0),
    fileOutput_(new FileOuput)
{
    QSharedPointer<QList<QSharedPointer<View> > > possibleViews;

    if (isDesign_)
    {
        possibleViews = findPossibleViews(input);
    }
    else
    {
        possibleViews = findPossibleViews(input.component);
    }

    QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > possibleInstantiations =
        input.component->getComponentInstantiations();

    QSharedPointer<QList<QSharedPointer<FileSet> > > possibleFileSets = input.component->getFileSets();

    // Initialize model for view selection.
     viewSelection_ = QSharedPointer<ViewSelection>(
        new ViewSelection(factory->getLanguage(), settings_->lastViewName_, settings_->lastFileSetName_,
        possibleViews, possibleInstantiations, possibleFileSets));

    // Set the defaults for convenience.
    getFileOuput()->setOutputPath(defaultOutputPath());
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::~GeneratorConfiguration()
//-----------------------------------------------------------------------------
GeneratorConfiguration::~GeneratorConfiguration()
{
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::writeDocuments()
//-----------------------------------------------------------------------------
void GeneratorConfiguration::writeDocuments()
{
    // Remember the values chosen by the user.
    settings_->lastFileSetName_ = getViewSelection()->getFileSetName();

    if (getViewSelection()->getView())
    {
        settings_->lastViewName_ = getViewSelection()->getView()->name();
    }

    QSharedPointer<View> activeView = viewSelection_->getView();
    QSharedPointer<ComponentInstantiation> instantiation = viewSelection_->getInstantiation();
    QSharedPointer<FileSet> fileSet = viewSelection_->getFileSet();

    // If the file set does not exist, create a new one with the same name.
    if (!fileSet)
    {
        fileSet = QSharedPointer<FileSet>(new FileSet);
        fileSet->setName(viewSelection_->getFileSetName());
        input_.component->getFileSets()->append(fileSet);
    }

    // If instantiation exists, make sure that the instantiation refers to the file set.
    if (instantiation && !instantiation->getFileSetReferences()->contains(fileSet->name()))
    {
        instantiation->getFileSetReferences()->append(fileSet->name());
    }

    // Time to write the contents to files
    //library->printInfo(tr("Writing the file(s) %1.").arg(QDateTime::currentDateTime().toString(Qt::LocalDate)));

    foreach(QSharedPointer<GenerationFile> gFile, *fileOutput_->getFiles())
    {
        QString absFilePath = fileOutput_->getOutputPath() + "/" + gFile->fileName_;

        QFile outputFile(absFilePath); 
        if (!outputFile.open(QIODevice::WriteOnly))
        {
            //library->printError(tr("Could not open output file for writing: %1").arg(absFilePath));
            return;
        }

        QTextStream outputStream(&outputFile);

        outputStream << gFile->fileContent_;

        outputFile.close();

        // Need a path for the file: It must be relative to the file path of the document.
        QString ipFilePath = relativePathFromXmlToFile(absFilePath);
        // Add the new file to the file set.
        QSettings settings;
        // The resulting file will be added to the file set.
        QSharedPointer<File> ipFile = fileSet->addFile(ipFilePath, settings);

        // Insert the proper description to the file.
        insertFileDescription(ipFile);
    }

    // Write files
    //library->printInfo(tr("Finished writing the file(s)."));

    // Finally, save the changes to the affected document.
    saveChanges();
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::validSelections()
//-----------------------------------------------------------------------------
bool GeneratorConfiguration::validSelections(QString &warning)
{
    // Must have a file set as well.
    if (!viewSelection_->validSelections(warning))
    {
        return false;
    }

    // Must have valid file output.
    if (!fileOutput_->validSelections(warning))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::parseDocuments()
//-----------------------------------------------------------------------------
void GeneratorConfiguration::parseDocuments()
{
    // Clear the existing list of files.
    fileOutput_->getFiles()->clear();

    if (isDesign_)
    {
        // Parse the design hierarchy.
        QList<QSharedPointer<MetaDesign> > designs =
            MetaDesign::parseHierarchy(library_, input_, viewSelection_->getView());

        // Go through the parsed designs.
        foreach(QSharedPointer<MetaDesign> design, designs)
        {
            QSharedPointer<GenerationFile> gFile = factory_->prepareDesign(design);

            if (!gFile)
            {
                continue;
            }

            gFile->write();

            fileOutput_->getFiles()->append(gFile);
        }
    }
    else
    {
        QSharedPointer<HDLComponentParser> componentParser
            (new HDLComponentParser(library_, input_.component, viewSelection_->getView()));

        QSharedPointer<GenerationFile> gFile = factory_->prepareComponent(fileOutput_->getOutputPath(), componentParser);

        if (!gFile)
        {
            return;
        }

        gFile->write();

        fileOutput_->getFiles()->append(gFile);
    }
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getViewSelection()
//-----------------------------------------------------------------------------
QSharedPointer<ViewSelection> GeneratorConfiguration::getViewSelection() const
{
    return viewSelection_;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getFileOuput()
//-----------------------------------------------------------------------------
QSharedPointer<FileOuput> GeneratorConfiguration::getFileOuput() const
{
    return fileOutput_;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getSettings()
//-----------------------------------------------------------------------------
GenerationSettings* GeneratorConfiguration::getSettings() const
{
    return settings_;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::findPossibleViews()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<View> > > GeneratorConfiguration::findPossibleViews(
    QSharedPointer<const Component> targetComponent) const
{
    // If the generation is targeted to a component, return the flat views of the component.
    QSharedPointer<QList<QSharedPointer<View> > > views = QSharedPointer<QList<QSharedPointer<View> > >
        (new QList<QSharedPointer<View> >);

    // In practice, go through each view, and if it is not hierarchical, append to the list.
    foreach (QSharedPointer<View> view, *targetComponent->getViews())
    {
        if (!view->isHierarchical())
        {
            views->append(view);
        }
    }

    return views;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::findPossibleViews()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<View> > > GeneratorConfiguration::findPossibleViews(
    GenerationTuple input) const
{
    // Create a new list object.
    QSharedPointer<QList<QSharedPointer<View> > > views = QSharedPointer<QList<QSharedPointer<View> > >
        (new QList<QSharedPointer<View> >);

    // Go through each view of the containing component and pick the eligible ones.
    foreach(QSharedPointer<View> view, *input.component->getViews())
    {
        // Find the relevant instantiations of the inspected view.
        QSharedPointer<DesignConfigurationInstantiation> disg = input.component->getModel()->
            findDesignConfigurationInstantiation(view->getDesignConfigurationInstantiationRef());
        QSharedPointer<DesignInstantiation> dis = input.component->getModel()->
            findDesignInstantiation(view->getDesignInstantiationRef());

        // If either of the exists AND targets the correct VLNV, the view is eligible.
        if (disg && *disg->getDesignConfigurationReference() == input.designConfiguration->getVlnv()
            || dis && *dis->getDesignReference() == input.design->getVlnv())
        {
            views->append(view);
        }
    }

    // Finally, return the pickings.
    return views;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::defaultOutputPath()
//-----------------------------------------------------------------------------
QString GeneratorConfiguration::defaultOutputPath() const
{
    QString suggestedDir = "";

    QString topComponentPath = library_->getPath(input_.component->getVlnv());
    QString xmlDir =  QFileInfo(topComponentPath).canonicalPath();
    suggestedDir = xmlDir;

    return suggestedDir;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::relativePathForFile()
//-----------------------------------------------------------------------------
QString GeneratorConfiguration::relativePathFromXmlToFile(QString const& filePath) const
{
    QString xmlPath = library_->getPath(input_.component->getVlnv());
    return General::getRelativePath(xmlPath, filePath);
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::insertFileDescription()
//-----------------------------------------------------------------------------
void GeneratorConfiguration::insertFileDescription(QSharedPointer<File> file)
{
    // Get the current description.
    QString desc = file->getDescription();

    // Find the existing generation time statement.
    QRegularExpression regExp = QRegularExpression("(Generated at).+(by Kactus2. *)");
    // Rip it off.
    QRegularExpressionMatch match = regExp.match(desc);
    desc = desc.remove(match.capturedStart(), match.capturedLength());

    // Form string time and date string.
    QDateTime generationTime = QDateTime::currentDateTime();
    QString date = generationTime.date().toString("dd.MM.yyyy");
    QString time = generationTime.time().toString("hh:mm:ss");

    // Append the generation time description to the description.
    file->setDescription("Generated at " + time + " on " + date + " by Kactus2. " + desc);
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::saveChanges()
//-----------------------------------------------------------------------------
void GeneratorConfiguration::saveChanges()
{
    // The component VLNV in string format.
    QString component = input_.component->getVlnv().toString();

    // Try to save.
    bool saveSucceeded = library_->writeModelToFile(input_.component);

    if (saveSucceeded)
    {
        // Success: Inform the user.
        //library_->printInfo(tr("Saved changes to component %1.").arg(component));
    }    
    else
    {
        // Fail: Report the error, including the path.
        QString savePath = library_->getPath(input_.component->getVlnv());
        //library->printError(tr("Could not write component %1 to file %2.").arg(component, savePath));
    }
}