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

#include "PropertyEditorComponent.h"
#include "PropertyEditor.h"
#include <GCF/ComponentFactory.h>
#include <GCF/IComponentPlugin.h>
#include "ValueEditorCreators.h"
#include "ValueEditorFactory.h"

struct GCF::Components::PropertyEditorComponentData
{
    PropertyEditorComponentData() : propertyEditor(0),
        valueEditorFactory(0) { }

    GCF::Components::PropertyEditor* propertyEditor;
    GCF::Components::ValueEditorFactory* valueEditorFactory;
};

GCF_DEFINE_STD_COMPONENT(PropertyEditorComponent)

GCF::Components::PropertyEditorComponent & GCF::Components::PropertyEditorComponent::instance()
{
    static GCF::Components::PropertyEditorComponent* theInstance = GCF_CREATE_COMPONENT(PropertyEditorComponent);
    return *theInstance;
}

GCF::Components::PropertyEditorComponent::PropertyEditorComponent()
{
    d = new GCF::Components::PropertyEditorComponentData;
    d->propertyEditor = new GCF::Components::PropertyEditor;
}

GCF::Components::PropertyEditorComponent::~PropertyEditorComponent()
{
    delete d->valueEditorFactory;
    delete d;
}

QObject* GCF::Components::PropertyEditorComponent::fetchObject(const QString& completeName) const
{
    QStringList comps = completeName.split('.');
	GCF::Components::PropertyEditorComponent* that = const_cast<GCF::Components::PropertyEditorComponent*>(this);

    if(comps.last() == "controller")
        return that;

    return 0;
}

QWidget* GCF::Components::PropertyEditorComponent::fetchWidget(const QString& completeName) const
{
    QStringList comps = completeName.split('.');

    if(comps.last() == "propertyEditor")
        return d->propertyEditor;

    return 0;
}

void GCF::Components::PropertyEditorComponent::addChildWidget(QWidget* parent, QWidget* child, const QString& hint)
{
    Q_UNUSED(hint);
    Q_UNUSED(parent);
    child->setParent(0);
}

void GCF::Components::PropertyEditorComponent::initializeComponent()
{
    GCF::Components::ValueEditorFactory* factory = new GCF::Components::ValueEditorFactory;
    d->propertyEditor->setEditorFactory(factory);
    d->valueEditorFactory = factory;

    // Default editor creator set
    static GCF::Components::NumberEditorCreator intCreator(QVariant::Int);
    factory->registerCreator(&intCreator);
    static GCF::Components::NumberEditorCreator uintCreator(QVariant::UInt);
    factory->registerCreator(&uintCreator);
    static GCF::Components::NumberEditorCreator doubleCreator(QVariant::Double);
    factory->registerCreator(&doubleCreator);
    static GCF::Components::BoolEditorCreator boolCreator;
    factory->registerCreator(&boolCreator);
    static GCF::Components::StringEditorCreator stringCreator;
    factory->registerCreator(&stringCreator);
    static GCF::Components::PointEditorCreator pointCreator;
    factory->registerCreator(&pointCreator);
    static GCF::Components::ColorEditorCreator colorCreator;
    factory->registerCreator(&colorCreator);
    static GCF::Components::VectorEditorCreator vectorCreator;
    factory->registerCreator(&vectorCreator);
    static GCF::Components::TriangleEditorCreator triangleCreator;
    factory->registerCreator(&triangleCreator);
	static GCF::Components::BoundingBoxEditorCreator boundingBoxCreator;
	factory->registerCreator(&boundingBoxCreator);
    static GCF::Components::ValueRangeEditorCreator valueRangeEditorCreator;
    factory->registerCreator(&valueRangeEditorCreator);
    static GCF::Components::FileNameEditorCreator fileNameCreator;
    factory->registerCreator(&fileNameCreator);
    static GCF::Components::ListEditorCreator doubleListEditorCreator(true);
    factory->registerCreator(&doubleListEditorCreator);
    static GCF::Components::ListEditorCreator intListEditorCreator(false);
    factory->registerCreator(&intListEditorCreator);
    static GCF::Components::EnumEditorCreator enumCreator;
    factory->setEnumCreator(&enumCreator);
}

void GCF::Components::PropertyEditorComponent::finalizeComponent()
{
    d->propertyEditor->setObject(0);
}

QWidget* GCF::Components::PropertyEditorComponent::createWidget(const QString& completeName) const
{
    QStringList comps = completeName.split('.');

    if(comps.last() == "propertyEditor")
    {
        GCF::Components::PropertyEditor* propEditor = new GCF::Components::PropertyEditor;
        propEditor->setEditorFactory(d->valueEditorFactory);
        return propEditor;
    }

    return 0;
}

QObject* GCF::Components::PropertyEditorComponent::containerObject()
{
    return this;
}

QString GCF::Components::PropertyEditorComponent::productName() const
{
    return "Property Editor";
}

QString GCF::Components::PropertyEditorComponent::organization() const
{
    return "VCreate Logic (P) Ltd";
}

QImage GCF::Components::PropertyEditorComponent::programLogo() const
{
    return QImage();
}

QString GCF::Components::PropertyEditorComponent::version() const
{
    return "1.0";
}

QString GCF::Components::PropertyEditorComponent::shortDescription() const
{
    return "This component provides a property editor widget that can show properties of "
        "any QObject in the system.";
}

QString GCF::Components::PropertyEditorComponent::homepage() const
{
    return "http://www.vcreatelogic.com/oss/gcf";
}

QString GCF::Components::PropertyEditorComponent::bugAddress() const
{
    return "http://www.vcreatelogic.com/oss/gcf/feedback/bugs";
}

const QList<GCF::AboutPerson> GCF::Components::PropertyEditorComponent::authors() const
{
    static QList<GCF::AboutPerson> retList;
    if(retList.count() == 0)
    {
        retList << GCF::AboutPerson("Prashanth N Udupa", "prashanth@vcreatelogic.com", "http://www.vcreatelogic.com", "Founder, Vision, Design, Development");
        retList << GCF::AboutPerson("Lohith D. S", "NA", "http://www.vcreatelogic.com", "Design, Development");
    }
    return retList;
}

const QList<GCF::AboutPerson> GCF::Components::PropertyEditorComponent::credits() const
{
    static QList<GCF::AboutPerson> retList;
    return retList;
}

QString GCF::Components::PropertyEditorComponent::license() const
{
    return "Dual License: GPL Version 2 for FOSS development, Commercial for others.";
}

QString GCF::Components::PropertyEditorComponent::copyrightStatement() const
{
    return QString();
}

GCF_EXPORT_STD_COMPONENT_PLUGIN(PropertyEditorComponent)

