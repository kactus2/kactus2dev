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

/*
Model::Model(QDomNode &modelNode):
views_(),
ports_(),
modelParameters_(new QList<QSharedPointer<ModelParameter> >()) {

	for (int i = 0; i < modelNode.childNodes().count(); ++i) {
		QDomNode tempNode = modelNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:views")) {

			// go through all the views
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode viewNode = tempNode.childNodes().at(j);
				if (!viewNode.isComment()) {
					views_.append(QSharedPointer<View>(
							new View(viewNode)));
				}
			}

		}
		else if (tempNode.nodeName() == QString("spirit:ports")) {

			// go through all ports
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode portNode = tempNode.childNodes().at(j);
				if (!portNode.isComment()) {
					ports_.append(QSharedPointer<Port>(new Port(portNode)));
				}
			}
		}

		else if (tempNode.nodeName() == QString("spirit:modelParameters")) {

			// go through all model parameters
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode modelParameterNode = tempNode.childNodes().at(j);
				if (!modelParameterNode.isComment()) {

					modelParameters_->append(QSharedPointer<ModelParameter>(new ModelParameter(modelParameterNode)));
				}
			}
		}
	}
	return;
}*/

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
Model::Model( const Model& other ):
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
Model& Model::operator=( const Model &other )
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
	return;
}

/*
//-----------------------------------------------------------------------------
// Function: model::isValid()
//-----------------------------------------------------------------------------
bool Model::isValid( const QStringList& fileSetNames,
                    QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
					QStringList& errorList, 
					const QString& parentIdentifier ) const {
	bool valid = true;
	bool hasViews = !views_.isEmpty();

	QStringList portNames;
	foreach (QSharedPointer<Port> port, ports_) {

		if (portNames.contains(port->name())) {
			errorList.append(QObject::tr("%1 contains several ports with name %2").arg(
				parentIdentifier).arg(port->name()));
			valid = false;
		}
		else {
			portNames.append(port->name());
		}

		if (!port->isValid(hasViews, errorList, parentIdentifier)) {
			valid = false;
		}
	}

	QStringList viewNames;
	foreach (QSharedPointer<View> view, views_) {

		if (viewNames.contains(view->name())) {
			errorList.append(QObject::tr("%1 contains several views with name %2").arg(
				parentIdentifier).arg(view->name()));
			valid = false;
		}
		else {
			viewNames.append(view->name());
		}

		if (!view->isValid(fileSetNames, componentChoices, errorList, parentIdentifier)) {
			valid = false;
		}
	}

    //ModelParameterValidator validator;
	QStringList modelParamNames;
	foreach (QSharedPointer<ModelParameter> modelParam, *modelParameters_) 
    {
		if (modelParamNames.contains(modelParam->name())) {
			errorList.append(QObject::tr("%1 contains several model parameters"
				" with name %2").arg(parentIdentifier).arg(modelParam->name()));
			valid = false;
		}
		else 
        {
			modelParamNames.append(modelParam->name());
		}

//         errorList.append(validator.findErrorsIn(modelParam.data(), parentIdentifier, componentChoices));
//         if (!validator.validate(modelParam.data(), componentChoices))
//         {
//             valid = false;
//         }
	}

	return valid;
}*/
/*
//-----------------------------------------------------------------------------
// Function: model::isValid()
//-----------------------------------------------------------------------------
bool Model::isValid(const QStringList& fileSetNames,
     QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const {
	bool hasViews = !views_.isEmpty();

	QStringList portNames;
	foreach (QSharedPointer<Port> port, ports_) {

		if (portNames.contains(port->name())) {
			return false;
		}
		else {
			portNames.append(port->name());
		}

		if (!port->isValid(hasViews)) {
			return false;
		}
	}

	QStringList viewNames;
	foreach (QSharedPointer<View> view, views_) {

		if (viewNames.contains(view->name())) {
			return false;
		}
		else {
			viewNames.append(view->name());
		}

		if (!view->isValid(fileSetNames, componentChoices)) {
			return false;
		}
	}

    //ModelParameterValidator validator;
	QStringList modelParamNames;
	foreach (QSharedPointer<ModelParameter> modelParam, *modelParameters_)
    {
		if (modelParamNames.contains(modelParam->name()))
        {
			return false;
		}
		else
        {
			modelParamNames.append(modelParam->name());
		}

// 		if (!validator.validate(modelParam.data(), componentChoices))
//         {
// 			return false;
// 		}
	}

	return true;
}*/

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
// Function: Model::getComponentInstantiations()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > Model::getComponentInstantiations() const
{
    return componentInstantiations_;
}

