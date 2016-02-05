//-----------------------------------------------------------------------------
// File: activeviewmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 24.08.2011
//
// Description:
// Model that contains the items to set the active views for component instances.
//-----------------------------------------------------------------------------

#ifndef ACTIVEVIEWMODEL_H
#define ACTIVEVIEWMODEL_H

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <designEditors/HWDesign/HWComponentItem.h>

#include <common/IEditProvider.h>

#include <QAbstractTableModel>
#include <QString>
#include <QList>
#include <QSharedPointer>

class DesignWidget;
class HWComponentItem;

//-----------------------------------------------------------------------------
//! Model that contains the items to set the active views for component instances.
//-----------------------------------------------------------------------------
class ActiveViewModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the owner of this widget.
	 */
	ActiveViewModel(QObject *parent);
	
	//! The destructor.
	virtual ~ActiveViewModel();

	/*!
     *  Set the design that's active views are edited.
	 *
	 *      @param [in] designWidget    Pointer to the design widget that manages the design.
	 *      @param [in] desConf         Pointer to the configuration to edit.
	 */
	void setDesign(DesignWidget* designWidget, QSharedPointer<DesignConfiguration> desConf);

	/*!
     *  Clear the model's contents.
	 */
	void clear();

	/*!
     *  Get the number of rows an item contains.
	 *
	 *      @param [in] parent  Identifies the parent that's row count is requested.
	 *
	 *      @return Number of rows the item has.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
     *  Get the number of columns the item has to be displayed.
	 *
	 *      *param [in] parent  Identifies the parent that's column count is requested.
	 *
	 *      @return The number of columns to be displayed.
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
     *  Get the data for specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is requested.
	 *      @param [in] role    The role that defines what kind of data is requested.
	 *
	 *      @return QVariant Contains the data for the item.
	 */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*!
     *  Get the header data for specified header.
	 *
	 *      @param [in] section         The section specifies the row/column number for the header.
	 *      @param [in] orientation     Specified if horizontal or vertical header is wanted.
	 *      @param [in] role            Specifies the type of the requested data.
	 *
	 *      @return QVariant Contains the requested data.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
     *  Save the data to the model for specified item
	 *
	 *      @param [in] index   The model index of the item that's data is to be saved.
	 *      @param [in] value   The data that is to be saved.
	 *      @param [in] role    The role specifies what kind of data should be saved.
	 *
	 *      @return True if saving happened successfully.
	 */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	/*!
     *  Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index   Model index that identifies the item.
	 *
	 *      @return Qt::ItemFlags specify the possible operations for the item.
	 */
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*!
     *  Checks if there is created configuration for this view.
	 *
	 *      @return bool True if configuration exists.
	 */
	bool hasConfiguration() const;

public slots:

	/*!
     *  Rename a component instance in the model.
	 *
	 *      @param [in] newName     The new name of the instance.
	 *      @param [in] oldName     The old name of the instance.
	 */
	void renameInstance(const QString& newName, const QString& oldName);

	/*!
     *  Remove the instance from the model.
	 *
	 *      @param [in] diaComp     Pointer to the diagram component being removed.
	 */
	void removeInstance(ComponentItem* diaComp);

	/*!
     *  Set the active view for given component instance.
	 *  NOTE: Using this function to set the active view for instance does not generate commands to the undo/redo 
     *  stack.
	 * 
	 *      @param [in] instanceName    Identifies the component instance by name.
	 *      @param [in] viewName        The new view name to be set for instance.
	 */
	void setActiveView(const QString& instanceName, const QString& viewName);

	/*!
     *  Add a new component instance to the model.
	 *
	 *      @param [in] diaComp     Pointer to the new component instance.
	 */
	void addInstance(ComponentItem* diaComp);

private:

	//! No copying.
	ActiveViewModel(const ActiveViewModel& other);

	//! No assignment.
	ActiveViewModel& operator=(const ActiveViewModel& other);

	//! The struct to contain the name-viewname pair for the configuration.
	struct InstanceViewPair
    {
		//! The name of the component instance.
		QString instanceName_;

		//! The name of the active view for the component instance.
		QString viewName_;

		/*!
         *  The struct constructor
		 *
		 *      @param [in] instanceName    The name for the component instance.
		 *      @param [in] viewName        The name of the active view.
		 */
		InstanceViewPair(const QString& instanceName, const QString& viewName);

		/*!
         *  The struct constructor
		 *
		 *      @param [in] instanceName    The name for the component instance.
		 */
		InstanceViewPair(const QString& instanceName);

		/*!
         *  Operator ==
		 *
		 *      @param [in] other   Reference to the InstanceViewPair to compare to.
		 *
		 *      @return bool True if the component instance names are the same.
		 */
		bool operator==(const InstanceViewPair& other);

		/*!
         *  Operator != 
		 *
		 *      @param [in] other Reference to the InstanceViewPair to compare to.
		 *
		 *      @return bool True if the component instance names are different.
		 */
		bool operator!=(const InstanceViewPair& other);
	};

    /*!
     *  Change the active view from the selected view configuration.
     *
     *      @param [in] instanceName    The name of the selected component instance.
     *      @param [in] newViewName     The new active view.
     */
    void changeViewConfiguration(QString const& instanceName, QString const& newViewName);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the design widget that manages the design.
	DesignWidget* designWidget_;

	//! List that contains the items to display.
	QList<InstanceViewPair> table_;

	//! Pointer to the design configuration being edited.
	QSharedPointer<DesignConfiguration> desConf_;

	//! Contains pointers to the component instances being displayed.
	QList<ComponentItem*> instances_;

	//! Pointer to the edit provider that manages the undo stack.
	QSharedPointer<IEditProvider> editProvider_;
};

#endif // ACTIVEVIEWMODEL_H
