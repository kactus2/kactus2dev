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

class WriteChannel;

class ConsoleEditor : public QPlainTextEdit
{
    Q_OBJECT
public:

    //! The constructor.
    ConsoleEditor(WriteChannel* outputChannel, QWidget* parent = nullptr);
    
    //! The destructor.
    virtual ~ConsoleEditor() = default;

public slots:

    void print(QString const& input);


    void printError(QString const& input);
protected:

    virtual void keyPressEvent(QKeyEvent *e) override;

private:

    int lockedLines_;

    QString promptText_;

    WriteChannel* outputChannel_;

    QString fontFamily_;
};


#endif // CONSOLEEDITOR_H
