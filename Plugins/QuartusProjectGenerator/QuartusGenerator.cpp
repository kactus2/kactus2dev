/* 
 *
 *  Created on: 24.11.2010
 *      Author: Antti Kamppi
 */

#include "QuartusGenerator.h"

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>

#include <IPXACTmodels/utilities/Search.h>
#include <IPXACTmodels/utilities/ComponentSearch.h>

#include <KactusAPI/include/IPluginUtility.h>
#include <KactusAPI/include/LibraryInterface.h>

#include <QDateTime>
#include <QDir>

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::QuartusGenerator()
//-----------------------------------------------------------------------------
QuartusGenerator::QuartusGenerator(IPluginUtility* utility) :
    utility_(utility),
    files_(),
    assignments_(),
    handler_(utility->getLibraryInterface()),
    timeReplacement_()
{

}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::readExistingPinMap()
//-----------------------------------------------------------------------------
void QuartusGenerator::readExistingPinMap(QSharedPointer<Component> component)
{
	assignments_.clear();

	// find the file wrapper that holds the assignments
    QSharedPointer<File> fileWrapper = getQuartusPinMap(component);
    if (fileWrapper.isNull())
    {
		utility_->printInfo(tr("No pin map found on the component"));
		return;
	}

	QString pinmapPath = General::getAbsolutePath(handler_->getPath(component->getVlnv()), fileWrapper->name());	
	QFileInfo pinmapInfo(pinmapPath);
	if (!pinmapInfo.exists())
    {
		utility_->printError(tr("The pin map file %1 was not found in file system.").arg(pinmapPath));
		return;
	}

	QFile mappingsFile(pinmapPath);
	if (!mappingsFile.open(QIODevice::ReadOnly))
    {
		utility_->printError(tr("File %1 could not be opened for reading.").arg(pinmapPath));
		return;
	}

	QTextStream stream(&mappingsFile);
    assignments_ = stream.readAll().split(QStringLiteral("\n"));
	mappingsFile.close();
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::parseFiles()
//-----------------------------------------------------------------------------
void QuartusGenerator::parseFiles(QSharedPointer<Component> component, QSharedPointer<View> view)
{
    Q_ASSERT_X(component, "QuartusGenerator::parseFiles", "Null component-pointer given as parameter");

    if (!view)
    {
        utility_->printInfo(tr("Component %1 didn't contain an active view, adding all found RTL-files from "
            "component file sets.").arg(component->getVlnv().getName()));

        parseAllFileSets(component);
    }
    else
    {
        utility_->printInfo(tr("Processing view %1 of component %2").arg(view->name(), 
            component->getVlnv().toString()));

        if (view->isHierarchical())
        {
            parseFilesFromHierarchicalView(view, component);
        }
        else
        {
            parseFilesFromFlatView(component, view);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::generateProject()
//-----------------------------------------------------------------------------
void QuartusGenerator::generateProject(QString const& outputPath, QString const& topEntity, 
    QString const& generatorInformation)
{
    // If the output dir doesn't exist then create it.
    QDir outputDir(outputPath);
    if (!outputDir.exists())
    {
        outputDir.mkpath(outputPath);
    }

    utility_->printInfo(tr("Writing Quartus project file."));
    writeQuartusProjectFile(outputPath, topEntity, generatorInformation);

    utility_->printInfo(tr("Writing Quartus settings file."));
    writeQuartusSettingsFile(outputPath, topEntity, generatorInformation);
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::replaceTime()
//-----------------------------------------------------------------------------
void QuartusGenerator::replaceTime(QString const& newTime)
{
    timeReplacement_ = newTime;
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::getQuartusPinMap()
//-----------------------------------------------------------------------------
QSharedPointer<File> QuartusGenerator::getQuartusPinMap(QSharedPointer<Component> component)
{
    QStringList pinTypes({ QStringLiteral("quartusPinmap"), QStringLiteral("quartusPinFile") });

    for (QSharedPointer<FileSet> fileSet : *component->getFileSets())
    {
        for (QSharedPointer<File> currentFile : *fileSet->getFiles())
        {
            for (QString fileType : *currentFile->getFileTypes())
            {
                if (pinTypes.contains(fileType))
                {
                    return currentFile;
                }
            }
        }
    }

    return QSharedPointer<File>();
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::writeQuartusProjectFile()
//-----------------------------------------------------------------------------
void QuartusGenerator::writeQuartusProjectFile(QString const& outputPath, QString const& topEntity, 
    QString const& generatorInformation)
{
	// Save previous working directory.
	QDir savedCurrentDir = QDir::current();

    QFile* quartusProjectFile = createQuartusProjectFile(outputPath, topEntity, QStringLiteral(".qpf"));
    if (quartusProjectFile->isOpen())
    {
        QTextStream stream(quartusProjectFile);

        writeHeader(stream, generatorInformation);

        // write project info.
        stream << QStringLiteral("QUARTUS_VERSION = \"10.0\"") << Qt::endl << Qt::endl;
        stream << QStringLiteral("# Revisions") << Qt::endl << Qt::endl;
        stream << QStringLiteral("PROJECT_REVISION = \"");
        stream << topEntity;
        stream << QStringLiteral("\"") << Qt::endl;

        quartusProjectFile->close();

        utility_->printInfo(tr("Finished writing file %1.").arg(quartusProjectFile->fileName()));
    }

    // Restore previous working directory.
	QDir::setCurrent(savedCurrentDir.path());
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::createQuartusProjectFile()
//-----------------------------------------------------------------------------
QFile* QuartusGenerator::createQuartusProjectFile(QString const& outputPath, QString const& topEntity,
    QString const& fileSuffix)
{
    QDir::setCurrent(outputPath);
    
    QString fileName = topEntity + fileSuffix;
    QFile* quartusProjectFile = new QFile(fileName);

    if (!quartusProjectFile->open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QString message(tr("Can't open file %1 for writing").arg(quartusProjectFile->fileName()));
        utility_->printError(message);
    }

    return quartusProjectFile;
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::writeHeader()
//-----------------------------------------------------------------------------
void QuartusGenerator::writeHeader(QTextStream& stream, QString const& generatorInformation)
{
    QString currentTime = timeReplacement_;
    if (currentTime.isEmpty())
    {
        currentTime = QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss dd.MM.yyyy"));
    }

    stream << "# ----------------------------------------------------------- #" << Qt::endl << Qt::endl;
    stream << tr("# Quartus project generated by Kactus2") << Qt::endl;
    stream << tr("# Date created ") << currentTime << Qt::endl;
    stream << tr("# This file was generated with ") << generatorInformation << Qt::endl << Qt::endl;
    stream << "# ----------------------------------------------------------- #" << Qt::endl << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::writeQuartusSettingsFile()
//-----------------------------------------------------------------------------
void QuartusGenerator::writeQuartusSettingsFile(QString const& outputPath, QString const& topEntity,
    QString const& generatorInformation)
{
    // Save previous working directory.
	QDir savedCurrentDir = QDir::current();

    QFile* quartusSettingsFile = createQuartusProjectFile(outputPath, topEntity, QStringLiteral(".qsf"));
    if (quartusSettingsFile->isOpen())
    {
        QTextStream stream(quartusSettingsFile);

        writeHeader(stream, generatorInformation);

        stream << "set_global_assignment -name TOP_LEVEL_ENTITY " << topEntity << Qt::endl;
        stream << "set_global_assignment -name ORIGINAL_QUARTUS_VERSION \"10.0 SP1\"" << Qt::endl;

        writeIncludedFiles(stream);
        writePinAssignments(stream);

        quartusSettingsFile->close();

        utility_->printInfo(tr("Finished writing file %1").arg(quartusSettingsFile->fileName()));
    }

    // Restore previous working directory.
    QDir::setCurrent(savedCurrentDir.path());
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::writeIncludedFiles()
//-----------------------------------------------------------------------------
void QuartusGenerator::writeIncludedFiles(QTextStream& stream)
{
    for (QString const& fileName : files_)
    {
        QString fileSuffix = QFileInfo(fileName).suffix();

        if (fileSuffix.compare(QLatin1String("vhd")) == 0 || fileSuffix.compare(QLatin1String("vhdl")) == 0)
        {
            stream << "set_global_assignment -name VHDL_FILE " << fileName << Qt::endl;
        }
        else if (fileSuffix.compare(QLatin1String("v")) == 0)
        {
            stream << "set_global_assignment -name VERILOG_FILE " << fileName << Qt::endl;
        }
        else if (fileSuffix.compare(QLatin1String("sv")) == 0)
        {
            stream << "set_global_assignment -name SYSTEMVERILOG_FILE " << fileName << Qt::endl;
        }
        else if (fileSuffix.compare(QLatin1String("qip")) == 0)
        {
            stream << "set_global_assignment -name QIP_FILE " << fileName << Qt::endl;
        }
        else if (fileSuffix.compare(QLatin1String("sdc")) == 0)
        {
            stream << "set_global_assignment -name SDC_FILE " << fileName << Qt::endl;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::writePinAssignments()
//-----------------------------------------------------------------------------
void QuartusGenerator::writePinAssignments(QTextStream &stream)
{
    for (QString const& assignment : assignments_)
    {
        stream << assignment << Qt::endl;
    }
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::parseAllFileSets()
//-----------------------------------------------------------------------------
void QuartusGenerator::parseAllFileSets(QSharedPointer<Component> component)
{
    Q_ASSERT_X(component, "ModelsimGenerator::parseAllFileSets", "Null component pointer given as parameter");

    QString basePath = handler_->getPath(component->getVlnv());
    if (basePath.isEmpty())
    {
        utility_->printError(tr("Component %1was not found in library.").arg(component->getVlnv().toString()));
        return;
    }

    QString componentVLNV = component->getVlnv().toString();

    // Parse all RTL files of the component.
	for (QSharedPointer<FileSet> fileSet : *component->getFileSets())
	{
		for (QSharedPointer<File> file : *fileSet->getFiles())
		{
			if (file->isRTLFile())
			{
                parseSingleFile(file, basePath, componentVLNV);
            }
		}
	}
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::parseSingleFile()
//-----------------------------------------------------------------------------
void QuartusGenerator::parseSingleFile(QSharedPointer<File> targetFile, QString const& basePath,
    QString const& componentVLNV)
{
    QString absolutePath = General::getAbsolutePath(basePath, targetFile->name());
    if (absolutePath.isEmpty())
    {
        utility_->printError(tr("The file %1 was not found within %2").arg(targetFile->getFileName(),
            absolutePath));
        return;
    }

    QFileInfo filePathInfo(absolutePath);
    if (!filePathInfo.exists())
    {
        utility_->printError(tr("The file %1 needed by component %2 was not found in the file system.").arg(
            absolutePath, componentVLNV));
        return;
    }

    if (files_.contains(absolutePath) == false)
    {
        files_.append(absolutePath);
    }
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::parseFileSets()
//-----------------------------------------------------------------------------
void QuartusGenerator::parseFileSets(QSharedPointer<Component> component, QStringList const& fileSetNames)
{
    Q_ASSERT_X(component, "ModelsimGenerator::parseFileSets", "Null component-pointer given as parameter");

    QString basePath = handler_->getPath(component->getVlnv());
    if (basePath.isEmpty())
    {
        utility_->printError(tr("Component %1 was not found within library. Stopping generation.").
            arg(component->getVlnv().toString()));
        return;
    }

    for (QString const& fileSetName : fileSetNames)
    {
        if (!component->hasFileSet(fileSetName))
        {
            utility_->printError(tr("Fileset %1 was not found within component %2.").arg(fileSetName, 
                component->getVlnv().toString()));
        }
        else
        {
            for (QSharedPointer<File> file : *component->getFileSet(fileSetName)->getFiles())
            {
                parseSingleFile(file, basePath, component->getVlnv().toString());
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::parseFilesFromHierarchicalView()
//-----------------------------------------------------------------------------
void QuartusGenerator::parseFilesFromHierarchicalView(QSharedPointer<View> view, QSharedPointer<Component> component)
{
    VLNV designVLNV = ComponentSearch::findDesignReference(component, view);
    VLNV desConfVLNV = ComponentSearch::findDesignConfigurationReference(component, view);

    QSharedPointer<const DesignConfiguration> designConf;

    if (desConfVLNV.isValid())
    {
        designConf = handler_->getModelReadOnly<DesignConfiguration>(desConfVLNV);

        if (designVLNV.isValid() == false)
        {
            designVLNV = designConf->getDesignRef();
        }
    }
    else if (designVLNV.isValid() == false)
    {
        utility_->printError(tr("Could not find valid design. Stopping generation."));
        return;
    }

    if (!handler_->contains(designVLNV))
    {
        utility_->printError(tr("Design %1 referenced withing design configuration %2 was not found within "
            "library. Stopping generation.").arg(designVLNV.toString(), desConfVLNV.getName()));
        return;
    }

    QSharedPointer<const Design> design = handler_->getModelReadOnly<Design>(designVLNV);

	readDesign(design, designConf);

    parseFilesFromFlatView(component, view);
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::readDesign()
//-----------------------------------------------------------------------------
void QuartusGenerator::readDesign(QSharedPointer<const Design> design, 
    QSharedPointer<const DesignConfiguration> desConf)
{
    Q_ASSERT_X(design, "ModelsimGenerator::readDesign", "Null Design-pointer given as parameter");

    for (QSharedPointer<ComponentInstance> instance : *design->getComponentInstances())
    {
        VLNV vlnv = *instance->getComponentRef();

        if (!handler_->contains(vlnv))
        {
            utility_->printError(tr("Component %1 was not found within library. Skipping.").
                arg(vlnv.toString()));
            continue;
        }

        QSharedPointer<Component> component = handler_->getModel<Component>(vlnv);
        if (component.isNull())
        {
            utility_->printError(tr("Referenced item %1 was not a component.").arg(vlnv.toString()));
            continue;
        }

        QSharedPointer<View> view;

        if (desConf && desConf->hasActiveView(instance->getInstanceName()))
        {
            QMap<QString, QString> viewOverrides = desConf->getKactus2ViewOverrides();

            QString viewName = viewOverrides.value(instance->getUuid(), 
                desConf->getActiveView(instance->getInstanceName()));

            view = Search::findByName(viewName, *component->getViews());
        }
        // if design configuration is not used or view was not found
        else
        {
            utility_->printInfo(tr("No active view selected for instance %1 of component %2.").arg(
                instance->getInstanceName(), vlnv.toString()));
        }

        parseFiles(component, view);
    }
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::parseFilesFromFlatView()
//-----------------------------------------------------------------------------
void QuartusGenerator::parseFilesFromFlatView(QSharedPointer<Component> component, QSharedPointer<View> view)
{
    QStringList fileSets;

    auto instance = Search::findByName(view->getComponentInstantiationRef(),
        *component->getComponentInstantiations());

    if (instance)
    {
        fileSets = *instance->getFileSetReferences();
    }

    parseFileSets(component, fileSets);
}
