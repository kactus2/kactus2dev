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

#include "ScriptEngineManager.h"
#include <QList>
#include <QScriptValue>
#include <QScriptEngine>
#include <QColor>
#include <QList>

#include "UserDefinedDataTypes.h"
#include "ScriptableObjectExplorer.h"
#include "ScriptEditor.h"

Q_DECLARE_METATYPE(QScriptEngine*)
Q_DECLARE_METATYPE(QList<QObject*>)

QScriptValue toScriptValue_Point(QScriptEngine* engine, const Point3D& p);
void fromScriptValue_Point(const QScriptValue& value, Point3D& p);
QScriptValue create_Point(QScriptContext* context, QScriptEngine* engine);

QScriptValue toScriptValue_Vector(QScriptEngine* engine, const Vector3D& p);
void fromScriptValue_Vector(const QScriptValue& value, Vector3D& p);
QScriptValue create_Vector(QScriptContext* context, QScriptEngine* engine);

QScriptValue toScriptValue_Triangle(QScriptEngine* engine, const Triangle& p);
void fromScriptValue_Triangle(const QScriptValue& value, Triangle& p);
QScriptValue create_Triangle(QScriptContext* context, QScriptEngine* engine);

QScriptValue toScriptValue_BoundingBox(QScriptEngine* engine, const BoundingBox3D& p);
void fromScriptValue_BoundingBox(const QScriptValue& value, BoundingBox3D& p);
QScriptValue create_BoundingBox(QScriptContext* context, QScriptEngine* engine);

QScriptValue toScriptValue_ValueRange(QScriptEngine* engine, const ValueRange& p);
void fromScriptValue_ValueRange(const QScriptValue& value, ValueRange& p);
QScriptValue create_ValueRange(QScriptContext* context, QScriptEngine* engine);

QScriptValue toScriptValue_QColor(QScriptEngine* engine, const QColor& p);
void fromScriptValue_QColor(const QScriptValue& value, QColor& p);
QScriptValue create_QColor(QScriptContext* context, QScriptEngine* engine);

QScriptValue toScriptValue_QRectF(QScriptEngine* engine, const QRectF& r);
void fromScriptValue_QRectF(const QScriptValue& value, QRectF& r);
QScriptValue create_QRectF(QScriptContext* context, QScriptEngine* engine);

QScriptValue toScriptValue_LineSegment(QScriptEngine* engine, const LineSegment& r);
void fromScriptValue_LineSegment(const QScriptValue& value, LineSegment& r);
QScriptValue create_LineSegment(QScriptContext* context, QScriptEngine* engine);

namespace GCF
{
	namespace Components
	{
		struct ScriptableObject
		{
			ScriptableObject() : Object(0), ExcludeChildren(true) { }
			ScriptableObject(QObject* object, QString name, bool ec=true) : Object(object), Name(name), ExcludeChildren(ec) { }

			QObject* Object;
			QString Name;
			bool ExcludeChildren;
		};
	}
}

struct GCF::Components::ScriptEngineManagerData
{
    ScriptEngineManagerData() : scriptEngine(0), scriptModified(false),
        scriptableObjectExplorer(0), scriptEditor(0), updateEditor(true) { }

    QList<GCF::Components::ScriptableObject> scriptableObjectList;
    QList<IScriptDataType*> scriptDataTypeList;
    QScriptEngine* scriptEngine;
    QString script;
    bool scriptModified;
    GCF::Components::ScriptableObjectExplorer* scriptableObjectExplorer;
    GCF::Components::ScriptEditor* scriptEditor;
    bool updateEditor;

    int findScriptableObject(QString name);
    int findScriptableObject(QObject* object);
};

GCF::Components::ScriptEngineManager::ScriptEngineManager()
{
    d = new GCF::Components::ScriptEngineManagerData;
    d->scriptDataTypeList.append(qobject_cast<IScriptDataType*>(this));
    resetEngine();
}

GCF::Components::ScriptEngineManager::~ScriptEngineManager()
{
    delete d;
}

