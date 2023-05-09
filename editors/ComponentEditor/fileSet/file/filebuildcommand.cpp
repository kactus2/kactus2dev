//-----------------------------------------------------------------------------
// File: filebuildcommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.2.2011
//
// Description:
// FileBuildCommand is a group box for editing buildCommand of a file.
//-----------------------------------------------------------------------------

#include "filebuildcommand.h"

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <KactusAPI/include/ParameterFinder.h>
#include <KactusAPI/include/ExpressionParser.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/FileInterface.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/BuildCommand.h>

#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::FileBuildCommand()
//-----------------------------------------------------------------------------
FileBuildCommand::FileBuildCommand(std::string fileName, FileInterface* fileInterface,
    QString const& componentPath, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionParser> expressionParser, QWidget *parent):
QGroupBox(tr("Build command"), parent),
fileName_(fileName),
fileInterface_(fileInterface),
componentPath_(componentPath),
targetEditor_(this),
browseTargetButton_(QIcon(":icons/common/graphics/opened-folder.png"), QString(), this),
commandEditor_(this),
flagsEditor_(this),
replaceDefaultEditor_(new ExpressionEditor(parameterFinder, this)),
expressionParser_(expressionParser)
{
    Q_ASSERT_X(
        fileInterface_, "FileBuildCommand constructor", "Null File interface-pointer given to the constructor");

    replaceDefaultEditor_->setFixedHeight(20);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser);

    ParameterCompleter* replaceCompleter = new ParameterCompleter(this);
    replaceCompleter->setModel(componentParametersModel);

    replaceDefaultEditor_->setAppendingCompleter(replaceCompleter);

    targetEditor_.setAcceptDrops(true);
    
    browseTargetButton_.setToolTip(tr("Browse..."));

    setupLayout();

	refresh();

    connect(&targetEditor_, SIGNAL(editingFinished()), this, SLOT(onTargetChanged()), Qt::UniqueConnection);
    connect(&browseTargetButton_, SIGNAL(clicked()), this, SLOT(onBrowseTarget()), Qt::UniqueConnection);
    connect(&commandEditor_, SIGNAL(textEdited(QString const& )), 
        this, SLOT(onCommandChanged()), Qt::UniqueConnection);
    connect(&flagsEditor_, SIGNAL(textEdited(QString const& )), this, SLOT(onFlagsChanged()), Qt::UniqueConnection);

    connect(replaceDefaultEditor_, SIGNAL(editingFinished()),
        this, SLOT(onReplaceDefaultChanged()), Qt::UniqueConnection);
    connect(replaceDefaultEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(replaceDefaultEditor_, SIGNAL(increaseReference(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(replaceDefaultEditor_, SIGNAL(decreaseReference(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::refresh()
//-----------------------------------------------------------------------------
void FileBuildCommand::refresh()
{   
    commandEditor_.setText(QString::fromStdString(fileInterface_->getBuildCommandText(fileName_)));
    flagsEditor_.setText(QString::fromStdString(fileInterface_->getBuildCommandFlags(fileName_)));

    replaceDefaultEditor_->blockSignals(true);

    replaceDefaultEditor_->setExpression(
        QString::fromStdString(fileInterface_->getBuildCommandReplaceDefaultFlagsExpression(fileName_)));
    replaceDefaultEditor_->setToolTip(
        QString::fromStdString(fileInterface_->getBuildCommandReplaceDefaultFlagsValue(fileName_)));

    replaceDefaultEditor_->blockSignals(false);

    targetEditor_.setText(QString::fromStdString(fileInterface_->getBuildCommandTarget(fileName_)));
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::onCommandChanged()
//-----------------------------------------------------------------------------
void FileBuildCommand::onCommandChanged()
{
    fileInterface_->setBuildCommand(fileName_, commandEditor_.text().toStdString());

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::onFlagsChanged()
//-----------------------------------------------------------------------------
void FileBuildCommand::onFlagsChanged()
{
    fileInterface_->setBuildCommandFlags(fileName_, flagsEditor_.text().toStdString());

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::onTargetChanged()
//-----------------------------------------------------------------------------
void FileBuildCommand::onTargetChanged()
{
    fileInterface_->setBuildCommandTarget(fileName_, targetEditor_.text().toStdString());

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::onReplaceDefaultChanged()
//-----------------------------------------------------------------------------
void FileBuildCommand::onReplaceDefaultChanged()
{
    replaceDefaultEditor_->finishEditingCurrentWord();

    QString newReplace = replaceDefaultEditor_->getExpression();
    fileInterface_->setbuildCommandReplaceDefaultFlags(fileName_, newReplace.toStdString());

    replaceDefaultEditor_->setToolTip(
        QString::fromStdString(fileInterface_->getBuildCommandReplaceDefaultFlagsValue(fileName_)));
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::onBrowseTarget()
//-----------------------------------------------------------------------------
void FileBuildCommand::onBrowseTarget()
{
    QFileDialog browseDialog(this, tr("Set a target file"), componentPath_);
    if (browseDialog.exec() == QDialog::Accepted)
    {
        QStringList selectedFiles = browseDialog.selectedFiles();

        if (!selectedFiles.isEmpty())
        {
            QString relativePath = General::getRelativeSavePath(componentPath_, selectedFiles.first());

            targetEditor_.setText(relativePath);
            onTargetChanged();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::setupLayout()
//-----------------------------------------------------------------------------
void FileBuildCommand::setupLayout()
{
    QGridLayout* topLayout = new QGridLayout(this);

    QString targetToolTip(tr("Target defines the path to the file derived from the source file."));
    QLabel* targetLabel = new QLabel(tr("Target file:"), this);
    targetLabel->setToolTip(targetToolTip);
    targetEditor_.setToolTip(targetToolTip);

    topLayout->addWidget(targetLabel, 0, 0, 1, 1);
    topLayout->addWidget(&targetEditor_, 0, 1, 1, 1);
    topLayout->addWidget(&browseTargetButton_, 0, 2, 1, 1);

    QString commandToolTip(tr("Command defines a compiler or assembler tool that processes files of this type."));
    QLabel* commandLabel = new QLabel(tr("Command:"), this);
    commandLabel->setToolTip(commandToolTip);
    commandEditor_.setToolTip(commandToolTip);

    topLayout->addWidget(commandLabel, 1, 0, 1, 1);
    topLayout->addWidget(&commandEditor_, 1, 1, 1, 2);

    QString flagToolTip(tr("Documents any flags to be passed along with the software tool command."));
    QLabel* flagLabel = new QLabel(tr("Flags:"), this);
    flagLabel->setToolTip(flagToolTip);
    flagsEditor_.setToolTip(flagToolTip);

    topLayout->addWidget(flagLabel, 2, 0, 1, 1);
    topLayout->addWidget(&flagsEditor_, 2, 1, 1, 2);

    QString replaceFlagsToolTip(tr("Expression that must evaluate to true or false. When true, the flags replace "
        "any default flags from the build script. When false, the given flags are appended."));
    QLabel* replaceLabel = new QLabel(tr("Replace default flags, f(x):"), this);
    replaceLabel->setToolTip(replaceFlagsToolTip);

    topLayout->addWidget(replaceLabel, 3, 0, 1, 1);
    topLayout->addWidget(replaceDefaultEditor_, 3, 1, 1, 2);
    topLayout->setColumnStretch(1, 1);

    setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::fileRenamed()
//-----------------------------------------------------------------------------
void FileBuildCommand::fileRenamed(std::string const& newName)
{
    fileName_ = newName;
}
