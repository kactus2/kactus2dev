/* 
 *
 *  Created on: 24.11.2010
 *      Author: Antti Kamppi
 */

#include "quartusgenerator.h"

#include <library/LibraryManager/libraryhandler.h>

#include <IPXACTmodels/vlnv.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/librarycomponent.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/file.h>
#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/view.h>

#include <QObject>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QDateTime>
#include <QIODevice>
#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::QuartusGenerator()
//-----------------------------------------------------------------------------
QuartusGenerator::QuartusGenerator(LibraryInterface *handler, QWidget *parent):
QObject(parent), 
files_(), 
assignments_(),  
handler_(handler),
parent_(parent),
timeReplacement_()
{
	Q_ASSERT_X(handler, "QuartusGenerator constructor", "Null LibraryHandler pointer given for generator");
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::~QuartusGenerator()
//-----------------------------------------------------------------------------
QuartusGenerator::~QuartusGenerator()
{

}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::setBoardName()
//-----------------------------------------------------------------------------
void QuartusGenerator::readExistingPinMap(QSharedPointer<Component> component)
{
	assignments_.clear();

	// find the file wrapper that holds the assignments
    QSharedPointer<File> fileWrapper = getQuartusPinMap(component);

    if (!fileWrapper.data())
    {
		emit noticeMessage(tr("No pin map found on the component"));
		return;
	}

	QString pinmapPath = General::getAbsolutePath(handler_->getPath(*component->getVlnv()), fileWrapper->getName());
	
	// if the file did not exist
	QFileInfo pinmapInfo(pinmapPath);
	if (!pinmapInfo.exists())
    {
		emit errorMessage(tr("The pin map file %1 was not found in file system").arg(pinmapPath));
		return;
	}

	QFile mappingsFile(pinmapPath);

	// if the file can not be opened for reading
	if (!mappingsFile.open(QIODevice::ReadOnly))
    {
		emit errorMessage(tr("File %1 could not be opened for reading").arg(pinmapPath));
		return;
	}
	QTextStream stream(&mappingsFile);

    assignments_ = stream.readAll().split("\n");

	mappingsFile.close();
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::getQuartusPinMap()
//-----------------------------------------------------------------------------
QSharedPointer<File> QuartusGenerator::getQuartusPinMap(QSharedPointer<Component> component)
{
    foreach (QSharedPointer<FileSet> fileSet, component->getFileSets())
    {
        foreach (QSharedPointer<File> currentFile, fileSet->getFiles())
        {
            foreach (QString fileType, currentFile->getUserFileTypes())
            {
                if (fileType == "quartusPinmap" || fileType == "quartusPinFile")
                {
                    return currentFile;
                }
            }
        }
    }

    return QSharedPointer<File>();
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::generateProject()
//-----------------------------------------------------------------------------
void QuartusGenerator::generateProject(QString outputPath, const QString& top_entity,
    QString const& generatorInformation)
{
	// make sure the outputPath is a directory
	QFileInfo outPutInfo(outputPath);

	// if was not a directory
	if (!outPutInfo.isDir())
		outputPath = outPutInfo.absolutePath();

	// if the output dir doesn't exist then create it
	QDir outputDir(outputPath);
	if (!outputDir.exists())
    {
		outputDir.mkpath(outputPath);
	}

    emit noticeMessage(tr("Writing Quartus project file."));
	writeQuartusProjectFile(outputPath, top_entity, generatorInformation);

    emit noticeMessage(tr("Writing Quartus settings file."));
	writeQuartusSettingsFile(outputPath, top_entity, generatorInformation);
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::writeQuartusProjectFile()
//-----------------------------------------------------------------------------
void QuartusGenerator::writeQuartusProjectFile(const QString& outPutDir, const QString& top_entity,
    QString const& generatorInformation)
{
	// save previous working directory
	QDir savedCurrentDir = QDir::current();

    QSharedPointer<QFile> quartusProjectFile = createQuartusProjectFile(outPutDir, top_entity, ".qpf");

    if (quartusProjectFile->isOpen())
    {
        QTextStream stream(quartusProjectFile.data());

        writeHeader(stream, generatorInformation);

        QString entityName = top_entity;
        entityName.remove(".comp");

        // write project info
        stream << "QUARTUS_VERSION = \"10.0\"" << endl << endl;
        stream << "# Revisions" << endl << endl;
        stream << "PROJECT_REVISION = \"";
        stream << entityName;
        stream << "\"" << endl;

        quartusProjectFile->close();

        emit noticeMessage(tr("Finished writing file %1.").arg(quartusProjectFile->fileName()));
    }

    // restore previous working directory
	QDir::setCurrent(savedCurrentDir.path());
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::writeQuartusSettingsFile()
//-----------------------------------------------------------------------------
void QuartusGenerator::writeQuartusSettingsFile(const QString& outputDir, const QString& top_entity,
    QString const& generatorInformation)
{
	QDir savedCurrentDir = QDir::current();

    QSharedPointer<QFile> quartusSettingsFile = createQuartusProjectFile(outputDir, top_entity, ".qsf");

    if (quartusSettingsFile->isOpen())
    {
        QTextStream stream(quartusSettingsFile.data());

        writeHeader(stream, generatorInformation);

        stream << "set_global_assignment -name TOP_LEVEL_ENTITY " << top_entity << endl;
        stream << "set_global_assignment -name ORIGINAL_QUARTUS_VERSION \"10.0 SP1\"" << endl;

        for (int i = 0; i < files_.size(); ++i)
        {
            QFileInfo fileInfo(files_.at(i));

            if (fileInfo.suffix() == QString("vhd") || fileInfo.suffix() == QString("vhdl") )
            {
                stream << "set_global_assignment -name VHDL_FILE " << files_.at(i) << endl;
            }
            else if (fileInfo.suffix() == QString("v"))
            {
                stream << "set_global_assignment -name VERILOG_FILE " << files_.at(i) << endl;
            }
            else if (fileInfo.suffix() == QString("qip"))
            {
                stream << "set_global_assignment -name QIP_FILE " << files_.at(i) << endl;
            }
            else if (fileInfo.suffix() == QString("sdc"))
            {
                stream << "set_global_assignment -name SDC_FILE " << files_.at(i) << endl;
            }	

        }

        // write the pin assignments
        for (int i = 0; i < assignments_.size(); ++i)
        {
            stream << assignments_.at(i) << endl;
        }

        quartusSettingsFile->close();

        emit noticeMessage(tr("Finished writing file %1").arg(quartusSettingsFile->fileName()));
    }

    QDir::setCurrent(savedCurrentDir.path());
}


//-----------------------------------------------------------------------------
// Function: QuartusGenerator::createQuartusProjectFile()
//-----------------------------------------------------------------------------
QSharedPointer<QFile> QuartusGenerator::createQuartusProjectFile(QString const& outputDirectory,
    QString const& topEntity, QString const& fileSuffix)
{
    QDir::setCurrent(outputDirectory);

    // create file instance for the project file
    QString fileName = topEntity;
    fileName.remove(".comp");
    fileName += fileSuffix;
    QSharedPointer<QFile> quartusProjectFile (new QFile(fileName));

    if (!quartusProjectFile->open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QString message(tr("Can't open file %1 for writing").arg(quartusProjectFile->fileName()));
        emit errorMessage(message);
    }

    return quartusProjectFile;
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::writeHeader()
//-----------------------------------------------------------------------------
void QuartusGenerator::writeHeader(QTextStream& stream, QString const& generatorInformation)
{
    QString currentTime = QDateTime::currentDateTime().toString(QString("hh:mm:ss dd.MM.yyyy"));

    if (!timeReplacement_.isEmpty())
    {
        currentTime = timeReplacement_;
    }

	stream << "# ----------------------------------------------------------- #" << endl << endl;
	stream << tr("# Quartus project generated by Kactus2") << endl;
    stream << tr("# Date created ") << currentTime << endl;
    stream << tr("# This file was generated with ") << generatorInformation << endl << endl;
	stream << "# ----------------------------------------------------------- #" << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::parseFiles()
//-----------------------------------------------------------------------------
void QuartusGenerator::parseFiles(QSharedPointer<Component> component, const QString& viewName)
{
	Q_ASSERT_X(component, "QuartusGenerator::parseFiles", "Null component-pointer given as parameter");

    emit noticeMessage(tr("Processing view %1 of component %2").arg(viewName, component->getVlnv()->toString()));

	View* view = component->findView(viewName);
	if (!view)
    {
        emit noticeMessage(tr("Component %1 didn't contain an active view, adding all found RTL-files from "
            "component file sets.").arg(component->getVlnv()->getName()));

        parseBlindFileSet(component);
        return;
	}

	if (view && !view->isHierarchical())
    {
		QStringList fileSets = view->getFileSetRefs();
		parseFileSets(component, fileSets);
	}

	else if (view && view->isHierarchical())
    {
        parseFilesFromHierarchicalView(view, component);
	}
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::parseFileSets()
//-----------------------------------------------------------------------------
void QuartusGenerator::parseFileSets(QSharedPointer<Component> component, const QStringList& fileSetNames)
{
	Q_ASSERT_X(component, "ModelsimGenerator::parseFileSets", "Null component-pointer given as parameter");

	QString basePath = handler_->getPath(*component->getVlnv());
	if (basePath.isEmpty())
    {
        emit errorMessage(tr("Component %1 was not found within library. Stopping generation.").
            arg(component->getVlnv()->toString()));
        return;
	}

	foreach (QString fileSetName, fileSetNames)
    {
		if (!component->hasFileSet(fileSetName))
        {
            emit errorMessage(tr("Fileset %1 was not found within component %2.").
                arg(fileSetName, component->getVlnv()->toString()));
            continue;
		}

		QSharedPointer<FileSet> fileSet = component->getFileSet(fileSetName);

        foreach (QSharedPointer<File> file, fileSet->getFiles())
        {
            parseSingleFile(file, basePath, component->getVlnv()->toString());
		}
	}
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::parseSingleFile()
//-----------------------------------------------------------------------------
void QuartusGenerator::parseSingleFile(QSharedPointer<File> currentFile, QString const& basePath,
    QString const& componentVLNV)
{
    QString absolutePath = General::getAbsolutePath(basePath, currentFile->getName());
    QFileInfo filePathInfo(absolutePath);
    absolutePath = filePathInfo.canonicalFilePath();

    if (absolutePath.isEmpty())
    {
        emit errorMessage(tr("The file %1 was not found within %2").arg(currentFile->getName(), basePath));
        return;
    }

    if (!filePathInfo.exists())
    {
        emit errorMessage(tr("The file %1 needed by component %2 was not found in the file system.").
            arg(absolutePath, componentVLNV));
        return;
    }

    if (!files_.contains(absolutePath))
    {
        files_.append(absolutePath);
    }
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::parseBlindFileSet()
//-----------------------------------------------------------------------------
void QuartusGenerator::parseBlindFileSet(QSharedPointer<Component> component)
{
    Q_ASSERT_X(component, "ModelsimGenerator::parseBlindFileSet", "Null component pointer given as parameter");

    QString basePath = handler_->getPath(*component->getVlnv());
    if (basePath.isEmpty())
    {
        emit errorMessage(tr("Component %1:%2:%3:%4 was not found in library."));
        return;
    }

    // get all rtl files of the component
    QList<QSharedPointer<File> > fileList = component->getRTLFiles();
    foreach (QSharedPointer<File> file, fileList)
    {
        QString absolutePath = General::getAbsolutePath(basePath, file->getName());

        QFileInfo fileInfo(absolutePath);
        if (!fileInfo.exists())
        {
            emit errorMessage(tr("The file %1 needed by component %2 was not found in the file system.").
                arg(absolutePath, component->getVlnv()->toString()));
            continue;
        }
        if (!files_.contains(absolutePath))
        {
            files_.append(absolutePath);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::parseFilesFromHierarchicalView()
//-----------------------------------------------------------------------------
void QuartusGenerator::parseFilesFromHierarchicalView(View* view, QSharedPointer<Component> component)
{
    VLNV designVLNV;
    QSharedPointer<Design> design;

    VLNV desConfVLNV;
    QSharedPointer<DesignConfiguration> designConf;

    VLNV vlnv = view->getHierarchyRef();

    if (!handler_->contains(vlnv))
    {
        emit errorMessage(tr("Hierarchy reference %1 referenced within view %2 in component %3 was not found "
            "in the library. Stopping generation.").arg(vlnv.toString(), view->getName(),
            component->getVlnv()->getName()));
        return;
    }

    vlnv.setType(handler_->getDocumentType(vlnv));
    if (vlnv.getType() == VLNV::DESIGNCONFIGURATION)
    {
        desConfVLNV = vlnv;
        QSharedPointer<LibraryComponent> libComp = handler_->getModel(desConfVLNV);
        designConf = libComp.staticCast<DesignConfiguration>();

        designVLNV = designConf->getDesignRef();
    }
    else if (vlnv.getType() == VLNV::DESIGN)
    {
        designVLNV = vlnv;
    }
    else
    {
        emit errorMessage(tr("Item %1 was neither a design nor design configuration. Stopping generation.").
            arg(vlnv.toString()));
        return;
    }

    if (!handler_->contains(designVLNV))
    {
        emit errorMessage(tr("Design %1 referenced withing design configuration %2 was not found within "
            "library. Stopping generation.").arg(designVLNV.toString(), desConfVLNV.getName()));
        return;
    }

    QSharedPointer<LibraryComponent> libComp = handler_->getModel(designVLNV);
    design = libComp.staticCast<Design>();

    readDesign(design, designConf);

    QString topLevelView = view->getTopLevelView();
    if (!topLevelView.isEmpty())
    {
        parseFiles(component, topLevelView);
    }
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::readDesign()
//-----------------------------------------------------------------------------
void QuartusGenerator::readDesign(const QSharedPointer<Design> design,
                                  const QSharedPointer<DesignConfiguration> desConf)
{
	Q_ASSERT_X(design, "ModelsimGenerator::readDesign", "Null Design-pointer given as parameter");

	QList<ComponentInstance> instances = design->getComponentInstances();
	foreach (ComponentInstance const& instance, instances)
    {
		VLNV vlnv = instance.getComponentRef();

		if (!handler_->contains(vlnv))
        {
            emit errorMessage(tr("Component %1 was not found within library. Stopping generation.").
                arg(vlnv.toString()));
			continue;
		}

		QSharedPointer<Component> component;
		QSharedPointer<LibraryComponent> libComp = handler_->getModel(vlnv);

		if (libComp->getVlnv()->getType() == VLNV::COMPONENT)
        {
			component = libComp.staticCast<Component>();
        }
		else
        {
            emit errorMessage(tr("Referenced item %1 was not a component.").arg(vlnv.toString()));
			continue;
		}

		QString viewName;

		if (desConf && desConf->hasActiveView(instance.getInstanceName()))
        {
            QMap<QString, QString> viewOverrides = desConf->getKactus2ViewOverrides();

            viewName = viewOverrides.value(instance.getUuid(), desConf->getActiveView(instance.getInstanceName()));
		}
		// if design configuration is not used or view was not found
		else
        {
            emit noticeMessage(tr("No active view selected for instance %1 of component %2.").
                arg(instance.getInstanceName(), vlnv.toString()));
		}

		parseFiles(component, viewName);
	}
}

//-----------------------------------------------------------------------------
// Function: QuartusGenerator::replaceTime()
//-----------------------------------------------------------------------------
void QuartusGenerator::replaceTime(QString const& newTime)
{
    timeReplacement_ = newTime;
}
