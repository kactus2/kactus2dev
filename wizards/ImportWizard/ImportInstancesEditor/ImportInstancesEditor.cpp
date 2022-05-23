//-----------------------------------------------------------------------------
// File: ImportInstancesEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 02.04.2019
//
// Description:
// Used to generate IP-XACT component instances from parsed source files.
//-----------------------------------------------------------------------------

#include "ImportInstancesEditor.h"

#include <common/dialogs/LibrarySettingsDialog/LibrarySettingsDialog.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <wizards/ImportWizard/ImportInstancesEditor/ImportInstancesColumns.h>
#include <wizards/ImportWizard/ImportInstancesEditor/ImportInstancesDelegate.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/File.h>

#include <QHeaderView>
#include <QSettings>
#include <QMenu>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::ImportInstancesEditor()
//-----------------------------------------------------------------------------
ImportInstancesEditor::ImportInstancesEditor(QSharedPointer<Component> workingComponent,
    LibraryInterface* libraryAccess, QWidget* parent) :
QTableWidget(parent),
workingComponent_(workingComponent),
defaultPath_(""),
instanceDelegate_(),
libraryAccess_(libraryAccess),
browseXMLFileAction_(QString(tr("Browse")), this),
browseImportFileAction_(QString(tr("Browse")), this),
selectedItem_(),
libraryLocations_()
// instances_()
{
    QSettings settings(this);
    defaultPath_ = settings.value("Library/DefaultLocation", QString()).toString();

    setActiveLibraryLocations(settings);

    instanceDelegate_ = new ImportInstancesDelegate(workingComponent, defaultPath_, libraryAccess, this);

    setColumnCount(ImportInstancesColumns::COLUMN_COUNT);
    verticalHeader()->hide();
    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    horizontalHeader()->setStretchLastSection(true);

    QStringList horizontalHeaders;
    horizontalHeaders.insert(ImportInstancesColumns::INSTANCE, QString(tr("Instance")));
    horizontalHeaders.insert(ImportInstancesColumns::VLNV, QString(tr("VLNV")));
    horizontalHeaders.insert(ImportInstancesColumns::PATH, QString(tr("Path")));
    horizontalHeaders.insert(ImportInstancesColumns::XML_PATH, QString(tr("XML path")));

    setHorizontalHeaderLabels(horizontalHeaders);

    setItemDelegate(instanceDelegate_);

    connect(instanceDelegate_, SIGNAL(changeItemEditableStatus(QModelIndex const&, bool)),
        this, SLOT(changeItemEditableStatus(QModelIndex const&, bool)), Qt::UniqueConnection);
    connect(instanceDelegate_, SIGNAL(checkForNewLibraryLocation(QString const&)),
        this, SLOT(onCheckForNewLibraryLocation(QString const&)), Qt::UniqueConnection);
    connect(instanceDelegate_, SIGNAL(changeIndexItemTooltip(QModelIndex const&, QString const&)),
        this, SLOT(onChangeIndexItemTooltip(QModelIndex const&, QString const&)), Qt::UniqueConnection);

    connect(&browseImportFileAction_, SIGNAL(triggered()), this, SLOT(onBrowseImportFile()), Qt::UniqueConnection);
    connect(&browseXMLFileAction_, SIGNAL(triggered()), this, SLOT(onBrowseXMLFile()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::setActiveLibraryLocations()
//-----------------------------------------------------------------------------
void ImportInstancesEditor::setActiveLibraryLocations(QSettings const& settings)
{
    libraryLocations_.clear();
    QStringList activeLocations = settings.value("Library/ActiveLocations", QStringList()).toStringList();

    foreach(QString location, activeLocations)
    {
        if (!QFileInfo(location).isAbsolute())
        {
            QString fullLocation = QFileInfo(location).absoluteFilePath();
            libraryLocations_.append(fullLocation);
        }
        else
        {
            libraryLocations_.append(location);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::setupInstances()
//-----------------------------------------------------------------------------
void ImportInstancesEditor::setupInstances(QVector<InstanceData::instanceData> const& newInstances)
{
    clearContents();
    setRowCount(newInstances.count());
    if (!newInstances.isEmpty())
    {
        for (int i = 0; i < newInstances.size(); ++i)
        {
            InstanceData::instanceData const& instance = newInstances.at(i);
            createInstanceNameItem(instance.instanceName_, i);
        }

        createInstancePathItems(newInstances);
        createInstanceVLNVItems(newInstances);
    }

    horizontalHeader()->stretchLastSection();

    for (int i = 0; i < ImportInstancesColumns::COLUMN_COUNT; ++i)
    {
        if (i < ImportInstancesColumns::PATH)
        {
            resizeColumnToContents(i);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::createInstanceNameItem()
//-----------------------------------------------------------------------------
void ImportInstancesEditor::createInstanceNameItem(QString const& instanceName, int const& instanceRow)
{
    QTableWidgetItem* newItem = new QTableWidgetItem(instanceName);

    newItem->setFlags(newItem->flags() & ~Qt::ItemIsEnabled);
    newItem->setToolTip(instanceName);

    setItem(instanceRow, ImportInstancesColumns::INSTANCE, newItem);
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::createInstancePathItems()
//-----------------------------------------------------------------------------
void ImportInstancesEditor::createInstancePathItems(QVector<InstanceData::instanceData> const& instances)
{
    QMap<QString, QString> instanceFilePaths = getPossibleFilePaths(instances);

    for (int i = 0; i < instances.size(); ++i)
    {
        InstanceData::instanceData const& singleInstance = instances.at(i);
        QList<QString> filePaths = instanceFilePaths.values(singleInstance.instanceName_);
        QString selectedFile("");
        if (!filePaths.isEmpty())
        {
            selectedFile = filePaths.first();
        }

        QTableWidgetItem* newPathItem = new QTableWidgetItem(selectedFile);
        newPathItem->setToolTip(selectedFile);

        setItem(i, ImportInstancesColumns::PATH, newPathItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::getPossibleFilePaths()
//-----------------------------------------------------------------------------
QMap<QString, QString> ImportInstancesEditor::getPossibleFilePaths(
    QVector<InstanceData::instanceData> const& instances) const
{
    QMap<QString, QString> instanceFilePaths;

    QString componentPath = libraryAccess_->getPath(workingComponent_->getVlnv());

    QRegularExpression moduleExpression(QStringLiteral("module\\s+(\\w+)"));

    QVector<QSharedPointer<File> > verilogFiles = getCompatibleComponentFiles(workingComponent_);
    for (auto file : verilogFiles)
    {
        QString absoluteFilePath = General::getAbsolutePath(componentPath, file->name());
        QString fileText = readInputFile(file->name(), componentPath);

        QRegularExpressionMatch moduleMatch = moduleExpression.match(fileText);
        if (moduleMatch.hasMatch())
        {
            QString moduleName = moduleMatch.captured(1);

            for (auto const& singleInstance : instances)
            {
                if (moduleName.compare(singleInstance.moduleName_, Qt::CaseInsensitive) == 0)
                {
                    instanceFilePaths.insertMulti(singleInstance.instanceName_, absoluteFilePath);
                }
            }
        }
    }

    return instanceFilePaths;
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::getCompatibleComponentFiles()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<File> > ImportInstancesEditor::getCompatibleComponentFiles(
    QSharedPointer<Component> component) const
{
    QVector < QSharedPointer<File> > verilogFiles;

    for (auto fileSet : *component->getFileSets())
    {
        for (auto singleFile : *fileSet->getFiles())
        {
            if (fileIsVerilogFile(singleFile))
            {
                verilogFiles.append(singleFile);
            }
        }
    }

    return verilogFiles;
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::readInputFile()
//-----------------------------------------------------------------------------
QString ImportInstancesEditor::readInputFile(QString const& relativePath, QString const& basePath) const
{
    QString fileContent = "";

    QString absoluteFilePath = General::getAbsolutePath(basePath, relativePath);

    QFile importFile(absoluteFilePath);
    if (QFileInfo(absoluteFilePath).exists() && importFile.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&importFile);
        fileContent = stream.readAll();
        fileContent.replace("\r\n", "\n");
        importFile.close();
    }
    else
    {
        fileContent = tr("<File %1 not found in the file sets.>").arg(relativePath);
    }

    return fileContent;
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::fileIsVerilogFile()
//-----------------------------------------------------------------------------
bool ImportInstancesEditor::fileIsVerilogFile(QSharedPointer<File> file) const
{
    for (auto const& fileType : *file->getFileTypes())
    {
        if (fileType.contains(QStringLiteral("verilog"), Qt::CaseInsensitive))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::createInstanceVLNVItem()
//-----------------------------------------------------------------------------
void ImportInstancesEditor::createInstanceVLNVItems(QVector<InstanceData::instanceData> const& instances)
{
    QMap < QString, QSharedPointer<const Component> > instanceComponentMatches;
    for (auto const& vlnv : libraryAccess_->getAllVLNVs())
    {
        QSharedPointer<const Component> componentCandidate = getComponentFromVLNV(vlnv);
        if (componentCandidate)
        {
            QString vlnvName = vlnv.getName();

            for (auto const& singleInstance : instances)
            {
                if (vlnvName.compare(singleInstance.moduleName_) == 0 &&
                    instanceDataMatchesComponentPorts(singleInstance, componentCandidate))
                {
                    instanceComponentMatches.insertMulti(singleInstance.instanceName_, componentCandidate);
                }
            }
        }
    }

    instanceDelegate_->setupVLNVEditors(instanceComponentMatches, instances);

    for (int i = 0; i < instances.size(); ++i)
    {
        InstanceData::instanceData const& singleInstance = instances.at(i);

        VLNV newItemVLNV;
        QString itemVLNVText("");
        QString vlnvPath("");
        QList<QSharedPointer<const Component> > possibleComponents =
            instanceComponentMatches.values(singleInstance.instanceName_);

        bool pathIsEditable = true;

        if (possibleComponents.isEmpty())
        {
            newItemVLNV = workingComponent_->getVlnv();
            newItemVLNV.setName(singleInstance.moduleName_);
            newItemVLNV.setVersion(QStringLiteral("1.0"));

            QString newItemFileName =
                newItemVLNV.getName() + QLatin1Char('.') + newItemVLNV.getVersion() + QLatin1String(".xml");
            vlnvPath =
                defaultPath_ + newItemVLNV.toString(QStringLiteral("/")) + QLatin1Char('/') + newItemFileName;
        }
        else
        {
            newItemVLNV = possibleComponents.first()->getVlnv();
            vlnvPath = libraryAccess_->getPath(newItemVLNV);
            pathIsEditable = false;
        }

        itemVLNVText = newItemVLNV.toString();

        QTableWidgetItem* newVLNVItem = new QTableWidgetItem(itemVLNVText);
        QTableWidgetItem* newPathItem = new QTableWidgetItem(vlnvPath);
        newVLNVItem->setToolTip(itemVLNVText);
        newPathItem->setToolTip(vlnvPath);

        if (!pathIsEditable)
        {
            newPathItem->setFlags(newPathItem->flags() & ~Qt::ItemIsEnabled);
        }

        setItem(i, ImportInstancesColumns::VLNV, newVLNVItem);
        setItem(i, ImportInstancesColumns::XML_PATH, newPathItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::getComponentFromVLNV()
//-----------------------------------------------------------------------------
QSharedPointer<const Component> ImportInstancesEditor::getComponentFromVLNV(VLNV const& componentVLNV) const
{
    QSharedPointer<const Document> document = libraryAccess_->getModelReadOnly(componentVLNV);
    if (componentVLNV.getType() == VLNV::COMPONENT && document)
    {
        QSharedPointer<const Component> possibleComponent = document.dynamicCast<const Component>();
        return possibleComponent;
    }

    return QSharedPointer<const Component>();
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::instanceDataMatchesComponentPorts()
//-----------------------------------------------------------------------------
bool ImportInstancesEditor::instanceDataMatchesComponentPorts(InstanceData::instanceData const& instance,
    QSharedPointer<const Component> component) const
{
    if (instance.portConnections_.isEmpty())
    {
        return true;
    }

    if (!component->getPorts()->isEmpty())
    {
        for (auto const& instancePort : instance.portConnections_)
        {
            if (!component->getPortNames().contains(instancePort.first))
            {
                return false;
            }
        }

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::changeItemEditableStatus()
//-----------------------------------------------------------------------------
void ImportInstancesEditor::changeItemEditableStatus(QModelIndex const& index, bool isEditable)
{
    QTableWidgetItem* indexedItem = itemFromIndex(index);

    if (isEditable)
    {
        indexedItem->setFlags(indexedItem->flags() | Qt::ItemIsEnabled);
    }
    else
    {
        indexedItem->setFlags(indexedItem->flags() & ~Qt::ItemIsEnabled);
    }
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::contextMenuEvent()
//-----------------------------------------------------------------------------
void ImportInstancesEditor::contextMenuEvent(QContextMenuEvent* event)
{
    QModelIndex index = indexAt(event->pos());
    selectedItem_  = itemFromIndex(index);

    if (selectedItem_ && selectedItem_->flags() & Qt::ItemIsEnabled)
    {
        QMenu menu(this);
        if (index.column() == ImportInstancesColumns::PATH)
        {
            menu.addAction(&browseImportFileAction_);
        }
        else if (index.column() == ImportInstancesColumns::XML_PATH)
        {
            menu.addAction(&browseXMLFileAction_);
        }

        menu.exec(event->globalPos());
        event->accept();
    }

    QTableWidget::contextMenuEvent(event);
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::onBrowseImportFile()
//-----------------------------------------------------------------------------
void ImportInstancesEditor::onBrowseImportFile()
{
    QTableWidgetItem* instanceItem = item(selectedItem_->row(), ImportInstancesColumns::INSTANCE);
    QString instanceName = instanceItem->text();

    QString currentPath = getCurrentPath();

    QString targetPath = QFileDialog::getOpenFileName(
        NULL, tr("Select verilog file for instance %1").arg(instanceName), currentPath, tr("(*.v *.sv)"));
    if (targetPath.size() < 1)
    {
        return;
    }

    setTargetPath(targetPath);
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::getCurrentPath()
//-----------------------------------------------------------------------------
QString ImportInstancesEditor::getCurrentPath() const
{
    QString currentPath = selectedItem_->data(Qt::DisplayRole).toString();
    if (currentPath.size() < 1)
    {
        currentPath = defaultPath_;
    }

    return currentPath;
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::setTargetPath()
//-----------------------------------------------------------------------------
void ImportInstancesEditor::setTargetPath(QString const& targetPath)
{
    QString targetDirectory = QFileInfo(targetPath).absolutePath();

    if (targetDirectory.size() < 1)
    {
        targetDirectory = ".";
    }

    onCheckForNewLibraryLocation(targetDirectory);

    selectedItem_->setText(targetPath);
    selectedItem_->setToolTip(targetPath);
    selectedItem_ = nullptr;
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::onCheckForNewLibraryLocation()
//-----------------------------------------------------------------------------
void ImportInstancesEditor::onCheckForNewLibraryLocation(QString const& newPath)
{
    QSettings settings(this);
    while (!pathExistsInActiveLibraries(newPath))
    {
        QMessageBox warningDialog(QMessageBox::Warning, "Warning",
            "Chosen path is not in any active library. Do you want to configure libraries?",
            QMessageBox::Yes | QMessageBox::Cancel, this);

        warningDialog.setDetailedText(
            newPath + "\nis not in any of the active libraries:\n" + libraryLocations_.join("\n"));

        if (warningDialog.exec() == QMessageBox::Yes)
        {
            LibrarySettingsDialog dialog(settings, this);
            dialog.exec();
            setActiveLibraryLocations(settings);
        }
        else //if( msg == QMessageBox::Cancel )
        {
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::onChangeIndexItemTooltip()
//-----------------------------------------------------------------------------
void ImportInstancesEditor::onChangeIndexItemTooltip(QModelIndex const& index, QString const& newText)
{
    QTableWidgetItem* indexedItem = item(index.row(), index.column());
    indexedItem->setToolTip(newText);
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::onBrowseXMLFile()
//-----------------------------------------------------------------------------
void ImportInstancesEditor::onBrowseXMLFile()
{
    QString currentPath = getCurrentPath();

    QString targetPath = QFileDialog::getSaveFileName(NULL, tr("Save the XML to..."),
        currentPath, tr("(*.xml)"));

    if (targetPath.size() < 1)
    {
        return;
    }

    setTargetPath(targetPath);
}

//-----------------------------------------------------------------------------
// Function: ImportInstancesEditor::pathExistsInActiveLibraries()
//-----------------------------------------------------------------------------
bool ImportInstancesEditor::pathExistsInActiveLibraries(QString const& path) const
{
    //! Problem with folders like:
    //! .../verilogTests/example.v
    //! .../verilogTests2/example.v

    for (auto const& libraryPath : libraryLocations_)
    {
        if (path.startsWith(libraryPath))
        {
            return true;
        }
    }

    return false;
}
