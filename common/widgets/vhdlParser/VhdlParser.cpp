//-----------------------------------------------------------------------------
// File: VhdlParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.06.2013
//
// Description:
// VhdlParser for Component Wizard.
//-----------------------------------------------------------------------------
#include "VhdlParser.h"

#include <QFile>
#include <QFont>
#include <QTextCursor>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QTextStream>
#include <QScrollBar>
#include <QRegExp>
#include <QList>
#include <QTextBlockFormat>
#include <QApplication>
#include <QAbstractTextDocumentLayout>

#include <models/modelparameter.h>
#include <models/port.h>
#include <common/widgets/vhdlParser/VhdlEntityHighlighter.h>
#include <common/widgets/vhdlParser/VhdlSyntax.h>

//-----------------------------------------------------------------------------
// Function: VhdlParser()
//-----------------------------------------------------------------------------
VhdlParser::VhdlParser(QWidget*parent) : QPlainTextEdit( parent ), 
    portsMap_(), 
    genericsMap_(), 
    generics_(), 
    genericsInPorts_(), 
    genericsInGenerics_(), 
    portBlocks_(), 
    genericBlocks_(), 
    highlighter_(new VhdlEntityHighlighter(document())),
    entityBegin_(VhdlSyntax::SPACE + VhdlSyntax::ENTITY_BEGIN_EXP.pattern(), Qt::CaseInsensitive),  
    entityEnd_("",Qt::CaseInsensitive), 
    portsBegin_(VhdlSyntax::SPACE + VhdlSyntax::PORTS_BEGIN_EXP.pattern() +
                VhdlSyntax::SPACE + "("+VhdlSyntax::ENDLINE+")?", Qt::CaseInsensitive),
    portsEnd_(VhdlSyntax::SPACE + VhdlSyntax::PORTS_END_EXP.pattern(), Qt::CaseInsensitive),
    portExp_(VhdlSyntax::SPACE + VhdlSyntax::PORT_EXP.pattern(), Qt::CaseInsensitive),
    typeExp_(VhdlSyntax::TYPE_EXP.pattern().replace("(?:","("), Qt::CaseInsensitive), 
    genericsBegin_(VhdlSyntax::SPACE + VhdlSyntax::GENERICS_BEGIN_EXP.pattern(), Qt::CaseInsensitive), 
    genericsEnd_(VhdlSyntax::SPACE + VhdlSyntax::GENERICS_END_EXP.pattern(), Qt::CaseInsensitive), 
    genericExp_(VhdlSyntax::SPACE + VhdlSyntax::GENERIC_EXP.pattern(), Qt::CaseInsensitive),
    equationExp_( QString(VhdlSyntax::MATH_EXP).replace("(?:","("))
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
    if ( checkEntityStructure(toPlainText()) )
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
    if ( !vhdlFile.open(QIODevice::ReadOnly) )
    {
        QMessageBox::information(this, "Error", vhdlFile.errorString()+": "+absolutePath);
        return;
    }

    removePorts();
    removeGenerics();
    
    setPlainText("");
    createDocument(vhdlFile);       
    highlighter_->rehighlight();

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
    foreach ( QList< QSharedPointer<ModelParameter> > paramList, genericsMap_.values() )
    {
        if ( paramList.contains( removedParameter ) )
        {
            QTextBlock block = genericsMap_.key(paramList);
            block.setUserState(VhdlEntityHighlighter::GENERIC_NOT_SELECTED);
            highlighter_->rehighlight();
            return;
        }
    }
}


