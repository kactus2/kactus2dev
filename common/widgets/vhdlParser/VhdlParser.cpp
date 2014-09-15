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

#include <QFile>
#include <QFont>
#include <QTextCursor>
#include <QMessageBox>
#include <QTextStream>
#include <QTextBlock>
#include <QRegExp>
#include <QList>
#include <QApplication>

#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/port.h>
#include <common/widgets/vhdlParser/VhdlSyntax.h>

#include <wizards/ComponentWizard/VhdlImportEditor/EquationParser.h>
#include <wizards/ComponentWizard/VhdlImportEditor/SourceFileDisplayer.h>
#include <wizards/ComponentWizard/VhdlImportEditor/VHDLHighlighter.h>


//-----------------------------------------------------------------------------
// Function: VhdlParser()
//-----------------------------------------------------------------------------
VhdlParser::VhdlParser(SourceFileDisplayer* display, QObject* parent) : 
QObject(parent),
    ports_(),
    generics_(),
    genericsInPorts_(),
    genericsInGenerics_(),
    display_(display),
    highlighter_(new VHDLHighlighter(display, this))    
{
    
}

//-----------------------------------------------------------------------------
// Function: ~VhdlParser()
//-----------------------------------------------------------------------------
VhdlParser::~VhdlParser()
{

}

//-----------------------------------------------------------------------------
// Function: scrollToEntityBegin()
//-----------------------------------------------------------------------------
void VhdlParser::scrollToEntityBegin()
{  
    int entityBegin = VhdlSyntax::ENTITY_BEGIN_EXP.indexIn(display_->toPlainText());
    if (entityBegin != -1)
    {
        display_->scrollToCharacterPosition(entityBegin);
    }
}

//-----------------------------------------------------------------------------
// Function: parseFile()
//-----------------------------------------------------------------------------
void VhdlParser::parseFile(QString const& absolutePath)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);    

    clear();

    loadFileToDisplay(absolutePath);

    if (hasValidEntity())
    {        
        parseGenerics();
        parsePorts();

        importGenerics();
        importPorts();
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
    highlighter_->onSourceFileChanged();
}

//-----------------------------------------------------------------------------
// Function: modelParameterChanged()
//-----------------------------------------------------------------------------
void VhdlParser::editorChangedModelParameter(QSharedPointer<ModelParameter> changedParameter)
{
    if ( genericsInPorts_.contains(changedParameter) )
    {
        foreach (QSharedPointer<Port> port, genericsInPorts_.value(changedParameter) )
        {
            assignGenerics(port);
        } 
    }

    if ( genericsInGenerics_.contains(changedParameter) )
    {
        foreach (QSharedPointer<ModelParameter> param, genericsInGenerics_.value(changedParameter) )
        {
            if ( param != changedParameter )
            {
                assignGenerics(param);
            } 
        } 
    }
}

//-----------------------------------------------------------------------------
// Function: editorRemovedModelParameter()
//-----------------------------------------------------------------------------
void VhdlParser::editorRemovedModelParameter(QSharedPointer<ModelParameter> removedParameter)
{
    foreach ( QList< QSharedPointer<ModelParameter> > parameterList, generics_.values() )
    {
        if ( parameterList.contains(removedParameter) )
        {
            SelectionInfo info = generics_.key(parameterList);
            
            highlighter_->unSelect(info.beginPos, info.endPos);

            generics_.remove(info);
            info.enabled = false;
            generics_.insert(info,parameterList);

            return;
        }
    }
}


