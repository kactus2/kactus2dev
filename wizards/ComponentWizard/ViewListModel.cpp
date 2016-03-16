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


#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>

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

	return component_->getViews()->count();
}

//-----------------------------------------------------------------------------
// Function: ViewListModel::data()
//-----------------------------------------------------------------------------
QVariant ViewListModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || component_.isNull() || index.row() < 0 || index.row() >= component_->getViews()->count())
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

    int endOfList = component_->getViews()->count();

	beginInsertRows(QModelIndex(), endOfList, endOfList);
    QSharedPointer<View> newView(new View());
	component_->getViews()->append(newView);    
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
    
    beginResetModel();
    component_->getViews()->removeAt(index.row());
	endResetModel();
}
