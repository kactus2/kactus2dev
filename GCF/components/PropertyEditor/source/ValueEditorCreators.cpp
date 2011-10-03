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

#include "ValueEditorCreators.h"
#include "UserDefinedDataTypes.h"

#include <QUrl>
#include <QStyle>
#include <QRegExp>
#include <QPainter>

#include <QMetaEnum>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QApplication>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>
#include <QAction>
#include <QMenu>
#include <QWidgetAction>
#include <QComboBox>

#include "ColorPickerWidget.h"


////////////////////////////////////////////////////////////////////////////////////////////
/// GCF::Components::EnumEditorCreator
////////////////////////////////////////////////////////////////////////////////////////////

GCF::Components::EnumEditorCreator::EnumEditorCreator()
{

}

GCF::Components::EnumEditorCreator::~EnumEditorCreator()
{

}

GCF_BEGIN_QUERY_TABLE(GCF::Components::EnumEditorCreator)
    GCF_IMPLEMENTS(IValueEditorCreator)
    GCF_IMPLEMENTS(IEnumValueEditorCreator)
GCF_END_QUERY_TABLE(GCF::Components::EnumEditorCreator)

// IValueEditorCreator implementation
const char* GCF::Components::EnumEditorCreator::typeName()
{
    return "GenericEnumeration";
}

QWidget* GCF::Components::EnumEditorCreator::createEditor(QWidget* parent)
{
    QComboBox* combo = new QComboBox(parent);
    combo->setFrame(false);
    return combo;
}

void GCF::Components::EnumEditorCreator::connectEditor(QWidget* editor, QObject* receiver, const char* slot)
{
    QObject::connect(editor, SIGNAL(currentIndexChanged(int)), receiver, slot);
}

bool GCF::Components::EnumEditorCreator::setEditorValue(QWidget* editor, QVariant value)
{
    QComboBox* combo = qobject_cast<QComboBox*>(editor);
    int intVal = value.toInt();
    if(combo)
    {
        for(int i=0; i<combo->count(); i++)
        {
            if(combo->itemData(i).toInt() == intVal)
            {
                combo->setCurrentIndex(i);
                return true;
            }
        }
    }
    return false;
}

QVariant GCF::Components::EnumEditorCreator::editorValue(QWidget* editor)
{
    QComboBox* combo = qobject_cast<QComboBox*>(editor);
    if(!combo)
        return QVariant();

    int index = combo->currentIndex();
    return combo->itemData(index);
}

void GCF::Components::EnumEditorCreator::closeEditor(QWidget* editor)
{
    Q_UNUSED(editor);
}

QString GCF::Components::EnumEditorCreator::toString(QVariant value)
{
    return QString("%1").arg(value.toInt());
}

QVariant GCF::Components::EnumEditorCreator::fromString(QString value)
{
    return value.toInt();
}

bool GCF::Components::EnumEditorCreator::paintValue(QPainter* paint, QRect rect, QVariant value)
{
    if(!mEnumStruct.isValid())
        paint->drawText(rect, Qt::AlignLeft|Qt::AlignVCenter, toString(value));
    else
        paint->drawText(rect, Qt::AlignLeft|Qt::AlignVCenter, mEnumStruct.valueToKey(value.toInt()));
    return true;
}

void GCF::Components::EnumEditorCreator::initialize(QWidget* editor, QMetaEnum enumStruct)
{
    QComboBox* combo = qobject_cast<QComboBox*>(editor);
    if(!combo)
        return;

    for(int i=enumStruct.keyCount()-1; i>=0; i--)
    {
        QString key = QString("%1").arg(enumStruct.key(i));
        int value = enumStruct.value(i);
        combo->insertItem(0, key);
        combo->setItemData(0, value);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GCF::Components::NumberEditorCreator
////////////////////////////////////////////////////////////////////////////////////////////

GCF::Components::NumberEditorCreator::NumberEditorCreator(QVariant::Type type)
{
    mType = type;
}

GCF::Components::NumberEditorCreator::~NumberEditorCreator()
{

}

GCF_BEGIN_QUERY_TABLE(GCF::Components::NumberEditorCreator)
    GCF_IMPLEMENTS(IValueEditorCreator)
GCF_END_QUERY_TABLE(GCF::Components::NumberEditorCreator)

const char* GCF::Components::NumberEditorCreator::typeName()
{
    return QVariant::typeToName(mType);
}

QWidget* GCF::Components::NumberEditorCreator::createEditor(QWidget* parent)
{
    QLineEdit* lineEdit = new QLineEdit(parent);
    lineEdit->setFrame(true);

    QValidator* validator = 0;
    if(mType == QVariant::Double)
    {
        QDoubleValidator* dv = new QDoubleValidator(lineEdit);
        dv->setDecimals(2);
        validator = dv;
    }
    else
    {
        QIntValidator* iv = new QIntValidator(lineEdit);
        validator = iv;
    }

    lineEdit->setValidator(validator);
    return lineEdit;
}

void GCF::Components::NumberEditorCreator::connectEditor(QWidget* editor, QObject* receiver, const char* slot)
{
    QObject::connect(editor, SIGNAL(textEdited(const QString&)), receiver, slot);
}

bool GCF::Components::NumberEditorCreator::setEditorValue(QWidget* editor, QVariant value)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(lineEdit)
        lineEdit->setProperty("text", value);
    return lineEdit != 0;
}

QVariant GCF::Components::NumberEditorCreator::editorValue(QWidget* editor)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(lineEdit)
        return lineEdit->text();
    return QVariant();
}

