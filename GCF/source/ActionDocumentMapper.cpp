/****************************************************************************
**
** Copyright (C) VCreate Logic Private Limited, Bangalore
**
** Use of this file is limited according to the terms specified by
** VCreate Logic Private Limited, Bangalore.  Details of those terms
** are listed in licence.txt included as part of the distribution package
** of this file. This file may not be distributed without including the
** licence.txt file.
**
** Contact info@vcreatelogic.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <GCF/ActionDocumentMapper>

#include <QUrl>
#include <QAction>
#include <QSignalMapper>
#include <QDesktopServices>

/**
\class ActionDocumentMapper QObject
\brief Maps a QAction to a Document
\internal

Each component has an internal object called "actionDocumentMapper".

If a component has QActions that are created with the "document" attribute;
then the component's "actionDocumentMapper" responds to the action's
triggered(bool) signal and launches the document using QDesktopServices

Note: This is an internal class.
*/

GCF::ActionDocumentMapper::ActionDocumentMapper(QObject* parent)
:QSignalMapper(parent)
{
    connect(this, SIGNAL(mapped(QString)), this, SLOT(slotMapped(QString)) );
}

GCF::ActionDocumentMapper::~ActionDocumentMapper()
{

}

void GCF::ActionDocumentMapper::mapAction(QAction* action, const QString& url)
{
    if(!action || url.isEmpty())
        return;

    this->setMapping(action, url);
}

void GCF::ActionDocumentMapper::slotMapped(const QString& url)
{
    QDesktopServices::openUrl( QUrl(url) );
}

