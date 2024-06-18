//-----------------------------------------------------------------------------
// File: Model.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.10.2015
//
// Description:
// Describes the ipxact:model element in an IP-XACT document
//-----------------------------------------------------------------------------

#include "Model.h"
#include "View.h"
#include "ComponentInstantiation.h"
#include "DesignInstantiation.h"
#include "DesignConfigurationInstantiation.h"
#include "Port.h"

#include <IPXACTmodels/utilities/Search.h>
#include <IPXACTmodels/utilities/Copy.h>

//-----------------------------------------------------------------------------
// Function: Model::Model()
//-----------------------------------------------------------------------------
Model::Model(Model const& other)
{
    Copy::copyList(other.views_, views_); 
    Copy::copyList(other.componentInstantiations_, componentInstantiations_);
    Copy::copyList(other.designInstantiations_, designInstantiations_);
    Copy::copyList(other.designConfigurationInstantiations_, designConfigurationInstantiations_);
    Copy::copyList(other.ports_, ports_);
}

//-----------------------------------------------------------------------------
// Function: Model::operator=()
//-----------------------------------------------------------------------------
Model& Model::operator=(Model const& other )
{
    if (this != &other)
    {
        views_->clear();
        Copy::copyList(other.views_, views_);
        componentInstantiations_->clear();
        Copy::copyList(other.componentInstantiations_, componentInstantiations_);
        designInstantiations_->clear();
        Copy::copyList(other.designInstantiations_, designInstantiations_);
        designConfigurationInstantiations_->clear();
        Copy::copyList(other.designConfigurationInstantiations_, designConfigurationInstantiations_);
        ports_->clear();
        Copy::copyList(other.ports_, ports_);
    }
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Model::getViews()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<View> > > Model::getViews() const
{
    return views_;
}

//-----------------------------------------------------------------------------
// Function: Model::setViews()
//-----------------------------------------------------------------------------
void Model::setViews(QSharedPointer<QList<QSharedPointer<View> > > newViews)
{
	views_ = newViews;
}

//-----------------------------------------------------------------------------
// Function: Model::findComponentInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstantiation> Model::findComponentInstantiation(QString const& name) const
{
	return Search::findByName(name, componentInstantiations_);
}

//-----------------------------------------------------------------------------
// Function: Model::getComponentInstantiations()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > Model::getComponentInstantiations() const
{
    return componentInstantiations_;
}

//-----------------------------------------------------------------------------
// Function: Model::setComponentInstantiations()
//-----------------------------------------------------------------------------
void Model::setComponentInstantiations(
    QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > instantiations)
{
    componentInstantiations_ = instantiations;
}

//-----------------------------------------------------------------------------
// Function: Model::findDesignInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<DesignInstantiation> Model::findDesignInstantiation(QString const& name) const
{
    return Search::findByName(name, designInstantiations_);
}

//-----------------------------------------------------------------------------
// Function: Model::getDesignInstantiations()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<DesignInstantiation> > > Model::getDesignInstantiations() const
{
    return designInstantiations_;
}

//-----------------------------------------------------------------------------
// Function: Model::setDesignInstantiations()
//-----------------------------------------------------------------------------
void Model::setDesignInstantiations(QSharedPointer<QList<QSharedPointer<DesignInstantiation> > > newInstantiations)
{
    designInstantiations_ = newInstantiations;
}

//-----------------------------------------------------------------------------
// Function: Model::findDesignInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfigurationInstantiation> Model::findDesignConfigurationInstantiation(QString const& name) 
    const
{
    return Search::findByName(name, designConfigurationInstantiations_);
}

//-----------------------------------------------------------------------------
// Function: Model::getDesignConfigurationInstantiations()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<DesignConfigurationInstantiation> > > 
    Model::getDesignConfigurationInstantiations() const
{
    return designConfigurationInstantiations_;
}

//-----------------------------------------------------------------------------
// Function: Model::setDesignConfigurationInstantiations()
//-----------------------------------------------------------------------------
void Model::setDesignConfigurationInstantiations
    (QSharedPointer<QList<QSharedPointer<DesignConfigurationInstantiation> > > newInstantiations)
{
    designConfigurationInstantiations_ = newInstantiations;
}

//-----------------------------------------------------------------------------
// Function: Model::getPorts()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Port> > > Model::getPorts() const
{
    return ports_;
}

//-----------------------------------------------------------------------------
// Function: Model::setPorts()
//-----------------------------------------------------------------------------
void Model::setPorts(QSharedPointer<QList<QSharedPointer<Port> > > newPorts)
{
    ports_ = newPorts;
}

//-----------------------------------------------------------------------------
// Function: Model::hasViews()
//-----------------------------------------------------------------------------
bool Model::hasViews() const
{
    return !views_->isEmpty();
}

//-----------------------------------------------------------------------------
// Function: Model::getViewNames()
//-----------------------------------------------------------------------------
QStringList Model::getViewNames() const
{
    return Search::getNames(views_);
}

//-----------------------------------------------------------------------------
// Function: Model::getHierViews()
//-----------------------------------------------------------------------------
QStringList Model::getHierViews() const
{
    QStringList viewList;
    for (QSharedPointer<View> view : *views_)
    {
        if (view->isHierarchical())
        {
            viewList.append(view->name());
        }
    }
    return viewList;
}

//-----------------------------------------------------------------------------
// Function: Model::getFlatViews()
//-----------------------------------------------------------------------------
QStringList Model::getFlatViews() const
{
    QStringList viewList;
    for (QSharedPointer<View> view : *views_)
    {
        if (!view->isHierarchical())
        {
            viewList.append(view->name());
        }
    }
    return viewList;
}

//-----------------------------------------------------------------------------
// Function: Model::findView()
//-----------------------------------------------------------------------------
QSharedPointer<View> Model::findView(const QString name) const
{
	return Search::findByName(name, views_);
}

//-----------------------------------------------------------------------------
// Function: Model::getHierRef()
//-----------------------------------------------------------------------------
VLNV Model::getHierRef(QString const& viewName) const
{
    QSharedPointer<View> view = findView(viewName);
    if (view)
    {
        if (!view->getDesignConfigurationInstantiationRef().isEmpty())
        {
            QSharedPointer<DesignConfigurationInstantiation> viewDesignConfiguration =
                findDesignConfigurationInstantiation(view->getDesignConfigurationInstantiationRef());
            if (viewDesignConfiguration)
            {
                return *viewDesignConfiguration->getDesignConfigurationReference();
            }
        }

        if (!view->getDesignInstantiationRef().isEmpty())
        {
            QSharedPointer<DesignInstantiation> viewDesign = 
                findDesignInstantiation(view->getDesignInstantiationRef());
            if (viewDesign)
            {
                return *viewDesign->getDesignReference();
            }
        }
    }

    return VLNV();
}

//-----------------------------------------------------------------------------
// Function: Model::getHierarchyRefs()
//-----------------------------------------------------------------------------
QList<VLNV> Model::getHierarchyRefs() const
{
    QList<VLNV> list;

    for (QSharedPointer<View> view : *views_)
    {
        if (view->isHierarchical())
        {
            QSharedPointer<DesignInstantiation> viewDesign = findDesignInstantiation(view->getDesignInstantiationRef());
            if (viewDesign)
            {
                list.append(*viewDesign->getDesignReference());
            }

            QSharedPointer<DesignConfigurationInstantiation> viewDesignConfiguration =
                findDesignConfigurationInstantiation(view->getDesignConfigurationInstantiationRef());
            if (viewDesignConfiguration)
            {
                list.append(*viewDesignConfiguration->getDesignConfigurationReference());
            }
        }
    }

    return list;
}

//-----------------------------------------------------------------------------
// Function: Model::getPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> Model::getPort(QString const& name) const
{
    return Search::findByName(name, ports_);
}

//-----------------------------------------------------------------------------
// Function: Model::hasPort()
//-----------------------------------------------------------------------------
bool Model::hasPort(QString const& name) const
{
    return getPort(name).isNull() == false;
}

//-----------------------------------------------------------------------------
// Function: Model::hasWires()
//-----------------------------------------------------------------------------
bool Model::hasWires() const
{
    return std::any_of(ports_->cbegin(), ports_->cend(), [](QSharedPointer<Port> port)
        {
            return port->getWire();
        });
}

//-----------------------------------------------------------------------------
// Function: Model::hasTransactionals()
//-----------------------------------------------------------------------------
bool Model::hasTransactionals() const
{
    return std::any_of(ports_->cbegin(), ports_->cend(), [](QSharedPointer<Port> port)
        {
            return port->getTransactional();
        });
}

//-----------------------------------------------------------------------------
// Function: Model::getPortNames()
//-----------------------------------------------------------------------------
QStringList Model::getPortNames() const
{
    return Search::getNames(ports_);
}

//-----------------------------------------------------------------------------
// Function: Model::hasPorts()
//-----------------------------------------------------------------------------
bool Model::hasPorts() const
{
    return !ports_->isEmpty();
}

//-----------------------------------------------------------------------------
// Function: Model::hasHierView()
//-----------------------------------------------------------------------------
bool Model::hasHierView() const
{
    return std::any_of(views_->cbegin(), views_->cend(), [](const auto& view) { return view->isHierarchical(); });
}

//-----------------------------------------------------------------------------
// Function: Model::getViewReferences()
//-----------------------------------------------------------------------------
QList<VLNV> Model::getViewReferences() const
{
    QList<VLNV> vlnvList;

    for (QSharedPointer<View> view : *views_)
    {
        if (!view->getDesignInstantiationRef().isEmpty())
        {
            QSharedPointer<DesignInstantiation> viewDesign =
                findDesignInstantiation(view->getDesignInstantiationRef());
            if (viewDesign)
            {
                vlnvList.append(*viewDesign->getDesignReference());
            }
        }

        if (!view->getDesignConfigurationInstantiationRef().isEmpty())
        {
            QSharedPointer<DesignConfigurationInstantiation> viewDesignConfiguration =
                findDesignConfigurationInstantiation(view->getDesignConfigurationInstantiationRef());
            if (viewDesignConfiguration)
            {
                vlnvList.append(*viewDesignConfiguration->getDesignConfigurationReference());
            }

        }
    }
    return vlnvList;
}

//-----------------------------------------------------------------------------
// Function: Model::hasContents()
//-----------------------------------------------------------------------------
bool Model::hasContents() const
{
    return (!views_->isEmpty() || !componentInstantiations_->isEmpty() || !designInstantiations_->isEmpty() ||
        !designConfigurationInstantiations_->isEmpty() || !ports_->isEmpty());
}
