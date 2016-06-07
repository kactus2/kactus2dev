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

#include <library/LibraryManager/libraryhandler.h>

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/ports/portseditor.h>

#include <wizards/ComponentWizard/ImportRunner.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <IPXACTmodels/Component/validators/PortValidator.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDesktopServices>

//-----------------------------------------------------------------------------
// Function: ImportEditor::ImportEditor()
//-----------------------------------------------------------------------------
ImportEditor::ImportEditor(QSharedPointer<Component> component, LibraryInterface* handler,
    PluginManager const& pluginMgr, QSharedPointer <ComponentParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget *parent):
QWidget(parent),
splitter_(Qt::Vertical, this),
componentXmlPath_(handler->getPath(component->getVlnv())),
component_(component),
importComponent_(new Component(*component_)),
selectedSourceFile_(),
portEditor_(0),
fileSelector_(new FileSelector(component, this)),
editButton_(new QPushButton(tr("Open editor"), this)),
refreshButton_(new QPushButton(QIcon(":/icons/common/graphics/refresh.png"), "", this)),
sourceDisplayTabs_(new QTabWidget(this)),
runner_(new ImportRunner(parameterFinder, sourceDisplayTabs_, this)),
messageBox_(new QLabel(this)),
componentViews_(component->getViews())
{
    QSharedPointer<ExpressionParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));

    QSharedPointer<PortValidator> portValidator (new PortValidator(expressionParser, componentViews_));

    portEditor_ = new PortsEditor(
        importComponent_, handler, parameterFinder, expressionFormatter, portValidator, &splitter_);

    // CSV import/export is disabled in the wizard.
	portEditor_->setAllowImportExport(false);

    runner_->setVerilogExpressionParser(expressionParser);
    runner_->loadPlugins(pluginMgr);

    connect(portEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(fileSelector_, SIGNAL(fileSelected(const QString&, QSharedPointer<FileSet>)), this,
		SLOT(onFileSelected(const QString&, QSharedPointer<FileSet>)), Qt::UniqueConnection);

    connect(refreshButton_, SIGNAL(clicked()),this, SLOT(onRefresh()), Qt::UniqueConnection);
    connect(editButton_, SIGNAL(clicked()), this, SLOT(onOpenEditor()), Qt::UniqueConnection);

    connect(runner_, SIGNAL(noticeMessage(QString const&)), 
        messageBox_, SLOT(setText(QString const&)), Qt::UniqueConnection);

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
// Function: ImportEditor::onRefresh()
//-----------------------------------------------------------------------------
void ImportEditor::onRefresh() 
{
    importComponent_ = runner_->run(selectedSourceFile_, componentXmlPath_, component_);

    portEditor_->setComponent(importComponent_);

    componentViews_ = importComponent_->getViews();

	if ( selectedFileSet_ )
	{
		foreach( QSharedPointer<ComponentInstantiation> componentInstantiation, 
			*importComponent_->getComponentInstantiations() )
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

    QHBoxLayout* selectorLayout = new QHBoxLayout();
    QLabel* selectorLabel = new QLabel(tr("Top-level file to import:"), this);
    selectorLayout->addWidget(selectorLabel);
    selectorLayout->addWidget(fileSelector_, 1);
    selectorLayout->addWidget(editButton_);    
    selectorLayout->addWidget(refreshButton_);    

    sourceLayout->addLayout(selectorLayout);
    sourceLayout->addWidget(sourceDisplayTabs_);

    splitter_.addWidget(sourceWidget);
    splitter_.addWidget(portEditor_);
    splitter_.setStretchFactor(0, 40);
    splitter_.setStretchFactor(1, 10);
    splitter_.setStretchFactor(2, 20);
    splitter_.setStretchFactor(3, 30);
}
