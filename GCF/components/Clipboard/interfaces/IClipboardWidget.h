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

#ifndef I_CLIPBOARD_WIDGET_H
#define I_CLIPBOARD_WIDGET_H

#include <GCF/IContainer.h>

class QMenu;
struct ClipboardData;

class IClipboard;
class IClipboardWidget : virtual public IContainer
{
public:
    virtual IClipboard* clipboard() const = 0;

    // must be implemented as signals
    virtual void dataClicked(const QString& context, const ClipboardData& data, int dataIndex) = 0;
    virtual void dataDoubleClicked(const QString& context, const ClipboardData& data, int dataIndex) = 0;
    virtual void dataRightClicked(const QString& context, const ClipboardData& data, int dataIndex, QMenu* menu) = 0;
};

Q_DECLARE_INTERFACE(IClipboardWidget, "com.vcreatelogic.IClipboardWidget/1.0");

#endif