void GCF::Components::NumberEditorCreator::closeEditor(QWidget* editor)
{
    Q_UNUSED(editor);
}

QString GCF::Components::NumberEditorCreator::toString(QVariant value)
{
    switch(mType)
    {
    case QVariant::Int: return QString("%1").arg(value.toInt());
    case QVariant::UInt: return QString("%1").arg(value.toUInt());
    case QVariant::Double: return QString("%1").arg(value.toDouble(),0,'g',4);
    }
    return value.toString();
}

QVariant GCF::Components::NumberEditorCreator::fromString(QString value)
{
    return QVariant(value.toDouble());
}

bool GCF::Components::NumberEditorCreator::paintValue(QPainter* paint, QRect rect, QVariant value)
{
    paint->drawText(rect, Qt::AlignLeft|Qt::AlignVCenter, toString(value));
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////

GCF_BEGIN_QUERY_TABLE(GCF::Components::BoolEditorCreator)
    GCF_IMPLEMENTS(IValueEditorCreator)
GCF_END_QUERY_TABLE(GCF::Components::BoolEditorCreator)

const char* GCF::Components::BoolEditorCreator::typeName()
{
    return QVariant::typeToName(QVariant::Bool);
}

QWidget* GCF::Components::BoolEditorCreator::createEditor(QWidget* parent)
{
    QCheckBox* checkBox = new QCheckBox(parent);
    return checkBox;
}

void GCF::Components::BoolEditorCreator::connectEditor(QWidget* editor, QObject* receiver, const char* slot)
{
    QObject::connect(editor, SIGNAL(clicked()), receiver, slot);
}

bool GCF::Components::BoolEditorCreator::setEditorValue(QWidget* editor, QVariant value)
{
    QCheckBox* checkBox = qobject_cast<QCheckBox*>(editor);
    if(checkBox)
        checkBox->setChecked(value.toBool());
    return checkBox != 0;
}

QVariant GCF::Components::BoolEditorCreator::editorValue(QWidget* editor)
{
    QCheckBox* checkBox = qobject_cast<QCheckBox*>(editor);
    if(checkBox)
        return checkBox->isChecked();
    return QVariant();
}

void GCF::Components::BoolEditorCreator::closeEditor(QWidget* editor)
{
    Q_UNUSED(editor);
}

QString GCF::Components::BoolEditorCreator::toString(QVariant value)
{
    if(value.toBool())
        return "true";
    return "false";
}

QVariant GCF::Components::BoolEditorCreator::fromString(QString value)
{
    if(value == "true")
        return true;
    return false;
}

bool GCF::Components::BoolEditorCreator::paintValue(QPainter* paint, QRect rect, QVariant value)
{
    paint->drawText(rect, Qt::AlignLeft|Qt::AlignVCenter, toString(value));
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GCF::Components::StringEditorCreator
////////////////////////////////////////////////////////////////////////////////////////////

GCF_BEGIN_QUERY_TABLE(GCF::Components::StringEditorCreator)
    GCF_IMPLEMENTS(IValueEditorCreator)
GCF_END_QUERY_TABLE(GCF::Components::StringEditorCreator)

const char* GCF::Components::StringEditorCreator::typeName()
{
    return QVariant::typeToName(QVariant::String);
}

QWidget* GCF::Components::StringEditorCreator::createEditor(QWidget* parent)
{
    QLineEdit* lineEdit = new QLineEdit(parent);
    lineEdit->setFrame(true);
    return lineEdit;
}

void GCF::Components::StringEditorCreator::connectEditor(QWidget* editor, QObject* receiver, const char* slot)
{
    QObject::connect(editor, SIGNAL(textEdited(const QString&)), receiver, slot);
}

bool GCF::Components::StringEditorCreator::setEditorValue(QWidget* editor, QVariant value)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(lineEdit)
        lineEdit->setText(value.toString());
    return lineEdit != 0;
}

QVariant GCF::Components::StringEditorCreator::editorValue(QWidget* editor)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(lineEdit)
        return lineEdit->text();
    return QVariant();
}

