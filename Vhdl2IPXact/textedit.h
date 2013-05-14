#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QTextEdit>
#include <QMouseEvent>

class textEdit : public QTextEdit
{
    Q_OBJECT
public:

    //standard constructor and deconstructor
    //only addition: make text read only
    textEdit( QWidget *parent = 0);
    ~textEdit();
signals:

    //signal to get function call in main class
    void selectedWord (const QString &);
protected:

    //implemention of mouse event for textEdit
    //
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    //virtual void mousePressEvent(QMouseEvent *e);

};

#endif // TEXTEDIT_H
