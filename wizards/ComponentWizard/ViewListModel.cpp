//-----------------------------------------------------------------------------
// File: ViewListModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.10.2014
//
// Description:
// Model for listing component views.
//-----------------------------------------------------------------------------

#include "ViewListModel.h"


#include <IPXACTmodels/component.h>
#include <IPXACTmodels/view.h>

#include <QIcon>

//-----------------------------------------------------------------------------
// Function: ViewListModel::ViewListModel()
//-----------------------------------------------------------------------------
ViewListModel::ViewListModel(QObject *parent): QAbstractListModel(parent), component_()
{

}

//-----------------------------------------------------------------------------
// Function: ViewListModel::~ViewListModel()
//-----------------------------------------------------------------------------
ViewListModel::~ViewListModel()
{

}

//-----------------------------------------------------------------------------
// Function: ViewListModel::setComponent()
//-----------------------------------------------------------------------------
void ViewListModel::setComponent(QSharedPointer<Component> component)
{
    component_ = component;
}

//-----------------------------------------------------------------------------
// Function: ViewListModel::rowCount()
//-----------------------------------------------------------------------------
int ViewListModel::rowCount(QModelIndex const& parent) const {
	
	if (parent.isValid() || component_.isNull())
    {
		return 0;
    }

	return component_->viewCount();
}

//-----------------------------------------------------------------------------
// Function: ViewListModel::data()
//-----------------------------------------------------------------------------
QVariant ViewListModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || component_.isNull())
    {
		return QVariant();
    }

	if (index.row() < 0 || index.row() >= component_->viewCount())
    {
		return QVariant();
    }

    QString viewName = component_->getViewNames().at(index.row());  

	if (role == Qt::DisplayRole)
    {
        if (viewName.isEmpty())
        {
            viewName = "<unnamed>";
        }

        return viewName;        
	}
    else if (role == Qt::DecorationRole)
    {
        QStringList hierViews = component_->getHierViews();
        if (hierViews.contains(viewName))
        {
            return QIcon(":icons/common/graphics/hierarchy.png");
        }
        else
        {
            return QVariant();
        }
    }
    else if(role == Qt::ForegroundRole)
    {
        if (component_->getViewNames().count(viewName) > 1 || viewName.isEmpty())
        {
            return QColor("red");
        }
        else
        {
            return QColor("black");
        }
    }
	else
    {
		return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ViewListModel::addView()
//-----------------------------------------------------------------------------
void ViewListModel::addView()
{
    if (component_.isNull())
    {
        return;
    }

    int endOfList = component_->viewCount();

	beginInsertRows(QModelIndex(), endOfList, endOfList);
	View* view = component_->createView();
    view->addEnvIdentifier("::");
	endInsertRows();
}

//-----------------------------------------------------------------------------
// Function: ViewListModel::removeView()
//-----------------------------------------------------------------------------
void ViewListModel::removeView(QModelIndex const& index)
{
	if (!index.isValid() || component_.isNull())
    {
		return;
    }
    
    QString view = component_->getViewNames().at(index.row());

    beginResetModel();
    component_->removeView(view);
	endResetModel();
}
