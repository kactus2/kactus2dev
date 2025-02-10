//-----------------------------------------------------------------------------
// File: DocumentFileAccess.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 09.02.2018
//
// Description:
// Reading and writing of IP-XACT documents using a standard file system.
//-----------------------------------------------------------------------------

#ifndef DOCUMENTFILEACCESS_H
#define DOCUMENTFILEACCESS_H

#include <KactusAPI/KactusAPIGlobal.h>

#include <QSharedPointer>
#include <QString>


class Document;
class MessageMediator;

//-----------------------------------------------------------------------------
//! Reading and writing of IP-XACT documents using a standard file system.
//-----------------------------------------------------------------------------
namespace DocumentFileAccess 
{
    //! Reads the document from given path.
    KACTUS2_API QSharedPointer<Document> readDocument(QString const& path);

    //! Writes to document model to the given path.
    KACTUS2_API bool writeDocument(QSharedPointer<Document> model, QString const& path);

};

#endif // DOCUMENTFILEACCESS_H
