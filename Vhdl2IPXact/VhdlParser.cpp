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

#include <models/modelparameter.h>
#include <models/port.h>
#include <vhdl2IPXact/VhdlEntityHighlighter.h>

//-----------------------------------------------------------------------------
// Function: VhdlParser()
//-----------------------------------------------------------------------------
VhdlParser::VhdlParser( QWidget *parent ) : QTextEdit( parent ), portsMap_(), genericsMap_(), 
    generics_(), genericUsage_(), highlighter_(), portBlocks_(), genericBlocks_(), 
    entityBegin_(),  entityEnd_(), portsBegin_(), portsEnd_(), portPattern_(), typePattern_(), 
    genericsBegin_(), genericsEnd_(), genericPattern_(), commentPattern_(), defaultPattern_()
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

    // Entity declaration is ENTITY <name> IS
    entityBegin_.setPattern("\\b(?:ENTITY)\\s+(\\w+)\\s+(?:IS)\\s*");
    entityBegin_.setCaseSensitivity(Qt::CaseInsensitive);

    // Entity end declaration is END [ENTITY] [<name>];
    entityEnd_.setPattern("\\b(END)\\s*(ENTITY)?\\s*(NAME)?\\s*[;]\\s*");
    entityEnd_.setCaseSensitivity(Qt::CaseInsensitive);

    portsBegin_.setPattern("\\b(PORT)\\s*[(]");
    portsBegin_.setCaseSensitivity(Qt::CaseInsensitive);

    portsEnd_.setPattern("\\s*[)]\\s*[;]\\s*(?=(?:[-][-][^\\n]*\\s*)*(END|BEGIN|GENERIC))\\s*");
    portsEnd_.setCaseSensitivity(Qt::CaseInsensitive);

    // Port declaration is <port_names> : <direction> <type> [<default>]; [--description]
    portPattern_.setPattern("\\s*((?:\\w+\\s*[,]?\\s*)+)\\s*[:]\\s*(\\w+)\\s+([^:;$]+)\\s*(?:[:][=]\\s*([^;-]+))?\\s*($|[;](?:(?:[ \\t])*(?:[-][-]\\s*([^\\n]*)))?|(?:(?:[ \\t])*(?:[-][-]\\s*([^\\n]*)))$)");

   // Type definition in port declaration is <typename>[(<left> downto <right>)].
    typePattern_.setPattern("(\\w+)\\s*(?:[(]\\s*(\\w+)\\s+\\w+\\s+(\\w+)[)])?\\s*");
  
    genericsBegin_.setPattern("\\b(GENERIC)\\s*[(]");
    genericsBegin_.setCaseSensitivity(Qt::CaseInsensitive);

    genericsEnd_.setPattern("\\s*[)]\\s*[;]\\s*(?=(?:[-][-][^\\n]*\\s*)*(END|BEGIN|PORT))\\s*");
    genericsEnd_.setCaseSensitivity(Qt::CaseInsensitive);

    // Generic declaration is <generic_names> : <type> [<default>];     
    genericPattern_.setPattern("\\s*((?:\\w+\\s*[,]?\\s*)+)\\s*[:]\\s*(\\w+)\\s*(?:[:][=]\\s*([^;-]+))?\\s*($|[;](?:(?:[ \\t])*(?:[-][-]\\s*([^\\n]*)))?|(?:(?:[ \\t])*(?:[-][-]\\s*([^\\n]*)))$)");

    // Description definition is a vhdl comment: --<description>.
    commentPattern_.setPattern("\\s*[-][-]([^\\n]*)");

    // Default definition is := <value>.
    defaultPattern_.setPattern("[:][=]\\s*(.*)\\s*");
}

//-----------------------------------------------------------------------------
// Function: ~VhdlParser()
//-----------------------------------------------------------------------------
VhdlParser::~VhdlParser()
{
}

//-----------------------------------------------------------------------------
// Function: readFile()
//-----------------------------------------------------------------------------
bool VhdlParser::readFile(QString absolutePath)
{
    QFile vhdlFile(absolutePath);
    if ( !vhdlFile.open(QIODevice::ReadOnly) )
    {
        QMessageBox::information(this, "Error", vhdlFile.errorString()+": "+absolutePath);
        return false;
    }

    removePorts();
    removeGenerics();
    setText(QString());
    createDocument(vhdlFile);    
    importGenerics();
    importPorts();
    
    return true;
}