//-----------------------------------------------------------------------------
// Function: editorRemovedPort()
//-----------------------------------------------------------------------------
void VhdlParser::editorRemovedPort(QSharedPointer<Port> removedPort)
{
    foreach ( QList< QSharedPointer<Port> > portList, ports_.values() )
    {
        if ( portList.contains(removedPort) )
        {
            SelectionInfo info = ports_.key(portList);

            highlighter_->unSelect(info.beginPos, info.endPos);
              
            ports_.remove(info);   
            info.enabled = false;
            ports_.insert(info,portList);
        
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::toggleAt()
//-----------------------------------------------------------------------------
void VhdlParser::toggleAt(int characterPosition)
{
    foreach( SelectionInfo info, ports_.keys() )
    {
        if( info.beginPos <= characterPosition && characterPosition <= info.endPos )
        {           
            toggleSelection(info);
            QList<QSharedPointer<Port> > portList = ports_.take(info);
            ports_.insert(info, portList);
            return;
        }
    }

    foreach( SelectionInfo info, generics_.keys() )
    {
        if( info.beginPos <= characterPosition && characterPosition <= info.endPos )
        {           
            toggleSelection(info);
            QList<QSharedPointer<ModelParameter> > genericList = generics_.take(info);
            generics_.insert(info, genericList);
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: importPorts()
//-----------------------------------------------------------------------------
void VhdlParser::importPorts()
{
    foreach ( QList< QSharedPointer<Port> > list, ports_.values() )
    {
        foreach ( QSharedPointer<Port> port, list )
        {
            assignGenerics(port);
            emit addPort(port);
        }
    }
}   

//-----------------------------------------------------------------------------
// Function: removePorts()
//-----------------------------------------------------------------------------
void VhdlParser::removePreviousPorts()
{
    foreach ( QList< QSharedPointer<Port> > list, ports_.values() )
    {
        foreach ( QSharedPointer<Port> port, list )
        {
            emit removePort(port);
        }
    }
    ports_.clear();
}   

//-----------------------------------------------------------------------------
// Function: importGenerics()
//-----------------------------------------------------------------------------
void VhdlParser::importGenerics()
{
    foreach (QSharedPointer<ModelParameter> parameter, getAllModelParameters() )
    {
        assignGenerics(parameter);
        emit addGeneric(parameter);
    }
}  

//-----------------------------------------------------------------------------
// Function: removeGenerics()
//-----------------------------------------------------------------------------
void VhdlParser::removePreviousGenerics()
{
    foreach (QSharedPointer<ModelParameter> parameter, getAllModelParameters())
    {
        emit removeGeneric(parameter);
    }

    generics_.clear();
    genericsInPorts_.clear();
    genericsInGenerics_.clear();
}  

//-----------------------------------------------------------------------------
// Function: assignGenerics()
//-----------------------------------------------------------------------------
void VhdlParser::assignGenerics(QSharedPointer<Port> port)
{
    SelectionInfo info = SelectionInfo();
    foreach (QList< QSharedPointer<Port> > portList, ports_.values())
    {
        if ( portList.contains(port) )
        {
            info = ports_.key(portList); 
            break;
        }
    }
    
    QString declaration = display_->toPlainText().mid(info.beginPos, info.endPos - info.beginPos);
    VhdlSyntax::PORT_EXP.indexIn(declaration);
    QString portType = VhdlSyntax::PORT_EXP.cap(3);
    QString defaultValueString = VhdlSyntax::PORT_EXP.cap(4).trimmed();        

    QRegExp typeExpression = QRegExp(VhdlSyntax::TYPE_EXP.pattern().replace("(?:","("), Qt::CaseInsensitive);
    typeExpression.indexIn(portType);
    QString plainType = typeExpression.cap(1);
    QString bounded = typeExpression.cap(2);

    EquationParser equationParser(getAllModelParameters());   
      
    if (!bounded.isEmpty())
    {
        port->setLeftBound(parseLeftBound(bounded, equationParser));
        port->setRightBound(parseRightBound(bounded, equationParser));
    }

    int defaultValue = equationParser.parse(defaultValueString);
    if (defaultValue != -1)
    {
        port->setDefaultValue(QString::number(defaultValue));
    }

    foreach ( QSharedPointer<ModelParameter> parameter, getAllModelParameters() ) 
    {
        if ( declaration.contains(parameter->getName()) )
        {
            QList< QSharedPointer<Port> > ports = genericsInPorts_.value(parameter);
            if ( !ports.contains(port) )
            {
                ports.append(port);
                genericsInPorts_.insert(parameter, ports);
            }   
        }
    }
}

//-----------------------------------------------------------------------------
// Function: assignGenerics()
//-----------------------------------------------------------------------------
void VhdlParser::assignGenerics(QSharedPointer<ModelParameter> param)
{
    SelectionInfo info = SelectionInfo();
    foreach ( QList< QSharedPointer<ModelParameter> > genericList, generics_.values() )
    {
        if ( genericList.contains(param) ){
            info = generics_.key(genericList); 
            break;
        }
    }

    QString declaration = display_->toPlainText().mid(info.beginPos, info.endPos - info.beginPos);
    
    VhdlSyntax::GENERIC_EXP.indexIn(declaration);
    QString defaultValue = VhdlSyntax::GENERIC_EXP.cap(3);        
    
    EquationParser equationParser(getAllModelParameters());

    int def = equationParser.parse(defaultValue);
    if ( def != -1 )
    {
        param->setValue(QString::number(def));
    }

    foreach ( QSharedPointer<ModelParameter> parameter, getAllModelParameters() ) 
    {
        if ( declaration.contains(parameter->getName()) )
        {
            QList< QSharedPointer<ModelParameter> > generics = genericsInGenerics_.value(parameter);
            if ( !generics.contains(param) )
            {
                generics.append(param);
                genericsInGenerics_.insert(parameter,generics);
            }   
        }
    }             
}

//-----------------------------------------------------------------------------
// Function: hasValidEntity()
//-----------------------------------------------------------------------------
bool VhdlParser::hasValidEntity() const
{
    QRegExp entityExp = QRegExp("ENTITY\\s+(\\w+)\\s+IS\\s+.*END\\s+(?:ENTITY\\s+)?(\\1)?\\s*[;]", 
        Qt::CaseInsensitive);

    QString const& sourceContent = display_->toPlainText();

    int entityStartIndex = entityExp.indexIn(sourceContent);
    bool hasEntity = entityStartIndex != -1;
    bool hasOnlyOneEntity = entityStartIndex == entityExp.lastIndexIn(sourceContent);

    return hasEntity && hasOnlyOneEntity;
}

//-----------------------------------------------------------------------------
// Function: parseSectionContent()
//-----------------------------------------------------------------------------
QString VhdlParser::parseSectionContent(QRegExp const& begin, QRegExp const& end, 
    QString const& text) const
{   
    int sectionBeginIndex = begin.indexIn(text);
    int sectionEndIndex = end.indexIn(text,sectionBeginIndex);

    if ( sectionBeginIndex == -1 || sectionEndIndex == -1 )
    {
        return QString();
    }

    sectionBeginIndex += begin.matchedLength();
    return text.mid(sectionBeginIndex, sectionEndIndex - sectionBeginIndex);
}

//-----------------------------------------------------------------------------
// Function: parsePorts()
//-----------------------------------------------------------------------------
void VhdlParser::parsePorts()
{   
    QString ports = parseSectionContent(VhdlSyntax::PORTS_BEGIN_EXP, VhdlSyntax::PORTS_END_EXP, display_->toPlainText());

    int offset = display_->toPlainText().indexOf(ports);

    QRegExp portExp = VhdlSyntax::PORT_EXP;

    int portIndex = portExp.indexIn(ports, 0);    

    while ( portIndex != -1 )
    {
        int length = 0;

        int lastNewline = ports.lastIndexOf(QRegExp("\n"), portIndex);

        int lastComment = VhdlSyntax::COMMENT_LINE_EXP.indexIn(ports, lastNewline);

        if (lastComment != -1 && lastComment < portIndex)
        {
            length =  VhdlSyntax::COMMENT_LINE_EXP.matchedLength();
        }
        else
        {
            length = portExp.matchedLength();

            SelectionInfo info;
            info.beginPos = portIndex + offset;
            info.endPos = info.beginPos + length;
            info.enabled = true;

            highlighter_->selectPort(info.beginPos, info.endPos);
            createPort(info, portExp.cap());
        }

        portIndex = portExp.indexIn(ports, portIndex + length);
    }
}

//-----------------------------------------------------------------------------
// Function: parseGenerics()
//-----------------------------------------------------------------------------
void VhdlParser::parseGenerics()
{
    QString generics = parseSectionContent(VhdlSyntax::GENERICS_BEGIN_EXP, VhdlSyntax::GENERICS_END_EXP, display_->toPlainText());

    int offset = display_->toPlainText().indexOf(generics);

    QRegExp genericExp = VhdlSyntax::GENERIC_EXP;

    int genericIndex = genericExp.indexIn(generics, 0);

    while ( genericIndex != -1 )
    {
        int length = 0;   

        int lastNewline = generics.lastIndexOf(QRegExp("(^|\n)"), genericIndex);

        int lastComment = VhdlSyntax::COMMENT_LINE_EXP.indexIn(generics, lastNewline);

        if (lastComment != -1 && lastComment < genericIndex)
        {
            length =  VhdlSyntax::COMMENT_LINE_EXP.matchedLength();
        }
        else
        {       
            length =  genericExp.matchedLength();

            SelectionInfo info;
            info.beginPos = genericIndex + offset;
            info.endPos = info.beginPos + length;
            info.enabled = true;

            highlighter_->selectModelParameter(info.beginPos, info.endPos);
            createGeneric(info, genericExp.cap());

        }

        genericIndex = genericExp.indexIn(generics, genericIndex + length);
    }
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::parseLeftBound()
//-----------------------------------------------------------------------------
int VhdlParser::parseLeftBound(QString const& rangeDeclaration, EquationParser const& parser) const
{
    static QRegExp rangeExp = QRegExp("[(]\\s*(" + VhdlSyntax::MATH_EXP + ")\\s+\\w+\\s+" + 
                                             "(" + VhdlSyntax::MATH_EXP + ")\\s*[)]");

    if( rangeExp.indexIn(rangeDeclaration) != -1 )
    {
        QString leftEquation = rangeExp.cap(1);
        return parser.parse(leftEquation);
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::parseRightBound()
//-----------------------------------------------------------------------------
int VhdlParser::parseRightBound(QString const& rangeDeclaration, EquationParser const& parser) const
{
    static QRegExp rangeExp = QRegExp("[(]\\s*(" + VhdlSyntax::MATH_EXP + ")\\s+\\w+\\s+" + 
        "(" + VhdlSyntax::MATH_EXP + ")\\s*[)]");

    if( rangeExp.indexIn(rangeDeclaration) != -1 )
    {
        QString rightEquation = rangeExp.cap(2);
        return parser.parse(rightEquation);
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: createPort()
//-----------------------------------------------------------------------------
void VhdlParser::createPort(SelectionInfo const& info, QString const& portDeclaration)
{
    VhdlSyntax::PORT_EXP.indexIn(portDeclaration);
    QString portNames = VhdlSyntax::PORT_EXP.cap(1);
	General::Direction direction = General::str2Direction(VhdlSyntax::PORT_EXP.cap(2).toLower(),General::DIRECTION_INVALID);
    QString portType = VhdlSyntax::PORT_EXP.cap(3);
    QString defaultValue = VhdlSyntax::PORT_EXP.cap(4).trimmed();
    QString description = VhdlSyntax::PORT_EXP.cap(5).trimmed();

    if ( description.isEmpty() )
    {
        description = VhdlSyntax::PORT_EXP.cap(6).trimmed();
    }

    QRegExp typeExpression = QRegExp(VhdlSyntax::TYPE_EXP.pattern().replace("(?:","("), Qt::CaseInsensitive);
    typeExpression.indexIn(portType);
    QString type = typeExpression.cap(1); 
    QString bounded = typeExpression.cap(2);

    EquationParser equationParser(getAllModelParameters());
    
    int leftBound = 0;
    int rightBound = 0;

    if (!bounded.isEmpty())
    {
        leftBound = parseLeftBound(bounded, equationParser);
        rightBound = parseRightBound(bounded, equationParser);
    }    

    // Port names are separated by a colon (,).
    QList< QSharedPointer<Port> > ports;
    QRegExp nameDelimiter("\\s*[,]\\s*");
    foreach( QString name, portNames.split(nameDelimiter, QString::SkipEmptyParts) )
    {   
        QSharedPointer<Port> port(new Port(name, direction, leftBound, rightBound, type, "", 
                                            defaultValue, description));
        ports.append(port);
    } 

    ports_.insert(info, ports);
}

//-----------------------------------------------------------------------------
// Function: createGeneric()
//-----------------------------------------------------------------------------
void VhdlParser::createGeneric(SelectionInfo const& info, QString const& declaration)
{
    VhdlSyntax::GENERIC_EXP.indexIn(declaration);
    QString genericNames = VhdlSyntax::GENERIC_EXP.cap(1);
    QString type = VhdlSyntax::GENERIC_EXP.cap(2);
    QString defaultValue = VhdlSyntax::GENERIC_EXP.cap(3);
    QString description = VhdlSyntax::GENERIC_EXP.cap(4).trimmed();

    if ( description.isEmpty() )
    {
        description = VhdlSyntax::GENERIC_EXP.cap(5).trimmed();
    }

    // Generic names are separated by a colon (,).
    QRegExp nameDelimiter("\\s*[,]\\s*");
    QList< QSharedPointer<ModelParameter> > generics;
    foreach( QString name, genericNames.split(nameDelimiter, QString::SkipEmptyParts) )
    {   
        QSharedPointer<ModelParameter> parameter(new ModelParameter());
        parameter->setName(name.trimmed());
        parameter->setDataType(type);
        parameter->setDescription(description);
        parameter->setValue(defaultValue);
        parameter->setUsageType("nontyped");
        generics.append(parameter);            
    } 

    generics_.insert(info,generics);
}

//-----------------------------------------------------------------------------
// Function: toggleSelection()
//-----------------------------------------------------------------------------
void VhdlParser::toggleSelection(SelectionInfo& info)
{
    if ( ports_.contains(info) )
    {
        if ( info.enabled )
        {    
            foreach (QSharedPointer<Port> port, ports_.value(info) )
            {
                emit removePort(port);
            } 
            
            highlighter_->unSelect(info.beginPos, info.endPos);
        }
        else
        {            
            foreach (QSharedPointer<Port> port, ports_.value(info) )
            {
                emit addPort(port);
            } 

            highlighter_->selectPort(info.beginPos, info.endPos);
        }

        info.enabled = !info.enabled;     
    }

    else if (generics_.contains(info))
    {
        if ( info.enabled )
        {         
            foreach (QSharedPointer<ModelParameter> parameter, generics_.value(info) )
            {
                emit removeGeneric(parameter);
            } 
            
            highlighter_->unSelect(info.beginPos, info.endPos);
        }
        else
        {            
            foreach (QSharedPointer<ModelParameter> parameter, generics_.value(info) )
            {
                emit addGeneric(parameter);
            } 

            highlighter_->selectModelParameter(info.beginPos, info.endPos);
        }

        info.enabled = !info.enabled;   
    }
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::getAllModelParameters()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ModelParameter> > VhdlParser::getAllModelParameters() const
{
    QList<QSharedPointer<ModelParameter> > modelParamters;
    foreach(QList<QSharedPointer<ModelParameter> > parameters, generics_.values())
    {
        modelParamters.append(parameters);
    }

    return modelParamters;
}