void GCF::Components::StringEditorCreator::closeEditor(QWidget* editor)
{
    Q_UNUSED(editor);
}

QString GCF::Components::StringEditorCreator::toString(QVariant value)
{
    return value.toString();
}

QVariant GCF::Components::StringEditorCreator::fromString(QString value)
{
    return QVariant(value);
}

bool GCF::Components::StringEditorCreator::paintValue(QPainter* paint, QRect rect, QVariant value)
{
    paint->drawText(rect, Qt::AlignLeft|Qt::AlignVCenter, toString(value));
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GCF::Components::PointEditorCreator
////////////////////////////////////////////////////////////////////////////////////////////

GCF_BEGIN_QUERY_TABLE(GCF::Components::PointEditorCreator)
    GCF_IMPLEMENTS(IValueEditorCreator)
GCF_END_QUERY_TABLE(GCF::Components::PointEditorCreator)

const char* GCF::Components::PointEditorCreator::typeName()
{
    return "Point3D";
}

QWidget* GCF::Components::PointEditorCreator::createEditor(QWidget* parent)
{
    QLineEdit* lineEdit = new QLineEdit(parent);
    lineEdit->setFrame(true);
    return lineEdit;
}

void GCF::Components::PointEditorCreator::connectEditor(QWidget* editor, QObject* receiver, const char* slot)
{
    QObject::connect(editor, SIGNAL(editingFinished()), receiver, slot);
}

bool GCF::Components::PointEditorCreator::setEditorValue(QWidget* editor, QVariant value)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(lineEdit)
    {
        QString text = toString(value);
        lineEdit->setText(text);
    }
    return lineEdit != 0;
}

QVariant GCF::Components::PointEditorCreator::editorValue(QWidget* editor)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(lineEdit)
    {
        QString text = lineEdit->text();
        return fromString(text);
    }
    return QVariant();
}

void GCF::Components::PointEditorCreator::closeEditor(QWidget* editor)
{
    Q_UNUSED(editor);
}

QString GCF::Components::PointEditorCreator::toString(QVariant value)
{
    Point3D point = value.value<Point3D>();
    QString ret = QString("%1, %2, %3")
                    .arg(point.x, 0, 'g', 4)
                    .arg(point.y, 0, 'g', 4)
                    .arg(point.z, 0, 'g', 4);
    return ret;
}

QVariant GCF::Components::PointEditorCreator::fromString(QString value)
{
    QStringList comps = value.split(',');
    Point3D point;

    if(comps.count() >= 1)
        point.x = comps[0].simplified().toDouble();
    if(comps.count() >= 2)
        point.y = comps[1].simplified().toDouble();
    if(comps.count() >= 3)
        point.z = comps[2].simplified().toDouble();

    return qVariantFromValue<Point3D>(point);
}

bool GCF::Components::PointEditorCreator::paintValue(QPainter* paint, QRect rect, QVariant value)
{
    paint->drawText(rect, Qt::AlignLeft|Qt::AlignVCenter, toString(value));
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GCF::Components::VectorEditorCreator
////////////////////////////////////////////////////////////////////////////////////////////

GCF_BEGIN_QUERY_TABLE(GCF::Components::VectorEditorCreator)
    GCF_IMPLEMENTS(IValueEditorCreator)
GCF_END_QUERY_TABLE(GCF::Components::VectorEditorCreator)

const char* GCF::Components::VectorEditorCreator::typeName()
{
    return "Vector3D";
}

QWidget* GCF::Components::VectorEditorCreator::createEditor(QWidget* parent)
{
    QLineEdit* lineEdit = new QLineEdit(parent);
    lineEdit->setFrame(true);
    return lineEdit;
}

void GCF::Components::VectorEditorCreator::connectEditor(QWidget* editor, QObject* receiver, const char* slot)
{
    QObject::connect(editor, SIGNAL(editingFinished()), receiver, slot);
}

bool GCF::Components::VectorEditorCreator::setEditorValue(QWidget* editor, QVariant value)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(lineEdit)
    {
        QString text = toString(value);
        lineEdit->setText(text);
    }
    return lineEdit != 0;
}

QVariant GCF::Components::VectorEditorCreator::editorValue(QWidget* editor)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(lineEdit)
    {
        QString text = lineEdit->text();
        return fromString(text);
    }
    return QVariant();
}

void GCF::Components::VectorEditorCreator::closeEditor(QWidget* editor)
{
    Q_UNUSED(editor);
}

