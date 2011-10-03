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

#include <GCF/ConfigurationDialogComponent.h>
#include <GCF/ConfigurationDialog.h>
#include <GCF/ComponentFactory.h>

#include <QMainWindow>
#include <QPushButton>

#include <GCF/IConfigurationDialogPageProvider.h>

struct GCF::ConfigurationDialogComponentData
{
    ConfigurationDialogComponentData() : configDialog(0) { }

    GCF::ConfigurationDialog* configDialog;

    QMap<IConfigurationDialogPageProvider*, QWidget*> providerPageMap;
};

GCF_DEFINE_COMPONENT(GCF::ConfigurationDialogComponent)

GCF::ConfigurationDialogComponent::ConfigurationDialogComponent()
{
    d = new GCF::ConfigurationDialogComponentData;
}

GCF::ConfigurationDialogComponent::~ConfigurationDialogComponent()
{
    delete d;
}

QWidget* GCF::ConfigurationDialogComponent::fetchWidget(const QString& completeName) const
{
    QStringList comps = completeName.split('.');
    if(comps.last() == "configDialog")
    {
        if(d->configDialog)
            return d->configDialog;
        d->configDialog = new GCF::ConfigurationDialog(GCF::AbstractComponent::mainWindow());
        return d->configDialog;
    }

    if(comps.last() == "okButton")
        return d->configDialog->okButton();

    if(comps.last() == "cancelButton")
        return d->configDialog->cancelButton();

    return 0;
}

void GCF::ConfigurationDialogComponent::addChildWidget(QWidget* parent, QWidget* child, const QString& hint)
{
    if(parent == d->configDialog)
    {
        child->setWindowIcon(QIcon(":/GCF/configure.png"));
        d->configDialog->addPage(child);
    }
    Q_UNUSED(hint);
}

void GCF::ConfigurationDialogComponent::initializeComponent()
{
    setGuiXmlFileName(":/GCF/guixml/ConfigurationDialogComponent.xml");
}

void GCF::ConfigurationDialogComponent::finalizeComponent()
{
    // Do nothing.
}

void GCF::ConfigurationDialogComponent::creationAnnounced()
{
    // Search for all configuration page providers
    QStringList names;
    QList<QObject*> objects = findObjects("IConfigurationDialogPageProvider", names);
    for(int i=0; i<objects.count(); i++)
    {
        IConfigurationDialogPageProvider* cpProvider = qobject_cast<IConfigurationDialogPageProvider*>(objects[i]);
        if( !cpProvider )
            continue;

        if(d->providerPageMap.contains(cpProvider))
            continue;

        QString title = cpProvider->pageTitle();
        QIcon icon = cpProvider->pageIcon();
        QWidget* page = cpProvider->createPage( d->configDialog->pageContainer() );

        if( icon.isNull() )
            icon = QIcon(":/GCF/configure.png");
        page->setWindowTitle( title );
        page->setWindowIcon( icon );

        d->configDialog->addPage( page );
        d->providerPageMap[cpProvider] = page;
    }

    int result = d->configDialog->exec();

    if( result == QDialog::Accepted )
    {
        QMap<IConfigurationDialogPageProvider*, QWidget*>::iterator it = d->providerPageMap.begin();
        QMap<IConfigurationDialogPageProvider*, QWidget*>::iterator end = d->providerPageMap.end();
        while( it != end )
        {
            it.key()->savePage( it.value() );
            ++it;
        }
    }

    d->providerPageMap.clear();
}

