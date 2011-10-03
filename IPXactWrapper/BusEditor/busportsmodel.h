/*
 *  Created on: 21.6.2011
 *      Author: Antti Kamppi
 * 		filename: busportsmodel.h
 */

#ifndef BUSPORTSMODEL_H
#define BUSPORTSMODEL_H

#include <models/abstractiondefinition.h>
#include <models/generaldeclarations.h>
#include <models/portabstraction.h>
#include <models/wireabstraction.h>

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>
#include <QString>

/*! \brief Model to provide the signals of abstraction definition for editing.
 *
 */
class BusPortsModel : public QAbstractTableModel {
	Q_OBJECT

public:

	//! \brief Enum that is used to select the mode of a port.
	enum PortMode {
		MODE_MASTER = 0,
		MODE_SLAVE,
		MODE_SYSTEM,
		MODE_ANY
	};
	/*! \brief Convert PortMode to string
	 *
	 * \param mode The mode to convert
	 *
	 * \return QString that matches the mode.
	*/
	QString portMode2Str(PortMode mode) const;

	/*! \brief Convert a string to PortMode
	 *
	 * \param str QString that contains the mode.
	 *
	 * \return BusPortsModel::PortMode
	*/
	PortMode str2PortMode(const QString& str) const;

	//! \brief Enum PortDirection is used to select the direction of a port.
	enum PortDirection {
		DIRECTION_IN = 0,
		DIRECTION_OUT,
		DIRECTION_INOUT
	};

	/*! \brief Convert PortDirection to string
	 *
	 * \param direction The direction to convert
	 *
	 * \return QString that matches the PortDirection.
	*/
	QString portDirection2Str(PortDirection direction) const;

	/*! \brief Convert string to a PortDirection
	 *
	 * \param str QString that contains the direction.
	 *
	 * \return BusPortsModel::PortDirection
	*/
	PortDirection str2PortDirection(const QString& str) const;

	/*! \brief Convert PortDirectio to General::Direction
	 *
	 * \param direction The PortDirection to convert.
	 *
	 * \return General::Direction
	*/
	General::Direction portDirection2Direction(PortDirection direction) const;

	//! \brief Enum PortDriver is used to select driver setting for a port.
	enum PortDriver {
		DRIVER_NONE = 0,
		DRIVER_ANY,
		DRIVER_CLOCK,
		DRIVER_SINGLESHOT
	};

	/*! \brief Convert PortDriver to string.
	 *
	 * \param driver PortDriver to convert.
	 *
	 * \return QString that matches the PortDriver.
	*/
	QString portDriver2Str(PortDriver driver) const;

	/*! \brief Convert string to PortDriver.
	 *
	 * \param str QString that contains the PortDriver.
	 *
	 * \return BusPortsModel::PortDriver
	*/
	PortDriver str2PortDriver(const QString& str) const;

	//! \brief Enum PortQualifier is used to select the qualifier element of a port.
	enum PortQualifier {
		QUALIFIER_ADDRESS = 0,
		QUALIFIER_DATA,
		QUALIFIER_RESET,
		QUALIFIER_CLOCK,
		QUALIFIER_ANY
	};

	/*! \brief Convert PortQualifier to string.
	 *
	 * \param qualifier PortQualifier to convert.
	 *
	 * \return QString that matches the PortQualifier.
	*/
	QString portQualifier2Str(PortQualifier qualifier) const;

	/*! \brief Convert a string to PortQualifier.
	 *
	 * \param str QString that contains the PortQualifier.
	 *
	 * \return BusPortsModel::PortQualifier
	*/
	PortQualifier str2PortQualifier(const QString& str) const;

	//! \brief Port contains the data for a single port in abstraction definition.
	struct Port {
		
		//! \brief Name of the port
		QString name_;

		//! \brief The qualifier identifies the type of the port.
		PortQualifier qualifier_;

		//! \brief Width contains the number of bits the port contains
		int width_;

		//! \brief The default value for the port
		QString defaultValue_;

		//! \brief Defines the mode of the port (master, slave or system)
		PortMode mode_;

		//! \brief The direction of the port (in, out or inout)
		PortDirection direction_;

		//! \brief Presence defines if port is required, optional or illegal in this bus
		General::Presence presence_;

		//! \brief Driver defines the driver type for this port
		PortDriver driver_;

		//! \brief Contains textual description of the port
		QString comment_;

		/*! \brief Default constructor
		 *
		*/
		Port();

		/*! \brief Constructor
		 *
		 * \param name Name for the port
		 *
		*/
		Port(const QString& name);

		/*! \brief Copy constructor
		 *
		 * \param other Reference to the port to copy.
		 *
		*/
		Port(const Port& other);

		/*! \brief Assignment operator
		 *
		 * \param other Reference to the port to assign
		 *
		*/
		Port& operator=(const Port& other);

		/*! \brief The == operator
		 *
		 * \param other Reference to Port to compare
		 *
		 * \return True if name and mode are equal.
		*/
		bool operator==(const Port& other) const;

		/*! \brief The != operator
		 *
		 * \param other Reference to Port to compare
		 *
		 * \return True if name or mode are not equal
		*/
		bool operator!=(const Port& other) const;

