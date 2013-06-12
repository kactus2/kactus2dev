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

bool VhdlParser::parsePorts(QString& entity){

    QRegExp portsBegin("\\b(PORT)\\s*[(]", Qt::CaseInsensitive);
    QRegExp portsEnd("\\b(END|BEGIN|GENERIC)\\b", Qt::CaseInsensitive);

    QString ports = parseSection(portsBegin,portsEnd,entity);

    if ( ports.size() == 0 )
    {
        qDebug() << entity;
        return false;
    }

    int left = ports.indexOf(portsBegin);
    int leftBound = portsBegin.matchedLength();
    ports = ports.mid(leftBound);
    ports = ports.left(ports.lastIndexOf(");")).trimmed();

    ports_.clear();
    // Port declaration is <port_name> : <direction> <type> [<default>]; [<description>].
    QRegExp portPattern("\\s*(\\w+)\\s*[:]\\s*(\\w+)\\s+(\\w+(?:[(].*[)])?)\\s*([:][=]\\s*(?:.*))?\\s*[;]?\\s*([-][-].*)?");

    // Type definition is <typename>[(<bounds>)]
    QRegExp typePattern("(\\w+)\\s*(?:[(]\\s*(\\d+)\\s+\\w+\\s+(\\d+)[)])?\\s*");

    // Default definition is := <value>
    QRegExp defaultPattern("[:][=]\\s*[\"|']?(\\w+)[\"|']?");

    // Description definition is a comment at end of line: --<description>
    QRegExp commentPattern("[-][-]\\s*((?:\\w+\\s*)*)");

    // End of port declaration is ; not followed by a comment on same line
    QRegExp declarationEnd("([;](?!(\\s*[-][-]))|[;]{0}\\s*[-][-]*\\n|\\n)");
    foreach ( QString portDeclaration, ports.split(declarationEnd,QString::SkipEmptyParts) )
    {
    
        //qDebug() << portDeclaration;
        int pos = portPattern.indexIn(portDeclaration);
        if ( pos == -1 )
        {
            QMessageBox::information(this,"Import warning","Invalid port declaration found. Cannot import ports.",
                QMessageBox::Ok );
            return false;
        }    

        QString portName = portPattern.cap(1);        
        General::Direction direction = General::str2Direction(portPattern.cap(2),General::DIRECTION_INVALID);
        QString portType = portPattern.cap(3);
        QString typeDefinition = portPattern.cap(3);
        QString defaultValue = portPattern.cap(4);
        QString description = portPattern.cap(5);

        int rightBound = 0;
        int leftBound = 0;
        if ( typePattern.indexIn(portType) != -1 )
        {
            portType = typePattern.cap(1);
            rightBound = typePattern.cap(2).toInt();
            leftBound = typePattern.cap(3).toInt();
        }    

        if ( defaultPattern.indexIn(defaultValue) != -1 )
        {
            defaultValue = defaultPattern.cap(1);
        }
 
        if ( commentPattern.indexIn(description) != -1 )
{
        description = commentPattern.cap(1);
}
  
        //qDebug() << portName << "," << General::direction2Str(direction) << "," << portType << "," << typeDefinition << ":=" << defaultValue << "," << description;       
        ports_.append(Port(portName,direction,leftBound,rightBound,portType,typeDefinition,defaultValue,description));
        emit addPort(ports_.back());
    }

    return true;
}

bool VhdlParser::parseGenerics(QString& entity)
{
    QRegExp genericsBegin("\\b(GENERIC)\\s*[(]", Qt::CaseInsensitive);
    QRegExp genericsEnd("[)][;]", Qt::CaseInsensitive);
    QString generics = parseSection(genericsBegin,genericsEnd,entity);

    if ( generics.size() == 0 )
    {
        return false;
    }

    return true;
}

QString VhdlParser::parseSection(QRegExp const& begin, QRegExp const& end, QString& text)
{
    if ( !text.contains( begin )  )
    {
        return QString();
    }
    
    int leftBound = text.indexOf(begin);
    int rightBound = end.indexIn(text,leftBound);

    if ( rightBound == -1 )
    {
        return QString();
    }

    rightBound +=  end.matchedLength();    
    QString section = text.mid(leftBound,rightBound-leftBound);
    return section;
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