QString GCF::Components::VectorEditorCreator::toString(QVariant value)
{
    Vector3D vector = value.value<Vector3D>();
    QString ret = QString("%1, %2, %3")
                    .arg(vector.x, 0, 'g', 4)
                    .arg(vector.y, 0, 'g', 4)
                    .arg(vector.z, 0, 'g', 4);
    return ret;
}

QVariant GCF::Components::VectorEditorCreator::fromString(QString value)
{
    QStringList comps = value.split(',');
    Vector3D vector;

    if(comps.count() >= 1)
        vector.x = comps[0].simplified().toDouble();
    if(comps.count() >= 2)
        vector.y = comps[1].simplified().toDouble();
    if(comps.count() >= 3)
        vector.z = comps[2].simplified().toDouble();

    return qVariantFromValue<Vector3D>(vector);
}

bool GCF::Components::VectorEditorCreator::paintValue(QPainter* paint, QRect rect, QVariant value)
{
    paint->drawText(rect, Qt::AlignLeft|Qt::AlignVCenter, toString(value));
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GCF::Components::TriangleEditorCreator
////////////////////////////////////////////////////////////////////////////////////////////

GCF_BEGIN_QUERY_TABLE(GCF::Components::TriangleEditorCreator)
    GCF_IMPLEMENTS(IValueEditorCreator)
GCF_END_QUERY_TABLE(GCF::Components::TriangleEditorCreator)

const char* GCF::Components::TriangleEditorCreator::typeName()
{
    return "Triangle";
}

QWidget* GCF::Components::TriangleEditorCreator::createEditor(QWidget* parent)
{
    QLineEdit* lineEdit = new QLineEdit(parent);
    lineEdit->setFrame(true);
    return lineEdit;
}

void GCF::Components::TriangleEditorCreator::connectEditor(QWidget* editor, QObject* receiver, const char* slot)
{
    QObject::connect(editor, SIGNAL(editingFinished()), receiver, slot);
}

bool GCF::Components::TriangleEditorCreator::setEditorValue(QWidget* editor, QVariant value)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(lineEdit)
    {
        QString text = toString(value);
        lineEdit->setText(text);
    }
    return lineEdit != 0;
}

QVariant GCF::Components::TriangleEditorCreator::editorValue(QWidget* editor)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(lineEdit)
    {
        QString text = lineEdit->text();
        return fromString(text);
    }
    return QVariant();
}

void GCF::Components::TriangleEditorCreator::closeEditor(QWidget* editor)
{
    Q_UNUSED(editor);
}

QString GCF::Components::TriangleEditorCreator::toString(QVariant value)
{
    Triangle triangle = value.value<Triangle>();
    QString ret = QString("%1, %2, %3")
                    .arg(triangle.a)
                    .arg(triangle.b)
                    .arg(triangle.c);
    return ret;
}

QVariant GCF::Components::TriangleEditorCreator::fromString(QString value)
{
    QStringList comps = value.split(',');
    Triangle triangle;

    if(comps.count() >= 1)
        triangle.a = comps[0].simplified().toInt();
    if(comps.count() >= 2)
        triangle.b = comps[1].simplified().toInt();
    if(comps.count() >= 3)
        triangle.c = comps[2].simplified().toInt();

    return qVariantFromValue<Triangle>(triangle);
}

bool GCF::Components::TriangleEditorCreator::paintValue(QPainter* paint, QRect rect, QVariant value)
{
    paint->drawText(rect, Qt::AlignLeft|Qt::AlignVCenter, toString(value));
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GCF::Components::BoundingBoxEditorCreator
////////////////////////////////////////////////////////////////////////////////////////////

GCF_BEGIN_QUERY_TABLE(GCF::Components::BoundingBoxEditorCreator)
    GCF_IMPLEMENTS(IValueEditorCreator)
GCF_END_QUERY_TABLE(GCF::Components::BoundingBoxEditorCreator)

const char* GCF::Components::BoundingBoxEditorCreator::typeName()
{
    return "BoundingBox3D";
}

QWidget* GCF::Components::BoundingBoxEditorCreator::createEditor(QWidget* parent)
{
    QLineEdit* lineEdit = new QLineEdit(parent);
    lineEdit->setFrame(true);
    return lineEdit;
}

void GCF::Components::BoundingBoxEditorCreator::connectEditor(QWidget* editor, QObject* receiver, const char* slot)
{
    QObject::connect(editor, SIGNAL(editingFinished()), receiver, slot);
}

bool GCF::Components::BoundingBoxEditorCreator::setEditorValue(QWidget* editor, QVariant value)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(lineEdit)
    {
        QString text = toString(value);
        lineEdit->setText(text);
    }
    return lineEdit != 0;
}

