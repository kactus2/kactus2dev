//-----------------------------------------------------------------------------
// File: ConsoleEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.10.2019
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef CONSOLEEDITOR_H
#define CONSOLEEDITOR_H

#include <QPlainTextEdit>

class ConsoleEditor : public QPlainTextEdit
{
    Q_OBJECT
public:

    //! The constructor.
    ConsoleEditor(QWidget* parent = nullptr);
    
    //! The destructor.
    virtual ~ConsoleEditor() = default;

    void print(QString const& input);

    void printError(QString const& input);

protected:

    virtual void keyPressEvent(QKeyEvent *e) override;

signals:
    void entered(QString const& line);

private:

    int lockedLines_;

    QString promptText_;
};


#endif // CONSOLEEDITOR_H
