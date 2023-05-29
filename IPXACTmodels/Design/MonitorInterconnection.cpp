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
MonitorInterconnection::MonitorInterconnection(std::string const& name,
    QSharedPointer<MonitorInterface> activeInterface, std::string const& displayName,
    std::string const& description) :
NameGroup(name, displayName, description),
monitoredActiveInterface_(activeInterface)
{

}

//-----------------------------------------------------------------------------
// Function: MonitorInterconnection::MonitorInterconnection()
//-----------------------------------------------------------------------------
MonitorInterconnection::MonitorInterconnection() :
NameGroup()
{

}

//-----------------------------------------------------------------------------
// Function: MonitorInterconnection::MonitorInterconnection()
//-----------------------------------------------------------------------------
MonitorInterconnection::MonitorInterconnection(const MonitorInterconnection& other) :
NameGroup(other),
isPresent_(other.isPresent_),
monitoredActiveInterface_(new MonitorInterface(*other.monitoredActiveInterface_))
{
    for (auto const& singleInterface : *other.monitorInterfaces_)
    {
        QSharedPointer<MonitorInterface> copy =
            QSharedPointer<MonitorInterface>(new MonitorInterface(*singleInterface));
        monitorInterfaces_->append(copy);
    }
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
            QSharedPointer<MonitorInterface>(new MonitorInterface(*other.monitoredActiveInterface_));

        monitorInterfaces_->clear();
        for (auto const& singleInterface : *other.monitorInterfaces_)
        {
            QSharedPointer<MonitorInterface> copy =
                QSharedPointer<MonitorInterface>(new MonitorInterface(*singleInterface));
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
std::string MonitorInterconnection::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: Interconnection::setIsPresent()
//-----------------------------------------------------------------------------
void MonitorInterconnection::setIsPresent(std::string const& newIsPresent)
{
    isPresent_ = newIsPresent;
}