QVariant GCF::Components::BoundingBoxEditorCreator::editorValue(QWidget* editor)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(lineEdit)
    {
        QString text = lineEdit->text();
        return fromString(text);
    }
    return QVariant();
}

void GCF::Components::BoundingBoxEditorCreator::closeEditor(QWidget* editor)
{
    Q_UNUSED(editor);
}

QString GCF::Components::BoundingBoxEditorCreator::toString(QVariant value)
{
    BoundingBox3D bbox = value.value<BoundingBox3D>();
    QString ret = QString("%1, %2, %3, %4, %5, %6")
                    .arg(bbox.xMin, 0, 'g', 4)
                    .arg(bbox.yMin, 0, 'g', 4)
                    .arg(bbox.zMin, 0, 'g', 4)
                    .arg(bbox.xMax, 0, 'g', 4)
                    .arg(bbox.yMax, 0, 'g', 4)
                    .arg(bbox.zMax, 0, 'g', 4);
    return ret;
}

QVariant GCF::Components::BoundingBoxEditorCreator::fromString(QString value)
{
    QStringList comps = value.split(',');
    BoundingBox3D bbox;

    if(comps.count() >= 1)
        bbox.xMin = comps[0].simplified().toDouble();
    if(comps.count() >= 2)
        bbox.yMin = comps[1].simplified().toDouble();
    if(comps.count() >= 3)
        bbox.zMin = comps[2].simplified().toDouble();
    if(comps.count() >= 4)
        bbox.xMax = comps[3].simplified().toDouble();
    if(comps.count() >= 5)
        bbox.yMax = comps[4].simplified().toDouble();
    if(comps.count() >= 6)
        bbox.zMax = comps[5].simplified().toDouble();

    return qVariantFromValue<BoundingBox3D>(bbox);
}

bool GCF::Components::BoundingBoxEditorCreator::paintValue(QPainter* paint, QRect rect, QVariant value)
{
    paint->drawText(rect, Qt::AlignLeft|Qt::AlignVCenter, toString(value));
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GCF::Components::ValueRangeEditorCreator
////////////////////////////////////////////////////////////////////////////////////////////

GCF_BEGIN_QUERY_TABLE(GCF::Components::ValueRangeEditorCreator)
    GCF_IMPLEMENTS(IValueEditorCreator)
GCF_END_QUERY_TABLE(GCF::Components::ValueRangeEditorCreator)

const char* GCF::Components::ValueRangeEditorCreator::typeName()
{
    return "ValueRange";
}

QWidget* GCF::Components::ValueRangeEditorCreator::createEditor(QWidget* parent)
{
    QLineEdit* lineEdit = new QLineEdit(parent);
    lineEdit->setFrame(true);
    return lineEdit;
}

void GCF::Components::ValueRangeEditorCreator::connectEditor(QWidget* editor, QObject* receiver, const char* slot)
{
    QObject::connect(editor, SIGNAL(editingFinished()), receiver, slot);
}

bool GCF::Components::ValueRangeEditorCreator::setEditorValue(QWidget* editor, QVariant value)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(lineEdit)
    {
        QString text = toString(value);
        lineEdit->setText(text);
    }
    return lineEdit != 0;
}

QVariant GCF::Components::ValueRangeEditorCreator::editorValue(QWidget* editor)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(lineEdit)
    {
        QString text = lineEdit->text();
        return fromString(text);
    }
    return QVariant();
}

void GCF::Components::ValueRangeEditorCreator::closeEditor(QWidget* editor)
{
    Q_UNUSED(editor);
}

QString GCF::Components::ValueRangeEditorCreator::toString(QVariant value)
{
    ValueRange valueRange = value.value<ValueRange>();
    QString ret = QString("%1, %2")
                    .arg(valueRange.min, 0, 'g', 4)
                    .arg(valueRange.max, 0, 'g', 4);
    return ret;
}

QVariant GCF::Components::ValueRangeEditorCreator::fromString(QString value)
{
    QStringList comps = value.split(',');
    ValueRange valueRange;

    if(comps.count() >= 1)
        valueRange.min = comps[0].simplified().toDouble();
    if(comps.count() >= 2)
        valueRange.max = comps[1].simplified().toDouble();

    return qVariantFromValue<ValueRange>(valueRange);
}

