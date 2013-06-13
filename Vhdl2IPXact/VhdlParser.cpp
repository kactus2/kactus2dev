//-----------------------------------------------------------------------------
// File: VhdlParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.06.2013
//
// Description:
// QVhdlParser with custom functionality
//-----------------------------------------------------------------------------
#include "VhdlParser.h"

#include <QFile>
#include <QTextCursor>
#include <QTextEdit>
#include <QMessageBox>
#include <QTextStream>
#include <QScrollBar>
#include <QRegExp>
#include <QDebug>

#include <models/modelparameter.h>
#include <models/port.h>

//-----------------------------------------------------------------------------
// Function: VhdlParser()
//-----------------------------------------------------------------------------
VhdlParser::VhdlParser( QWidget *parent ) : QTextEdit( parent ), ports_()
{
    setReadOnly( true );
    setCursorWidth(0);
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

    foreach ( Port port, ports_ )
    {
        emit removePort(port.getName());
    }
    ports_.clear();


    QFile vhdlFile(absolutePath);
    if ( !vhdlFile.open(QIODevice::ReadOnly) )
    {
        QMessageBox::information(this, "error", vhdlFile.errorString()+": "+absolutePath);
        return false;
    }
 
    QTextStream stream(&vhdlFile);
    QString fileString = stream.readAll(); 
    vhdlFile.close();

    // Entity declaration is ENTITY <name> IS
    QRegExp entityBegin("\\b(ENTITY)\\s+(\\w+)\\s+(IS)\\b", Qt::CaseInsensitive);

    // Entity end declaration is END [ENTITY] [<name>];
    QRegExp entityEnd("\\b(END)\\s*(ENTITY)?\\s*(\\w*)\\s*[;]", Qt::CaseInsensitive);
    QString entity =  parseSection(entityBegin, entityEnd, fileString);

    if ( entity.size() == 0 )
    {   
        QMessageBox::information(this,"Import warning","No entity found in given file.", QMessageBox::Ok );
        return false;
    }

    //TODO: Subclass QSyntaxHighlighter for entity highlight
    setText(fileString);

    //verticalScrollBar()->setValue(fontMetrics().height()*2);
    
    parsePorts(entity);

    parseGenerics(entity);

    return true;
}

//-----------------------------------------------------------------------------
// Function: parsePorts()
//-----------------------------------------------------------------------------
bool VhdlParser::parsePorts(QString& entity)
{
    QRegExp portsBegin("\\b(PORT)\\s*[(](\\s|\\n)*", Qt::CaseInsensitive);
    QRegExp portsEnd("\\s*[)]\\s*[;](?:\\s*\\n)*(END|BEGIN|GENERIC)\\b", Qt::CaseInsensitive);
    QString ports = parseSectionContent(portsBegin,portsEnd,entity);
    if ( ports.size() == 0 )
    {
        qDebug() << entity;
        return false;
    }

    qDebug() << ports;
    // Port declaration is <port_names> : <direction> <type> [<default>]; [<description>].
    QRegExp portPattern("\\s*((?:\\w+\\s*[,]?\\s*)+)\\s*[:]\\s*(\\w+)\\s+(\\w+(?:[(].*[)])?)\\s*([:][=]\\s*(?:.*))?\\s*[;]?\\s*([-][-].*)?");

    // Type definition is <typename>[(<bounds>)].
    QRegExp typePattern("(\\w+)\\s*(?:[(]\\s*(\\d+)\\s+\\w+\\s+(\\d+)[)])?\\s*");

    // Default definition is := <value>.
    QRegExp defaultPattern("[:][=]\\s*([\"'])?(\\w+)\1");

    // Description definition is a comment at end of line: --<description>.
    QRegExp commentPattern("[-][-]\\s*((?:\\w+\\s*)*)");

    // End of port declaration is ; not followed by a comment on the same line.
    // 1. Semicolon (;) not followed by a comment on the same line.
    // 3. Line end.
    QRegExp declarationEnd("(?:[;](?!(?:\\s*[-][-]))|\\s*\\n)"); //[;]{0}\\s*[-][-]*\\n|     \\s*\\n+\\s*|
    foreach ( QString portDeclaration, ports.split(declarationEnd,QString::SkipEmptyParts) )
    {

        qDebug() << portDeclaration;
        int index = portDeclaration.indexOf(QRegExp("\\s*[-][-]") );

        if (index == 0){
            continue;
        }

        if ( portPattern.indexIn(portDeclaration) == -1 )
        {
            QMessageBox::information(this,"Import warning","Invalid port declaration found. Cannot import ports.",
                QMessageBox::Ok );
            return false;
        }    

        QString portNames = portPattern.cap(1);
        General::Direction direction = General::str2Direction(portPattern.cap(2),General::DIRECTION_INVALID);
        QString portType = portPattern.cap(3);
        QString defaultValue = portPattern.cap(4);
        QString description = portPattern.cap(5);

        typePattern.indexIn(portType) ;
        QString type = typePattern.cap(1);
        int leftBound = typePattern.cap(2).toInt();
        int rightBound = typePattern.cap(3).toInt();

        defaultPattern.indexIn(defaultValue);
        defaultValue = defaultPattern.cap(1);

        commentPattern.indexIn(description); 
        description = commentPattern.cap(1);

        //qDebug() << portName << "," << General::direction2Str(direction) << "," << portType << "," << typeDefinition << ":=" << defaultValue << "," << description; 
        // Port names are separated by a colon (,).
        foreach( QString name, portNames.split(QRegExp("\\s*[,]\\s*"), QString::SkipEmptyParts) )
        {      
            ports_.append(Port(name, direction, leftBound, rightBound, portType, type, defaultValue, description));
            emit addPort(ports_.back());
        } 
    }

    return true;
}

bool VhdlParser::parseGenerics(QString& entity)
{
    QRegExp genericsBegin("\\b(GENERIC)\\s*[(]", Qt::CaseInsensitive);
    QRegExp genericsEnd("[)]\\s*[;]", Qt::CaseInsensitive);
    QString generics = parseSectionContent(genericsBegin,genericsEnd,entity);

    if ( generics.size() == 0 )
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: parseSection()
//-----------------------------------------------------------------------------
QString VhdlParser::parseSection(QRegExp const& begin, QRegExp const& end, QString& text)
{   
    int leftBound = begin.indexIn(text);
    int rightBound = end.indexIn(text,leftBound);

    if ( leftBound == -1 || rightBound == -1 )
    {
        return QString();
    }

    rightBound += end.matchedLength();      
    //qDebug() << text.mid(leftBound,rightBound-leftBound);;
    return text.mid(leftBound,rightBound-leftBound);
}

//-----------------------------------------------------------------------------
// Function: parseSectionContent()
//-----------------------------------------------------------------------------
QString VhdlParser::parseSectionContent(QRegExp const& begin, QRegExp const& end, QString& text)
{   
    int leftBound = begin.indexIn(text);
    int rightBound = end.indexIn(text,leftBound);

    if ( leftBound == -1 || rightBound == -1 )
    {
        return QString();
    }

    leftBound += begin.matchedLength();
    //qDebug() << text.mid(leftBound,rightBound-leftBound);;
    return text.mid(leftBound,rightBound-leftBound);
}

//-----------------------------------------------------------------------------
// Function: mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void VhdlParser::mouseDoubleClickEvent(QMouseEvent *e) {

    QTextCursor textc = cursorForPosition( e->pos() );
    textc.select(QTextCursor::LineUnderCursor );
    QString worduc = textc.selectedText();
    if ( !worduc.isEmpty() ) {
        emit selectedWord( worduc );
    }
}
