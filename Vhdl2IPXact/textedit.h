//-----------------------------------------------------------------------------
// File: textedit.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Timo Korpela
// Date: 24.05.2013
//
// Description:
// header file introducing textedit class
//-----------------------------------------------------------------------------
#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QTextEdit>
#include <QMouseEvent>


//-----------------------------------------------------------------------------
//! Class for text display. based to QTextEdit
//-----------------------------------------------------------------------------
class textEdit : public QTextEdit
{
    Q_OBJECT
public:

    /*!
    *   standard constructor and deconstructor
    *   only addition: make text read only
    */
    textEdit( QWidget *parent = 0);
    ~textEdit();
signals:

    /*!
    *   signal to get function call in main class
    */
    void selectedWord (const QString &);

protected:

    /*!
     * Custom mouseDoubleClickEvent handler
     * \param e QMouseEventfrom position of doubleclick
     */
    virtual void mouseDoubleClickEvent(QMouseEvent *e);

};

#endif // TEXTEDIT_H