//-----------------------------------------------------------------------------
// Function: editorRemovedPort()
//-----------------------------------------------------------------------------
void VhdlParser::editorRemovedPort(QSharedPointer<Port> removedPort)
{
    foreach ( QList< QSharedPointer<Port> > paramList, portsMap_.values() )
    {
        if ( paramList.contains(removedPort) )
        {
            QTextBlock block = portsMap_.key(paramList);
            block.setUserState(VhdlEntityHighlighter::PORT_NOT_SELECTED);
            highlighter_->rehighlight();
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void VhdlParser::mouseDoubleClickEvent(QMouseEvent *e) {

    QTextCursor cursor = cursorForPosition( e->pos() ); 
    QTextBlock block = cursor.block();

    toggleBlock(block);
    highlighter_->rehighlight();    
}


//-----------------------------------------------------------------------------
// Function: importPorts()
//-----------------------------------------------------------------------------
void VhdlParser::importPorts()
{
    foreach ( QList< QSharedPointer<Port> > list, portsMap_.values() )
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
    foreach ( QList< QSharedPointer<Port> > list, portsMap_.values() )
    {
        foreach ( QSharedPointer<Port> port, list )
        {
            emit removePort(port);
        }
    }
    portsMap_.clear();
    portBlocks_.clear();
}   

         
//-----------------------------------------------------------------------------
// Function: importGenerics()
//-----------------------------------------------------------------------------
void VhdlParser::importGenerics()
{
    foreach ( QList< QSharedPointer<ModelParameter> > list, genericsMap_.values() )
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
    foreach ( QList< QSharedPointer<ModelParameter> > list, genericsMap_.values() )
    {
        foreach ( QSharedPointer<ModelParameter> parameter, list )
        {
            emit removeGeneric(parameter);
        }
    }
    genericsMap_.clear();
    generics_.clear();
    genericsInPorts_.clear();
    genericsInGenerics_.clear();
}  

//-----------------------------------------------------------------------------
// Function: assignGenerics()
//-----------------------------------------------------------------------------
void VhdlParser::assignGenerics(QSharedPointer<Port> port)
{
    QTextBlock block = QTextBlock();
    foreach ( QList< QSharedPointer<Port> > portList, portsMap_.values() )
    {
        if ( portList.contains(port) ){
            block = portsMap_.key(portList); 
            break;
        }
    }

    if( block.isValid() )
    {
        portExp_.indexIn(block.text());
        QString portType = portExp_.cap(3);
        QString defaultValue = portExp_.cap(4).trimmed();        

        typeExp_.indexIn(portType);
        QString leftBound = typeExp_.cap(1);
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

        foreach ( QSharedPointer<ModelParameter> parameter, generics_ )
        {
            if ( block.text().contains(parameter->getName()) )
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
    QTextBlock block = QTextBlock();
    foreach ( QList< QSharedPointer<ModelParameter> > genericList, genericsMap_.values() )
    {
        if ( genericList.contains(param) ){
            block = genericsMap_.key(genericList); 
            break;
        }
    }

    if( block.isValid() )
    {
        genericExp_.indexIn(block.text());
        QString cap = genericExp_.cap(0);
        QString defaultValue = genericExp_.cap(3);        
        int def = parseEquation(defaultValue);
        if ( def != -1 )
        {
            param->setValue(QString::number(def));
        }

        foreach ( QSharedPointer<ModelParameter> parameter, generics_ )
        {
            if ( block.text().contains(parameter->getName()) )
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
    if ( !checkEntityStructure(fileString) )
    {
        setPlainText(fileString);
        return;
    }

    const int entityBeginIndex = entityBegin_.indexIn(fileString);  
    const int entityEndIndex = entityEnd_.indexIn(fileString,entityBeginIndex);
    const int portBeginIndex =  portsBegin_.indexIn(fileString);   
    const int genericBeginIndex =  genericsBegin_.indexIn(fileString);

    QList<int> indexes;
    indexes.append(entityBeginIndex);
    indexes.append(entityEndIndex);
    indexes.append(portBeginIndex);
    indexes.append(genericBeginIndex);

    QTextCursor cursor = textCursor();
    int index = 0;
    while ( index < fileString.length() )
    {            
        if ( entityBeginIndex == index )
        {   
            insertExtraText(fileString.mid(index,entityBegin_.matchedLength()), VhdlEntityHighlighter::INSIDE_ENTITY);
            index += entityBegin_.matchedLength();                       
        }

        else if ( genericBeginIndex == index )
        {                
            insertExtraText(fileString.mid(index,genericsBegin_.matchedLength()), VhdlEntityHighlighter::INSIDE_ENTITY);      
        
            parseGenerics(fileString);

            int endIndex = genericsEnd_.indexIn(fileString,index);
            insertExtraText(fileString.mid(endIndex,genericsEnd_.matchedLength()), VhdlEntityHighlighter::INSIDE_ENTITY);      
            index = endIndex+genericsEnd_.matchedLength();           
        } 
        
        else if ( portBeginIndex == index )
        {
            insertExtraText(fileString.mid(index,portsBegin_.matchedLength()), VhdlEntityHighlighter::INSIDE_ENTITY);      
            parsePorts(fileString);
           
            int endIndex = portsEnd_.indexIn(fileString,index);
         
            QString portEndStr = fileString.mid(endIndex,portsEnd_.matchedLength());
            insertExtraText(portEndStr, VhdlEntityHighlighter::INSIDE_ENTITY);      
            index = endIndex+portsEnd_.matchedLength();
        }        
     
        else if ( entityEndIndex == index )
        {           
            QString entityEnd = fileString.mid(index,entityEnd_.matchedLength());
            insertExtraText(entityEnd, VhdlEntityHighlighter::INSIDE_ENTITY);           
            index += entityEnd_.matchedLength();
        }
        else
        {          
            indexes.append(index);
            qSort(indexes);

            if ( index != indexes.last() )
            {
                int nextChange = indexes.at(indexes.indexOf(index)+1);
                indexes.removeAll(index);

                int skip = nextChange - index;             

            VhdlEntityHighlighter::BlockStyle prevState = VhdlEntityHighlighter::OUTSIDE_ENTITY;
            if ( entityBeginIndex < index && index < entityEndIndex )
            {
                prevState = VhdlEntityHighlighter::INSIDE_ENTITY;
            }
 
                insertExtraText(fileString.mid(index,skip), prevState);
                index += skip;
            }
            else 
            {                
                int endLenght = fileString.size() - index;
                insertExtraText(fileString.mid(index,endLenght), VhdlEntityHighlighter::OUTSIDE_ENTITY);
                return;
            }
        }
    }
}


//-----------------------------------------------------------------------------
// Function: checkEntityStructure()
//-----------------------------------------------------------------------------
bool VhdlParser::checkEntityStructure(QString const& fileString) const
{
    if( fileString.count(entityBegin_) != 1 || fileString.count(entityEnd_) == 0 )
    {
        return false;
    }

    QString entity = parseSection(entityBegin_, entityEnd_, fileString);
    int portBeginIndex =  VhdlSyntax::PORTS_BEGIN_EXP.indexIn(entity);
    int portEndIndex = portsEnd_.indexIn(entity, portBeginIndex);
    // int portCount = entity.count(portsBegin_);
    if ( portEndIndex < portBeginIndex )
    {       
       return false;
    }

    int genericBeginIndex =  VhdlSyntax::GENERICS_BEGIN_EXP.indexIn(entity);
    int genericEndIndex = genericsEnd_.indexIn(entity, genericBeginIndex);
    //int genericCount = entity.count(genericsBegin_);
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
    QString pattern = VhdlSyntax::SPACE+VhdlSyntax::ENTITY_END_EXP.pattern()+VhdlSyntax::SPACE+"("+VhdlSyntax::ENDLINE+")?";
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
void VhdlParser::parsePorts(QString const& fileString)
{
    QString entity = parseSection(entityBegin_,entityEnd_, fileString);
    QString ports = parseSectionContent(portsBegin_,portsEnd_,entity);
    QTextCursor cursor = textCursor();

    int index = 0;

    while ( index < ports.length() )
    {
        int portIndex = portExp_.indexIn(ports,index);
        int commentIndex = VhdlSyntax::COMMENT_LINE_EXP.indexIn(ports,index);

        if ( commentIndex == index )
        { 
            int commentLength = VhdlSyntax::COMMENT_LINE_EXP.matchedLength();
            QString comment = ports.mid(commentIndex,commentLength);
            insertExtraText(comment,VhdlEntityHighlighter::INSIDE_ENTITY);
            index += commentLength;

        }
        else if ( portIndex == index )
        {
            cursor.block().setUserState(VhdlEntityHighlighter::PORT_SELECTED);

            int length = portExp_.matchedLength();
            QString portDeclaration = ports.mid(portIndex,length).remove(VhdlSyntax::ENDLINE_EXP);
            cursor.insertText(portDeclaration);
            index = portIndex + length;

            createPort(portDeclaration, cursor.block());
            cursor.insertBlock();       
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
            

                insertExtraText(ports.mid(index,skipLength), VhdlEntityHighlighter::INSIDE_ENTITY);   
          
            index += skipLength;  


        } 
    }
}

//-----------------------------------------------------------------------------
// Function: parseGenerics()
//-----------------------------------------------------------------------------
void VhdlParser::parseGenerics(QString const& fileString)
{
    QString entity = parseSection(entityBegin_,entityEnd_, fileString);
    QString generics = parseSectionContent(genericsBegin_,genericsEnd_,entity);

    QTextCursor cursor = textCursor();
    int index = 0;

    while ( index < generics.length() )
    {
        int genericIndex = genericExp_.indexIn(generics,index);
        int commentIndex = VhdlSyntax::COMMENT_LINE_EXP.indexIn(generics,index);

        if ( commentIndex == index )
        {
            int commentLength = VhdlSyntax::COMMENT_LINE_EXP.matchedLength();
            QString comment = generics.mid(commentIndex,commentLength);
            insertExtraText(comment,VhdlEntityHighlighter::INSIDE_ENTITY);
            index += commentLength;
        }

        else if ( genericIndex == index )
        {
            
            cursor.block().setUserState(VhdlEntityHighlighter::GENERIC_SELECTED);
            int length = genericExp_.matchedLength();
            QString genericDeclaration = generics.mid(genericIndex,length).remove(VhdlSyntax::ENDLINE_EXP);
            cursor.insertText(genericDeclaration);
            index = genericIndex + length;             

            createGeneric(genericDeclaration, cursor.block());        
            cursor.insertBlock();
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
                        
            insertExtraText(generics.mid(index,skipLength), VhdlEntityHighlighter::INSIDE_ENTITY);
            index += skipLength;  
        }
    }
}

//-----------------------------------------------------------------------------
// Function: parseBounds()
//-----------------------------------------------------------------------------
void VhdlParser::parseBounds(QString const& rangeDeclaration, int& leftBound, int& rightBound) const
{
    QRegExp rangeExp = QRegExp("[(]\\s*("+equationExp_.pattern()+")\\s+\\w+\\s+("+equationExp_.pattern()+")\\s*[)]");

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
    
    // Do not calculate ogic values e.g. '1' or vectors e.g. "1001".
    if ( equation.contains(QRegExp("['\"]")) )
    {
        return -1;
    }

    static QRegExp multiplyDivide = QRegExp("[/*/]");
    static QRegExp power = QRegExp("[/*][/*]");
    static QRegExp extraOperand = QRegExp(VhdlSyntax::OPERATIONS+"\\s*(\\w+)");

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

        int left = valueForString(leftOperand);
        int right = valueForString(rightOperand);

        if ( left == -1 || right == -1 )
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

        int left = valueForString(leftOperand);
        int right = valueForString(rightOperand);
        
        if ( left == -1 || right == -1 || ( operation == "/" && right == 0 ) )
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
        int left = valueForString(listedEquation.value(0));
        int right = valueForString(listedEquation.value(2));

        if ( left == -1 || right == -1 )
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
        return valueForString(listedEquation.value(0));
    }   

    return -1;
}

//-----------------------------------------------------------------------------
// Function: createPort()
//-----------------------------------------------------------------------------
void VhdlParser::createPort(QString const& portDeclaration, QTextBlock const& portBlock)
{
    portExp_.indexIn(portDeclaration);
    QString portNames = portExp_.cap(1);
    General::Direction direction = General::str2Direction(portExp_.cap(2),General::DIRECTION_INVALID);
    QString portType = portExp_.cap(3);
    QString defaultValue = portExp_.cap(4).trimmed();
    QString description = portExp_.cap(5).trimmed();

    VhdlSyntax::TYPE_EXP.indexIn(portType);
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
        QSharedPointer<Port> port(new Port(name, direction, leftBound, rightBound, type, "", defaultValue, description));
        ports.append(port);
    } 

    portBlocks_.append(portBlock);
    portsMap_.insert(portBlock,ports);
}

//-----------------------------------------------------------------------------
// Function: createGeneric()
//-----------------------------------------------------------------------------
void VhdlParser::createGeneric(QString const& genericDeclaration, QTextBlock const& genericBlock)
{
    genericExp_.indexIn(genericDeclaration);
    QString genericNames = genericExp_.cap(1);

    QString type = genericExp_.cap(2);
    QString defaultValue = genericExp_.cap(3);
    QString description = genericExp_.cap(4).trimmed();

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
        generics_.append(parameter);
    } 

    genericBlocks_.append(genericBlock);
    genericsMap_.insert(genericBlock,generics);
}

//-----------------------------------------------------------------------------
// Function: insertExtraText()
//-----------------------------------------------------------------------------
void VhdlParser::insertExtraText(QString const& text, VhdlEntityHighlighter::BlockStyle style)
{
    QTextCursor cursor = textCursor();

    int prevPos = cursor.position();
    cursor.insertText(text);
    cursor.setPosition(prevPos);

    while( !cursor.atEnd() )
    {
        //QString blocktext = cursor.block().text();
        cursor.block().setUserState(style);
        cursor.movePosition(QTextCursor::NextBlock);
        cursor.movePosition(QTextCursor::EndOfBlock);
    }

    //QString blocktext = cursor.block().text();
    cursor.block().setUserState(style);
}

//-----------------------------------------------------------------------------
// Function: toggleBlock()
//-----------------------------------------------------------------------------
void VhdlParser::toggleBlock(QTextBlock& block)
{
    if ( portsMap_.contains(block) )
    {
        if ( block.userState() == VhdlEntityHighlighter::PORT_SELECTED )
        {
            block.setUserState(VhdlEntityHighlighter::PORT_NOT_SELECTED );
            foreach (QSharedPointer<Port> port, portsMap_.value(block) )
            {
                emit removePort(port);
            } 
        }
        else
        {
            block.setUserState(VhdlEntityHighlighter::PORT_SELECTED );
            foreach (QSharedPointer<Port> port, portsMap_.value(block) )
            {
                emit addPort(port);
            } 
        }
    }

    else if ( genericsMap_.contains(block) )
    {
        if ( block.userState() == VhdlEntityHighlighter::GENERIC_SELECTED )
        {
            block.setUserState(VhdlEntityHighlighter::GENERIC_NOT_SELECTED );
            foreach (QSharedPointer<ModelParameter> parameter, genericsMap_.value(block) )
            {
                emit removeGeneric(parameter);
            } 
        }

        else
        {
            block.setUserState(VhdlEntityHighlighter::GENERIC_SELECTED );
            foreach (QSharedPointer<ModelParameter> parameter, genericsMap_.value(block) )
            {
                emit addGeneric(parameter);
            } 
        }
    }
}

//-----------------------------------------------------------------------------
// Function: valueForString()
//----------------------------------------------------------------------------- 
int VhdlParser::valueForString(QString& string) const  
{
    bool isNumber = true;
    int value = string.toInt(&isNumber);
    if ( !isNumber )
    {
        foreach ( QSharedPointer<ModelParameter> parameter, generics_ )
        {
            if ( parameter->getName() == string )
            {
                int genericValue = parameter->getValue().toInt(&isNumber);
                if ( !isNumber )
                {
                    return -1;
                }

                return genericValue;
            }
        }

        return -1;
    }

    return value;
}