int GCF::Components::ScriptEngineManager::scriptableObjectCount() const
{
    return d->scriptableObjectList.count();
}

QObject* GCF::Components::ScriptEngineManager::scriptableObject(int index) const
{
    if(index < 0 || index >= d->scriptableObjectList.count())
        return 0;
    return d->scriptableObjectList[index].Object;
}

QString GCF::Components::ScriptEngineManager::scriptableObjectName(int index) const
{
    if(index < 0 || index >= d->scriptableObjectList.count())
        return QString();
    return d->scriptableObjectList[index].Name;
}

bool GCF::Components::ScriptEngineManager::scriptableObjectExcludeChildren(int index) const
{
    if(index < 0 || index >= d->scriptableObjectList.count())
        return true;
    return d->scriptableObjectList[index].ExcludeChildren;
}

void GCF::Components::ScriptEngineManager::setScriptableObjectExplorer(GCF::Components::ScriptableObjectExplorer* exp)
{
    d->scriptableObjectExplorer = exp;
}

GCF::Components::ScriptableObjectExplorer* GCF::Components::ScriptEngineManager::scriptableObjectExplorer() const
{
    return d->scriptableObjectExplorer;
}

void GCF::Components::ScriptEngineManager::setScriptEditor(GCF::Components::ScriptEditor* scriptEditor)
{
    d->scriptEditor = scriptEditor;
}

GCF::Components::ScriptEditor* GCF::Components::ScriptEngineManager::scriptEditor() const
{
    return d->scriptEditor;
}

QObject* GCF::Components::ScriptEngineManager::containerObject()
{
    return this;
}

void GCF::Components::ScriptEngineManager::addScriptableObject(QObject* object, QString name, bool excludeChildren)
{
    Q_UNUSED(excludeChildren);

    if(!object || name.isEmpty())
        return;

    int index = d->findScriptableObject(object);
    if(index >= 0)
        return;

    index = d->findScriptableObject(name);
    if(index >= 0)
        d->scriptableObjectList[index].Object = object;
    else
        d->scriptableObjectList << GCF::Components::ScriptableObject(object, name, excludeChildren);

    QScriptValue objectValue;

    QScriptEngine::QObjectWrapOptions options = QScriptEngine::AutoCreateDynamicProperties;
    objectValue = d->scriptEngine->newQObject(object, QScriptEngine::QtOwnership, options);
    d->scriptEngine->globalObject().setProperty(name, objectValue);

    // connect to the nodeNameChanged() signal
    if(object->inherits("IVisSystemNode"))
        connect(object, SIGNAL(nodeNameChanged()), this, SLOT(objectNameChanged()));
    if(d->scriptableObjectExplorer)
        QMetaObject::invokeMethod(d->scriptableObjectExplorer, "updateExplorer", Qt::QueuedConnection);
}

void GCF::Components::ScriptEngineManager::removeScriptableObject(QObject* object)
{
    if(!object)
        return;

    int index = d->findScriptableObject(object);
    if(index < 0)
        return;

    QString name = d->scriptableObjectList[index].Name;
    QScriptValue nullValue;
    d->scriptEngine->globalObject().setProperty(name, nullValue);
    d->scriptableObjectList.removeAt(index);

    if(object->inherits("IVisSystemNode"))
        disconnect(object, SIGNAL(nodeNameChanged()), this, SLOT(objectNameChanged()));
    if(d->scriptableObjectExplorer)
        QMetaObject::invokeMethod(d->scriptableObjectExplorer, "updateExplorer", Qt::QueuedConnection);
}

void GCF::Components::ScriptEngineManager::removeScriptableObject(QString name)
{
    if(name.isEmpty())
        return;

    int index = d->findScriptableObject(name);
    if(index < 0)
        return;

    QObject* object = d->scriptableObjectList[index].Object;
    QScriptValue nullValue;
    d->scriptEngine->globalObject().setProperty(name, nullValue);
    d->scriptableObjectList.removeAt(index);

    if(object->inherits("IVisSystemNode"))
        disconnect(object, SIGNAL(nodeNameChanged()), this, SLOT(objectNameChanged()));
    if(d->scriptableObjectExplorer)
        QMetaObject::invokeMethod(d->scriptableObjectExplorer, "updateExplorer", Qt::QueuedConnection);
}

