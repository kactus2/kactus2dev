//-----------------------------------------------------------------------------
// File: SystemMemoryMapHeaderWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.05.2015
//
// Description:
// The implementation for creating c-headers of a system, including other generated code for the HW platform.
//-----------------------------------------------------------------------------

#include "SystemMemoryMapHeaderWriter.h"

#include <library/LibraryManager/libraryinterface.h>

#include <Plugins/PluginSystem/IPluginUtility.h>
#include <Plugins/MemoryMapHeaderGenerator/fileSaveDialog/filesavedialog.h>

#include <IPXACTmodels/SWView.h>
#include <IPXACTmodels/SystemView.h>
#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/librarycomponent.h>

#include <QDate>
#include <QDir>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: SystemMemoryMapHeaderWriter::SystemMemoryMapHeaderWriter()
//-----------------------------------------------------------------------------
SystemMemoryMapHeaderWriter::SystemMemoryMapHeaderWriter(IPluginUtility* utility, QObject* parentObject):
BaseMemoryMapHeaderWriter(utility),
parentObject_(parentObject),
systemGeneratorSettings_()
{

}

//-----------------------------------------------------------------------------
// Function: SystemMemoryMapHeaderWriter::SystemMemoryMapHeaderWriter()
//-----------------------------------------------------------------------------
SystemMemoryMapHeaderWriter::~SystemMemoryMapHeaderWriter()
{

}

//-----------------------------------------------------------------------------
// Function: SystemMemoryMapHeaderWriter::writeMemoryMapHeader()
//-----------------------------------------------------------------------------
void SystemMemoryMapHeaderWriter::writeMemoryMapHeader(QSharedPointer<Component> component,
    QSharedPointer<DesignConfiguration> designConfiguration, QSharedPointer<Design> design)
{
    QSharedPointer<SystemView> systemView = findSystemView(component, designConfiguration);

    QString hwViewName = systemView->getHWViewRef();

    VLNV designVLNV;
    foreach (QSharedPointer<View> hwView, component->getViews())
    {
        if (hwView->getName() == hwViewName)
        {
            designVLNV = hwView->getHierarchyRef();
        }
    }

    QSharedPointer<Design> hwDesign = utility_->getLibraryInterface()->getDesign(designVLNV);

    Q_ASSERT(component);
    Q_ASSERT(designConfiguration);
	Q_ASSERT(design);
	Q_ASSERT(utility_);

    systemGeneratorSettings_.clear();

    QSharedPointer<SystemView> sysView = component->findSystemView(*designConfiguration->getVlnv());
	Q_ASSERT(sysView);

	QStringList usedFileTypes;
	usedFileTypes.append("cSource");
	usedFileTypes.append("cppSource");

	// find all CPU instances
	foreach (const ComponentInstance& instance, design->getComponentInstances())
    {
		VLNV instanceVLNV = instance.getComponentRef();
		QSharedPointer<const LibraryComponent> libComp = utility_->getLibraryInterface()->
            getModelReadOnly(instanceVLNV);
		QSharedPointer<const Component> instComponent = libComp.dynamicCast<const Component>();
		Q_ASSERT(instComponent);

		if (!instComponent->isCpu())
        {
			continue;
		}

		// create header settings object for the CPU instance
		SystemHeaderSaveModel::SysHeaderOptions systemHeaderOption(instance.getInstanceName(), instanceVLNV);

		systemHeaderOption.instanceId_ = instance.getUuid();

        QString activeView = designConfiguration->getActiveView(instance.getInstanceName());
		if (!activeView.isEmpty() && instComponent->hasSWView(activeView))
        {
			QSharedPointer<SWView> swView = instComponent->findSWView(activeView);
			QStringList fileSets = swView->getFileSetRefs();

			// the files that are included in the active view
			QStringList files = instComponent->getFilesFromFileSets(fileSets, usedFileTypes);

			QString sourcePath = utility_->getLibraryInterface()->getPath(instanceVLNV);

			// convert all relative paths from component to absolute path
			foreach (QString relPath, files)
            {
				QString absolutePath = General::getAbsolutePath(sourcePath, relPath);

				QFileInfo info(absolutePath);
				systemHeaderOption.includeFiles_.append(info);
			}
		}
        systemGeneratorSettings_.append(systemHeaderOption);
	}

	SystemHeaderSaveModel model(utility_->getLibraryInterface(), parentObject_);
    model.setObjects(component, systemGeneratorSettings_);

	FileSaveDialog dialog(utility_->getParentWidget());
	dialog.setModel(&model);

	int result = dialog.exec();

	if (result == QDialog::Rejected)
    {
        informGenerationAbort();
		return;
	}

    systemGeneratorSettings_ = model.getObjects();

    searchInstanceFiles(component, sysView->getHWViewRef());

	bool changed = false;

    informStartOfGeneration();

	// write the system headers for CPU instances
    foreach (const SystemHeaderSaveModel::SysHeaderOptions& systemHeaderOption, systemGeneratorSettings_)
    {
		QFile file(systemHeaderOption.sysHeaderInfo_.absoluteFilePath());

        checkDirectoryStructure(systemHeaderOption.sysHeaderInfo_.dir());

		if (!file.open(QFile::Truncate | QFile::WriteOnly))
        {
            openFileErrorMessage(systemHeaderOption.sysHeaderInfo_.absoluteFilePath());
			break;
		}

        QTextStream stream(&file);

        QString description(" * This file includes the header files which are automatically generated by Kactus2.\n"
            " * This file allows indirect includes for generated files, thus allowing more flexible SW "
            "development.\n"
            " * Generated for system design: " + design->getVlnv()->toString() + "\n"
            " * Target instance: " + systemHeaderOption.instanceName_ + "\n"
            "*/");

        QString headerGuard(component->getVlnv()->toString("_").toUpper() + "_" +
            systemHeaderOption.instanceName_.toUpper() + "_H");

        writeTopOfHeaderFile(stream, systemHeaderOption.sysHeaderInfo_.fileName(), headerGuard, description);

		foreach (const QFileInfo& includeFile, systemHeaderOption.includeFiles_)
        {
			stream << "#include \"" << includeFile.absoluteFilePath() << "\"" << endl;
		}

		stream << endl << "#endif /* " << headerGuard << " */" << endl;

        informWritingFinished(systemHeaderOption.sysHeaderInfo_.fileName());

		file.close();

		changed = true;

        addHeaderFile(component, systemHeaderOption.sysHeaderInfo_, systemHeaderOption.instanceName_,
            QStringList(), systemHeaderOption.instanceId_);
	}

	if (changed)
    {
		// save the changes to the file sets
        utility_->getLibraryInterface()->writeModelToFile(component);
	}

	// clear the settings after generation
    systemGeneratorSettings_.clear();

    informGenerationComplete();
}

