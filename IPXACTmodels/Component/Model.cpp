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

//-----------------------------------------------------------------------------
// Function: Model::Model()
//-----------------------------------------------------------------------------
Model::Model():
views_(new QList<QSharedPointer<View> > ()),
    componentInstantiations_(new QList<QSharedPointer<ComponentInstantiation> > ()),
    designInstantiations_(new QList<QSharedPointer<DesignInstantiation> > ()),
    designConfigurationInstantiations_(new QList<QSharedPointer<DesignConfigurationInstantiation> > ()),
    ports_(new QList<QSharedPointer<Port> > ())
{

}

//-----------------------------------------------------------------------------
// Function: Model::Model()
//-----------------------------------------------------------------------------
Model::Model(Model const& other):
views_(new QList<QSharedPointer<View> > ()),
    componentInstantiations_(new QList<QSharedPointer<ComponentInstantiation> > ()),
    designInstantiations_(new QList<QSharedPointer<DesignInstantiation> > ()),
    designConfigurationInstantiations_(new QList<QSharedPointer<DesignConfigurationInstantiation> > ()),
    ports_(new QList<QSharedPointer<Port> > ())
{
    copyViews(other);
    copyComponentInstantiations(other);
    copyDesignInstantiations(other);
    copyDesignConfigurationInstantiations(other);
    copyPorts(other);
}

//-----------------------------------------------------------------------------
// Function: Model::operator=()
//-----------------------------------------------------------------------------
Model& Model::operator=(Model const& other )
{
    if (this != &other)
    {
        views_->clear();
        copyViews(other);
        componentInstantiations_->clear();
        copyComponentInstantiations(other);
        designInstantiations_->clear();
        copyDesignInstantiations(other);
        designConfigurationInstantiations_->clear();
        copyDesignInstantiations(other);
        ports_->clear();
        copyPorts(other);
    }
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Model::~Model()
//-----------------------------------------------------------------------------
Model::~Model()
{
	views_.clear();
    componentInstantiations_.clear();
    designInstantiations_.clear();
    designConfigurationInstantiations_.clear();
	ports_.clear();
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
	foreach (QSharedPointer<ComponentInstantiation> currentInstantiation, *componentInstantiations_)
	{
		if (currentInstantiation->name() == name)
		{
			return currentInstantiation;
		}
	}

	return QSharedPointer<ComponentInstantiation>();
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
	foreach (QSharedPointer<DesignInstantiation> currentInstantiation, *designInstantiations_)
	{
		if (currentInstantiation->name() == name)
		{
			return currentInstantiation;
		}
	}

	return QSharedPointer<DesignInstantiation>();
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
	foreach (QSharedPointer<DesignConfigurationInstantiation> currentInstantiation, 
        *designConfigurationInstantiations_)
	{
		if (currentInstantiation->name() == name)
		{
			return currentInstantiation;
		}
	}

	return QSharedPointer<DesignConfigurationInstantiation>();
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
    QStringList nameList;
    foreach (QSharedPointer<View> view, *views_)
    {
        nameList.append(view->name());
    }
    return nameList;
}

//-----------------------------------------------------------------------------
// Function: Model::getHierViews()
//-----------------------------------------------------------------------------
QStringList Model::getHierViews() const
{
    QStringList viewList;
    foreach (QSharedPointer<View> view, *views_)
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
    foreach (QSharedPointer<View> view, *views_)
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
	foreach (QSharedPointer<View> currentView, *views_)
	{
		if (currentView->name() == name)
		{
			return currentView;
		}
	}

	return QSharedPointer<View>();
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

    foreach (QSharedPointer<View> view, *views_)
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
    foreach (QSharedPointer<Port> port, *ports_)
    {
        if (port->name() == name)
        {
            return port;
        }
    }
    return QSharedPointer<Port>();
}

//-----------------------------------------------------------------------------
// Function: Model::hasPort()
//-----------------------------------------------------------------------------
bool Model::hasPort(QString const& name) const
{
    foreach (QSharedPointer<Port> port, *ports_)
    {
        if (port->name() == name)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: Model::getPortNames()
//-----------------------------------------------------------------------------
QStringList Model::getPortNames() const
{
    QStringList portNames;
    foreach (QSharedPointer<Port> port, *ports_)
    {
        portNames.append(port->name());
    }
    return portNames;
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
    foreach (QSharedPointer<View> view, *views_)
    {
        if (view->isHierarchical())
        {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: Model::getViewReferences()
//-----------------------------------------------------------------------------
QList<VLNV> Model::getViewReferences() const
{
    QList<VLNV> vlnvList;

    foreach (QSharedPointer<View> view, *views_)
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
    if (!views_->isEmpty() || !componentInstantiations_->isEmpty() || !designInstantiations_->isEmpty() ||
        !designConfigurationInstantiations_->isEmpty() || !ports_->isEmpty())
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: Model::copyViews()
//-----------------------------------------------------------------------------
void Model::copyViews(Model const& other) const
{
    foreach (QSharedPointer<View> view, *other.views_)
    {
        if (view)
        {
            QSharedPointer<View> copy = QSharedPointer<View>(new View(*view.data()));
            views_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Model::copyComponentInstantiations()
//-----------------------------------------------------------------------------
void Model::copyComponentInstantiations(Model const& other) const
{
    foreach (QSharedPointer<ComponentInstantiation> instantiation, *other.componentInstantiations_)
    {
        if (instantiation)
        {
            QSharedPointer<ComponentInstantiation> copy =
                QSharedPointer<ComponentInstantiation>(new ComponentInstantiation(*instantiation.data()));
            componentInstantiations_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Model::copyDesignInstantiations()
//-----------------------------------------------------------------------------
void Model::copyDesignInstantiations(Model const& other) const
{
    foreach (QSharedPointer<DesignInstantiation> instantiation, *other.designInstantiations_)
    {
        if (instantiation)
        {
            QSharedPointer<DesignInstantiation> copy(new DesignInstantiation(*instantiation));
            designInstantiations_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Model::copyDesignConfigurationInstantiations()
//-----------------------------------------------------------------------------
void Model::copyDesignConfigurationInstantiations(const Model& other) const
{
    foreach (QSharedPointer<DesignConfigurationInstantiation> instantiation,
        *other.designConfigurationInstantiations_)
    {
        if (instantiation)
        {
            QSharedPointer<DesignConfigurationInstantiation> copy(new DesignConfigurationInstantiation(
                *instantiation));
            designConfigurationInstantiations_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Model::copyPorts()
//-----------------------------------------------------------------------------
void Model::copyPorts(Model const& other) const
{
    foreach (QSharedPointer<Port> port, *other.ports_)
    {
        if (port)
        {
            QSharedPointer<Port> copy = QSharedPointer<Port>(new Port(*port));
            ports_->append(copy);
        }
    }
}
