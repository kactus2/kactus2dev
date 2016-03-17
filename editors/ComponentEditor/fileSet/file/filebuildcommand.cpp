//-----------------------------------------------------------------------------
// File: filebuildcommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 7.2.2011
//
// Description:
// FileBuildCommand is a group box for editing buildCommand of a file.
//-----------------------------------------------------------------------------

#include "filebuildcommand.h"

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <IPXACTmodels/common/validators/ValueFormatter.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/BuildCommand.h>

#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::FileBuildCommand()
//-----------------------------------------------------------------------------
FileBuildCommand::FileBuildCommand(QWidget *parent, LibraryInterface* handler, QSharedPointer<Component> component,
                                   QSharedPointer<File> file, QSharedPointer<ParameterFinder> parameterFinder,
                                   QSharedPointer<ExpressionParser> expressionParser):
QGroupBox(tr("Build command"), parent),
file_(file),
buildCommand_(),
commandEditor_(this),
flagsEditor_(this),
replaceDefaultEditor_(new ExpressionEditor(parameterFinder, this)),
targetEditor_(this, handler, component),
expressionParser_(expressionParser)
{
    Q_ASSERT_X(file, "FileBuildCommand constructor", "Null File-pointer given to the constructor");

    replaceDefaultEditor_->setFixedHeight(20);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser);

    ParameterCompleter* replaceCompleter = new ParameterCompleter(this);
    replaceCompleter->setModel(componentParametersModel);

    replaceDefaultEditor_->setAppendingCompleter(replaceCompleter);

    setupLayout();

	refresh();

    connect(&targetEditor_, SIGNAL(contentChanged()), this, SLOT(onTargetChanged()), Qt::UniqueConnection);
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
// Function: FileBuildCommand::~FileBuildCommand()
//-----------------------------------------------------------------------------
FileBuildCommand::~FileBuildCommand()
{

}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::refresh()
//-----------------------------------------------------------------------------
void FileBuildCommand::refresh()
{   
    if (file_->getBuildCommand())
    {
        buildCommand_ = file_->getBuildCommand();
    }
    else
    {
        buildCommand_ = QSharedPointer<BuildCommand>(new BuildCommand());    
    }

    commandEditor_.setText(buildCommand_->getCommand());
    flagsEditor_.setText(buildCommand_->getFlags());

    replaceDefaultEditor_->blockSignals(true);

    replaceDefaultEditor_->setExpression(buildCommand_->getReplaceDefaultFlags());
    replaceDefaultEditor_->setToolTip(formattedValueFor(buildCommand_->getReplaceDefaultFlags()));

    replaceDefaultEditor_->blockSignals(false);

    targetEditor_.setText(buildCommand_->getTargetName());
}

//-----------------------------------------------------------------------------
// Function: filebuildcommand::formattedValueFor()
//-----------------------------------------------------------------------------
QString FileBuildCommand::formattedValueFor(QString const& expression) const
{
    if (expressionParser_->isValidExpression(expression))
    {
        ValueFormatter formatter;
        return formatter.format(expressionParser_->parseExpression(expression),
            expressionParser_->baseForExpression(expression));
    }
    else if (expressionParser_->isPlainValue(expression))
    {
        return expression;
    }
    else
    {
        return QLatin1String("n/a");
    }
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::onCommandChanged()
//-----------------------------------------------------------------------------
void FileBuildCommand::onCommandChanged()
{
	buildCommand_->setCommand(commandEditor_.text());
    updateFileBuildCommand();
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::onFlagsChanged()
//-----------------------------------------------------------------------------
void FileBuildCommand::onFlagsChanged()
{
	buildCommand_->setFlags(flagsEditor_.text());
    updateFileBuildCommand();
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::onTargetChanged()
//-----------------------------------------------------------------------------
void FileBuildCommand::onTargetChanged()
{
	buildCommand_->setTargetName(targetEditor_.text());
    updateFileBuildCommand();
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::onReplaceDefaultChanged()
//-----------------------------------------------------------------------------
void FileBuildCommand::onReplaceDefaultChanged()
{
    replaceDefaultEditor_->finishEditingCurrentWord();

    QString newReplace = replaceDefaultEditor_->getExpression();
    buildCommand_->setReplaceDefaultFlags(newReplace);

    replaceDefaultEditor_->setToolTip(formattedValueFor(newReplace));

    updateFileBuildCommand();
}

//-----------------------------------------------------------------------------
// Function: FileBuildCommand::updateFileBuildCommand()
//-----------------------------------------------------------------------------
void FileBuildCommand::updateFileBuildCommand()
{
    bool emptyBuildCommand = commandEditor_.text().isEmpty() && flagsEditor_.text().isEmpty() &&
        targetEditor_.text().isEmpty() && replaceDefaultEditor_->getExpression().isEmpty();

    if (!file_->getBuildCommand() && !emptyBuildCommand)
    {
        file_->setBuildcommand(buildCommand_);
    }
    else if (emptyBuildCommand)
    {
        file_->setBuildcommand(QSharedPointer<BuildCommand>(0));
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
    topLayout->addWidget(&targetEditor_, 0, 1, 1, 2);

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
    flagsEditor_.setToolTip(replaceFlagsToolTip);

    replaceDefaultEditor_->setFrameShadow(QFrame::Sunken);

    topLayout->addWidget(replaceLabel, 3, 0, 1, 1);
    topLayout->addWidget(replaceDefaultEditor_, 3, 1, 1, 2);

    setContentsMargins(0, 0, 0, 0);
}
