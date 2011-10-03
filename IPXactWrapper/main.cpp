#include <QtGui/QApplication>
#include "ipxactwrapper.h"
#include "BusDefinitionGenerator/busdefinitiongenerator.h"
#include <models/busdefinition.h>
#include <models/generaldeclarations.h>
#include "LibraryManager/vlnv.h"

// includes for attribute box
#include <common/widgets/attributeBox/attributebox.h>

// includes for nameGroupBox
#include <common/widgets/nameGroupBox/namegroupbox.h>

// includes for listManager
#include <common/widgets/listManager/listmanager.h>

// includes for componentGenerator
#include "IPXactWrapper/ComponentEditor/ipxactcomponenteditor.h"
#include <models/component.h>

#include <QDebug>
#include <QString>
#include <QMap>
#include <QList>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //IPXactWrapper w;
    //w.show();

    // the test for the busDefinitionGenerator
    /*
    BusDefinition* busDef = new BusDefinition();
    busDef->setVlnv(new VLNV(VLNV::BUSDEFINITION, QString("Vendori"), QString("kirjasto"),
                             QString("nimi"), QString("versio")));
    busDef->setExtends(new VLNV(VLNV::BUSDEFINITION, QString("extven"), QString("extkirjasto"),
                                QString("extnimi"), QString("extversio")));;
    busDef->setDirectConnection(true);
    busDef->setIsAddressable(false);

    busDef->setMaxSlaves(4);
    busDef->setDescription(QString("kuvaus joka on hyvä"));

    QList<QString> groups;
    groups.append(QString("group1"));
    groups.append(QString("group2"));
    groups.append(QString("lastGroup"));
    busDef->setSystemGroupNames(groups);

    QMap<QString, QString> att;
    att.insert(QString("attribute1"), QString("value1"));
    att.insert(QString("attribute2"), QString("value2"));
    busDef->setAttributes(att);

    BusDefinitionGenerator b(busDef);
    busDef = b.exec();

    if (busDef) {
        qDebug() << "main got valid pointer";

        QMap<QString, QString> attributes = busDef->getAttributes();
        for (QMap<QString, QString>::iterator i = attributes.begin();
        i != attributes.end(); ++i) {
            qDebug() << "Attribute " << i.key() << "=" << i.value();
        }

        qDebug() << "Vendor: " << busDef->getVlnv()->getVendor();
        qDebug() << "Library: " << busDef->getVlnv()->getLibrary();
        qDebug() << "Name: " << busDef->getVlnv()->getName();
        qDebug() << "Version: " << busDef->getVlnv()->getVersion();

        qDebug() << "directConnection: " << General::bool2Str(busDef->getDirectConnection());
        qDebug() << "isAddressable: " << General::bool2Str(busDef->getIsAddressable());

        qDebug() << "maxMasters: " << QString::number(busDef->getMaxMasters());
        qDebug() << "maxSlaves: " << QString::number(busDef->getMaxSlaves());

        QList<QString> names = busDef->getSystemGroupNames();
        for (int i = 0; i < names.size(); ++i) {
            qDebug() << "SystemGroupName " << i << ": " << names.at(i);
        }

        VLNV* extends = busDef->getExtends();
        qDebug() << "Extends:";
        if (extends) {
            qDebug() << "Vendor: " << busDef->getExtends()->getVendor();
            qDebug() << "Library: " << busDef->getExtends()->getLibrary();
            qDebug() << "Name: " << busDef->getExtends()->getName();
            qDebug() << "Version: " << busDef->getExtends()->getVersion();
        }

        qDebug() << "Description: " << busDef->getDescription();
    }
    else {
        qDebug() << "main got null pointer";
    }
    */

    // the test for the attributeBox
    /*
    QMap<QString, QString> attributes;
    attributes.insert(QString("nimi1"), QString("arvo1"));
    attributes.insert(QString("Nimi2"), QString("arvo2"));
    AttributeBox b(attributes);
    b.show();
    */

    // the test for nameGroupBox
    /*
    NameGroupBox b;
    b.show();
    */

    /*
    ListManager b;
    b.show();
    */

    Component* component = new Component();
    IPXactComponentEditor b(component);
    b.show();

    return a.exec();
}
