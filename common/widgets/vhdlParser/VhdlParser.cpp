//-----------------------------------------------------------------------------
// File: VhdlParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.06.2013
//
// Description:
// VhdlParser reads a given vhdl file, creates ports and generics accordingly
// and displays the file highlighting the created ports and generics. 
//-----------------------------------------------------------------------------
#include "VhdlParser.h"

#include <QApplication>
#include <QFile>
#include <QFont>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextStream>
#include <QMessageBox>
#include <QRegExp>
#include <QList>

#include <common/KactusColors.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/port.h>
#include <common/widgets/vhdlParser/VhdlSyntax.h>

#include <wizards/ComponentWizard/VhdlImportEditor/VHDLHighlighter.h>

#include <Plugins/VHDLParser/VHDLPortParser.h>
#include <Plugins/VHDLParser/VHDLGenericParser.h>

namespace
{
    const QRegExp ENTITY_EXP = QRegExp("ENTITY\\s+(\\w+)\\s+IS\\s+.*END\\s+(?:ENTITY\\s+)?(\\1)?\\s*[;]", 
        Qt::CaseInsensitive);
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::VhdlParser()
//-----------------------------------------------------------------------------
VhdlParser::VhdlParser(QPlainTextEdit* display, QObject* parent) : 
QObject(parent),
    parsedGenerics_(),
    dependedGenerics_(),
    display_(display),
    highlighter_(new VHDLHighlighter(display, this)),
    portParser_(this),
    genericParser_(this),
    targetComponent_(0),
    parsedPortDeclarations_()
{
    highlighter_->registerHighlightSource(&portParser_);
    highlighter_->registerHighlightSource(&genericParser_);

    connect(&portParser_, SIGNAL(add(QSharedPointer<Port>, QString const&)), 
        this, SLOT(onPortParsed(QSharedPointer<Port>, QString const&)), Qt::UniqueConnection);
    connect(&portParser_, SIGNAL(add(QSharedPointer<Port>, QString const&)), 
        this, SIGNAL(add(QSharedPointer<Port>)), Qt::UniqueConnection);

    connect(&genericParser_, SIGNAL(add(QSharedPointer<ModelParameter>, QString const&)), 
        this, SLOT(onGenericParsed(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);
    connect(&genericParser_, SIGNAL(add(QSharedPointer<ModelParameter>, QString const&)), 
        this, SIGNAL(add(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::~VhdlParser()
//-----------------------------------------------------------------------------
VhdlParser::~VhdlParser()
{

}

//-----------------------------------------------------------------------------
// Function: VhdlParser::scrollToEntityBegin()
//-----------------------------------------------------------------------------
void VhdlParser::scrollToEntityBegin()
{  
    int entityBegin = VhdlSyntax::ENTITY_BEGIN_EXP.indexIn(display_->toPlainText());
    if (entityBegin != -1)
    {
        QTextCursor cursor = display_->textCursor();
        cursor.setPosition(entityBegin);

        display_->verticalScrollBar()->setValue(cursor.block().firstLineNumber());
    }
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::parseFile()
//-----------------------------------------------------------------------------
void VhdlParser::runParser(QString const& absolutePath, QSharedPointer<Component> targetComponent)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);    

    targetComponent_ = targetComponent;

    clear();

    loadFileToDisplay(absolutePath);

    QString fileContent = display_->toPlainText();

    grayOutFileContent(fileContent);

    if (hasValidEntity(fileContent))
    {        
        highlightEntity(fileContent);

        genericParser_.runParser(display_->toPlainText(), targetComponent);
        portParser_.runParser(display_->toPlainText(), targetComponent);
    }

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::clear()
//-----------------------------------------------------------------------------
void VhdlParser::clear()
{
    removePreviousGenerics();
    removePreviousPorts();
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::onPortParsed()
//-----------------------------------------------------------------------------
void VhdlParser::onPortParsed(QSharedPointer<Port> parsedPort, QString const& declaration)
{
    parsedPortDeclarations_.insert(parsedPort, declaration);

    foreach(QSharedPointer<ModelParameter> modelParameter, targetComponent_->getModelParameters())
    {
        if (declaration.contains(modelParameter->getName()))
        {
            addDependencyOfGenericToPort(modelParameter, parsedPort);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::onGenericParsed()
//-----------------------------------------------------------------------------
void VhdlParser::onGenericParsed(QSharedPointer<ModelParameter> parsedGeneric)
{
    if (!parsedGenerics_.contains(parsedGeneric))
    {
        parsedGenerics_.append(parsedGeneric);
    }
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::loadFileToDisplay()
//-----------------------------------------------------------------------------
void VhdlParser::loadFileToDisplay(QString const& absolutePath)
{
    QFile vhdlFile(absolutePath);
    if (!vhdlFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(0, "Error ", vhdlFile.errorString() + ": " + absolutePath);
        return;
    }

    QTextStream stream(&vhdlFile);
    QString const& fileString = stream.readAll();
    vhdlFile.close();    

    display_->setPlainText(fileString);       
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::editorChangedModelParameter()
//-----------------------------------------------------------------------------
void VhdlParser::editorChangedModelParameter(QSharedPointer<ModelParameter> changedParameter) const
{
    foreach(QSharedPointer<Port> affectedPort, dependedGenerics_.value(changedParameter))
    {
        QString portDeclaration = parsedPortDeclarations_.value(affectedPort);

        affectedPort->setLeftBound(portParser_.parseLeftBound(portDeclaration, targetComponent_));
        affectedPort->setRightBound(portParser_.parseRightBound(portDeclaration, targetComponent_));
        affectedPort->setDefaultValue(portParser_.parseDefaultValue(portDeclaration));
    }
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::removePorts()
//-----------------------------------------------------------------------------
void VhdlParser::removePreviousPorts()
{
    foreach (QSharedPointer<Port> port, parsedPortDeclarations_.keys())
    {
        emit removePort(port);
    }

    parsedPortDeclarations_.clear();
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::removeGenerics()
//-----------------------------------------------------------------------------
void VhdlParser::removePreviousGenerics()
{
    foreach (QSharedPointer<ModelParameter> parsedGeneric, parsedGenerics_)
    {
        emit removeGeneric(parsedGeneric);
    }

    parsedGenerics_.clear();
    dependedGenerics_.clear();
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::hasValidEntity()
//-----------------------------------------------------------------------------
bool VhdlParser::hasValidEntity(QString const& fileContent) const
{
    int entityStartIndex = ENTITY_EXP.indexIn(fileContent);
    bool hasEntity = entityStartIndex != -1;
    bool hasOnlyOneEntity = entityStartIndex == ENTITY_EXP.lastIndexIn(fileContent);

    return hasEntity && hasOnlyOneEntity;
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::VhdlParser::grayOutFileContent()
//-----------------------------------------------------------------------------
void VhdlParser::grayOutFileContent(QString const& fileContent) const
{    
    highlighter_->applyFontColor(fileContent, QColor("gray"));
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::highlightEntity()
//-----------------------------------------------------------------------------
void VhdlParser::highlightEntity(QString const& fileContent) const
{
    ENTITY_EXP.indexIn(fileContent);

    highlighter_->applyFontColor(ENTITY_EXP.cap(0), QColor("black"));
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::addDependencyOfGenericToPort()
//-----------------------------------------------------------------------------
void VhdlParser::addDependencyOfGenericToPort(QSharedPointer<ModelParameter> modelParameter, 
    QSharedPointer<Port> parsedPort)
{
    QList<QSharedPointer<Port> > portList = dependedGenerics_.value(modelParameter);

    if (!portList.contains(parsedPort))
    {
        portList.append(parsedPort);
    }

    dependedGenerics_.insert(modelParameter, portList);
}
