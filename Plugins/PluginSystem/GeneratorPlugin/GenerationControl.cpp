//-----------------------------------------------------------------------------
// File: GenerationControl.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 26.01.2017
//
// Description:
// Class to control the over structure of generation.
//-----------------------------------------------------------------------------

#include "GenerationControl.h"

#include <library/LibraryInterface.h>

#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <QFileInfo>
#include <QRegularExpression>
#include <QDateTime>

//-----------------------------------------------------------------------------
// Function: GenerationControl::GenerationControl()
//-----------------------------------------------------------------------------
GenerationControl::GenerationControl(LibraryInterface* library,
    IWriterFactory* factory, GenerationTuple input, GenerationSettings* settings) :
	library_(library), factory_(factory), input_(input), settings_(settings), isDesignGeneration_(input.design != 0),
    outputControl_(new OutputControl)
{
    // Find views usable for the generation.
    QSharedPointer<QList<QSharedPointer<View> > > possibleViews;

    if (isDesignGeneration_)
    {
        possibleViews = findPossibleViews(input);
    }
    else
    {
        possibleViews = findPossibleViews(input.component);
    }

    // The usable instantiations and file sets are, in principle, the ones of the component.
    QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > possibleInstantiations =
        input.component->getComponentInstantiations();
    QSharedPointer<QList<QSharedPointer<FileSet> > > possibleFileSets = input.component->getFileSets();

    // Initialize model for view selection.
    viewSelection_ = QSharedPointer<ViewSelection>(
        new ViewSelection(factory->getLanguage(), factory_->getGroupIdentifier(), settings_->lastViewName_, settings_->lastFileSetName_,
        possibleViews, possibleInstantiations, possibleFileSets));

    // Set the defaults for convenience.
    outputControl_->setOutputPath(defaultOutputPath());
}

//-----------------------------------------------------------------------------
// Function: GenerationControl::~GenerationControl()
//-----------------------------------------------------------------------------
GenerationControl::~GenerationControl()
{
}

//-----------------------------------------------------------------------------
// Function: GenerationControl::writeDocuments()
//-----------------------------------------------------------------------------
bool GenerationControl::writeDocuments()
{
    // Remember the values chosen by the user.
    settings_->lastFileSetName_ = getViewSelection()->getFileSetName();

    if (getViewSelection()->getView())
    {
        settings_->lastViewName_ = getViewSelection()->getView()->name();
    }

    QSharedPointer<ComponentInstantiation> instantiation = viewSelection_->getInstantiation();
    QSharedPointer<FileSet> fileSet;

    if (viewSelection_->getSaveToFileset())
    {
        fileSet = viewSelection_->getFileSet();

        // If the file set does not exist, create a new one with the same name.
        if (!fileSet)
        {
            fileSet = QSharedPointer<FileSet>(new FileSet(viewSelection_->getFileSetName(), factory_->getGroupIdentifier()));
            input_.component->getFileSets()->append(fileSet);
        }
        else if (!factory_->getGroupIdentifier().isEmpty() &&
            !fileSet->getGroups()->contains(factory_->getGroupIdentifier()))
        {
            fileSet->getGroups()->append(factory_->getGroupIdentifier());
        }

        // If instantiation exists, make sure that the instantiation refers to the file set.
        if (instantiation && !instantiation->getFileSetReferences()->contains(fileSet->name()))
        {
            instantiation->getFileSetReferences()->append(fileSet->name());
        }
    }

    QString componentPath = library_->getPath(input_.component->getVlnv());

    bool fails = false;

    // Go through each potential file.
    foreach(QSharedPointer<GenerationOutput> output, *outputControl_->getOutputs())
    {
        // Form the path from the determined output path plus determined file name.
        QString absFilePath = outputControl_->getOutputPath() + "/" + output->fileName_;

        // Try to open the file.
        QFile outputFile(absFilePath); 
        if (!outputFile.open(QIODevice::WriteOnly))
        {
            input_.messages->errorMessage(QObject::tr("Could not open output file for writing: %1").arg(absFilePath));
            fails = true;
            continue;
        }

        // Put the content to the file and close it.
        QTextStream outputStream(&outputFile);

        outputStream << output->fileContent_;

        outputFile.close();

        if (viewSelection_->getSaveToFileset())
        {
            // Need a path for the IP-XACT file: It must be relative to the file path of the document.
            QString ipFilePath = General::getRelativePath(componentPath, absFilePath);
            // Add the new file to the file set.
            QSettings settings;
            QSharedPointer<File> ipFile = fileSet->addFile(ipFilePath, settings);

            // Insert the proper description to the IP-XACT file.
            insertFileDescription(ipFile);
        }
    }

    // Return false if something fails.
    return !fails && saveChanges();
}

