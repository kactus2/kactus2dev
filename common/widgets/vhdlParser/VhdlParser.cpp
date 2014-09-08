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
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QTextStream>
#include <QTextBlock>
#include <QScrollBar>
#include <QRegExp>
#include <QList>
#include <QApplication>
#include <QAbstractTextDocumentLayout>

#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/port.h>
#include <common/widgets/vhdlParser/VhdlSyntax.h>
#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: VhdlParser()
//-----------------------------------------------------------------------------
VhdlParser::VhdlParser(QWidget*parent) : QPlainTextEdit( parent ), 
    ports_(), 
    generics_(), 
    genericsInPorts_(), 
    genericsInGenerics_(), 
    entityEnd_("", Qt::CaseInsensitive), 
    typeExp_(VhdlSyntax::TYPE_EXP.pattern().replace("(?:","("), Qt::CaseInsensitive), 
    equationExp_(QString(VhdlSyntax::MATH_EXP).replace("(?:","(")),
    insideEntityFormat_(),
    outsideEntityFormat_(),
    selectedPortFormat_(),
    selectedGenericFormat_(),
    notSelectedFormat_()
{
    QFont font("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(9);
    setFont(font);

    int tabStop = 4;
    setTabStopWidth(tabStop * fontMetrics().width(' '));

    setReadOnly(true);
    setCursorWidth(0);

    insideEntityFormat_.setForeground(QColor("black"));

    outsideEntityFormat_.setForeground(QColor("gray"));

    selectedPortFormat_.setForeground(QColor("black"));
    selectedPortFormat_.setBackground(QBrush(KactusColors::SW_COMPONENT));

    selectedGenericFormat_.setForeground(QColor("black"));
    selectedGenericFormat_.setBackground(QBrush(KactusColors::HW_BUS_COMPONENT));

    notSelectedFormat_.setForeground(QColor("black"));
    notSelectedFormat_.setBackground(QBrush(KactusColors::MISSING_COMPONENT));
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
    setEntityEndExp(toPlainText());    
    if (checkEntityStructure(toPlainText()))
    {
        QTextCursor cursor = textCursor();
        cursor.setPosition(VhdlSyntax::ENTITY_BEGIN_EXP.indexIn(toPlainText()));
        int rowNumber = cursor.block().firstLineNumber();
        verticalScrollBar()->setSliderPosition(rowNumber);
    }
}

//-----------------------------------------------------------------------------
// Function: parseFile()
//-----------------------------------------------------------------------------
void VhdlParser::parseFile(QString absolutePath)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QFile vhdlFile(absolutePath);
    if (!vhdlFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this, "Error", vhdlFile.errorString()+": "+absolutePath);
        return;
    }

    removePorts();
    removeGenerics();

    setPlainText("");
    createDocument(vhdlFile);       

    importGenerics();
    importPorts();

    QApplication::restoreOverrideCursor();
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
            
            QTextCursor cursor = textCursor();
            cursor.setPosition(info.beginPos);
            cursor.setPosition(info.endPos,QTextCursor::KeepAnchor);
            cursor.setCharFormat(notSelectedFormat_);

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

            QTextCursor cursor = textCursor();
            cursor.setPosition(info.beginPos);
            cursor.setPosition(info.endPos,QTextCursor::KeepAnchor);
            cursor.setCharFormat(notSelectedFormat_);
              
            ports_.remove(info);   
            info.enabled = false;
            ports_.insert(info,portList);
        
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void VhdlParser::mouseDoubleClickEvent(QMouseEvent *e)
{
    QTextCursor cursor = cursorForPosition( e->pos() ); 
    int pos = cursor.position();

    foreach( SelectionInfo info, ports_.keys() )
    {
        if( info.beginPos <= pos && pos <= info.endPos )
        {           
            toggleSelection(info);
            QList<QSharedPointer<Port> > portList = ports_.take(info);
            ports_.insert(info, portList);
            return;
        }
    }

    foreach( SelectionInfo info, generics_.keys() )
    {
        if( info.beginPos <= pos && pos <= info.endPos )
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
void VhdlParser::removePorts()
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
    foreach ( QList< QSharedPointer<ModelParameter> > list, generics_.values() )
    {
        foreach ( QSharedPointer<ModelParameter> parameter, list )
        {
            assignGenerics(parameter);
            emit addGeneric(parameter);
        }
    }
}  

//-----------------------------------------------------------------------------
// Function: removeGenerics()
//-----------------------------------------------------------------------------
void VhdlParser::removeGenerics()
{
    foreach ( QList< QSharedPointer<ModelParameter> > list, generics_.values() )
    {
        foreach ( QSharedPointer<ModelParameter> parameter, list )
        {
            emit removeGeneric(parameter);
        }
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
    
    QString declaration = toPlainText().mid(info.beginPos,info.endPos-info.beginPos);
    VhdlSyntax::PORT_EXP.indexIn(declaration);
    QString portType = VhdlSyntax::PORT_EXP.cap(3);
    QString defaultValue = VhdlSyntax::PORT_EXP.cap(4).trimmed();        

    typeExp_.indexIn(portType);
    QString plainType = typeExp_.cap(1);
    QString bounded = typeExp_.cap(2);
    int left = 0;
    int right = 0;        
    parseBounds(bounded,left,right);        
    port->setLeftBound(left);
    port->setRightBound(right);
    int def = parseEquation(defaultValue);
    if ( def != -1 )
    {
        port->setDefaultValue(QString::number(def));
    }

    foreach (QList< QSharedPointer<ModelParameter> > genericList, generics_.values() )
    {
        foreach ( QSharedPointer<ModelParameter> parameter, genericList ) 
        {
            if ( declaration.contains(parameter->getName()) )
            {
                QList< QSharedPointer<Port> > ports = genericsInPorts_.value(parameter);
                if ( !ports.contains(port) )
                {
                    ports.append(port);
                    genericsInPorts_.insert(parameter,ports);
                }   
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

    QString declaration = toPlainText().mid(info.beginPos,info.endPos-info.beginPos);
    VhdlSyntax::GENERIC_EXP.indexIn(declaration);
    QString cap = VhdlSyntax::GENERIC_EXP.cap(0);
    QString defaultValue = VhdlSyntax::GENERIC_EXP.cap(3);        
    int def = parseEquation(defaultValue);
    if ( def != -1 )
    {
        param->setValue(QString::number(def));
    }

    foreach (QList< QSharedPointer<ModelParameter> > genericList, generics_.values() )
    {
        foreach ( QSharedPointer<ModelParameter> parameter, genericList ) 
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
}

//-----------------------------------------------------------------------------
// Function: createDocument()
//-----------------------------------------------------------------------------
void VhdlParser::createDocument(QFile& vhdlFile)
{
    QTextStream stream(&vhdlFile);
    QString fileString = stream.readAll();
    vhdlFile.close();

    setEntityEndExp(fileString);

    QTextCursor cursor = textCursor();    
    cursor.insertText(fileString, outsideEntityFormat_);

    if (!checkEntityStructure(fileString))
    {        
        return;
    }

    const int entityBeginIndex = VhdlSyntax::ENTITY_BEGIN_EXP.indexIn(toPlainText());  
    const int entityEndIndex = entityEnd_.indexIn(toPlainText(),entityBeginIndex);  

    formatSection(entityBeginIndex, entityEndIndex + entityEnd_.matchedLength() - entityBeginIndex,
                    insideEntityFormat_);

    parseGenerics();
    parsePorts();
}

//-----------------------------------------------------------------------------
// Function: checkEntityStructure()
//-----------------------------------------------------------------------------
bool VhdlParser::checkEntityStructure(QString const& fileString) const
{
    if (fileString.count(VhdlSyntax::ENTITY_BEGIN_EXP) != 1 || fileString.count(entityEnd_) == 0)
    {
        return false;
    }

    QString entity = parseSection(VhdlSyntax::ENTITY_BEGIN_EXP, entityEnd_, fileString);
    int portBeginIndex =  VhdlSyntax::PORTS_BEGIN_EXP.indexIn(entity);
    int portEndIndex = VhdlSyntax::PORTS_END_EXP.indexIn(entity, portBeginIndex);
    if ( portEndIndex < portBeginIndex )
    {       
        return false;
    }

    int genericBeginIndex =  VhdlSyntax::GENERICS_BEGIN_EXP.indexIn(entity);
    int genericEndIndex = VhdlSyntax::GENERICS_END_EXP.indexIn(entity, genericBeginIndex);
    if (  genericEndIndex < genericBeginIndex )
    {
        return false;
    }

    if ( (portBeginIndex < genericEndIndex && genericEndIndex <= portEndIndex) ||
        ( genericBeginIndex < portEndIndex && portEndIndex <= genericEndIndex ) )
    {

        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: setEntityEndExp()
//-----------------------------------------------------------------------------
void VhdlParser::setEntityEndExp(QString const& fileString)
{
    VhdlSyntax::ENTITY_BEGIN_EXP.indexIn(fileString);
    QString entityName = VhdlSyntax::ENTITY_BEGIN_EXP.cap(1);
    QString pattern = VhdlSyntax::ENTITY_END_EXP.pattern();
    pattern.replace("NAME",entityName);
    entityEnd_.setPattern(pattern);
}

//-----------------------------------------------------------------------------
// Function: parseSectionContent()
//-----------------------------------------------------------------------------
QString VhdlParser::parseSection(QRegExp const& begin, QRegExp const& end, 
    QString const& text) const
{   
    int leftBound = begin.indexIn(text);
    int rightBound = end.indexIn(text,leftBound);

    if ( leftBound == -1 || rightBound == -1 )
    {
        return QString();
    }

    rightBound += end.matchedLength();
    return text.mid(leftBound,rightBound-leftBound);
}

//-----------------------------------------------------------------------------
// Function: parseSectionContent()
//-----------------------------------------------------------------------------
QString VhdlParser::parseSectionContent(QRegExp const& begin, QRegExp const& end, 
    QString const& text) const
{   
    int leftBound = begin.indexIn(text);
    int rightBound = end.indexIn(text,leftBound);

    if ( leftBound == -1 || rightBound == -1 )
    {
        return QString();
    }

    leftBound += begin.matchedLength();
    return text.mid(leftBound,rightBound-leftBound);
}

//-----------------------------------------------------------------------------
// Function: parsePorts()
//-----------------------------------------------------------------------------
void VhdlParser::parsePorts()
{
    QString entity = parseSection(VhdlSyntax::ENTITY_BEGIN_EXP,entityEnd_, toPlainText());
    QString ports = parseSectionContent(VhdlSyntax::PORTS_BEGIN_EXP,VhdlSyntax::PORTS_END_EXP,entity);

    QTextCursor cursor = textCursor();
    int offset = toPlainText().indexOf(ports);
    int index = 0;

    while ( index < ports.length() )
    {
        int portIndex = VhdlSyntax::PORT_EXP.indexIn(ports,index);
        int commentIndex = VhdlSyntax::COMMENT_LINE_EXP.indexIn(ports,index);

        if ( commentIndex == index )
        { 
            int commentLength = VhdlSyntax::COMMENT_LINE_EXP.matchedLength();
            formatSection(index+offset,commentLength,insideEntityFormat_);       
            index += commentLength;
        }

        else if ( portIndex == index )
        {
            int length = VhdlSyntax::PORT_EXP.matchedLength();
             
            SelectionInfo info;
            info.beginPos = index+offset;
            info.endPos = info.beginPos + length;
            info.enabled = true;

            formatSection(info.beginPos, length, selectedPortFormat_);            
            createPort(info);

            index = portIndex + length;
        }

        else
        {        
            int skipLength = 0;            
            if ( portIndex != -1 && commentIndex != -1 )
            {
                if ( portIndex < commentIndex )
                {
                    skipLength = portIndex - index;
                }
                else
                {
                    skipLength = commentIndex - index;
                }
            }
            else if ( portIndex != -1 )
            {
                skipLength = portIndex - index;
            }
            else if ( commentIndex != -1) 
            {
                skipLength = commentIndex - index;
            }
            else
            {
                skipLength = ports.length() - index;
            }

            formatSection(index+offset,skipLength,insideEntityFormat_);       
            index += skipLength;  
        } 
    }
}

//-----------------------------------------------------------------------------
// Function: parseGenerics()
//-----------------------------------------------------------------------------
void VhdlParser::parseGenerics()
{
    QString entity = parseSection(VhdlSyntax::ENTITY_BEGIN_EXP,entityEnd_, toPlainText());
    QString generics = parseSectionContent(VhdlSyntax::GENERICS_BEGIN_EXP,VhdlSyntax::GENERICS_END_EXP,entity);

    QTextCursor cursor = textCursor();
    int offset = toPlainText().indexOf(generics);
    int index = 0;

    while ( index < generics.length() )
    {
        int genericIndex = VhdlSyntax::GENERIC_EXP.indexIn(generics,index);
        int commentIndex = VhdlSyntax::COMMENT_LINE_EXP.indexIn(generics,index);

        if ( commentIndex == index )
        {
            int commentLength = VhdlSyntax::COMMENT_LINE_EXP.matchedLength();

            formatSection(index+offset, commentLength, insideEntityFormat_);
            index += commentLength;
        }

        else if ( genericIndex == index )
        {
            int length = VhdlSyntax::GENERIC_EXP.matchedLength();   

            SelectionInfo info;
            info.beginPos = index+offset;
            info.endPos = info.beginPos + length;
            info.enabled = true;

            createGeneric(info);
            formatSection(info.beginPos,length,selectedGenericFormat_);

            index = genericIndex + length;           
        }

        else 
        {
            int skipLength = 0;  

            if ( genericIndex != -1 && commentIndex != -1 )
            {
                if ( genericIndex < commentIndex )
                {
                    skipLength = genericIndex - index;
                }
                else
                {
                    skipLength = commentIndex - index;
                }
            }
            else if ( genericIndex != -1 )
            {
                skipLength = genericIndex - index;
            }
            else if ( commentIndex != -1) 
            {
                skipLength = commentIndex - index;
            }
            else
            {
                skipLength = generics.length() - index;
            }
            
            formatSection(index+offset,skipLength,insideEntityFormat_);
            index += skipLength;  
        }
    }
}

//-----------------------------------------------------------------------------
// Function: parseBounds()
//-----------------------------------------------------------------------------
void VhdlParser::parseBounds(QString const& rangeDeclaration, int& leftBound, int& rightBound) const
{
    static QRegExp rangeExp = QRegExp("[(]\\s*(" + VhdlSyntax::MATH_EXP + ")\\s+\\w+\\s+" + 
                                             "(" + VhdlSyntax::MATH_EXP + ")\\s*[)]");

    if( rangeExp.indexIn(rangeDeclaration) != -1 )
    {
        QString left = rangeExp.cap(1);
        QString right = rangeExp.cap(2);

        leftBound = parseEquation(left);
        rightBound = parseEquation(right);
    }
}

//-----------------------------------------------------------------------------
// Function: parseEquation()
//-----------------------------------------------------------------------------
int VhdlParser::parseEquation(QString const& equation) const
{
    if ( equation.isEmpty() )
    {
        return -1;
    } 

    // Do not calculate hex (or other base) values.
    if ( equation.indexOf(QRegExp("\\d+[#]\\d+[#]")) != -1 )
    {
        return -1;
    }

    // Do not calculate logic values e.g. '1' or vectors e.g. "1001".
    if ( equation.contains(QRegExp("['\"]")) )
    {
        return -1;
    }

    static QRegExp multiplyDivide = QRegExp("[/*/]");
    static QRegExp power = QRegExp("[/*][/*]");
    static QRegExp extraOperand = QRegExp("(" + VhdlSyntax::OPERATIONS + ")\\s*(\\w+)");

    equationExp_.indexIn(equation);  
    QString value = equationExp_.cap(1);
    QString increments = equationExp_.cap(2);
    QStringList listedEquation = QStringList(value);

    int index = 0;
    while ( extraOperand.indexIn(increments,index) != -1)
    {
        listedEquation.append(extraOperand.cap(1));
        listedEquation.append(extraOperand.cap(2));
        index += extraOperand.matchedLength();        
    }

    // Power precedes all other operations.
    while ( listedEquation.indexOf(power) != -1 )
    {
        int operatorIndex = listedEquation.indexOf(power);

        Q_ASSERT(operatorIndex != 0 && operatorIndex != listedEquation.length());

        QString leftOperand = listedEquation.at(operatorIndex-1);
        QString rightOperand = listedEquation.at(operatorIndex+1);

        bool leftOk = true;
        bool rightOk = true;
        int left = valueForString(leftOperand, leftOk);
        int right = valueForString(rightOperand, rightOk);

        if ( leftOk == false || rightOk == false )
        {
            return -1;
        }

        int result = left;
        for( int i = 0; i < right; i++)
        {
            result *= left;
        }

        listedEquation.replace(operatorIndex-1, QString::number(result));
        listedEquation.removeAt(operatorIndex);
        listedEquation.removeAt(operatorIndex);
    }

    // Multiply and divide precedes addition and substraction.
    while ( listedEquation.indexOf(multiplyDivide) != -1 )
    {
        int operatorIndex = listedEquation.indexOf(multiplyDivide);

        Q_ASSERT(operatorIndex != 0 && operatorIndex != listedEquation.length());

        QString operation = listedEquation.at(operatorIndex);
        QString leftOperand = listedEquation.at(operatorIndex-1);
        QString rightOperand = listedEquation.at(operatorIndex+1);

        bool leftOk = true;
        bool rightOk = true;
        int left = valueForString(leftOperand, leftOk);
        int right = valueForString(rightOperand, rightOk);

        if ( leftOk == false || rightOk == false || ( operation == "/" && right == 0 ) )
        {
            return -1;
        }

        int result = 0;

        if ( operation == "*" )
        {
            result = left * right;
        }
        else
        {
            result = left / right;
        }

        listedEquation.replace(operatorIndex-1, QString::number(result));
        listedEquation.removeAt(operatorIndex);
        listedEquation.removeAt(operatorIndex);
    }

    while ( listedEquation.size() > 1 )
    {
        bool leftOk = true;
        bool rightOk = true;
        int left = valueForString(listedEquation.value(0), leftOk);
        int right = valueForString(listedEquation.value(2), rightOk);


        if ( leftOk == false || rightOk == false )
        {
            return -1;
        }

        int result = 0;

        if ( listedEquation.value(1) == "+" )
        {
            result = left + right;
        }
        else if ( listedEquation.value(1) == "-" )
        {
            result = left - right;
        }
        else
        {
            return -1;
        }

        listedEquation.replace(0, QString::number(result));
        listedEquation.removeAt(1);
        listedEquation.removeAt(1);
    }

    if ( listedEquation.size() == 1 )
    {
        bool ok = true;
        return valueForString(listedEquation.value(0),ok);
    }   

    return -1;
}

//-----------------------------------------------------------------------------
// Function: createPort()
//-----------------------------------------------------------------------------
void VhdlParser::createPort(SelectionInfo const& info)
{
    QString portNames = VhdlSyntax::PORT_EXP.cap(1);
	General::Direction direction = General::str2Direction(VhdlSyntax::PORT_EXP.cap(2).toLower(),General::DIRECTION_INVALID);
    QString portType = VhdlSyntax::PORT_EXP.cap(3);
    QString defaultValue = VhdlSyntax::PORT_EXP.cap(4).trimmed();
    QString description = VhdlSyntax::PORT_EXP.cap(5).trimmed();

    if ( description.isEmpty() )
    {
        description = VhdlSyntax::PORT_EXP.cap(6).trimmed();
    }

    typeExp_.indexIn(portType);
    QString type = typeExp_.cap(1); 
    QString bounded = typeExp_.cap(2);

    int leftBound = 0;
    int rightBound = 0;
    parseBounds(bounded,leftBound,rightBound);

    // Port names are separated by a colon (,).
    QList< QSharedPointer<Port> > ports;
    QRegExp nameDelimiter("\\s*[,]\\s*");
    foreach( QString name, portNames.split(nameDelimiter, QString::SkipEmptyParts) )
    {   
        QSharedPointer<Port> port(new Port(name, direction, leftBound, rightBound, type, "", 
                                            defaultValue, description));
        ports.append(port);
    } 

    ports_.insert(info,ports);
}

//-----------------------------------------------------------------------------
// Function: createGeneric()
//-----------------------------------------------------------------------------
void VhdlParser::createGeneric(SelectionInfo const& info)
{
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
            info.enabled = false;        
            foreach (QSharedPointer<Port> port, ports_.value(info) )
            {
                emit removePort(port);
            } 
            QTextCursor cursor = textCursor();
            cursor.setPosition(info.beginPos);
            cursor.setPosition(info.endPos,QTextCursor::KeepAnchor);
            cursor.setCharFormat(notSelectedFormat_);
        }
        else
        {            
            info.enabled = true;
            foreach (QSharedPointer<Port> port, ports_.value(info) )
            {
                emit addPort(port);
            } 
            QTextCursor cursor = textCursor();
            cursor.setPosition(info.beginPos);
            cursor.setPosition(info.endPos,QTextCursor::KeepAnchor);
            cursor.setCharFormat(selectedPortFormat_);
        }
    }

    else if (generics_.contains(info))
    {
        if ( info.enabled )
        {    
            info.enabled = false;        
            foreach (QSharedPointer<ModelParameter> parameter, generics_.value(info) )
            {
                emit removeGeneric(parameter);
            } 
            QTextCursor cursor = textCursor();
            cursor.setPosition(info.beginPos);
            cursor.setPosition(info.endPos,QTextCursor::KeepAnchor);
            cursor.setCharFormat(notSelectedFormat_);
        }
        else
        {            
            info.enabled = true;
            foreach (QSharedPointer<ModelParameter> parameter, generics_.value(info) )
            {
                emit addGeneric(parameter);
            } 
            QTextCursor cursor = textCursor();
            cursor.setPosition(info.beginPos);
            cursor.setPosition(info.endPos,QTextCursor::KeepAnchor);
            cursor.setCharFormat(selectedGenericFormat_);
        }

    }
}

//-----------------------------------------------------------------------------
// Function: valueForString()
//----------------------------------------------------------------------------- 
int  VhdlParser::valueForString(QString const& string, bool& ok) const  
{
    bool isNumber = true;
    int value = string.toInt(&isNumber);
    if ( !isNumber )
    {
        foreach (QList< QSharedPointer<ModelParameter> > genericList, generics_.values() )
        {
            foreach ( QSharedPointer<ModelParameter> parameter, genericList ) 
            {
                if ( parameter->getName() == string )
                {
                    int genericValue = parameter->getValue().toInt(&isNumber);
                    if ( !isNumber )
                    {
                        ok = false;
                        return -1;
                    }

                    ok = true;
                    return genericValue;
                }
            }
        }
        ok = false;
        return -1;
    }

    ok = true;
    return value;
}

//-----------------------------------------------------------------------------
// Function: VhdlParser::formatSection()
//-----------------------------------------------------------------------------
void VhdlParser::formatSection(int const pos, int const lenght, QTextCharFormat const& format)
{
    if ( pos >= 0 && lenght > 0 )
    {
        QTextCursor cursor = textCursor();
        cursor.setPosition(pos);
        cursor.setPosition(pos+lenght,QTextCursor::KeepAnchor);
        cursor.setCharFormat(format);
    }
}
