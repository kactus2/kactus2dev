/* 
 *  	Created on: 24.8.2011
 *      Author: Antti Kamppi
 * 		filename: activeviewmodel.h
 *		Project: Kactus 2
 */

#ifndef ACTIVEVIEWMODEL_H
#define ACTIVEVIEWMODEL_H

#include <models/designconfiguration.h>

#include <designwidget/diagramcomponent.h>

#include <common/GenericEditProvider.h>

#include <QAbstractTableModel>
#include <QString>
#include <QList>
#include <QSharedPointer>

class DesignWidget;
class DiagramComponent;

/*! \brief Model that contains the items to set the active views for component instances.
 * 
 */
class ActiveViewModel : public QAbstractTableModel {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	ActiveViewModel(QObject *parent);
	
	//! \brief The destructor
	virtual ~ActiveViewModel();

	/*! \brief Set the design that's active views are edited.
	 *
	 * \param designWidget Pointer to the design widget that manages the design.
	 * \param desConf Pointer to the configuration to edit.
	 *
	*/
	void setDesign(DesignWidget* designWidget, 
		QSharedPointer<DesignConfiguration> desConf);

	/*! \brief Clear the model's contents.
	 *
	*/
	void clear();

	/*! \brief Get the number of rows an item contains.
	 *
	 * \param parent Identifies the parent that's row count is requested.
	 *
	 * \return Number of rows the item has.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the number of columns the item has to be displayed.
	 *
	 * \param parent Identifies the parent that's column count is requested.
	 *
	 * \return The number of columns to be displayed.
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the data for specified item.
	 *
	 * \param index Specifies the item that's data is requested.
	 * \param role The role that defines what kind of data is requested.
	 *
	 * \return QVariant Contains the data for the item.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*! \brief Get the header data for specified header.
	 *
	 * \param section The section specifies the row/column number for the header.
	 * \param orientation Specified if horizontal or vertical header is wanted.
	 * \param role Specifies the type of the requested data.
	 *
	 * \return QVariant Contains the requested data.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, 
		int role = Qt::DisplayRole) const;

	/*! \brief Save the data to the model for specified item
	 *
	 * \param index The model index of the item that's data is to be saved.
	 * \param value The data that is to be saved.
	 * \param role The role specifies what kind of data should be saved.
	 *
	 * \return True if saving happened successfully.
	*/
	bool setData(const QModelIndex& index, const QVariant& value, 
		int role = Qt::EditRole);

	/*! \brief Get the item flags that defines the possible operations for the item.
	 *
	 * \param index Model index that identifies the item.
	 *
	 * \return Qt::ItemFlags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Checks if there is created configuration for this view.
	 *
	 *
	 * \return bool True if configuration exists.
	*/
	bool hasConfiguration() const;

public slots:

	/*! \brief Rename a component instance in the model.
	 *
	 * \param newName The new name of the instance.
	 * \param oldName The old name of the instance.
	 *
	*/
	void renameInstance(const QString& newName, const QString& oldName);

	/*! \brief Remove the instance from the model.
	 *
	 * \param obj Pointer to the diagram component being removed.
	 *
	*/
	void removeInstance(DiagramComponent* diaComp);

	/*! \brief Set the active view for given component instance.
	 * 
	 * NOTE: Using this function to set the active view for instance does not 
	 * generate commands to the undo/redo stack.
	 * 
	 * \param instanceName Identifies the component instance by name.
	 * \param viewName The new view name to be set for instance.
	 *
	*/
	void setActiveView(const QString& instanceName, const QString& viewName);

	/*! \brief Add a new component instance to the model.
	 *
	 * \param diaComp Pointer to the new component instance.
	 *
	*/
	void addInstance(DiagramComponent* diaComp);

private:
	//! \brief No copying
	ActiveViewModel(const ActiveViewModel& other);

	//! \brief No assignment
	ActiveViewModel& operator=(const ActiveViewModel& other);

	//! \brief The struct to contain the name-viewname pair for the configuration.
	struct InstanceViewPair {

		//! \brief The name of the component instance.
		QString instanceName_;

		//! \brief The name of the active view for the component instance.
		QString viewName_;

		/*! \brief The struct constructor
		 *
		 * \param instanceName The name for the component instance.
		 * \param viewName The name of the active view.
		 *
		*/
		InstanceViewPair(const QString& instanceName, const QString& viewName);

		/*! \brief The struct constructor
		 *
		 * \param instanceName The name for the component instance.
		 *
		*/
		InstanceViewPair(const QString& instanceName);

		/*! \brief Operator ==
		 *
		 * \param other Reference to the InstanceViewPair to compare to.
		 *
		 * \return bool True if the component instance names are the same.
		*/
		bool operator==(const InstanceViewPair& other);

		/*! \brief Operator != 
		 *
		 * \param other Reference to the InstanceViewPair to compare to.
		 *
		 * \return bool True if the component instance names are different.
		*/
		bool operator!=(const InstanceViewPair& other);
	};

	//! \brief Pointer to the design widget that manages the design.
	DesignWidget* designWidget_;

	//! \brief List that contains the items to display.
	QList<InstanceViewPair> table_;

	//! \brief Pointer to the design configuration being edited.
	QSharedPointer<DesignConfiguration> desConf_;

	//! \brief Contains pointers to the component instances being displayed.
	QList<DiagramComponent*> instances_;

	//! \brief Pointer to the edit provider that manages the undo stack.
	QSharedPointer<GenericEditProvider> editProvider_;
};

#endif // ACTIVEVIEWMODEL_H
