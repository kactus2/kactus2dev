//-----------------------------------------------------------------------------
// File: FileDependencyEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 22.01.2013
//
// Description:
// File dependency editor which encapsulates the whole dependency UI.
//-----------------------------------------------------------------------------

#include "FileDependencyEditor.h"

#include "FileDependencyColumns.h"
#include "FileDependencyItem.h"
#include "FileDependencyDelegate.h"

#include <KactusAPI/include/FileHandler.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/File.h>

#include <IPXACTmodels/common/VLNV.h>

#include <KactusAPI/include/PluginManager.h>
#include <KactusAPI/include/ISourceAnalyzerPlugin.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QIcon>
#include <QFileInfoList>
#include <QSettings>
#include <QHeaderView>
#include <QAction>
#include <QVariant>

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::FileDependencyEditor()
//-----------------------------------------------------------------------------
FileDependencyEditor::FileDependencyEditor(QSharedPointer<Component> component,
    QString const& basePath, QWidget* parent):
QWidget(parent),
    toolbar_(this),
    progressBar_(this),
    graphWidget_(this),
    infoWidget_(this),
    directoryEditor_(basePath, component->getSourceDirectories(), this),
    component_(component),
    fileTypeLookup_(),
    ignoreExtList_(),
    model_(component, basePath + "/"),
    basePath_(basePath),
    scanning_(false),
    filterActions_(this),
    runAnalysisAction_(0),
    timer_(0)
{
    // Initialize the widgets.
    progressBar_.setStyleSheet("QProgressBar:horizontal { margin: 0px; border: none; background: #cccccc; } "
                               "QProgressBar::chunk:horizontal { background: #009eff;}");
    progressBar_.setFixedHeight(2);
    progressBar_.setTextVisible(false);
    progressBar_.setContentsMargins(0, 0, 0, 0);
    progressBar_.setValue(0);

    graphWidget_.setContentsMargins(0, 0, 0, 0);
    graphWidget_.getView()->setModel(&model_);
    graphWidget_.getView()->setItemDelegate(new FileDependencyDelegate(component, this));

    graphWidget_.getView()->resizeColumnToContents(FileDependencyColumns::TREE_EXPAND);
    graphWidget_.getView()->resizeColumnToContents(FileDependencyColumns::STATUS);
    graphWidget_.getView()->resizeColumnToContents(FileDependencyColumns::CREATE_DEPENDENCY);
    graphWidget_.getView()->setColumnWidth(FileDependencyColumns::PATH, 250);
    graphWidget_.getView()->header()->setSectionResizeMode(FileDependencyColumns::TREE_EXPAND, QHeaderView::Fixed);
    graphWidget_.getView()->header()->setSectionResizeMode(FileDependencyColumns::STATUS, QHeaderView::Fixed);
    graphWidget_.getView()->header()->setSectionResizeMode(FileDependencyColumns::CREATE_DEPENDENCY, QHeaderView::Fixed);

    setupToolbar();

    setupLayout();

    connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(dependencyChanged(FileDependency*)), this, SIGNAL(dependenciesChanged()), Qt::UniqueConnection);
    connect(graphWidget_.getView(), SIGNAL(dependencyChanged(FileDependency*)), this, SIGNAL(dependenciesChanged()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(analysisProgressChanged(int)),
            this, SLOT(updateProgressBar(int)), Qt::UniqueConnection);
    connect(graphWidget_.getView(), SIGNAL(selectionChanged(FileDependency*)),
            &infoWidget_, SLOT(setEditedDependency(FileDependency*)), Qt::UniqueConnection);

    connect(&infoWidget_, SIGNAL(dependencyChanged(FileDependency*)),
            &model_, SIGNAL(dependencyChanged(FileDependency*)), Qt::UniqueConnection);

    connect(&directoryEditor_, SIGNAL(contentChanged()),
        this, SLOT(onSourceDirectoriesChanged()), Qt::UniqueConnection);
    connect(&directoryEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::~FileDependencyEditor()
//-----------------------------------------------------------------------------
FileDependencyEditor::~FileDependencyEditor()
{

}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::setCompact()
//-----------------------------------------------------------------------------
void FileDependencyEditor::setCompact(bool compact)
{
    toolbar_.setVisible(!compact);
    infoWidget_.setVisible(!compact);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::setDependenciesEditable()
//-----------------------------------------------------------------------------
void FileDependencyEditor::setDependenciesEditable(bool editable)
{
    graphWidget_.getView()->setDependenciesEditable(editable);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::isScanning()
//-----------------------------------------------------------------------------
bool FileDependencyEditor::isScanning() const
{
    return scanning_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::refresh()
//-----------------------------------------------------------------------------
void FileDependencyEditor::refresh()
{
    model_.refresh();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::scan()
//-----------------------------------------------------------------------------
void FileDependencyEditor::scan()
{
    if (isScanning())
    {
        model_.stopAnalysis();
        return;
    }

    emit scanStarted();

    QApplication::setOverrideCursor(Qt::WaitCursor);

    runAnalysisAction_->setIcon(QIcon(":/icons/common/graphics/control-stop.png"));
    scanning_ = true;

    // Preparations. Resolve file types for each extension.
    resolveExtensionFileTypes();

    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(scanDirectories()));
    timer_->start();

    // Phase 2. Run the dependency analysis.
    if (isEnabled())
    {
        progressBar_.setMaximum(model_.getTotalStepCount());
        model_.startAnalysis();
    }
    else
    {
        finishScan();
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::scanDirectories()
//-----------------------------------------------------------------------------
void FileDependencyEditor::scanDirectories()
{
    model_.beginReset();

    // First scan the source directories.
    if (isEnabled())
    {
        for (QString const& sourcePath: component_->getSourceDirectories())
        {
            scanFiles(sourcePath);
        }
    }

    // Then add files that are part of the file sets but were not added in the file scan.
    for (QSharedPointer<FileSet> fileSet : *component_->getFileSets())
    {
        for (QSharedPointer<File> file : *fileSet->getFiles())
        {
            // For non-url files, check if the model does not contain a corresponding file item.
            if (FileHandler::isURI(FileHandler::resolveURI(file->name())) == false && 
                model_.findFileItem(file->name()) == nullptr)
            {
                QFileInfo info(FileHandler::resolvePath(file->name()));
                QString folderPath = info.path();

                // Create a folder item for the file if not already created.
                FileDependencyItem* folderItem = model_.findFolderItem(folderPath);
                if (folderItem == nullptr)
                {
                    folderItem = model_.addFolder(folderPath);
                }

                // Create a file item.
                QList<QSharedPointer<File> > fileRefs;
                for (QSharedPointer<FileSet> fileSet : *component_->getFileSets())
                {
                    for (QSharedPointer<File> filesetFile : *fileSet->getFiles())
                    {
                        if (filesetFile->name() == file->name())
                        {
                            fileRefs.append(filesetFile);
                        }
                    }
                }

                folderItem->addFile(component_, file->name(), fileRefs);
            }
        }
    }

    model_.endReset();

    timer_->stop();
    delete timer_;
    timer_ = nullptr;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::updateProgressBar()
//-----------------------------------------------------------------------------
void FileDependencyEditor::updateProgressBar(int value)
{
    progressBar_.setValue(value);

    if (value == 0)
    {
        finishScan();
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::filterToggle()
//-----------------------------------------------------------------------------
void FileDependencyEditor::filterToggle(QAction* action)
{
    // Get current filters from the graph.
    FileDependencyGraphView::DependencyFilters filters = graphWidget_.getView()->getFilters();

    // Toggle the appropriate filter based on the button that was clicked.
    filters ^= action->data().toUInt();

    // Apply the new filter setup to the view.
    graphWidget_.getView()->setFilters(filters);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::onSourceDirectoriesChanged()
//-----------------------------------------------------------------------------
void FileDependencyEditor::onSourceDirectoriesChanged()
{
    component_->setSourceDirectories(directoryEditor_.getSourceDirectories());
    scan();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::resolveExtensionFileSets()
//-----------------------------------------------------------------------------
void FileDependencyEditor::resolveExtensionFileTypes()
{
    fileTypeLookup_.clear();
    
    // Retrieve the file types information from the settings.
    QSettings settings;
    ignoreExtList_ = settings.value("FileTypes/IgnoredExtensions").toString().split(';');

    fileTypeLookup_ = FileHandler::constructFileSuffixTable();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::finishScan()
//-----------------------------------------------------------------------------
void FileDependencyEditor::finishScan()
{
    scanning_ = false;
    runAnalysisAction_->setIcon(QIcon(":/icons/common/graphics/control-play.png"));

    QApplication::restoreOverrideCursor();

    emit filesUpdated();
    emit scanCompleted();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::scanFiles()
//-----------------------------------------------------------------------------
void FileDependencyEditor::scanFiles(QString const& path)
{
    FileDependencyItem* folderItem = model_.addFolder(path);

    QFileInfoList list = QDir(General::getAbsolutePath(basePath_ + "/", path)).entryInfoList(
        QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (QFileInfo const& info : list)
    {
        // Check if the entry is a directory.
        if (info.isDir())
        {
            scanFiles(General::getRelativePath(basePath_, info.absoluteFilePath()));
        }
        // Otherwise add the file if it does not belong to ignored extensions or is not an IP-XACT file.
        else if (!ignoreExtList_.contains(info.completeSuffix()) && !isFileIPXact(info.absoluteFilePath()))
        {
            QString fileType = fileTypeLookup_.value(info.completeSuffix(), "unknown");

            // Check if the file is already packaged into the metadata.
            QString relativePath = General::getRelativePath(basePath_, info.absoluteFilePath());

            QList<QSharedPointer<File> > fileRefs;
            for (QSharedPointer<FileSet> fileSet : *component_->getFileSets())
            {
                for (QSharedPointer<File> file : *fileSet->getFiles())
                {
                    if (FileHandler::resolvePath(file->name()) == relativePath)
                    {
                        fileRefs.append(file);
                    }
                }
            }

            // Add the file to the component file sets if not already found and is not yet ignored.
            if (fileRefs.empty())
            {
                QSharedPointer<FileSet> fileSet;

                QString fileSetName = QString(path).replace(QRegularExpression("^(./)?../.*/"), "external_");
                if (fileSetName == ".")
                {
                    fileSetName = tr("base");
                }

                // Check if the file set does not exist in the component.
                if (component_->hasFileSet(fileSetName))
                {
                    fileSet = component_->getFileSet(fileSetName);
                }
                else
                {
                    fileSet.reset(new FileSet(fileSetName, ""));
                    component_->getFileSets()->append(fileSet);
                    emit fileSetAdded(fileSet.data());
                }

                QSharedPointer<File> file(new File(relativePath));
                file->addFileType(fileType);
                fileSet->addFile(file);
                emit fileAdded(file.data());

                fileRefs.append(file);
            }

            folderItem->addFile(component_, relativePath, fileRefs);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::isFileIPXact()
//-----------------------------------------------------------------------------
bool FileDependencyEditor::isFileIPXact(QString const& filename) const
{
    // Try to open the file for reading.
    QFile file(filename);
    if (!file.open(QFile::ReadOnly))
    {
        return false;
    }

    QXmlStreamReader reader(&file);

    if (!reader.readNextStartElement())
    {
        return false;
    }

    if (reader.hasError())
    {
        return false;
    }

    QString type = reader.qualifiedName().toString();
    file.close();

    // Check if the type is a valid IP-XACT/Kactus2 object type.
    return (VLNV::string2Type(type) != VLNV::INVALID);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::setupToolbar()
//-----------------------------------------------------------------------------
void FileDependencyEditor::setupToolbar()
{
    filterActions_.setExclusive(false);

    // Set up the toolbar
    toolbar_.setFloatable(false);
    toolbar_.setMovable(false);
    toolbar_.setStyleSheet(QString("QToolBar { border: none; }"));

    // Create the filter buttons in the toolbar.
    addFilterButton(QIcon(":/icons/common/graphics/dependency_twoway.png"),
        tr("Show Bidirectional Dependencies"),
        FileDependencyGraphView::FILTER_TWO_WAY);
    addFilterButton(QIcon(":/icons/common/graphics/dependency_oneway.png"), 
        tr("Show Unidirectional Dependencies"),
        FileDependencyGraphView::FILTER_ONE_WAY);
    addFilterButton(QIcon(":/icons/common/graphics/dependency_manual.png"), 
        tr("Show Manual Dependencies"),
        FileDependencyGraphView::FILTER_MANUAL);
    addFilterButton(QIcon(":/icons/common/graphics/dependency_auto.png"), 
        tr("Show Analyzed Dependencies"),
        FileDependencyGraphView::FILTER_AUTOMATIC);
    addFilterButton(QIcon(":/icons/common/graphics/external.png"), tr("Show External"),
        FileDependencyGraphView::FILTER_EXTERNAL);
    addFilterButton(QIcon(":/icons/common/graphics/diff.png"), tr("Show Differences"),
        FileDependencyGraphView::FILTER_DIFFERENCE);

    connect(&filterActions_, SIGNAL(triggered(QAction*)), this, SLOT(filterToggle(QAction*)));

    toolbar_.addSeparator();

    runAnalysisAction_ = toolbar_.addAction(QIcon(":/icons/common/graphics/control-play.png"), tr("Rescan"),
        this, SLOT(scan()));
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::addFilterButton()
//-----------------------------------------------------------------------------
void FileDependencyEditor::addFilterButton(QIcon const& icon, QString const& iconText,
    FileDependencyGraphView::DependencyFilter filter)
{
    FileDependencyGraphView::DependencyFilters filters = graphWidget_.getView()->getFilters();

    QAction* tmp = toolbar_.addAction(icon, iconText);
    tmp->setData(filter);
    tmp->setCheckable(true);
    tmp->setChecked(filters & filter);

    filterActions_.addAction(tmp);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::setupLayout()
//-----------------------------------------------------------------------------
void FileDependencyEditor::setupLayout()
{
    // Create the layout.
    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(&toolbar_, 0, 0, 1, 2);
    layout->addWidget(&progressBar_, 1, 0, 1, 2);
    layout->addWidget(&graphWidget_, 2, 0, 1, 2);    
    layout->addWidget(&directoryEditor_, 3, 0, 1, 1);
    layout->addWidget(&infoWidget_, 3, 1, 1, 1);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->setRowStretch(2, 4);
    layout->setRowStretch(3, 1);
}