		/*! \brief The < operator
		 *
		 * \param other Reference to Port to compare
		 *
		 * \return True is name < other.name 
		*/
		bool operator<(const Port& other) const;
	};

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this model.
	 *
	 * \return 
	*/
	BusPortsModel(QObject *parent);
	
	//! \brief The destructor
	virtual ~BusPortsModel();

	/*! \brief Get the number of rows an item contains.
	 *
	 * \param parent This should always be invalid model index.
	 *
	 * \return Number of rows to item has
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the number of columns the model contains
	 *
	 * \param parent This should always be invalid model index.
	 *
	 * \return Always returns 9 for invalid model indexes.
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
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*! \brief Save the data to the model for specified item
	 *
	 * \param index The model index of the item that's data is to be saved.
	 * \param value The data that is to be saved.
	 * \param role The role specifies what kind of data should be saved.
	 *
	 * \return True if saving happened successfully.
	*/
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	/*! \brief Get the item flags that defines the possible operations for the item.
	 *
	 * \param index Model index that identifies the item.
	 *
	 * \return Qt::ItemFlags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Set the abstraction definition for the model.
	 *
	 * \param absDef Pointer to the Abstraction definition
	 *
	*/
	void setAbsDef(QSharedPointer<AbstractionDefinition> absDef);

	/*! \brief Write the ports from the table to the abstraction definition
	 *
	*/
	void save();

public slots:

	//! \brief Adds a new signal with mode as "any"
	void addSignal();

	//! \brief Adds all signal modes for selected ports.
	void addSignalOptions(QModelIndexList& indexes);

	/*! \brief Remove the items with given indexes from the model
	 *
	 * \param indexes List of QModelIndexes that identify items to remove.
	 *
	*/
	void removeIndexes(QModelIndexList& indexes);

	/*! \brief Copy the items with given indexes in the model.
	 *
	 * \param indexes List of QModelIndexes that identify items to copy.
	 *
	*/
	void copyIndexes(QModelIndexList& indexes);

	/*! \brief Export the ports from the model to a csv-file.
	 *
	 * \param path Path to the file the ports are exported to.
	 *
	*/
	void exportCSV(const QString& path);

	/*! \brief Import ports from a csv-file to the model.
	 *
	 * \param path Path to the file to read the ports from.
	 *
	*/
	void importCSV(const QString& path);

signals:

	//! \brief Inform that the state of the model has changed.
	void contentChanged();

	//! \brief Sends error message to be printed to user.
	void errorMessage(const QString& msg);

	//! \brief Sends notification to be printed to user.
	void noticeMessage(const QString& msg);

private:
	//! \brief No copying
	BusPortsModel(const BusPortsModel& other);

	//! \brief No assignment
	BusPortsModel& operator=(const BusPortsModel& other);

	//! \brief Read the ports from the abstraction definition to the table
	void readPorts();

	/*! \brief Read a single port into table from port abstraction
	 *
	 * \param portAbs Pointer to the port abstraction of the port.
	 * \param modeSpesific Pointer to the mode specific definitions of the port.
	 * \param mode The mode of the port to be created
	 *
	*/
	void readPort(QSharedPointer<PortAbstraction> portAbs,
		WireAbstraction::WirePort* modeSpesific,
		BusPortsModel::PortMode mode);

	/*! \brief Rename port(s) with given name to another name.
	 *
	 * \param oldPort The name of the port to rename.
	 * \param newPort The new name for the port.
	 *
	*/
	void renamePort(const QString oldPort, const QString newPort);

	/*! \brief Set qualifier(s) for port(s) with given name.
	 *
	 * \param portName Identifies the port that's qualifier is to be changed
	 * \param qualifier The value to set for the qualifier.
	 *
	*/
	void setQualifier(const QString& portName, const QString& qualifier);

	/*! \brief Set default value(s) for port(s) with given name.
	 *
	 * \param portName Identifies the port that's default value is to be set.
	 * \param defaultValue The value to set as default value.
	 *
	*/
	void setDefaultValue(const QString& portName, const QString& defaultValue);

	/*! \brief Set the driver(s) for port(s) with given name.
	 *
	 * \param portName Identifies the port that's driver is to be set.
	 * \param driverValue The driver value to set.
	 *
	*/
	void setDriver(const QString& portName, const QString& driverValue);

	/*! \brief Set the comment(s) for port(s) with given name.
	 *
	 * \param portName Identifies the port that's comment is to be set.
	 * \param comment The comment to set for the port(s)
	 *
	*/
	void setComment(const QString& portName, const QString& comment);

	/*! \brief Save the port from table to a port abstraction.
	 *
	 * \param portAbs Pointer to the port abstraction to save the port to.
	 * \param i Index of the port in the table.
	 *
	*/
	void savePort(QSharedPointer<PortAbstraction> portAbs, int i);

	//! \brief Pointer to the abstraction definition being edited.
	QSharedPointer<AbstractionDefinition> absDef_;

	//! \brief The pointer to the list containing the ports
	QList<QSharedPointer<PortAbstraction> >* ports_;

	//! \brief Contains the editable ports
	QList<Port> table_;
};

#endif // BUSPORTSMODEL_H
