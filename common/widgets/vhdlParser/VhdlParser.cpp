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
#include <QTextEdit>
#include <QMessageBox>
#include <QTextStream>
#include <QScrollBar>
#include <QRegExp>
#include <QList>
#include <QTextBlockFormat>
#include <QApplication>
#include <QAbstractTextDocumentLayout>

#include <QDebug>
#include <models/modelparameter.h>
#include <models/port.h>
#include <common/widgets/vhdlParser/VhdlEntityHighlighter.h>

//-----------------------------------------------------------------------------
// Function: VhdlParser()
//-----------------------------------------------------------------------------
VhdlParser::VhdlParser(QWidget*parent) : QTextEdit( parent ), 
    portsMap_(), 
    genericsMap_(), 
    generics_(), 
    genericsInPorts_(), 
    genericsInGenerics_(), 
    highlighter_(),
    portBlocks_(), 
    genericBlocks_(), 
    entityBegin_(),  
    entityEnd_(), 
    portsBegin_(),
    portsEnd_(),
    portPattern_(),
    typePattern_(), 
    genericsBegin_(), 
    genericsEnd_(), 
    genericExp_(),
    commentExp_(),
    defaultPattern_(),
    equationPattern_(),
    newline_()
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

    highlighter_ = new VhdlEntityHighlighter(document());

    QString newline = "(\\r\\n?|\\n\\r?)";
    QString space = "[ \\t]*";    
    QString comment = "[-][-]"+space+"([^\\r\\n]*)";

    // Entity declaration is ENTITY <name> IS
    entityBegin_.setPattern(space+"(?:ENTITY)\\s+(\\w+)\\s+(?:IS)");
    entityBegin_.setCaseSensitivity(Qt::CaseInsensitive);

    // Entity end declaration is END [ENTITY] [<name>];
    entityEnd_.setPattern(space+"(END)\\s*(ENTITY)?\\s*(NAME)?\\s*[;]");
    entityEnd_.setCaseSensitivity(Qt::CaseInsensitive);

    portsBegin_.setPattern(space+"(PORT)\\s*[(]");
    portsBegin_.setCaseSensitivity(Qt::CaseInsensitive);

    portsEnd_.setPattern(space+"[)]\\s*[;](?=\\s*(?:"+comment+"*\\s*)*(END|BEGIN|GENERIC))");
    portsEnd_.setCaseSensitivity(Qt::CaseInsensitive);

    QString names = "\\w+\\s*[,]?\\s*";
    QString operations = "[+-/*/]|[/*][/*]";
    QString bound = "(?:\\w+)(?:\\s*(?:"+operations+")\\s*(?:\\w+))*";
    QString portTypeExp = "(?:\\w+)\\s*(?:[(]\\s*(?:"+bound+")\\s+\\w+\\s+(?:"+bound+")\\s*[)])?";
    QString genericTypeExp = "\\w+";
    QString defaultExp = "[:][=]\\s*((?:"+bound+")|[^\\s:;]+)";

    // Port declaration is <port_names> : <direction> <type> [<default>]; [--description]
    portPattern_.setPattern(space+"("+names+")+\\s*[:]\\s*(\\w+)\\s+("+portTypeExp+")\\s*(?:"+defaultExp+")?(?:\\s*[;]"+space+"(?:"+comment+")?(?:"+newline+")?|"+space+"(?:"+comment+")?(?:"+newline+")?(?=(?:\\s*(?:"+comment+")*\\s*)*$))");

   // Type definition in port declaration is <typename>[(<left> downto <right>)].
    typePattern_.setPattern(portTypeExp.replace("(?:","("));
  
    genericsBegin_.setPattern(space+"(GENERIC)\\s*[(]");
    genericsBegin_.setCaseSensitivity(Qt::CaseInsensitive);

    genericsEnd_.setPattern(space+"[)]\\s*[;](?=(?:\\s*("+comment+")\\s*)*(END|BEGIN|PORT))*");
    genericsEnd_.setCaseSensitivity(Qt::CaseInsensitive);

    // Generic declaration is <generic_names> : <type> [<default>]; [--description]    
    genericExp_.setPattern(space+"("+names+")\\s*[:]\\s*("+genericTypeExp+")\\s*(?:"+defaultExp+")?\\s*(?:[;]"+space+"(?:"+comment+")?(?:"+newline+")?|(?:"+comment+")?(?:"+newline+")?(?=(?:\\s*(?:"+comment+")*\\s*)*$))");

    // Description definition is a vhdl comment: --<description>.
    commentExp_.setPattern(space+comment+newline);

    // Default definition is := <value>.
    defaultPattern_.setPattern(defaultExp);

    equationPattern_.setPattern(bound.replace("(?:","("));

    newline_.setPattern(newline);
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
        cursor.setPosition(entityBegin_.indexIn(toPlainText()));
        int rowNumber = cursor.block().firstLineNumber();
        int rowHeight = fontMetrics().height();
        verticalScrollBar()->setSliderPosition(rowHeight*rowNumber);
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
    
    setText(QString());
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
        portPattern_.indexIn(block.text());
        QString portType = portPattern_.cap(3);
        QString defaultValue = portPattern_.cap(4).trimmed();        

        typePattern_.indexIn(portType);
        QString leftBound = typePattern_.cap(1);
        QString bounded = typePattern_.cap(2);
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
        setText(fileString);
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
            cursor.block().setUserState(VhdlEntityHighlighter::INSIDE_ENTITY);
            cursor.insertText(fileString.mid(index,entityBegin_.matchedLength()).remove(newline_));
            index += entityBegin_.matchedLength();                       
        }

        else if ( genericBeginIndex == index )
        {     
            cursor.block().setUserState(VhdlEntityHighlighter::INSIDE_ENTITY);         
            cursor.insertText(fileString.mid(index,genericsBegin_.matchedLength()).remove(newline_));
        
            parseGenerics(fileString);

            int endIndex = genericsEnd_.indexIn(fileString,index);
            cursor.insertBlock();
            cursor.block().setUserState(VhdlEntityHighlighter::INSIDE_ENTITY);
            cursor.insertText(fileString.mid(endIndex,genericsEnd_.matchedLength()).remove(newline_));
            index = endIndex+genericsEnd_.matchedLength();           
        } 
        
        else if ( portBeginIndex == index )
        {
            cursor.block().setUserState(VhdlEntityHighlighter::INSIDE_ENTITY);
            cursor.insertText(fileString.mid(index,portsBegin_.matchedLength()).remove(newline_));
            
            parsePorts(fileString);
           
            int endIndex = portsEnd_.indexIn(fileString,index);
            cursor.insertBlock();
            cursor.block().setUserState(VhdlEntityHighlighter::INSIDE_ENTITY);
            QString portEndStr = fileString.mid(endIndex,portsEnd_.matchedLength()).remove(newline_);
            cursor.insertText(portEndStr);
            index = endIndex+portsEnd_.matchedLength();
        }        
     
        else if ( entityEndIndex == index )
        {
            cursor.block().setUserState(VhdlEntityHighlighter::INSIDE_ENTITY);
            QString entityEnd = fileString.mid(index,entityEnd_.matchedLength());
            cursor.insertText(entityEnd);
            index += entityEnd_.matchedLength();
        }
        else
        {
           
            VhdlEntityHighlighter::BlockState prevState = VhdlEntityHighlighter::OUTSIDE_ENTITY;
            if ( entityBeginIndex < index && index < entityEndIndex )
            {
                prevState = VhdlEntityHighlighter::INSIDE_ENTITY;
            }
           
            indexes.append(index);
            qSort(indexes);

            if ( index != indexes.last() )
            {
                int nextChange = indexes.at(indexes.indexOf(index)+1);
                indexes.removeAll(index);

                int skip = nextChange - index;              
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
bool VhdlParser::checkEntityStructure(QString const& fileString)
{
    if( fileString.count(entityBegin_) != 1 || fileString.count(entityEnd_) == 0 )
    {
        return false;
    }

    QString entity = parseSection(entityBegin_, entityEnd_, fileString);
    int portBeginIndex =  portsBegin_.indexIn(entity);
    int portEndIndex = portsEnd_.indexIn(entity, portBeginIndex);
    // int portCount = entity.count(portsBegin_);
    if ( portEndIndex < portBeginIndex )
    {       
       return false;
    }

    int genericBeginIndex =  genericsBegin_.indexIn(entity);
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
    entityEnd_.setPattern("\\b(END)\\s*(ENTITY)?\\s*(NAME)?\\s*[;]\\s*");
    entityBegin_.indexIn(fileString);
    QString entityName = entityBegin_.cap(1);
    QString pattern = entityEnd_.pattern();
    pattern.replace("NAME",entityName);
    entityEnd_.setPattern(pattern);
}

//-----------------------------------------------------------------------------
// Function: parseSectionContent()
//-----------------------------------------------------------------------------
QString VhdlParser::parseSection(QRegExp const& begin, QRegExp const& end, 
    QString const& text)
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
    QString const& text)
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
    if ( newline_.indexIn(ports) == 0 )
    {
        index = newline_.matchedLength();
    }
    while ( index < ports.length() )
    {
        int portIndex = portPattern_.indexIn(ports,index);
        int commentIndex = commentExp_.indexIn(ports,index);

        if ( commentIndex == index )
        {
            cursor.insertBlock();
            cursor.block().setUserState(VhdlEntityHighlighter::INSIDE_ENTITY);
            int commentLength = commentExp_.matchedLength();
            QString comment = ports.mid(commentIndex,commentLength).remove(newline_);
            cursor.insertText(comment);
           
            index = commentIndex + commentLength;

        }
        else if ( portIndex == index )
        {
            cursor.insertBlock();
            cursor.block().setUserState(VhdlEntityHighlighter::PORT_SELECTED);

            int length = portPattern_.matchedLength();
            QString portDeclaration = ports.mid(portIndex,length).remove(newline_);
            cursor.insertText(portDeclaration);
            index = portIndex + length;

            createPort(portDeclaration, cursor.block());       
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
    if ( newline_.indexIn(generics) == 0 )
    {
        index = newline_.matchedLength();
    }
    while ( index < generics.length() )
    {
        int genericIndex = genericExp_.indexIn(generics,index);
        int commentIndex = commentExp_.indexIn(generics,index);

        if ( commentIndex == index )
        {
            cursor.insertBlock();
            cursor.block().setUserState(VhdlEntityHighlighter::INSIDE_ENTITY);
            int commentLength = commentExp_.matchedLength();
            QString comment = generics.mid(commentIndex,commentLength).remove(newline_);
            cursor.insertText(comment);
            index = commentIndex + commentLength;      
        }

        else if ( genericIndex == index )
        {
            cursor.insertBlock();
            cursor.block().setUserState(VhdlEntityHighlighter::GENERIC_SELECTED);
            int length = genericExp_.matchedLength();
            QString genericDeclaration = generics.mid(genericIndex,length).remove(newline_);
            cursor.insertText(genericDeclaration);
            index = genericIndex + length;             

            createGeneric(genericDeclaration, cursor.block());        
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
void VhdlParser::parseBounds(QString& rangeDeclaration, int& leftBound, int& rightBound)
{
    QRegExp rangeExp = QRegExp("[(]\\s*("+equationPattern_.pattern()+")\\s+\\w+\\s+("+equationPattern_.pattern()+")\\s*[)]");

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
int VhdlParser::parseEquation(QString& equation)
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

    QRegExp multiplyDivide = QRegExp("[/*/]");
    QRegExp power = QRegExp("[/*][/*]");
    QRegExp extraOperand = QRegExp("([+-/*/]|[/*][/*])\\s*(\\w+)");

    equationPattern_.indexIn(equation);  
    QString value = equationPattern_.cap(1);
    QString increments = equationPattern_.cap(2);
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
    portPattern_.indexIn(portDeclaration);
    QString portNames = portPattern_.cap(1);
    General::Direction direction = General::str2Direction(portPattern_.cap(2),General::DIRECTION_INVALID);
    QString portType = portPattern_.cap(3);
    QString defaultValue = portPattern_.cap(4).trimmed();
    QString description = portPattern_.cap(5).trimmed();

    typePattern_.indexIn(portType);
    QString type = typePattern_.cap(1); 
    QString bounded = typePattern_.cap(2);

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
void VhdlParser::insertExtraText(QString& text, VhdlEntityHighlighter::BlockState state)
{
    QTextCursor cursor = textCursor();

    int prevPos = cursor.position();
    cursor.insertText(text);
    cursor.setPosition(prevPos);
    if ( cursor.block().userState() != VhdlEntityHighlighter::OUTSIDE_ENTITY )
    {
        cursor.movePosition(QTextCursor::NextBlock);
    }

    while( !cursor.atEnd() )
    {
        QString blocktext = cursor.block().text();
        cursor.block().setUserState(state);
        cursor.movePosition(QTextCursor::NextBlock);
        cursor.movePosition(QTextCursor::EndOfBlock);
    }

    QString blocktext = cursor.block().text();
    cursor.block().setUserState(state);
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
int VhdlParser::valueForString(QString& string)   
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