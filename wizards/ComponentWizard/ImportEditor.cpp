//-----------------------------------------------------------------------------
// File: ImportEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 10.6.2013
//
// Description:
// Used to parse source files and generating IP-XACT packages of them.
//-----------------------------------------------------------------------------

#include "ImportEditor.h"

#include <common/widgets/FileSelector/fileselector.h>

#include <KactusAPI/include/LibraryHandler.h>

#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/ports/portseditor.h>

#include <KactusAPI/include/PluginManager.h>

#include <KactusAPI/include/ImportRunner.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <IPXACTmodels/Component/validators/PortValidator.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFormLayout>

//-----------------------------------------------------------------------------
// Function: ImportEditor::ImportEditor()
//-----------------------------------------------------------------------------
ImportEditor::ImportEditor(QSharedPointer<Component> component, LibraryInterface* handler, QSharedPointer<ComponentParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter, BusInterfaceInterface* busInterface, QWidget *parent):
QWidget(parent),
splitter_(Qt::Vertical, this),
componentXmlPath_(handler->getPath(component->getVlnv())),
component_(component),
importComponent_(new Component(*component_)),
selectedSourceFile_(),
portEditor_(0),
fileSelector_(new FileSelector(component, this)),
editButton_(new QPushButton(QIcon(":/icons/common/graphics/edit.png"), tr("Edit file"), this)),
refreshButton_(new QPushButton(QIcon(":/icons/common/graphics/refresh.png"), tr("Refresh"), this)),
browseButton_(new QPushButton(this)),
componentSelector_(new QComboBox(this)),
sourceDisplayTabs_(new QTabWidget(this)),
runner_(new ImportRunner(parameterFinder, sourceDisplayTabs_, this)),
messageBox_(new QLabel(this)),
componentViews_(component->getViews())
{
    componentSelector_->setDisabled(true);

    browseButton_->setIcon(QIcon(":icons/common/graphics/folder-horizontal-open.png"));
    browseButton_->setToolTip(tr("Browse"));

    QSharedPointer<ExpressionParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));

    QSharedPointer<PortValidator> portValidator (new PortValidator(expressionParser, componentViews_));

    portEditor_ = new PortsEditor(importComponent_, handler, parameterFinder, expressionFormatter, portValidator,
        busInterface, &splitter_);

    // CSV import/export is disabled in the wizard.
	portEditor_->setAllowImportExport(false);

    runner_->setExpressionParser(expressionParser);
    runner_->loadPlugins(PluginManager::getInstance());

    connect(portEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(fileSelector_, SIGNAL(fileSelected(const QString&, QSharedPointer<FileSet>)), this,
		SLOT(onFileSelected(const QString&, QSharedPointer<FileSet>)), Qt::UniqueConnection);

    connect(refreshButton_, SIGNAL(clicked()),this, SLOT(onRefresh()), Qt::UniqueConnection);
    connect(editButton_, SIGNAL(clicked()), this, SLOT(onOpenEditor()), Qt::UniqueConnection);
    connect(browseButton_, SIGNAL(clicked()), this, SLOT(onBrowseFile()), Qt::UniqueConnection);

    connect(runner_, SIGNAL(noticeMessage(QString const&)), 
        messageBox_, SLOT(setText(QString const&)), Qt::UniqueConnection);

    connect(componentSelector_, SIGNAL(currentIndexChanged(int)),
        this, SLOT(onChangeSelectedComponent(int)), Qt::UniqueConnection);

    connect(portEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(portEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::~ImportEditor()
//-----------------------------------------------------------------------------
ImportEditor::~ImportEditor()
{

}

//-----------------------------------------------------------------------------
// Function: ImportEditor::initializeFileSelection()
//-----------------------------------------------------------------------------
void ImportEditor::initializeFileSelection()
{
    QStringList possibleFileTypes = runner_->importFileTypes();

    fileSelector_->clearFilters();

    QStringList possibleSuffixes = fileExtensionsForTypes(possibleFileTypes);
    foreach(QString possibleFileSuffix, possibleSuffixes)
    {
        fileSelector_->addFilter(possibleFileSuffix);
    }

    onRefresh();
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::checkEditorValidity()
//-----------------------------------------------------------------------------
bool ImportEditor::checkEditorValidity() const
{
    return portEditor_->isValid();
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::onFileSelected()
//-----------------------------------------------------------------------------
void ImportEditor::onFileSelected(QString const& filePath, QSharedPointer<FileSet> fileSet)
{
    selectedSourceFile_ = filePath;
	selectedFileSet_ = fileSet;

    onRefresh();
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::onOpenEditor()
//-----------------------------------------------------------------------------
void ImportEditor::onOpenEditor() 
{
    if (!selectedSourceFile_.isEmpty())
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(selectedFileAbsolutePath()));
    }
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::onBrowseFile()
//-----------------------------------------------------------------------------
void ImportEditor::onBrowseFile()
{
    QString path = selectedSourceFile_;
    if (path.isEmpty())
    {
        path = QFileInfo(componentXmlPath_).absolutePath();
    }

    QFileDialog browseDialog(this, tr("Select file to import"), path, getFileBrowsingFilters());
    if (browseDialog.exec() == QDialog::Accepted)
    {
        QStringList selectedFiles = browseDialog.selectedFiles();
        if (!selectedFiles.isEmpty())
        {
            QFileInfo componentFileInfo(componentXmlPath_);
            QString componentFolderPath = componentFileInfo.absolutePath();

            QDir componentDirectory(componentFolderPath);

            QString relativePath = componentDirectory.relativeFilePath(selectedFiles.first());

            if (!fileExistsInFileSelector(relativePath))
            {
                fileSelector_->addItem(relativePath);
            }

            fileSelector_->selectFile(relativePath);
            onFileSelected(relativePath, QSharedPointer<FileSet>());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::getFileBrowsingFilters()
//-----------------------------------------------------------------------------
QString ImportEditor::getFileBrowsingFilters() const
{
    QStringList possibleFileTypes = runner_->importFileTypes();
    QStringList possibleSuffixes = fileExtensionsForTypes(possibleFileTypes);

    QStringList filters;
    for (auto suffix : possibleSuffixes)
    {
        if (!suffix.isEmpty())
        {
            filters.append(suffix.prepend(QLatin1String("*.")));
        }
    }

    return filters.join(' ');
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::fileExistsInFileSelector()
//-----------------------------------------------------------------------------
bool ImportEditor::fileExistsInFileSelector(QString const& filePath) const
{
    for (int i = 0; i < fileSelector_->count(); ++i)
    {
        if (fileSelector_->itemText(i).compare(filePath) == 0)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::onRefresh()
//-----------------------------------------------------------------------------
void ImportEditor::onRefresh() 
{
    changeAvailableComponents();
}


//-----------------------------------------------------------------------------
// Function: ImportEditor::changeAvailableComponents()
//-----------------------------------------------------------------------------
void ImportEditor::changeAvailableComponents()
{
    QString currentComponent = componentSelector_->currentText();
    componentSelector_->clear();

    QStringList componentNames =
        runner_->constructComponentDataFromFile(selectedSourceFile_, componentXmlPath_, component_);
    if (!componentNames.isEmpty())
    {
        componentSelector_->setDisabled(false);

        componentSelector_->addItems(componentNames);

        componentSelector_->setCurrentIndex(0);
    }
    else
    {
        componentSelector_->setDisabled(true);
        onChangeSelectedComponent(0);
    }
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::onChangeSelectedComponent()
//-----------------------------------------------------------------------------
void ImportEditor::onChangeSelectedComponent(int index)
{
    QString componentName = componentSelector_->itemText(index);

    importComponent_ = runner_->run(componentName, selectedSourceFile_, componentXmlPath_, component_);

    portEditor_->setComponent(importComponent_);

    componentViews_ = importComponent_->getViews();

    if (selectedFileSet_)
    {
        foreach(QSharedPointer<ComponentInstantiation> componentInstantiation,
            *importComponent_->getComponentInstantiations())
        {
            if (!componentInstantiation->getFileSetReferences()->contains(selectedFileSet_->name()))
            {
                componentInstantiation->getFileSetReferences()->append(selectedFileSet_->name());
            }
        }
    }

    emit componentChanged(importComponent_);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::fileSuffixesForTypes()
//-----------------------------------------------------------------------------
QStringList ImportEditor::fileExtensionsForTypes(QStringList possibleFileTypes) const
{
    QStringList fileSuffixes;

    QSettings settings;
    settings.beginGroup("FileTypes");

    foreach (QString const& fileType, possibleFileTypes)
    {
        settings.beginGroup(fileType);

        QStringList fileTypeExtensions = settings.value("Extensions").toString().split(';');
        foreach (QString const& extension, fileTypeExtensions)
        {
            if (!fileSuffixes.contains(extension))
            {
                fileSuffixes.append(extension);
            }
        }
        settings.endGroup();
    }

    settings.endGroup();

    return fileSuffixes;
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::selectedFileAbsolutePath()
//-----------------------------------------------------------------------------
QString ImportEditor::selectedFileAbsolutePath() const
{
    return General::getAbsolutePath(componentXmlPath_, selectedSourceFile_);
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::setupLayout()
//-----------------------------------------------------------------------------
void ImportEditor::setupLayout()
{
    sourceDisplayTabs_->setMinimumHeight(300);

	QVBoxLayout* topLayout = new QVBoxLayout(this);    
    topLayout->addWidget(&splitter_);
    topLayout->addWidget(messageBox_);
    topLayout->setContentsMargins(0, 0, 0, 0);
    
    QWidget* sourceWidget = new QWidget(this);
    QVBoxLayout* sourceLayout = new QVBoxLayout(sourceWidget);    

    sourceLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* selectorLabel = new QLabel(tr("Select top-level file to import:"), this);
    QLabel* entityLabel = new QLabel(tr("Select component to import:"), this);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(browseButton_);
    buttonLayout->addWidget(editButton_);
    buttonLayout->addWidget(refreshButton_);

    QGridLayout* upperLayout = new QGridLayout();
    upperLayout->addWidget(selectorLabel, 0, 0);
    upperLayout->addWidget(fileSelector_, 0, 1, 1, 3);
    upperLayout->addWidget(entityLabel, 1, 0);
    upperLayout->addWidget(componentSelector_, 1, 1, 1, 3);
    upperLayout->addLayout(buttonLayout, 0, 4, Qt::AlignRight);

    sourceLayout->addLayout(upperLayout);
    sourceLayout->addWidget(sourceDisplayTabs_);

    splitter_.addWidget(sourceWidget);
    splitter_.addWidget(portEditor_);
    splitter_.setStretchFactor(0, 60);
    splitter_.setStretchFactor(1, 40);
}