//-----------------------------------------------------------------------------
// Function: SystemMemoryMapHeaderWriter::searchInstanceFiles()
//-----------------------------------------------------------------------------
void SystemMemoryMapHeaderWriter::searchInstanceFiles(QSharedPointer<const Component> component,
    const QString& hwViewName)
{
    if (!component->hasView(hwViewName))
    {
        return;
    }

    VLNV hierRef = component->getHierRef(hwViewName);

    if (!hierRef.isValid())
    {
        return;
    }

    QSharedPointer<const LibraryComponent> libDesConf = utility_->getLibraryInterface()->getModelReadOnly(hierRef);
    QSharedPointer<const DesignConfiguration> desConf = libDesConf.dynamicCast<const DesignConfiguration>();

    QSharedPointer<const Design> design;

    if (!desConf)
    {
        design = libDesConf.dynamicCast<const Design>();
    }
    else
    {
        VLNV designVLNV = desConf->getDesignRef();
        QSharedPointer<const LibraryComponent> libDes =
            utility_->getLibraryInterface()->getModelReadOnly(designVLNV);
        design = libDes.dynamicCast<const Design>();
    }

    if (!design)
    {
        return;
    }

    QList<SystemMemoryMapHeaderWriter::HierParsingInfo> instancesToParse;

    QStringList usedFileTypes;
    usedFileTypes.append("cSource");
    usedFileTypes.append("cppSource");

    foreach (const ComponentInstance& instance, design->getComponentInstances())
    {
        bool matched = false;

        // check if the instance matches one of the searched CPUs
        for (int i = 0; i < systemGeneratorSettings_.size(); ++i)
        {
            SystemHeaderSaveModel::SysHeaderOptions& systemHeaderOption = systemGeneratorSettings_[i];

            // if the CPU was already found before
            if (systemHeaderOption.found_)
            {
                continue;
            }

            else if (instance.getUuid() == systemHeaderOption.instanceId_)
            {
                systemHeaderOption.found_ = true;
                matched = true;

                QSharedPointer<const FileSet> instanceFileSet =
                    component->findFileSetById(systemHeaderOption.instanceId_);

                if (!instanceFileSet)
                {
                    break;
                }

                QStringList files = instanceFileSet->getFiles(usedFileTypes);

                QString sourcePath = utility_->getLibraryInterface()->getPath(*component->getVlnv());
                foreach (QString relPath, files)
                {
                    QString absolutePath = General::getAbsolutePath(sourcePath, relPath);

                    QFileInfo info(absolutePath);
                    systemHeaderOption.includeFiles_.append(info);
                }
                break;
            }
        }

        if (!matched)
        {
            VLNV instanceVLNV = instance.getComponentRef();
            QSharedPointer<const LibraryComponent> libComp =
                utility_->getLibraryInterface()->getModelReadOnly(instanceVLNV);
            QSharedPointer<const Component> instanceComponent = libComp.dynamicCast<const Component>();
            Q_ASSERT(instanceComponent);

            QString activeView = desConf->getActiveView(instance.getInstanceName());
            if (activeView.isEmpty())
            {
                QStringList hierViewNames = instanceComponent->getHierViews();

                if (hierViewNames.size() == 1)
                {
                    activeView = hierViewNames.first();
                }
                else
                {
                    continue;
                }
            }

            Q_ASSERT(!activeView.isEmpty());

            SystemMemoryMapHeaderWriter::HierParsingInfo info(instanceComponent, activeView);
            instancesToParse.append(info);
        }
    }

    bool allFound = true;
    foreach (const SystemHeaderSaveModel::SysHeaderOptions& systemHeaderOption, systemGeneratorSettings_)
    {
        if (!systemHeaderOption.found_)
        {
            allFound = false;
        }
    }

    // if there is at least one more CPU instance to find
    if (!allFound)
    {
        foreach (SystemMemoryMapHeaderWriter::HierParsingInfo info, instancesToParse)
        {
            searchInstanceFiles(info.comp_, info.activeView_);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemMemoryMapHeaderWriter::HierParsingInfo::HierParsingInfo()
//-----------------------------------------------------------------------------
SystemMemoryMapHeaderWriter::HierParsingInfo::HierParsingInfo(QSharedPointer<const Component> comp,
    const QString& viewName):
comp_(comp),
activeView_(viewName)
{

}

//-----------------------------------------------------------------------------
// Function: SystemMemoryMapHeaderWriter::HierParsingInfo::HierParsingInfo()
//-----------------------------------------------------------------------------
SystemMemoryMapHeaderWriter::HierParsingInfo::HierParsingInfo(const HierParsingInfo& other):
comp_(other.comp_),
activeView_(other.activeView_)
{

}

//-----------------------------------------------------------------------------
// Function: SystemMemoryMapHeaderWriter::HierParsingInfo::operator=()
//-----------------------------------------------------------------------------
SystemMemoryMapHeaderWriter::HierParsingInfo& SystemMemoryMapHeaderWriter::HierParsingInfo::operator=(
    const HierParsingInfo& other)
{
    if (&other != this)
    {
        comp_ = other.comp_;
        activeView_ = other.activeView_;
    }
    return *this;
}

//-----------------------------------------------------------------------------
// Function: SystemMemoryMapHeaderWriter::findSystemView()
//-----------------------------------------------------------------------------
QSharedPointer<SystemView> SystemMemoryMapHeaderWriter::findSystemView(QSharedPointer<Component> component,
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    foreach (QSharedPointer<SystemView> systemDesignView, component->getSystemViews())
    {
        if (systemDesignView->getHierarchyRef() == *designConfiguration->getVlnv())
        {
            return systemDesignView;
        }
    }

    // This should not be reached.
    return QSharedPointer<SystemView> ();
}