//-----------------------------------------------------------------------------
// Function: Model::setComponentInstantiations()
//-----------------------------------------------------------------------------
void Model::setComponentInstantiations
    (QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > instantiations)
{
    componentInstantiations_ = instantiations;
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
// Function: Model::getDesignConfigurationInstantiations()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<DesignConfigurationInstantiation> > > Model::
    getDesignConfigurationInstantiations() const
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
// Function: Model::getHierRef()
//-----------------------------------------------------------------------------
VLNV Model::getHierRef(const QString viewName) const
{
    foreach (QSharedPointer<View> view, *views_)
    {
        if (view->name() == viewName)
        {
            if (!view->getDesignInstantiationRef().isEmpty())
            {
                foreach (QSharedPointer<DesignInstantiation> instantiation, *designInstantiations_)
                {
                    if (instantiation->name() == view->getDesignInstantiationRef())
                    {
                        return *instantiation->getDesignReference().data();
                    }
                }
            }
            break;
        }
    }

    return VLNV();
}

//-----------------------------------------------------------------------------
// Function: Model::getHierarchyRefs()
//-----------------------------------------------------------------------------
const QList<VLNV> Model::getHierarchyRefs() const
{
    QList<VLNV> list;

    foreach (QSharedPointer<View> view, *views_)
    {
        if (view->isHierarchical())
        {
            if (!view->getDesignInstantiationRef().isEmpty())
            {
                foreach (QSharedPointer<DesignInstantiation> instantiation, *designInstantiations_)
                {
                    if (instantiation->name() == view->getDesignInstantiationRef())
                    {
                        list.append(*instantiation->getDesignReference().data());
                        break;
                    }
                }
            }
            if (!view->getDesignConfigurationInstantiationRef().isEmpty())
            {
                foreach (QSharedPointer<DesignConfigurationInstantiation> instantiation,
                    *designConfigurationInstantiations_)
                {
                    if (instantiation->name() == view->getDesignConfigurationInstantiationRef())
                    {
                        list.append(*instantiation->getDesignConfigurationReference().data());
                        break;
                    }
                }
            }
        }
    }

    return list;
}

//-----------------------------------------------------------------------------
// Function: Model::getPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> Model::getPort( const QString& name ) const
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
            foreach (QSharedPointer<DesignInstantiation> instantiation, *designInstantiations_)
            {
                if (instantiation->name() == view->getDesignInstantiationRef())
                {
                    vlnvList.append(*instantiation->getDesignReference().data());
                }
            }
        }
        if (!view->getDesignConfigurationInstantiationRef().isEmpty())
        {
            foreach (QSharedPointer<DesignConfigurationInstantiation> instantiation,
                *designConfigurationInstantiations_)
            {
                if (instantiation->name() == view->getDesignConfigurationInstantiationRef())
                {
                    vlnvList.append(*instantiation->getDesignConfigurationReference().data());
                }
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
void Model::copyViews(const Model& other) const
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
void Model::copyComponentInstantiations(const Model& other) const
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
void Model::copyDesignInstantiations(const Model& other) const
{
    foreach (QSharedPointer<DesignInstantiation> instantiation, *other.designInstantiations_)
    {
        if (instantiation)
        {
            QSharedPointer<DesignInstantiation> copy =
                QSharedPointer<DesignInstantiation>(new DesignInstantiation(*instantiation.data()));
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
            QSharedPointer<DesignConfigurationInstantiation> copy =
                QSharedPointer<DesignConfigurationInstantiation>(
                new DesignConfigurationInstantiation(*instantiation.data()));
            designConfigurationInstantiations_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Model::copyPorts()
//-----------------------------------------------------------------------------
void Model::copyPorts(const Model& other) const
{
    foreach (QSharedPointer<Port> port, *other.ports_)
    {
        if (port)
        {
            QSharedPointer<Port> copy = QSharedPointer<Port>(new Port(*port.data()));
            ports_->append(copy);
        }
    }
}