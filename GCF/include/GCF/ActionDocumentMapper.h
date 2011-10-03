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

#ifndef ACTION_DOCUMENT_MAPPER_H
#define ACTION_DOCUMENT_MAPPER_H

#include <GCF/Common>
#include <QSignalMapper>

class QAction;
namespace GCF
{
    class GCF_EXPORT_CLASS ActionDocumentMapper : public QSignalMapper
    {
        Q_OBJECT

    public:
        ActionDocumentMapper(QObject* parent=0);
        ~ActionDocumentMapper();

        void mapAction(QAction* action, const QString& url);

    protected slots:
        void slotMapped(const QString& url);
    };
}

#endif