bool GCF::Components::ValueRangeEditorCreator::paintValue(QPainter* paint, QRect rect, QVariant value)
{
    paint->drawText(rect, Qt::AlignLeft|Qt::AlignVCenter, toString(value));
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GCF::Components::ListEditorCreator
////////////////////////////////////////////////////////////////////////////////////////////

GCF_BEGIN_QUERY_TABLE(GCF::Components::ListEditorCreator)
    GCF_IMPLEMENTS(IValueEditorCreator)
GCF_END_QUERY_TABLE(GCF::Components::ListEditorCreator)

GCF::Components::ListEditorCreator::ListEditorCreator(bool doubleList)
:IsDoubleList(doubleList)
{

}

const char* GCF::Components::ListEditorCreator::typeName()
{
    if( IsDoubleList )
        return "QList<double>";

    return "QList<int>";
}

QWidget* GCF::Components::ListEditorCreator::createEditor(QWidget* parent)
{
    QLineEdit* lineEdit = new QLineEdit(parent);
    lineEdit->setFrame(true);
    return lineEdit;
}

void GCF::Components::ListEditorCreator::connectEditor(QWidget* editor, QObject* receiver, const char* slot)
{
    QObject::connect(editor, SIGNAL(editingFinished()), receiver, slot);
}

bool GCF::Components::ListEditorCreator::setEditorValue(QWidget* editor, QVariant value)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(lineEdit)
    {
        QString text = toString(value);
        lineEdit->setText(text);
        lineEdit->setProperty("_ListSize_", value.toList().count());
    }
    return lineEdit != 0;
}

QVariant GCF::Components::ListEditorCreator::editorValue(QWidget* editor)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(lineEdit)
    {
        QString text = lineEdit->text();
        QVariant ret = fromString(text);
        QList<QVariant> retList = ret.toList();
        int expectedSize = lineEdit->property("_ListSize_").toInt();
        while(retList.count() < expectedSize)
            retList << 0;
        return retList;
    }
    return QVariant();
}

void GCF::Components::ListEditorCreator::closeEditor(QWidget* editor)
{
    Q_UNUSED(editor);
}

QString GCF::Components::ListEditorCreator::toString(QVariant value)
{
    QList<QVariant> list = value.toList();
    if(!list.count())
        return QString();

    QString ret;
    if(IsDoubleList)
    {
        for(int i=0; i<list.count(); i++)
        {
            if(ret.isEmpty())
                ret += QString("%1").arg(list[i].toDouble(), 0, 'g', 4);
            else
                ret += QString(", %1").arg(list[i].toDouble(), 0, 'g', 4);
        }
    }
    else
    {
        for(int i=0; i<list.count(); i++)
        {
            if(ret.isEmpty())
                ret += QString("%1").arg(list[i].toInt());
            else
                ret += QString(", %1").arg(list[i].toInt());
        }
    }

    return ret;
}

QVariant GCF::Components::ListEditorCreator::fromString(QString value)
{
    QStringList comps = value.split(',');
    QList<QVariant> ret;

    if(comps.count() == 0)
        return ret;

    for(int i=0; i<comps.count(); i++)
    {
        QString comp = comps[i];
        if(IsDoubleList)
            ret << comp.toDouble();
        else
            ret << comp.toInt();
    }

    return ret;
}