void GCF::Components::ScriptEngineManager::changeScriptableObjectName(QObject* object, QString newName)
{
    if( !object )
        return;

    int index = d->findScriptableObject(object);
    if(index < 0)
        return;

    QString name = newName;
    QString oldName = d->scriptableObjectList[index].Name;
    QScriptEngine::QObjectWrapOptions options = QScriptEngine::AutoCreateDynamicProperties;
    QScriptValue objectValue = d->scriptEngine->newQObject(object, QScriptEngine::QtOwnership, options);
    QScriptValue nullValue;

    d->scriptEngine->globalObject().setProperty(oldName, nullValue);
    d->scriptEngine->globalObject().setProperty(name, objectValue);
    d->scriptableObjectList[index].Name = name;

    if(d->scriptableObjectExplorer)
        QMetaObject::invokeMethod(d->scriptableObjectExplorer, "updateExplorer", Qt::QueuedConnection);
}

QScriptEngine* GCF::Components::ScriptEngineManager::engine()
{
    return d->scriptEngine;
}

void GCF::Components::ScriptEngineManager::resetEngine()
{
    if(d->scriptEngine)
    {
        delete d->scriptEngine;
        d->scriptEngine = 0;
    }

    d->scriptEngine = new QScriptEngine(this);

    // Register C++ types for interoperation between script and C++
    for(int i=0; i<d->scriptDataTypeList.count(); i++)
    {
        IScriptDataType* type = d->scriptDataTypeList[i];
        type->registerTypes(d->scriptEngine);
    }

    for(int i=0; i<d->scriptableObjectList.count(); i++)
    {
        GCF::Components::ScriptableObject so = d->scriptableObjectList[i];
        QObject* object = so.Object;
        QString name = so.Name;

        QScriptEngine::QObjectWrapOptions  options = QScriptEngine::AutoCreateDynamicProperties;
        QScriptValue objectValue = d->scriptEngine->newQObject(object, QScriptEngine::QtOwnership, options);
        d->scriptEngine->globalObject().setProperty(name, objectValue);
    }
}

void GCF::Components::ScriptEngineManager::setScript(QString script)
{
    if(d->script == script)
        return;

    d->script = script;
    d->scriptModified = true;
    if(d->updateEditor && d->scriptEditor)
        d->scriptEditor->setCode(d->script);
}

QString GCF::Components::ScriptEngineManager::script()
{
    return d->script;
}

void GCF::Components::ScriptEngineManager::executeScript()
{
    if(d->scriptModified)
        resetEngine();

    QScriptValue value = d->scriptEngine->evaluate(d->script);
    if(value.isError())
    {
        QString errString = value.toString();
        qWarning("Script Error: %s", qPrintable(errString));
        emit scriptError(errString, d->scriptEngine->uncaughtExceptionLineNumber());
    }
}

void GCF::Components::ScriptEngineManager::addScriptDataType(IScriptDataType* type)
{
    if(!type || d->scriptDataTypeList.contains(type))
        return;

    d->scriptDataTypeList.append(type);
}

void GCF::Components::ScriptEngineManager::removeScriptDataType(IScriptDataType* type)
{
    if(!type || !d->scriptDataTypeList.contains(type))
        return;

    d->scriptDataTypeList.removeAll(type);
}

int GCF::Components::ScriptEngineManager::scriptDataTypeCount()
{
    return d->scriptDataTypeList.count();
}

IScriptDataType* GCF::Components::ScriptEngineManager::scriptDataType(int index)
{
    if(index < 0 || index >= d->scriptDataTypeList.count())
        return 0;

    return d->scriptDataTypeList[index];
}