//-----------------------------------------------------------------------------
// Function: GenerationControl::saveChanges()
//-----------------------------------------------------------------------------
bool GenerationControl::saveChanges()
{
    // The component VLNV in string format.
    QString component = input_.component->getVlnv().toString();

    // Try to save.
    bool saveSucceeded = library_->writeModelToFile(input_.component);

    if (saveSucceeded)
    {
        return true;
    }    

    // Fail: Report the error, including the path.
    QString savePath = library_->getPath(input_.component->getVlnv());
    input_.messages->errorMessage(QObject::tr("Could not write component %1 to file %2.").arg(component, savePath));
    return false;
}

//-----------------------------------------------------------------------------
// Function: GenerationControl::validSelections()
//-----------------------------------------------------------------------------
bool GenerationControl::validSelections(QString &warning)
{
    // Must have a file set as well.
    if (!viewSelection_->validSelections(warning))
    {
        return false;
    }

    // Must have valid file output.
    if (!outputControl_->validSelections(warning))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: GenerationControl::parseDocuments()
//-----------------------------------------------------------------------------
void GenerationControl::parseDocuments()
{
    // Clear the existing list of files.
    outputControl_->getOutputs()->clear();

    if (isDesignGeneration_)
    {
        // Time to write the contents to files
        input_.messages->sendNotice(QObject::tr("Parsing hierarchy %1.").
            arg(QDateTime::currentDateTime().toString(Qt::LocalDate)));

        // Parse the design hierarchy.
        QList<QSharedPointer<MetaDesign> > designs =
            MetaDesign::parseHierarchy(library_, input_, viewSelection_->getView());

        // No results -> return.
        if (designs.size() < 1)
        {
            input_.messages->errorMessage("No parsed designs in the hierarchy!");
            return;
        }

        // Write outputs.
        input_.messages->sendNotice(QObject::tr("Writing content for preview %1.").
            arg(QDateTime::currentDateTime().toString(Qt::LocalDate)));

        // Pass the topmost design.
        QList<QSharedPointer<GenerationOutput> > documents = factory_->prepareDesign(designs);

        // Go through the parsed designs.
        foreach(QSharedPointer<GenerationOutput> output, documents)
        {
            if (!output)
            {
                continue;
            }

            // Write the contents to a file.
            output->write(outputControl_->getOutputPath());

            // Append to the list of proposed outputs.
            outputControl_->getOutputs()->append(output);
        }
    }
    else
    {
        // Parse component metadata.
        input_.messages->sendNotice(QObject::tr("Formatting component %1.").
            arg(QDateTime::currentDateTime().toString(Qt::LocalDate)));

        QSharedPointer<MetaComponent> componentParser
            (new MetaComponent(input_.messages, input_.component, viewSelection_->getView()));
        componentParser->formatComponent();

        // Form writers from parsed data.
        QSharedPointer<GenerationOutput> output = factory_->
            prepareComponent(outputControl_->getOutputPath(), componentParser);

        if (!output)
        {
            return;
        }

        // Write outputs.
        input_.messages->sendNotice(QObject::tr("Writing content for preview %1.").
            arg(QDateTime::currentDateTime().toString(Qt::LocalDate)));
        output->write(outputControl_->getOutputPath());

        // Append to the list of proposed outputs.
        outputControl_->getOutputs()->append(output);
    }
}

//-----------------------------------------------------------------------------
// Function: GenerationControl::getViewSelection()
//-----------------------------------------------------------------------------
QSharedPointer<ViewSelection> GenerationControl::getViewSelection() const
{
    return viewSelection_;
}

//-----------------------------------------------------------------------------
// Function: GenerationControl::getFileOutput()
//-----------------------------------------------------------------------------
QSharedPointer<OutputControl> GenerationControl::getOutputControl() const
{
    return outputControl_;
}

//-----------------------------------------------------------------------------
// Function: GenerationControl::getSettings()
//-----------------------------------------------------------------------------
GenerationSettings* GenerationControl::getSettings() const
{
    return settings_;
}

//-----------------------------------------------------------------------------
// Function: GenerationControl::isDesignGeneration()
//-----------------------------------------------------------------------------
bool GenerationControl::isDesignGeneration() const
{
    return isDesignGeneration_;
}

bool GenerationControl::isUnder() const
{
    QString componentPath = library_->getPath(input_.component->getVlnv());
    QFileInfo fileInfo = QFileInfo(componentPath);

    return outputControl_->getOutputPath().contains(fileInfo.absolutePath());
}

//-----------------------------------------------------------------------------
// Function: GenerationControl::findPossibleViews()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<View> > > GenerationControl::findPossibleViews(
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
// Function: GenerationControl::findPossibleViews()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<View> > > GenerationControl::findPossibleViews(
    GenerationTuple input) const
{
    // Create a new list object.
    QSharedPointer<QList<QSharedPointer<View> > > views = QSharedPointer<QList<QSharedPointer<View> > >
        (new QList<QSharedPointer<View> >);

    // The input must have a design
    if (!input.design)
    {
        return views;
    }

    // Go through each view of the containing component and pick the eligible ones.
    foreach(QSharedPointer<View> view, *input.component->getViews())
    {
        // Find the design instantiation of the view.
        QSharedPointer<DesignInstantiation> dis = input.component->getModel()->
            findDesignInstantiation(view->getDesignInstantiationRef());

        // If it exists it must refer to the input design.
        if (dis && *dis->getDesignReference() == input.design->getVlnv())
        {
            continue;
        }

        // Find the design configuration instantiation of the view.
        QSharedPointer<DesignConfigurationInstantiation> disg = input.component->getModel()->
            findDesignConfigurationInstantiation(view->getDesignConfigurationInstantiationRef());

        // If input has no design configuration, the instantiation must not exist.
        if (!input.designConfiguration && disg)
        {
            continue;
        }

        // If it does, the instantiation must exist and VLNVs must match.
        if (input.designConfiguration && !disg ||
            *disg->getDesignConfigurationReference() != input.designConfiguration->getVlnv())
        {
            continue;
        }

        // If above conditions are satisfied, the view may be used for generation.
        views->append(view);
    }

    // Finally, return the pickings.
    return views;
}

//-----------------------------------------------------------------------------
// Function: GenerationControl::defaultOutputPath()
//-----------------------------------------------------------------------------
QString GenerationControl::defaultOutputPath() const
{
    QString suggestedDir = "";

    QString topComponentPath = library_->getPath(input_.component->getVlnv());
    QString xmlDir =  QFileInfo(topComponentPath).canonicalPath();
    suggestedDir = xmlDir;

    return suggestedDir;
}

//-----------------------------------------------------------------------------
// Function: GenerationControl::relativePathForFile()
//-----------------------------------------------------------------------------
QString GenerationControl::relativePathFromXmlToFile(QString const& filePath) const
{
    QString xmlPath = library_->getPath(input_.component->getVlnv());
    return General::getRelativePath(xmlPath, filePath);
}

//-----------------------------------------------------------------------------
// Function: GenerationControl::insertFileDescription()
//-----------------------------------------------------------------------------
void GenerationControl::insertFileDescription(QSharedPointer<File> file)
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