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

#ifndef CONFIGURATION_DIALOG_H
#define CONFIGURATION_DIALOG_H

#include "Common.h"
#include <QDialog>

class QPushButton;
class QTreeWidgetItem;

namespace GCF
{
    struct ConfigurationDialogData;
    class GCF_EXPORT_CLASS ConfigurationDialog : public QDialog
    {
        Q_OBJECT

    public:
        ConfigurationDialog(QWidget* parent=0);
        ~ConfigurationDialog();

        void addPage(QWidget* child);

        QWidget* pageContainer() const;
        QPushButton* okButton() const;
        QPushButton* cancelButton() const;

    protected:
        void showEvent(QShowEvent* se);

    protected slots:
        void on_lstComponents_currentRowChanged(int index);

    protected:
        ConfigurationDialogData* d;
    };

    struct ComponentPropertyEditorPageData;
    class GCF_EXPORT_CLASS ComponentPropertyEditorPage : public QWidget
    {
        Q_OBJECT

    public:
        ComponentPropertyEditorPage(QWidget* parent=0);
        ~ComponentPropertyEditorPage();

    public slots:
        void loadProperties();
        void saveProperties();

    protected slots:
        void on_componentTree_itemClicked(QTreeWidgetItem* item);
        void on_cmdAddProperty_clicked();
        void on_cmdRemoveProperty_clicked();
        void on_propertyEditor_itemChanged(QTreeWidgetItem* item, int col);

    private:
        ComponentPropertyEditorPageData* d;
    };

}
#endif
