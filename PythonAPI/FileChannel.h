//-----------------------------------------------------------------------------
// File: FileChannel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.02.2021
//
// Description:
// Writable output channel for files e.g stdout.
//-----------------------------------------------------------------------------

#ifndef FILECHANNEL_H
#define FILECHANNEL_H

#include <PythonAPI/WriteChannel.h>

#include <QObject>
#include <QString>
#include <QTextStream>

class FileChannel: public QObject, public WriteChannel
{
    Q_OBJECT
public:

    //! The constructor.
    FileChannel(FILE* fileHandle);
    
    //! The destructor.
    ~FileChannel() = default;

public slots:

    // Called when text is written into the file.
    virtual void write(QString const& text) override final;

private:

    QTextStream output_;
};


#endif // FILECHANNEL_H
