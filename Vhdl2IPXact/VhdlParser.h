//-----------------------------------------------------------------------------
// File: VhdlParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.06.2013
//
// Description:
// TODO:
//-----------------------------------------------------------------------------
#ifndef VhdlParser_H
#define VhdlParser_H

#include <QTextEdit>
#include <QMouseEvent>

class ModelParameter;
class Port;

//-----------------------------------------------------------------------------
//! Class VhdlParser.
//-----------------------------------------------------------------------------
class VhdlParser : public QTextEdit
{
    Q_OBJECT
public:


    VhdlParser(QWidget* parent = 0);

    ~VhdlParser();

    bool readFile(QString absolutePath);

signals:

    /*!
    *   signal to get function call in main class
    */
    void selectedWord (const QString &);

    void addPort(const Port& port);
    void removePort(const QString& portName);

    void addGeneric();
    void removeGeneric();

protected:
 
    virtual void mouseDoubleClickEvent(QMouseEvent *e);

private:

    //! No copying.
    VhdlParser(const VhdlParser&);

    //! No assignment.
    VhdlParser& operator=(const VhdlParser&);

    

    bool parsePorts(QString& entity);

    bool parseGenerics(QString& entity);

   QString parseSection(QRegExp const& begin, QRegExp const& end, QString& text);

    QList<Port> ports_;
};

#endif // VhdlParser_H