void GCF::Components::ScriptEngineManager::registerTypes(QScriptEngine* engine)
{
    // Register the Point3D type
    qScriptRegisterMetaType(engine, toScriptValue_Point, fromScriptValue_Point);
    engine->globalObject().setProperty("Point3D", engine->newFunction(create_Point));
    engine->evaluate(
        "function NewPoint3D(x, y, z)\n"
        "{\n"
        "    var ret = new Point3D;\n"
        "    ret.x = x; ret.y = y; ret.z = z;\n"
        "    return ret;\n"
        "}\n"
        );

    // Register the Vector3D type
    qScriptRegisterMetaType(engine, toScriptValue_Vector, fromScriptValue_Vector);
    engine->globalObject().setProperty("Vector3D", engine->newFunction(create_Vector));
    engine->evaluate(
        "function NewVector3D(x, y, z)\n"
        "{\n"
        "    var ret = new Vector3D;\n"
        "    ret.x = x; ret.y = y; ret.z = z;\n"
        "    return ret;\n"
        "}\n"
        );

    // Register the Triangle type
    qScriptRegisterMetaType(engine, toScriptValue_Triangle, fromScriptValue_Triangle);
    engine->globalObject().setProperty("Triangle", engine->newFunction(create_Triangle));
    engine->evaluate(
        "function NewTriangle(a, b, c)\n"
        "{\n"
        "    var ret = new Triangle;\n"
        "    ret.a = a; ret.b = b; ret.c = c;\n"
        "    return ret;\n"
        "}\n"
        );

    // Register the BoundingBox3D type
    qScriptRegisterMetaType(engine, toScriptValue_BoundingBox, fromScriptValue_BoundingBox);
    engine->globalObject().setProperty("BoundingBox3D", engine->newFunction(create_BoundingBox));
    engine->evaluate(
        "function NewBoundingBox3D(xMin, xMax, yMin, yMax, zMin, zMax)\n"
        "{\n"
        "    var ret = new BoundingBox3D;\n"
        "    ret.xMin = xMin; ret.xMax = xMax;\n"
        "    ret.yMin = yMin; ret.yMax = yMax;\n"
        "    ret.zMin = zMin; ret.zMax = zMax;\n"
        "    return ret;\n"
        "}\n"
        );


    // Register the ValueRange type
    qScriptRegisterMetaType(engine, toScriptValue_ValueRange, fromScriptValue_ValueRange);
    engine->globalObject().setProperty("ValueRange", engine->newFunction(create_ValueRange));
    engine->evaluate(
        "function NewValueRange(min, max)\n"
        "{\n"
        "    var ret = new ValueRange;\n"
        "    ret.min = min; ret.max = max;\n"
        "    return ret;\n"
        "}\n"
        );

    // Register the QColor type.
    qScriptRegisterMetaType(engine, toScriptValue_QColor, fromScriptValue_QColor);
    engine->globalObject().setProperty("QColor", engine->newFunction(create_QColor));
    engine->evaluate(
        "function NewQColor(red, green, blue)\n"
        "{\n"
        "    var ret = new QColor;\n"
        "    ret.red = red; ret.blue = blue; ret.green = green;\n"
        "    return ret;\n"
        "}\n"
        );


    // Register the QRectF type.
    qScriptRegisterMetaType(engine, toScriptValue_QRectF, fromScriptValue_QRectF);
    engine->globalObject().setProperty("QRectF", engine->newFunction(create_QRectF));
    engine->evaluate(
        "function NewQRectF(x, y, width, height)\n"
        "{\n"
        "    var ret = new QRectF(x, y, width, height);\n"
        "    return ret;\n"
        "}\n"
        );

    // Register the LineSegment type.
    qScriptRegisterMetaType(engine, toScriptValue_LineSegment, fromScriptValue_LineSegment);
    engine->globalObject().setProperty("LineSegment", engine->newFunction(create_LineSegment));
    engine->evaluate(
        "function NewLineSegment(a, b)\n"
        "{\n"
        "    var ret = new LineSegment(a, b);\n"
        "    return ret;\n"
        "}\n"
        );

    // Register lists
    qScriptRegisterSequenceMetaType< QList<QObject*> >(engine);
    qScriptRegisterSequenceMetaType< QList<double> >(engine);
    qScriptRegisterSequenceMetaType< QList<int> >(engine);
}

