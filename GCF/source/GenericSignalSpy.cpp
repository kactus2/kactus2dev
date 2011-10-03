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

// This class is based on Q4puGenericSignalSpy a class written by me.
// It is modified for use in GCF. You can download the original class
// from http://www.vcreatelogic.com/oss/signalspy/index.html

#include <GCF/GenericSignalSpy.h>
#include <QMetaObject>
#include <QMetaMethod>

/**
\class GCF::GenericSignalSpy GCF::GenericSignalSpy.h
\brief Spies for signals and slots in an object

GCF::GenericSignalSpy is a Qt4 avtar of QpuGenericSignalSpy written
for Qt3. This class took lesser time to implement because of Qt4.
The meta object system in Qt4 has received a good overhaul and the
changes are very nice. Infact the new meta object system is easier
to use than the previous ones.

\section sec1 What does this class do ?
This class looks for signals and slots emitted from a QObject in
a Qt4 application. Whenever the object that is being spied on
emits a signal or has one of its slots invoked; this class emits
a signal informing the world about it.

Take a look at the example below
\code
int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    QWidget * topLevel = new QWidget;
    QVBoxLayout * layout = new QVBoxLayout(topLevel);
    QDirModel * dirModel = new QDirModel(topLevel);
    QTreeView * treeView = new QTreeView(topLevel);
    treeView->setObjectName("DirTreeView");
    layout->addWidget(treeView);
    treeView->setModel(dirModel);
    Q3TextEdit * textEdit = new Q3TextEdit(topLevel);
        layout->addWidget(textEdit);
    GCF::GenericSignalSpy * spy = new GCF::GenericSignalSpy(&a);
    spy->spyOn(treeView);
    QObject::connect(spy, SIGNAL(caughtSignal(const QString&)),
                     textEdit, SLOT(append(const QString&)));
    QObject::connect(spy, SIGNAL(caughtSlot(const QString&)),
                     textEdit, SLOT(append(const QString&)));
    topLevel->show();
    return a.exec();
}
\endcode

*/

///////////////////////////////////////////////////////////////////////////////////

/////////////========== Copied from qobject_p.h
// qobject_p.h is not a public Qt4 API. Hence
// #include <qobject_p.h> wont work.
// So we have copied some lines from qobject_p.h
// in here to serve the purpose.
struct QSignalSpyCallbackSet
{
    typedef void (*BeginCallback)(QObject *caller, int method_index, void **argv);
    typedef void (*EndCallback)(QObject *caller, int method_index);
    BeginCallback signal_begin_callback;
    BeginCallback slot_begin_callback;
    EndCallback signal_end_callback;
    EndCallback slot_end_callback;
};
void Q_CORE_EXPORT qt_register_signal_spy_callbacks(const QSignalSpyCallbackSet &callback_set);
extern QSignalSpyCallbackSet Q_CORE_EXPORT qt_signal_spy_callback_set;
/////////////==========

void q4pugss_BeginCallBackSignal(QObject* caller, int method_index, void **argv);
void q4pugss_registerSpy(GCF::GenericSignalSpy* spy);
void q4pugss_unregisterSpy(GCF::GenericSignalSpy* spy);
static QList<GCF::GenericSignalSpy*> SpyList;

void q4pugss_registerCallBacks()
{
    QSignalSpyCallbackSet cb;
    cb.signal_begin_callback = q4pugss_BeginCallBackSignal;
    cb.signal_end_callback   = 0;
    cb.slot_begin_callback   = 0;
    cb.slot_end_callback     = 0;
    qt_register_signal_spy_callbacks(cb);
}

void q4pugss_registerSpy(GCF::GenericSignalSpy* spy)
{
    if( !SpyList.count() )
        q4pugss_registerCallBacks();
    if( SpyList.indexOf(spy) == -1 )
        SpyList.append(spy);
}

void q4pugss_unregisterSpy(GCF::GenericSignalSpy* spy)
{
    int index;
    if( (index=SpyList.indexOf(spy)) == -1 )
        return;
    SpyList.removeAt(index);
}