bool GCF::Components::ListEditorCreator::paintValue(QPainter* paint, QRect rect, QVariant value)
{
    paint->drawText(rect, Qt::AlignLeft|Qt::AlignVCenter, toString(value));
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GCF::Components::ColorEditorCreator
////////////////////////////////////////////////////////////////////////////////////////////

GCF_BEGIN_QUERY_TABLE(GCF::Components::ColorEditorCreator)
    GCF_IMPLEMENTS(IValueEditorCreator)
GCF_END_QUERY_TABLE(GCF::Components::ColorEditorCreator)

const char* GCF::Components::ColorEditorCreator::typeName()
{
    return QVariant::typeToName(QVariant::Color);
}

QWidget* GCF::Components::ColorEditorCreator::createEditor(QWidget* parent)
{
    return new GCF::Components::ColorEdit(parent);
}

void GCF::Components::ColorEditorCreator::connectEditor(QWidget* editor, QObject* receiver, const char* slot)
{
    QObject::connect(editor, SIGNAL(colorChanged(QColor)), receiver, slot);
}

bool GCF::Components::ColorEditorCreator::setEditorValue(QWidget* editor, QVariant value)
{
    GCF::Components::ColorEdit* colorEdit = qobject_cast<GCF::Components::ColorEdit*>(editor);
    if(colorEdit)
    {
        colorEdit->setColor( value.value<QColor>() );
        return true;
    }

    return false;
}

QVariant GCF::Components::ColorEditorCreator::editorValue(QWidget* editor)
{
    GCF::Components::ColorEdit* colorEdit = qobject_cast<GCF::Components::ColorEdit*>(editor);
    if(colorEdit)
    {
        return qVariantFromValue<QColor>( colorEdit->color() );
    }
    return QVariant();
}

void GCF::Components::ColorEditorCreator::closeEditor(QWidget* editor)
{
    Q_UNUSED(editor);
}

QString GCF::Components::ColorEditorCreator::toString(QVariant value)
{
    QColor color = value.value<QColor>();
    if(color == Qt::transparent)
        return "transparent";
    return color.name();
}

QVariant GCF::Components::ColorEditorCreator::fromString(QString value)
{
    if(value == "transparent")
        return Qt::transparent;

    QColor color(value);
    return qVariantFromValue<QColor>(color);
}

struct ColorInfo
{
    ColorInfo() { }
    ColorInfo(QColor c, QString n)
        : color(c), name(n) { }

    QString name;
    QColor color;
};

bool GCF::Components::ColorEditorCreator::paintValue(QPainter* paint, QRect rect, QVariant value)
{
    static QList<ColorInfo> colorInfoList;

    if(colorInfoList.isEmpty())
    {
        colorInfoList << ColorInfo(Qt::black, "Black");
        colorInfoList << ColorInfo(Qt::white, "White");
        colorInfoList << ColorInfo(Qt::red, "Red");
        colorInfoList << ColorInfo(Qt::darkRed, "Dark red");
        colorInfoList << ColorInfo(Qt::green, "Green");
        colorInfoList << ColorInfo(Qt::darkGreen, "Dark green");
        colorInfoList << ColorInfo(Qt::blue, "Blue");
        colorInfoList << ColorInfo(Qt::darkBlue, "Dark blue");
        colorInfoList << ColorInfo(Qt::cyan, "Cyan");
        colorInfoList << ColorInfo(Qt::darkCyan, "Dark cyan");
        colorInfoList << ColorInfo(Qt::magenta, "Magenta");
        colorInfoList << ColorInfo(Qt::darkMagenta, "Dark magenta");
        colorInfoList << ColorInfo(Qt::yellow, "Yellow");
        colorInfoList << ColorInfo(Qt::darkYellow, "Dark yellow");
        colorInfoList << ColorInfo(Qt::gray, "Gray");
        colorInfoList << ColorInfo(Qt::darkGray, "Dark gray");
        colorInfoList << ColorInfo(Qt::lightGray, "Light gray");
    }

    QString colorName = "User Color";
    QColor color = value.value<QColor>();
    QBrush brush = paint->brush();

    for(int i=0; i<colorInfoList.count(); i++)
    {
        if(colorInfoList[i].color == color)
        {
            colorName = colorInfoList[i].name;
            break;
        }
    }
    paint->setBrush(color);
    paint->drawRect(rect.left()+3, rect.center().y()-6, 12, 12);
    paint->drawText(rect.adjusted(18, 0, 0, 0), Qt::AlignVCenter|Qt::AlignLeft, colorName);
    paint->setBrush(brush);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GCF::Components::FileNameEditorCreator
////////////////////////////////////////////////////////////////////////////////////////////

GCF_BEGIN_QUERY_TABLE(GCF::Components::FileNameEditorCreator)
    GCF_IMPLEMENTS(IValueEditorCreator)
GCF_END_QUERY_TABLE(GCF::Components::FileNameEditorCreator)

const char* GCF::Components::FileNameEditorCreator::typeName()
{
    return QVariant::typeToName(QVariant::Url);
}

QWidget* GCF::Components::FileNameEditorCreator::createEditor(QWidget* parent)
{
    GCF::Components::UrlEdit* urlEdit = new GCF::Components::UrlEdit(parent);
    return urlEdit;
}

void GCF::Components::FileNameEditorCreator::connectEditor(QWidget* editor, QObject* receiver, const char* slot)
{
    QObject::connect(editor, SIGNAL(urlChanged(const QUrl&)), receiver, slot);
}

bool GCF::Components::FileNameEditorCreator::setEditorValue(QWidget* editor, QVariant value)
{
    GCF::Components::UrlEdit* urlEdit = qobject_cast<GCF::Components::UrlEdit*>(editor);
    if(urlEdit)
    {
        QUrl url = value.toUrl();
        urlEdit->setUrl(url);
    }
    return urlEdit != 0;
}

QVariant GCF::Components::FileNameEditorCreator::editorValue(QWidget* editor)
{
    GCF::Components::UrlEdit* urlEdit = qobject_cast<GCF::Components::UrlEdit*>(editor);
    if(urlEdit)
        return urlEdit->url();
    return QVariant();
}

void GCF::Components::FileNameEditorCreator::closeEditor(QWidget* editor)
{
    Q_UNUSED(editor);
}

QString GCF::Components::FileNameEditorCreator::toString(QVariant value)
{
    QUrl url = value.toUrl();
    return url.toLocalFile();
}

QVariant GCF::Components::FileNameEditorCreator::fromString(QString value)
{
    return QUrl::fromLocalFile(value);
}

bool GCF::Components::FileNameEditorCreator::paintValue(QPainter* paint, QRect rect, QVariant value)
{
    paint->drawText(rect, Qt::AlignVCenter|Qt::AlignLeft, toString(value));
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GCF::Components::UrlEdit
////////////////////////////////////////////////////////////////////////////////////////////

struct GCF::Components::UrlEditData
{
    QPushButton* urlButton;
    QPushButton* clearButton;
    QUrl url;
};

GCF::Components::UrlEdit::UrlEdit(QWidget* parent)
:QWidget(parent)
{
    d = new GCF::Components::UrlEditData;
    d->urlButton = new QPushButton(this);
    d->urlButton->setText("...");
    d->clearButton = new QPushButton(this);
    d->clearButton->setIcon(qApp->style()->standardIcon(QStyle::SP_TrashIcon));
    connect(d->urlButton, SIGNAL(clicked()), this, SLOT(on_urlButton_clicked()));
    connect(d->clearButton, SIGNAL(clicked()), this, SLOT(on_clearButton_clicked()));
}

GCF::Components::UrlEdit::~UrlEdit()
{
    delete d;
}

void GCF::Components::UrlEdit::setUrl(QUrl url)
{
    d->url = url;
    update();
}

QUrl GCF::Components::UrlEdit::url() const
{
    return d->url;
}

void GCF::Components::UrlEdit::on_urlButton_clicked()
{
    QString file = d->url.toLocalFile();
    QFileInfo fi(file);
    file = QFileDialog::getOpenFileName(this, "Select file to open", fi.absolutePath());
    if(file.isEmpty())
        return;
    QUrl url = QUrl::fromLocalFile(file);
    if(url != d->url)
    {
        setUrl(url);
        emit urlChanged(url);
    }
}

void GCF::Components::UrlEdit::on_clearButton_clicked()
{
    if(d->url.isEmpty())
        return;

    QUrl nullUrl;
    setUrl(nullUrl);
    emit urlChanged(nullUrl);
}

void GCF::Components::UrlEdit::paintEvent(QPaintEvent*)
{
    QPainter paint(this);
    QString fileName = d->url.toLocalFile();
    QRect rect = this->rect();
    rect.setRight(rect.right()-d->clearButton->width()-d->urlButton->width());

    if(rect.width() < fontMetrics().width(fileName))
    {
        int scw = fontMetrics().width("_");
        int nrChars = rect.width() / scw - 4;
        fileName = fileName.left(nrChars) + "...";
    }

    paint.drawText(rect, Qt::AlignVCenter|Qt::AlignLeft, fileName);
}

void GCF::Components::UrlEdit::resizeEvent(QResizeEvent*)
{
    int h = this->height();
    d->clearButton->resize(h, h);
    d->urlButton->resize(h, h);
    d->urlButton->move(width()-2*h, 0);
    d->clearButton->move(width()-h, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GCF::Components::ColorEdit
////////////////////////////////////////////////////////////////////////////////////////////

struct GCF::Components::ColorEditData
{
    GCF::Components::ColorPickerWidget* colorPickerWidget;
    QMenu* actionMenu;
    QWidgetAction* cpwAction;
    QColor color;
};

GCF::Components::ColorEdit::ColorEdit(QWidget* parent)
:QToolButton(parent)
{
    d = new GCF::Components::ColorEditData;

    d->actionMenu = new QMenu(this);
    setMenu(d->actionMenu);
    setPopupMode(QToolButton::InstantPopup);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    d->colorPickerWidget = new GCF::Components::ColorPickerWidget(d->actionMenu);
    d->cpwAction = new QWidgetAction(d->actionMenu->menuAction());
    d->cpwAction->setDefaultWidget(d->colorPickerWidget);
    d->actionMenu->addAction(d->cpwAction);
    d->color = Qt::white;
    setColor(Qt::black);

    connect(d->colorPickerWidget, SIGNAL(currentColorChanged(QColor)),
            this, SLOT(on_currentColorChanged(QColor)));
}

GCF::Components::ColorEdit::~ColorEdit()
{
    d->actionMenu->deleteLater();
    d->cpwAction->deleteLater();
    d->colorPickerWidget->deleteLater();
    delete d;
}

void GCF::Components::ColorEdit::setColor(QColor color)
{
    if(d->color == color)
        return;

    d->color = color;

    QPixmap pm(10, 10);
    pm.fill(color);

    d->colorPickerWidget->setCurrentColor(color);
    setText( color.name() );
    setIcon( QIcon(pm) );

    emit colorChanged(color);
}

QColor GCF::Components::ColorEdit::color() const
{
    return d->color;
}

void GCF::Components::ColorEdit::on_currentColorChanged(QColor color)
{
    setColor( color );
}