void GCF::Components::ScriptEngineManager::objectNameChanged()
{
    QObject* object = sender();
    int index = d->findScriptableObject(object);
    if(index < 0)
        return;

    QString name = object->objectName();
    QString oldName = d->scriptableObjectList[index].Name;
    QScriptEngine::QObjectWrapOptions options = QScriptEngine::AutoCreateDynamicProperties;
    QScriptValue objectValue = d->scriptEngine->newQObject(object, QScriptEngine::QtOwnership, options);
    QScriptValue nullValue;

    d->scriptEngine->globalObject().setProperty(oldName, nullValue);
    d->scriptEngine->globalObject().setProperty(name, objectValue);
    d->scriptableObjectList[index].Name = name;

    if(d->scriptableObjectExplorer)
        QMetaObject::invokeMethod(d->scriptableObjectExplorer, "updateExplorer", Qt::QueuedConnection);
}

void GCF::Components::ScriptEngineManager::setUpdateEditor(bool val)
{
    d->updateEditor = val;
}

int GCF::Components::ScriptEngineManagerData::findScriptableObject(QString name)
{
    for(int i=0; i<scriptableObjectList.count(); i++)
        if(scriptableObjectList[i].Name == name)
            return i;
    return -1;
}

int GCF::Components::ScriptEngineManagerData::findScriptableObject(QObject* object)
{
    for(int i=0; i<scriptableObjectList.count(); i++)
        if(scriptableObjectList[i].Object == object)
            return i;
    return -1;
}

// Support for additional script value types.
QScriptValue toScriptValue_Point(QScriptEngine* engine, const Point3D& p)
{
    QScriptValue obj = engine->newObject();
    obj.setProperty("x", QScriptValue(engine, p.x));
    obj.setProperty("y", QScriptValue(engine, p.y));
    obj.setProperty("z", QScriptValue(engine, p.z));
    return obj;
}

void fromScriptValue_Point(const QScriptValue& value, Point3D& p)
{
    p.x = value.property("x").toNumber();
    p.y = value.property("y").toNumber();
    p.z = value.property("z").toNumber();
}

QScriptValue create_Point(QScriptContext*, QScriptEngine* engine)
{
    Point3D point;
    return engine->toScriptValue(point);
}

QScriptValue toScriptValue_Vector(QScriptEngine* engine, const Vector3D& p)
{
    QScriptValue obj = engine->newObject();
    obj.setProperty("x", QScriptValue(engine, p.x));
    obj.setProperty("y", QScriptValue(engine, p.y));
    obj.setProperty("z", QScriptValue(engine, p.z));
    return obj;
}

void fromScriptValue_Vector(const QScriptValue& value, Vector3D& p)
{
    p.x = value.property("x").toNumber();
    p.y = value.property("y").toNumber();
    p.z = value.property("z").toNumber();
}

QScriptValue create_Vector(QScriptContext*, QScriptEngine* engine)
{
    Vector3D vector;
    return engine->toScriptValue(vector);
}

QScriptValue toScriptValue_Triangle(QScriptEngine* engine, const Triangle& p)
{
    QScriptValue obj = engine->newObject();
    obj.setProperty("a", QScriptValue(engine, p.a));
    obj.setProperty("b", QScriptValue(engine, p.b));
    obj.setProperty("c", QScriptValue(engine, p.c));
    return obj;
}

void fromScriptValue_Triangle(const QScriptValue& value, Triangle& p)
{
    p.a = value.property("a").toNumber();
    p.b = value.property("b").toNumber();
    p.c = value.property("c").toNumber();
}

QScriptValue create_Triangle(QScriptContext*, QScriptEngine* engine)
{
    Triangle triangle;
    return engine->toScriptValue(triangle);
}