void q4pugss_BeginCallBackSignal(QObject* caller, int method_index, void **argv)
{
    for(int i=0; i<SpyList.count(); i++)
    {
        GCF::GenericSignalSpy * spy = SpyList[i];
        if( spy->sender() == caller )
            spy->methodInvoked(method_index, argv);
    }
}

///////////////////////////////////////////////////////////////////////////////////
// GCF::GenericSignalSpy implementation
///////////////////////////////////////////////////////////////////////////////////

struct GCF::GenericSignalSpyData
{
    GenericSignalSpyData() : sender(0), valid(false), signalIndex(-1) { }

    QObject* sender;
    QByteArray signal;
    bool valid;
    int signalIndex;
    QList<QVariant::Type> argTypes;
};

/**
\brief Constructor
*/
GCF::GenericSignalSpy::GenericSignalSpy(QObject* parent, QObject* sender, const char* signal)
                     :QObject(parent)
{
    d = new GCF::GenericSignalSpyData;
    d->sender = sender;
    d->signal = QMetaObject::normalizedSignature(signal+1);
    connect(sender, SIGNAL(destroyed(QObject*)), this, SLOT(senderDestroyed(QObject*)));

    // Search for the signal in the sender
    const QMetaObject* mo = sender->metaObject();
    d->signalIndex = mo->indexOfSignal( d->signal.constData() );

    // Decide on spy validity
    d->valid = (d->signalIndex >= 0);

    // Register for spying only if the object is valid.
    if( this->isValid() )
        q4pugss_registerSpy(this);

    // If the spy is valid, cache the argument types. This will help in
    // faster argument resolution.
    QMetaMethod metaMethod = mo->method(d->signalIndex);
    QList<QByteArray> params = metaMethod.parameterTypes();
    for(int i=0; i<params.count(); i++)
    {
        QVariant::Type type = QVariant::nameToType( params[i].constData() );
        d->argTypes << type;
    }
}

/**
\brief Destructor
*/
GCF::GenericSignalSpy::~GenericSignalSpy()
{
    if( d->valid )
        q4pugss_unregisterSpy(this);
}

/**
Returns whether the spy is valid or not. A spy is said to be valid
if it is spying on a valid QObject's signal. This means that both the
object and its signal should be valid.
*/
bool GCF::GenericSignalSpy::isValid() const
{
    return d->valid && d->sender && d->signal.size();
}

/**
Returns a pointer to the sender whose signal this class is spying.
*/
QObject* GCF::GenericSignalSpy::sender() const
{
    return d->sender;
}

/**
Returns the normalized signal name that this class is spying on.
*/
QByteArray GCF::GenericSignalSpy::signal() const
{
    return d->signal;
}

/**
Returns the signal method index of the signal being spied on.
*/
int GCF::GenericSignalSpy::signalMethodIndex() const
{
    return d->signalIndex;
}

/**
\internal
*/
void GCF::GenericSignalSpy::methodInvoked(int method_index, void **argv)
{
    if( d->signalIndex != method_index )
        return;

    // resolve arguments
    QVariantList args;
    for(int i=0; i<d->argTypes.count(); i++)
    {
        // It is argv[i+1] and not argv[i], because argv[i] contains
        // the return value.
        args << QVariant( d->argTypes[i], argv[i+1] );
    }

    // handle signal emission
    this->handleSignalEmission(args);
}

/**
You can implement this function to deal with the signal emission. The default
implementation emits a caughtSignal() signal with the arguments.
*/
void GCF::GenericSignalSpy::handleSignalEmission(const QVariantList& args)
{
    emit caughtSignal(args);
}

/**
\internal
*/
void GCF::GenericSignalSpy::senderDestroyed(QObject* object)
{
    if( d->sender == object )
    {
        d->sender = 0;
        d->signal.clear();
        d->argTypes.clear();
    }
}

