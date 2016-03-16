//-----------------------------------------------------------------------------
// File: MonitorInterconnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.08.2015
//
// Description:
// Describes the ipxact:monitorInterconnection element in an IP-XACT document.
//-----------------------------------------------------------------------------

#include "MonitorInterconnection.h"

//-----------------------------------------------------------------------------
// Function: MonitorInterconnection::MonitorInterconnection()
//-----------------------------------------------------------------------------
MonitorInterconnection::MonitorInterconnection(QString const name,
    QSharedPointer<MonitorInterface> activeInterface, QString const& displayName /* = QString() */,
    QString const& description /* = QString() */) :
NameGroup(name, displayName, description),
isPresent_(),
monitoredActiveInterface_(activeInterface),
monitorInterfaces_(new QList<QSharedPointer<MonitorInterface> > ())
{

}

//-----------------------------------------------------------------------------
// Function: MonitorInterconnection::MonitorInterconnection()
//-----------------------------------------------------------------------------
MonitorInterconnection::MonitorInterconnection() :
NameGroup(),
isPresent_(),
monitoredActiveInterface_(new MonitorInterface()),
monitorInterfaces_(new QList<QSharedPointer<MonitorInterface> > ())
{

}

//-----------------------------------------------------------------------------
// Function: MonitorInterconnection::MonitorInterconnection()
//-----------------------------------------------------------------------------
MonitorInterconnection::MonitorInterconnection(const MonitorInterconnection& other) :
NameGroup(other),
isPresent_(other.isPresent_),
monitoredActiveInterface_(new MonitorInterface(*other.monitoredActiveInterface_.data())),
monitorInterfaces_(new QList<QSharedPointer<MonitorInterface> > ())
{
    foreach (QSharedPointer<MonitorInterface> singleInterface, *other.monitorInterfaces_)
    {
        if (singleInterface)
        {
            QSharedPointer<MonitorInterface> copy =
                QSharedPointer<MonitorInterface> (new MonitorInterface(*singleInterface.data()));
            monitorInterfaces_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MonitorInterconnection::~MonitorInterconnection()
//-----------------------------------------------------------------------------
MonitorInterconnection::~MonitorInterconnection()
{
    monitoredActiveInterface_.clear();
    monitorInterfaces_.clear();
}

//-----------------------------------------------------------------------------
// Function: MonitorInterconnection::operator=()
//-----------------------------------------------------------------------------
MonitorInterconnection& MonitorInterconnection::operator=(const MonitorInterconnection& other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);

        isPresent_ = other.isPresent_;

        monitoredActiveInterface_.clear();
        monitoredActiveInterface_ =
            QSharedPointer<MonitorInterface>(new MonitorInterface(*other.monitoredActiveInterface_.data()));

        monitorInterfaces_->clear();
        foreach (QSharedPointer<MonitorInterface> singleInterface, *other.monitorInterfaces_)
        {
            QSharedPointer<MonitorInterface> copy =
                QSharedPointer<MonitorInterface>(new MonitorInterface(*singleInterface.data()));
            monitorInterfaces_->append(copy);
        }
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: MonitorInterconnection::getMonitoredActiveInterface()
//-----------------------------------------------------------------------------
QSharedPointer<MonitorInterface> MonitorInterconnection::getMonitoredActiveInterface() const
{
    return monitoredActiveInterface_;
}

//-----------------------------------------------------------------------------
// Function: MonitorInterconnection::setMonitoredctiveInterface()
//-----------------------------------------------------------------------------
void MonitorInterconnection::setMonitoredctiveInterface(QSharedPointer<MonitorInterface> newActiveInterface)
{
    monitoredActiveInterface_ = newActiveInterface;
}

//-----------------------------------------------------------------------------
// Function: MonitorInterconnection::getMonitorInterfaces()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<MonitorInterface> > > MonitorInterconnection::getMonitorInterfaces() const
{
    return monitorInterfaces_;
}

//-----------------------------------------------------------------------------
// Function: MonitorInterconnection::setMonitorInterfaces()
//-----------------------------------------------------------------------------
void MonitorInterconnection::setMonitorInterfaces(
    QSharedPointer<QList<QSharedPointer<MonitorInterface> > > newMonitorInterfaces)
{
    monitorInterfaces_ = newMonitorInterfaces;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::getIsPresent()
//-----------------------------------------------------------------------------
QString MonitorInterconnection::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::setIsPresent()
//-----------------------------------------------------------------------------
void MonitorInterconnection::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}