//-----------------------------------------------------------------------------
// Function: modelParameterChanged()
//-----------------------------------------------------------------------------
void VhdlParser::editorChangedModelParameter(QString const& parameterName)
{
    foreach (QSharedPointer<ModelParameter> parameter, genericUsage_.keys() )
    {
        if ( parameter->getName() == parameterName )
        {
            foreach (QSharedPointer<Port> port, genericUsage_.value(parameter) )
            {
                assignGenerics(port);
            } 
        }
    } 
}

//-----------------------------------------------------------------------------
// Function: editorRemovedModelParameter()
//-----------------------------------------------------------------------------
void VhdlParser::editorRemovedModelParameter(QString const& parameterName)
{
    foreach ( QList<QSharedPointer<ModelParameter>> paramList, genericsMap_.values() )
    {
        foreach (QSharedPointer<ModelParameter> parameter, paramList )
        {
            if ( parameter->getName() == parameterName )
            {
                QTextBlock block = genericsMap_.key(paramList);
                block.setUserState(VhdlEntityHighlighter::GENERIC_NOT_SELECTED);
                highlighter_->rehighlight();
                return;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: editorRemovedPort()
//-----------------------------------------------------------------------------
void VhdlParser::editorRemovedPort(QString const& portName)
{
    foreach ( QList<QSharedPointer<Port>> paramList, portsMap_.values() )
    {
        foreach (QSharedPointer<Port> port, paramList )
        {
            if ( port->getName() == portName )
            {
                QTextBlock block = portsMap_.key(paramList);
                block.setUserState(VhdlEntityHighlighter::PORT_NOT_SELECTED);
                highlighter_->rehighlight();
                return;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void VhdlParser::mouseDoubleClickEvent(QMouseEvent *e) {

    QTextCursor cursor = cursorForPosition( e->pos() ); 

    toggleBlock(cursor.block());
    highlighter_->rehighlight();    
}

//-----------------------------------------------------------------------------
// Function: importPorts()
//-----------------------------------------------------------------------------
void VhdlParser::importPorts()
{
    foreach ( QList<QSharedPointer<Port>> list, portsMap_.values() )
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
    foreach ( QList<QSharedPointer<Port>> list, portsMap_.values() )
    {
        foreach ( QSharedPointer<Port> port, list )
        {
            emit removePort(port->getName());
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
    foreach ( QList<QSharedPointer<ModelParameter>> list, genericsMap_.values() )
    {
        foreach ( QSharedPointer<ModelParameter> parameter, list )
        {
            emit addGeneric(parameter);
        }
    }
}  

//-----------------------------------------------------------------------------
// Function: removeGenerics()
//-----------------------------------------------------------------------------
void VhdlParser::removeGenerics()
{

    foreach ( QList<QSharedPointer<ModelParameter>> list, genericsMap_.values() )
    {
        foreach ( QSharedPointer<ModelParameter> parameter, list )
        {
            emit removeGeneric(parameter->getName());
        }
    }
    genericsMap_.clear();
    generics_.clear();
    genericUsage_.clear();
}  

//-----------------------------------------------------------------------------
// Function: assignGenerics()
//-----------------------------------------------------------------------------
void VhdlParser::assignGenerics(QSharedPointer<Port> port)
{
    QTextBlock block = QTextBlock();
    foreach ( QList<QSharedPointer<Port>> portList, portsMap_.values() )
    {
        if ( portList.contains(port) ){
            block = portsMap_.key(portList); break;
        }
    }

    if( block.isValid() )
    {
        portPattern_.indexIn(block.text());
        QString portType = portPattern_.cap(3);
        typePattern_.indexIn(portType);
        QString leftBound = typePattern_.cap(2);
        QString rightBound = typePattern_.cap(3);
        QString defaultValue = portPattern_.cap(4).trimmed();


        foreach ( QSharedPointer<ModelParameter> parameter, generics_ )
        {
            if ( parameter->getName() == leftBound )
            {
                port->setLeftBound(genericToInt(parameter));
                QList<QSharedPointer<Port>> ports = genericUsage_.value(parameter);
                if ( !ports.contains(port) )
                {
                    ports.append(port);
                    genericUsage_.insert(parameter,ports);
                }   
            }

            if ( parameter->getName() == rightBound )
            {
                port->setRightBound(genericToInt(parameter));
                QList<QSharedPointer<Port>> ports = genericUsage_.value(parameter);
                if ( !ports.contains(port) )
                {
                    ports.append(port);
                    genericUsage_.insert(parameter,ports);
                }
            }

            if ( parameter->getName() == defaultValue )
            {
                port->setDefaultValue( parameter->getValue() );
                QList<QSharedPointer<Port>> ports = genericUsage_.value(parameter);
                if ( !ports.contains(port) )
                {
                    ports.append(port);
                    genericUsage_.insert(parameter,ports);
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

    const int entityBeginIndex = entityBegin_.indexIn(fileString);  
    QString entityName = entityBegin_.cap(1);
    QString pattern = entityEnd_.pattern();
    pattern.replace("NAME",entityName);
    entityEnd_.setPattern(pattern);
    if ( !checkEntityStructure(fileString) )
    {
        setText(fileString);
        return;
    }

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
            cursor.insertBlock();
            cursor.block().setUserState(VhdlEntityHighlighter::INSIDE_ENTITY);
            cursor.insertText(fileString.mid(index,entityBegin_.matchedLength()));
            index += entityBegin_.matchedLength();                       
        }

        else if ( genericBeginIndex == index )
        {            
            cursor.block().setUserState(VhdlEntityHighlighter::INSIDE_ENTITY);
            cursor.insertText(fileString.mid(index,genericsBegin_.matchedLength()));
        
            parseGenerics(fileString);

            int endIndex = genericsEnd_.indexIn(fileString,index);
            cursor.insertBlock();
            cursor.block().setUserState(VhdlEntityHighlighter::INSIDE_ENTITY);
            cursor.insertText(fileString.mid(endIndex,genericsEnd_.matchedLength()).remove("\n"));
            index = endIndex+genericsEnd_.matchedLength();
        } 
        
        else if ( portBeginIndex == index )
        {
            cursor.block().setUserState(VhdlEntityHighlighter::INSIDE_ENTITY);
            cursor.insertText(fileString.mid(index,portsBegin_.matchedLength()));
            
            parsePorts(fileString);
           
            int endIndex = portsEnd_.indexIn(fileString,index);
            cursor.insertBlock();
            cursor.block().setUserState(VhdlEntityHighlighter::INSIDE_ENTITY);
            QString portEndStr = fileString.mid(endIndex,portsEnd_.matchedLength());
            cursor.insertText(portEndStr.remove("\n"));
            index = endIndex+portsEnd_.matchedLength();
        }        
     
        else if ( entityEndIndex == index )
        {
            cursor.insertBlock();
            cursor.block().setUserState(VhdlEntityHighlighter::INSIDE_ENTITY);
            cursor.insertText(fileString.mid(index,entityEnd_.matchedLength()).remove("\n"));
            index += entityEnd_.matchedLength();
            
            cursor.insertBlock();
            cursor.block().setUserState(VhdlEntityHighlighter::OUTSIDE_ENTITY);
        }
        else
        {
            VhdlEntityHighlighter::BlockState prevState = static_cast<VhdlEntityHighlighter::BlockState>(cursor.block().userState());
            if ( entityBeginIndex < index && index < entityEndIndex )
            {
                prevState = VhdlEntityHighlighter::INSIDE_ENTITY;
            }
            cursor.block().setUserState(prevState); 

            indexes.append(index);
            qSort(indexes);

            if ( index != indexes.last() )
            {
                int nextChange = indexes.at(indexes.indexOf(index)+1);
                indexes.removeAll(index);
                cursor.insertText(fileString.mid(index,nextChange - index));
                index += nextChange - index;

            }
            else 
            {
                cursor.insertText(fileString.mid(index,fileString.size() - index));
                index += fileString.size() - index;                
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

    int portBeginIndex =  portsBegin_.indexIn(fileString);
    int portEndIndex = portsEnd_.indexIn(fileString, portBeginIndex);
    int portCount = fileString.count(portsBegin_);
    if ( portCount > 1 || portEndIndex < portBeginIndex )
    {       
        return false;
    }

    int genericBeginIndex =  genericsBegin_.indexIn(fileString);
    int genericEndIndex = genericsEnd_.indexIn(fileString, genericBeginIndex);
    int genericCount = fileString.count(genericsBegin_);
    if ( genericCount > 1 || genericEndIndex < genericBeginIndex )
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
    QString ports = parseSectionContent(portsBegin_,portsEnd_,fileString);

    QTextCursor cursor = textCursor();
    int index = 0;
    while ( index < ports.length() )
    {
        int portIndex = portPattern_.indexIn(ports,index);
        int commentIndex = commentPattern_.indexIn(ports,index);

        if ( commentIndex == index )
        {
            cursor.insertBlock();
            cursor.block().setUserState(VhdlEntityHighlighter::INSIDE_ENTITY);
            int commentLength = commentPattern_.matchedLength();
            QString comment = ports.mid(commentIndex,commentLength).remove("\n");
            cursor.insertText(comment);
           
            index = commentIndex + commentLength;

        }
        else if ( portIndex == index )
        {
            cursor.insertBlock();
            cursor.block().setUserState(VhdlEntityHighlighter::PORT_SELECTED);

            int length = portPattern_.matchedLength();
            QString portDeclaration = ports.mid(portIndex,length).remove("\n");
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

            foreach ( QString invalidLine,  ports.mid(index,skipLength).split("\n",QString::SkipEmptyParts) )
            {          
                cursor.insertBlock();
                cursor.block().setUserState(VhdlEntityHighlighter::INSIDE_ENTITY);  
                cursor.insertText(invalidLine);
            }

            index += skipLength;  
        } 
    }
}

//-----------------------------------------------------------------------------
// Function: parseGenerics()
//-----------------------------------------------------------------------------
void VhdlParser::parseGenerics(QString const& fileString)
{
    QString generics = parseSectionContent(genericsBegin_,genericsEnd_,fileString);
    //qDebug() << generics;

    QTextCursor cursor = textCursor();
    int index = 0;
    while ( index < generics.length() )
    {
        int genericIndex = genericPattern_.indexIn(generics,index);
        int commentIndex = commentPattern_.indexIn(generics,index);

        if ( commentIndex == index )
        {
            cursor.insertBlock();
            cursor.block().setUserState(VhdlEntityHighlighter::INSIDE_ENTITY);
            int commentLength = commentPattern_.matchedLength();
            QString comment = generics.mid(commentIndex,commentLength).remove("\n");
            cursor.insertText(comment);
            index = commentIndex + commentLength;      
        }

        else if ( genericIndex == index )
        {
            cursor.insertBlock();
            cursor.block().setUserState(VhdlEntityHighlighter::GENERIC_SELECTED);
            int length = genericPattern_.matchedLength();
            QString genericDeclaration = generics.mid(genericIndex,length).remove("\n");
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

            foreach ( QString invalidLine,  generics.mid(index,skipLength).split("\n",QString::SkipEmptyParts) )
            {          
                cursor.insertBlock();
                cursor.block().setUserState(VhdlEntityHighlighter::INSIDE_ENTITY);  
                cursor.insertText(invalidLine);
            }

            index += skipLength;  
        }
    }
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
    QString description = portPattern_.cap(5);

    typePattern_.indexIn(portType) ;
    QString type = typePattern_.cap(1);         
    QString left = typePattern_.cap(2);
    QString right = typePattern_.cap(3);

    bool isNumber = true;
    int leftBound = left.toInt(&isNumber);
    if ( left.isEmpty() )
    {
        leftBound = 0;
    }
    else if ( !isNumber )
    {
        leftBound = -1;
    }

    int rightBound = right.toInt(&isNumber);
    if ( right.isEmpty() )
    {
        rightBound = 0;
    }
    else if ( !isNumber )
    {
        rightBound = -1;
    }  

    commentPattern_.indexIn(description);
    description = commentPattern_.cap(1);

    // Port names are separated by a colon (,).
    QList<QSharedPointer<Port>> ports;
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
    genericPattern_.indexIn(genericDeclaration);
    QString genericNames = genericPattern_.cap(1);

    QString type = genericPattern_.cap(2);
    QString defaultValue = genericPattern_.cap(3);
    QString description = genericPattern_.cap(4);

    commentPattern_.indexIn(description);
    description = commentPattern_.cap(1);

    QRegExp nameDelimiter("\\s*[,]\\s*");
    QList<QSharedPointer<ModelParameter>> generics;
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
                emit removePort(port->getName());
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
                emit removeGeneric(parameter->getName());
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
// Function: genericToInt()
//-----------------------------------------------------------------------------
int VhdlParser::genericToInt(QSharedPointer<ModelParameter> parameter)
{
    bool isNumber = true;
    int value = parameter->getValue().toInt(&isNumber);
    if ( !isNumber )
    {
        return -1;
    }

    return value;
}