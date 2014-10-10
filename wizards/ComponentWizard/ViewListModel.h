//-----------------------------------------------------------------------------
// File: ViewListModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.10.2014
//
// Description:
// Model for listing component views.
//-----------------------------------------------------------------------------

#ifndef VIEWLISTMODEL_H
#define VIEWLISTMODEL_H

#include <QAbstractListModel>
#include <QSharedPointer>

class Component;

//-----------------------------------------------------------------------------
//! Model for listing component views.
//-----------------------------------------------------------------------------
class ViewListModel : public QAbstractListModel
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parent   The parent object.
	 */
	ViewListModel(QObject *parent);
	
	//! The destructor
	virtual ~ViewListModel();

    /*!
     *  Sets the component whose views to list.
     *
     *      @param [in] component   The component whose views to list in the model.
     */
    void setComponent(QSharedPointer<Component> component);

	/*!
	 *  Returns the number of rows in the model.
	 *
	 *      @param [in] parent   The parent index whose row count to return. Should always be invalid in list.
	 *
	 *      @return The number of rows in the model.
	 */
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	
	/*!
	 *  Returns the data in the model.
	 *
	 *      @param [in] index   The index whose data to fetch.
	 *      @param [in] role    The role of the data to fetch.
	 *
	 *      @return The model data for the given role.
	 */
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;


public slots:

	//! Adds a new view to the component and the list.
	virtual void addView();

	//! Removes all the views with same name as the view in given model index.
	virtual void removeView(const QModelIndex& index);

private:

	//! \brief No copying
	ViewListModel(const ViewListModel& other);

	//! No assignment
	ViewListModel& operator=(const ViewListModel& other);

    //! The component whose views to list.
    QSharedPointer<Component> component_;
};

#endif // VIEWLISTMODEL_H