QScriptValue toScriptValue_BoundingBox(QScriptEngine* engine, const BoundingBox3D& p)
{
    QScriptValue obj = engine->newObject();
    obj.setProperty("xMin", QScriptValue(engine, p.xMin));
    obj.setProperty("xMax", QScriptValue(engine, p.xMax));
    obj.setProperty("yMin", QScriptValue(engine, p.yMin));
    obj.setProperty("yMax", QScriptValue(engine, p.yMax));
    obj.setProperty("zMin", QScriptValue(engine, p.zMin));
    obj.setProperty("zMax", QScriptValue(engine, p.zMax));
    return obj;
}

void fromScriptValue_BoundingBox(const QScriptValue& value, BoundingBox3D& p)
{
    p.xMin = value.property("xMin").toNumber();
    p.xMax = value.property("xMax").toNumber();
    p.yMin = value.property("yMin").toNumber();
    p.yMax = value.property("yMax").toNumber();
    p.zMin = value.property("zMin").toNumber();
    p.zMax = value.property("zMax").toNumber();
}

QScriptValue create_BoundingBox(QScriptContext*, QScriptEngine* engine)
{
    BoundingBox3D bbox;
    return engine->toScriptValue(bbox                                                             );
}

QScriptValue toScriptValue_ValueRange(QScriptEngine* engine, const ValueRange& p)
{
    QScriptValue obj = engine->newObject();
    obj.setProperty("min", QScriptValue(engine, p.min));
    obj.setProperty("max", QScriptValue(engine, p.max));
    return obj;
}

void fromScriptValue_ValueRange(const QScriptValue& value, ValueRange& p)
{
    p.min = value.property("min").toNumber();
    p.max = value.property("max").toNumber();
}

QScriptValue create_ValueRange(QScriptContext*, QScriptEngine* engine)
{
    ValueRange triangle;
    return engine->toScriptValue(triangle);
}

QScriptValue toScriptValue_QColor(QScriptEngine* engine, const QColor& color)
{
    QScriptValue obj = engine->newObject();
    obj.setProperty("red", QScriptValue(engine, color.redF()));
    obj.setProperty("green", QScriptValue(engine, color.greenF()));
    obj.setProperty("blue", QScriptValue(engine, color.blueF()));
    return obj;
}

void fromScriptValue_QColor(const QScriptValue& value, QColor& color)
{
    double red = value.property("red").toNumber();
    double green = value.property("green").toNumber();
    double blue = value.property("blue").toNumber();
    color.setRgbF(red, green, blue);
}

QScriptValue create_QColor(QScriptContext*, QScriptEngine* engine)
{
    QColor color;
    return engine->toScriptValue(color);
}

QScriptValue toScriptValue_QRectF(QScriptEngine* engine, const QRectF& rect)
{
    QScriptValue obj = engine->newObject();
    obj.setProperty("x", QScriptValue(engine, rect.x()));
    obj.setProperty("y", QScriptValue(engine, rect.y()));
    obj.setProperty("width", QScriptValue(engine, rect.width()));
    obj.setProperty("height", QScriptValue(engine, rect.height()));
    return obj;
}

void fromScriptValue_QRectF(const QScriptValue& value, QRectF& rect)
{
    double x = value.property("x").toNumber();
    double y = value.property("y").toNumber();
    double width = value.property("width").toNumber();
    double height = value.property("height").toNumber();
    rect = QRectF(x, y, width, height);
}

QScriptValue create_QRectF(QScriptContext*, QScriptEngine* engine)
{
    QRectF rect;
    return engine->toScriptValue(rect);
}

QScriptValue toScriptValue_LineSegment(QScriptEngine* engine, const LineSegment& lineSeg)
{
    QScriptValue obj = engine->newObject();
    obj.setProperty("a", QScriptValue(engine, lineSeg.a));
    obj.setProperty("b", QScriptValue(engine, lineSeg.b));
    return obj;
}

void fromScriptValue_LineSegment(const QScriptValue& value, LineSegment& lineSeg)
{
    lineSeg.a = uint( value.property("a").toNumber() );
    lineSeg.b = uint( value.property("b").toNumber() );
}

QScriptValue create_LineSegment(QScriptContext*, QScriptEngine* engine)
{
    LineSegment lineSeg;
    return engine->toScriptValue(lineSeg);
}
