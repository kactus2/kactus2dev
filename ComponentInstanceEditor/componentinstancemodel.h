/* 
 *  	Created on: 12.8.2011
 *      Author: Antti Kamppi
 * 		filename: componentinstancemodel.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTINSTANCEMODEL_H
#define COMPONENTINSTANCEMODEL_H

#include <common/GenericEditProvider.h>

#include <QAbstractTableModel>
#include <QMap>
#include <QList>
#include <QString>
#include <QSharedPointer>

class DiagramComponent;

/*! \brief Model class to manage the configurable element values being edited.
 *
 */
class ComponentInstanceModel : public QAbstractTableModel {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the owner of this model.
	 *
	*/
	ComponentInstanceModel(QObject *parent);
	
	//! \brief The destructor
	virtual ~ComponentInstanceModel();

	/*! \brief Set the component being edited.
	 *
	 * \param component Pointer to the component being edited.
	 *
	*/
	void setComponent(DiagramComponent* component);

	/*! \brief Get the number of rows to be displayed.
	 *
	 * \param parent Identifies the parent, must always be invalid index.
	 *
	 * \return int - the number of rows to display.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the number of columns to display.
	 *
	 * \param parent Identifies the parent , must always be invalid index.
	 *
	 * \return int - the number of columns to display, always 2 for invalid parents.
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get that data for specified index in given data role.
	 *
	 * \param index Identifies the object that's data is wanted.
	 * \param role Identifies the type of data being requested.
	 *
	 * \return QVariant contains the requested data.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*! \brief Get the data for given header in given data role.
	 *
	 * \param section Identifies the header section.
	 * \param orientation Only Qt::Horizontal is supported.
	 * \param role Identifies the type of the data being requested.
	 *
	 * \return QVariant contains the requested data.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/*! \brief Set the data for specified index.
	 *
	 * \param index Identifies the object that's data is to be saved.
	 * \param value Contains the data to be saved.
	 * \param role Identifies the type of the data to be saved.
	 *
	 * \return bool True if data was saved successfully.
	*/
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );

	/*! \brief Get info on what operations are possible for specified item.
	 *
	 * \param index Identifies the item that's operations are requested.
	 *
	 * \return Qt::ItemFlags Contains info on which operations are available for given index.
	*/
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

public slots:

	/*! \brief Add a new configurable element to the table
	 *
	 * \param name Name of the configurable element
	 *
	*/
	void onAdd();

	/*! \brief Remove the specified item from the model.
	 *
	 * \param index Identifies the configurable element to remove.
	 *
	*/
	void onRemove(const QModelIndex& index);

	/*! \brief Clear the model.
	 *
	*/
	void clear();

	//! \brief Change the configurable elements of this model.
	void changeElements(const QMap<QString, QString>& confElements);

signals:

	//! \brief Emitted when contents of the model changes.
	void contentChanged();

private:
	//! \brief No copying
	ComponentInstanceModel(const ComponentInstanceModel& other);

	//! \brief No assignment
	ComponentInstanceModel& operator=(const ComponentInstanceModel& other);

	//! \brief Struct that contains the name-value pair for configurable element.
	struct ConfigurableElement {

		//! \brief The name of the configurable element.
		QString name_;

		//! \brief The value of the configurable element.
		QString value_;

		/*! \brief Struct constructor
		 *
		 * \param name Name of the configurable element
		 * \param value Value of the configurable element.
		 *
		*/
		ConfigurableElement(const QString& name, const QString& value);

		/*! \brief Constructs struct with empty value
		 * 
		 * \param name Name of the configurable element
		 *
		*/
		ConfigurableElement(const QString& name);

		/*! \brief Operator ==
		 *
		 * \param other Reference to the Configurable element to compare to.
		 *
		 * \return bool True if the names of the configurable elements match.
		*/
		bool operator==(const ConfigurableElement& other) const;
	};

	//! \brief Save the elements from the table to values_ map.
	void save();

	//! \brief Read the values_ and add the items to the table to be displayed.
	void readValues();

	//! \brief Pointer to the component instance being edited.
	DiagramComponent* component_;

	//! \brief Reference to the map containing the actual configurable elements.
	QMap<QString, QString> values_;

	//! \brief The list that is used to display the elements in a table form.
	QList<ConfigurableElement> table_;

	//! \brief Pointer to the generic edit provider that manages the undo/redo stack.
	QSharedPointer<GenericEditProvider> editProvider_;
};

#endif // COMPONENTINSTANCEMODEL_H
