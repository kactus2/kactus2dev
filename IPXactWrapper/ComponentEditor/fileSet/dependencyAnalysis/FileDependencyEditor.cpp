//-----------------------------------------------------------------------------
// File: FileDependencyEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.01.2013
//
// Description:
// File dependency editor which encapsulates the whole dependency UI.
//-----------------------------------------------------------------------------

#include "FileDependencyEditor.h"

#include "FileDependencySourceDialog.h"
#include "FileDependencyItem.h"
#include "FileDependencyDelegate.h"

#include <common/widgets/ScanProgressWidget/scanprogresswidget.h>

#include <models/component.h>
#include <models/fileset.h>
#include <models/file.h>

#include <PluginSystem/PluginManager.h>
#include <PluginSystem/ISourceAnalyzerPlugin.h>

#include <QVBoxLayout>
#include <QIcon>
#include <QFileInfoList>
#include <QSettings>
#include <QHeaderView>
#include <QDebug>
#include <QAction>
#include <QVariant>

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::FileDependencyEditor()
//-----------------------------------------------------------------------------
FileDependencyEditor::FileDependencyEditor(QSharedPointer<Component> component,
                                           QString const& basePath,
                                           PluginManager& pluginMgr, QWidget* parent)
    : QWidget(parent),
      toolbar_(this),
      filterActions_(this),
      progressBar_(this),
      graphWidget_(this),
      infoWidget_(this),
      component_(component),
      fileTypeLookup_(),
      model_(pluginMgr, component, basePath + "/"),
      basePath_(basePath),
      scanning_(false),
      timer_(0),
      progWidget_(0)
{
    // Initialize the widgets.
    progressBar_.setStyleSheet("QProgressBar:horizontal { margin: 0px; border: none; background: #cccccc; } "
                               "QProgressBar::chunk:horizontal { background: #009eff;}");
    progressBar_.setFixedHeight(2);
    progressBar_.setTextVisible(false);
    progressBar_.setContentsMargins(0, 0, 0, 0);
    progressBar_.setValue(0);

    graphWidget_.setContentsMargins(0, 0, 0, 0);
    graphWidget_.getView().setModel(&model_);
    graphWidget_.getView().setItemDelegate(new FileDependencyDelegate(component, this));

    graphWidget_.getView().resizeColumnToContents(FILE_DEPENDENCY_COLUMN_TREE);
    graphWidget_.getView().resizeColumnToContents(FILE_DEPENDENCY_COLUMN_STATUS);
    graphWidget_.getView().resizeColumnToContents(FILE_DEPENDENCY_COLUMN_CREATE);
    graphWidget_.getView().setColumnWidth(FILE_DEPENDENCY_COLUMN_PATH, 250);
    graphWidget_.getView().header()->setSectionResizeMode(FILE_DEPENDENCY_COLUMN_TREE, QHeaderView::Fixed);
    graphWidget_.getView().header()->setSectionResizeMode(FILE_DEPENDENCY_COLUMN_STATUS, QHeaderView::Fixed);
    graphWidget_.getView().header()->setSectionResizeMode(FILE_DEPENDENCY_COLUMN_CREATE, QHeaderView::Fixed);

    filterActions_.setExclusive(false);

    // Set up the toolbar
    toolbar_.setFloatable(false);
    toolbar_.setMovable(false);
    toolbar_.setStyleSheet(QString("QToolBar { border: none; }"));

    // Create the filter buttons in the toolbar. The button actions are also added to the separate actiongroup
    addFilterButton(QIcon(":/icons/graphics/traffic-light_green.png"), "Show Unchanged Files",
                    FileDependencyGraphView::FILTER_GREEN);
    addFilterButton(QIcon(":/icons/graphics/traffic-light_yellow.png"), "Show Changed Files (Only Contents Changed)",
                    FileDependencyGraphView::FILTER_YELLOW);
    addFilterButton(QIcon(":/icons/graphics/traffic-light_red.png"), "Show Changed Files (Dependencies Changed)",
                    FileDependencyGraphView::FILTER_RED);
    addFilterButton(QIcon(":/icons/graphics/dependency_twoway.png"), "Show Bidirectional Dependencies",
                    FileDependencyGraphView::FILTER_TWO_WAY);
    addFilterButton(QIcon(":/icons/graphics/dependency_oneway.png"), "Show Unidirectional Dependencies",
                    FileDependencyGraphView::FILTER_ONE_WAY);
    addFilterButton(QIcon(":/icons/graphics/dependency_manual.png"), "Show Manual Dependencies",
                    FileDependencyGraphView::FILTER_MANUAL);
    addFilterButton(QIcon(":/icons/graphics/dependency_auto.png"), "Show Analyzed Dependencies",
                    FileDependencyGraphView::FILTER_AUTOMATIC);
    addFilterButton(QIcon(":/icons/graphics/external.png"), "Show External",
                    FileDependencyGraphView::FILTER_EXTERNAL);
    addFilterButton(QIcon(":/icons/graphics/diff.png"), "Show Differences",
                    FileDependencyGraphView::FILTER_DIFFERENCE);
    connect(&filterActions_, SIGNAL(triggered(QAction*)), this, SLOT(filterToggle(QAction*)));


    toolbar_.addSeparator();
    toolbar_.addAction(QIcon(":/icons/graphics/import_folders.png"), "Import Source Directories",
                       this, SLOT(openSourceDialog()));
    runAnalysisAction_ = toolbar_.addAction(QIcon(":/icons/graphics/control-play.png"), "Rescan",
                                            this, SLOT(scan()));

    // Create the layout.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&toolbar_);
    layout->addWidget(&progressBar_);
    layout->addWidget(&graphWidget_, 1);
    layout->addWidget(&infoWidget_);
    layout->setContentsMargins(0, 0, 0, 0);

    connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(dependenciesChanged()), this, SIGNAL(dependenciesChanged()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(analysisProgressChanged(int)),
            this, SLOT(updateProgressBar(int)), Qt::UniqueConnection);
    connect(&graphWidget_.getView(), SIGNAL(selectionChanged(FileDependency*)),
            &infoWidget_, SLOT(setEditedDependency(FileDependency*)), Qt::UniqueConnection);

    connect(&infoWidget_, SIGNAL(dependencyChanged(FileDependency*)),
            &model_, SIGNAL(dependencyChanged(FileDependency*)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::~FileDependencyEditor()
//-----------------------------------------------------------------------------
FileDependencyEditor::~FileDependencyEditor()
{

}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::openSourceDialog()
//-----------------------------------------------------------------------------
void FileDependencyEditor::openSourceDialog()
{
    // Show the source directories dialog.
    FileDependencySourceDialog dialog(basePath_, component_->getSourceDirectories(), this);

    if (dialog.exec() == QDialog::Accepted)
    {
        component_->setSourceDirectories(dialog.getSourceDirectories());
        scan();
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::scan()
//-----------------------------------------------------------------------------
void FileDependencyEditor::scan()
{
    if (scanning_)
    {
        model_.stopAnalysis();
        return;
    }

    runAnalysisAction_->setIcon(QIcon(":/icons/graphics/control-stop.png"));
    scanning_ = true;

    // Preparations. Resolve file types for each extension.
    resolveExtensionFileTypes();

    // Phase 1. Scan all files and folders in the source paths recursively.
    
    progWidget_ = new ScanProgressWidget(this);
    progWidget_->setRange(0, 1);
    progWidget_->setValue(1);
    progWidget_->setMessage(tr("Scanning source directories..."));
    
    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(scanDirectories()));
    timer_->start();

    progWidget_->exec();

    // Phase 2. Run the dependency analysis.
    progressBar_.setMaximum(model_.getTotalStepCount());
    model_.startAnalysis();

    emit fileSetsUpdated();
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

    settings.beginGroup("FileTypes");

    QStringList fileTypes = settings.childKeys();

    foreach (QString const& fileType, fileTypes)
    {
        if (fileType != "IgnoredExtensions")
        {
            // Enumerate all extensions for the currently investigated file type.
            QStringList extensions = settings.value(fileType).toString().split(';');

            foreach (QString const& ext, extensions)
            {
                // Add to the lookup map only if the extension is not already in use.
                if (!fileTypeLookup_.contains(ext))
                {
                    fileTypeLookup_.insert(ext, fileType);
                }
            }
        }
    }

    settings.endGroup();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::scanFiles()
//-----------------------------------------------------------------------------
void FileDependencyEditor::scanFiles(QString const& path)
{
    FileDependencyItem* folderItem = model_.addFolder(path);

    QFileInfoList list =
        QDir(General::getAbsolutePath(basePath_ + "/", path)).entryInfoList(QDir::Files | QDir::Dirs |
                                                                           QDir::NoDotAndDotDot);

    foreach (QFileInfo const& info, list)
    {
        // Check if the entry is a directory.
        if (info.isDir())
        {
            scanFiles(General::getRelativePath(basePath_, info.absoluteFilePath()));
        }
        // Otherwise add the file if it does not belong to ignored extensions.
        else if (!ignoreExtList_.contains(info.completeSuffix()))
        {
            // Otherwise the entry is a file.
            // Check which file type corresponds to the extension.
            QString fileType = fileTypeLookup_.value(info.completeSuffix(), "unknown");

            // Check if the file is already packaged into the metadata.
            QString relativePath = General::getRelativePath(basePath_, info.absoluteFilePath());

            QList<File*> fileRefs;
            component_->getFiles(relativePath, fileRefs);

            if (fileRefs.empty())
            {
                QSharedPointer<FileSet> fileSet;

                // Check if the file set does not exist in the component.
                if (component_->hasFileSet(fileType + "s"))
                {
                    fileSet = component_->getFileSet(fileType + "s");
                }
                else
                {
                    fileSet.reset(new FileSet(fileType + "s", ""));
                    component_->addFileSet(fileSet);
                    emit fileSetAdded(fileSet.data());
                }

                QSharedPointer<File> file(new File(relativePath, fileSet.data()));
                file->addFileType(fileType);
                fileSet->addFile(file);
                emit fileAdded(file.data());

                fileRefs.append(file.data());
            }

            folderItem->addFile(component_.data(), relativePath, fileRefs);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::updateProgressBar()
//-----------------------------------------------------------------------------
void FileDependencyEditor::updateProgressBar(int value)
{
    progressBar_.setValue(value);

    if (value == 0)
    {
        runAnalysisAction_->setIcon(QIcon(":/icons/graphics/control-play.png"));
        scanning_ = false;
        emit filesUpdated();
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::filterToggle()
//-----------------------------------------------------------------------------
void FileDependencyEditor::filterToggle(QAction* action)
{
    // Get current filters from the graph.
    FileDependencyGraphView::DependencyFilters filters = graphWidget_.getView().getFilters();

    // Toggle the appropriate filter based on the button that was clicked.
    if (filters & action->data().toUInt())
    {
        // Filter was on, disable it.
        action->setChecked(false);
        filters ^= action->data().toUInt();
    }
    else
    {
        // Filter was off, enable it.
        action->setChecked(true);
        filters ^= action->data().toUInt();
    }

    // Apply the new filter setup to the view.
    graphWidget_.getView().setFilters(filters);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::addFilterButton()
//-----------------------------------------------------------------------------
void FileDependencyEditor::addFilterButton(QIcon icon, QString iconText,
                                           FileDependencyGraphView::DependencyFilter filter)
{
    QAction* tmp = 0;
    FileDependencyGraphView::DependencyFilters filters = graphWidget_.getView().getFilters();
    tmp = toolbar_.addAction(icon, iconText);
    tmp->setData(filter);
    tmp->setCheckable(true);
    tmp->setChecked(filters & filter);
    filterActions_.addAction(tmp);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::scanDirectories()
//-----------------------------------------------------------------------------
void FileDependencyEditor::scanDirectories()
{
    model_.beginReset();

    foreach (QString const& sourcePath, component_->getSourceDirectories())
    {
        scanFiles(sourcePath);
    }

    model_.endReset();

    timer_->stop();
    delete timer_;
    delete progWidget_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::setCompact()
//-----------------------------------------------------------------------------
void FileDependencyEditor::setCompact(bool compact)
{
    toolbar_.setVisible(!compact);
    infoWidget_.setVisible(!compact);
}

