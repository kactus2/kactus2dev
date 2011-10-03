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

#include "HelpSystemConfigurationPage.h"
#include "ui_HelpSystemConfigurationPage.h"
#include "HelpSystemComponent.h"
#include "HelpView.h"

#include <QFontDialog>
#include <QFileDialog>
#include <QHelpEngine>
#include <QMessageBox>

struct GCF::Components::HelpSystemConfigurationPageData
{
    HelpSystemConfigurationPageData():helpSystemComponent(0) {  }

    Ui::HelpSystemConfigurationPage ui;
    GCF::Components::HelpSystemComponent* helpSystemComponent;
    QStringList regDocs;
    QStringList unregDocs;
};

GCF::Components::HelpSystemConfigurationPage::HelpSystemConfigurationPage(QWidget* parent)
                                :QWidget(parent)
{
    d = new GCF::Components::HelpSystemConfigurationPageData;
    d->ui.setupUi(this);
}

GCF::Components::HelpSystemConfigurationPage::~HelpSystemConfigurationPage()
{
    delete d;
}

void GCF::Components::HelpSystemConfigurationPage::initFrom(GCF::Components::HelpSystemComponent* helpSystemComponent)
{
    if(!helpSystemComponent)
        return;

    d->helpSystemComponent = helpSystemComponent;
    GCF::Components::HelpView* helpView = helpSystemComponent->helpView();

    if(!helpView)
        return;

    d->ui.fontComboBox->setCurrentFont( QFont(helpView->font().family()) );
    d->ui.fontSize->setValue( helpView->currentFont().pointSize() );

    QStringList list = helpSystemComponent->registeredDocumentations();
    d->ui.listWidget->addItems(list);
}

void GCF::Components::HelpSystemConfigurationPage::saveInto(GCF::Components::HelpSystemComponent* helpSystemComponent)
{
    if(!helpSystemComponent)
        return;

    GCF::Components::HelpView* helpView = helpSystemComponent->helpView();
    if(!helpView)
        return;

    helpView->setFont( d->ui.fontComboBox->currentFont().family() );
    helpView->setFontSize( d->ui.fontSize->value() );

    if (d->unregDocs.count())
    {
        foreach (QString doc, d->unregDocs)
            d->helpSystemComponent->helpEngine()->unregisterDocumentation(doc);
    }

    if (d->regDocs.count() || d->unregDocs.count())
        d->helpSystemComponent->helpEngine()->setupData();
}

void GCF::Components::HelpSystemConfigurationPage::on_cmdRemove_clicked()
{
    QListWidgetItem *item = d->ui.listWidget->takeItem(d->ui.listWidget->currentRow());
    if (!item)
        return;

    d->unregDocs.append(item->text());
    delete item;

    if (d->ui.listWidget->count())
        d->ui.listWidget->setCurrentRow(0);
}

void GCF::Components::HelpSystemConfigurationPage::on_cmdAdd_clicked()
{
    if(!d->helpSystemComponent)
        return;

    const QStringList fileNames = QFileDialog::getOpenFileNames(this,
        tr("Add Documentation"), QString(), tr("Qt Compressed Help Files (*.qch)"));

    if (fileNames.isEmpty())
        return;

    foreach(const QString fileName, fileNames)
    {
        const QString ns = QHelpEngineCore::namespaceName(fileName);
        if (ns.isEmpty())
        {
            QMessageBox::warning(this, tr("Add Documentation"),
                tr("The specified file is not a valid Qt Help File!"));
            continue;
        }

        if (d->ui.listWidget->findItems(ns, Qt::MatchFixedString).count())
        {
            QMessageBox::warning(this, tr("Add Documentation"),
                tr("The namespace %1 is already registered!").arg(ns));
            continue;
        }

        d->helpSystemComponent->helpEngine()->registerDocumentation(fileName);
        d->ui.listWidget->addItem(ns);
        d->regDocs.append(ns);
    }
}

