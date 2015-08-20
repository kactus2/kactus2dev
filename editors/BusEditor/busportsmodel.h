//-----------------------------------------------------------------------------
// File: busportsmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 21.6.2011
//
// Description:
// Data model for the wires within abstraction definition.
//-----------------------------------------------------------------------------

#ifndef BUSPORTSMODEL_H
#define BUSPORTSMODEL_H

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/common/Qualifier.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WirePort.h>

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>
#include <QString>

//-----------------------------------------------------------------------------
//! Data model for the wires within abstraction definition.
//-----------------------------------------------------------------------------
class BusPortsModel : public QAbstractTableModel {
	Q_OBJECT

public:


	//! \brief Port contains the data for a single port in abstraction definition.
	struct Port {
		
		//! \brief Name of the port
		QString name_;

        //! The system group of the port.
        QString group_;

		//! \brief The qualifier identifies the type of the port.
		Qualifier::Type qualifier_;

		//! \brief Width contains the number of bits the port contains
		int width_;

		//! \brief The default value for the port
		QString defaultValue_;

		//! \brief Defines the mode of the port (master, slave or system)
		General::InterfaceMode mode_;

		//! \brief The direction of the port (in, out or inout)
		General::Direction direction_;

		//! \brief Presence defines if port is required, optional or illegal in this bus
		General::Presence presence_;

		//! \brief Driver defines the driver type for this port
		General::DriverType driver_;

		//! \brief Contains textual description of the port
		QString description_;

		/*! \brief Default constructor
		 *
		*/
		Port();

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

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parent   Pointer to the owner of this model.
	 */
	BusPortsModel(QObject *parent);
	
	/*!
	 *  The destructor.
	 */
	virtual ~BusPortsModel();

	/*!
	 *  Get the number of rows an item contains.
	 *
	 *      @param [in] parent      This should always be invalid model index.
     *
     *      @return                 Number of rows the item has.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
	 *  Get the number of columns the model contains.
	 *
	 *      @param [in] parent      This should always be invalid model index.
     *
     *      @return                 Always returns 9 for invalid model indexes.
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    
	/*!
	 *  Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index       Model index that identifies the item.
     *
     *      @return                 Qt::ItemFlags specify the possible operations for the item.
	 */
	Qt::ItemFlags flags(const QModelIndex& index) const;
    
	/*!
	 *  Get the header data for specified header.
	 *
	 *      @param [in] section         The section specifies the row/column number for the header.
	 *      @param [in] orientation     Specifies if horizontal or vertical header is wanted.
	 *      @param [in] role            Specifies the type of the requested data.
     *
     *      @return                     QVariant contains the requested data.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
	 *  Get the data for the specified item.
	 *
	 *      @param [in] index       Specifies the item that's data is requested.
	 *      @param [in] role        The role that defines what kind of data is requested.
     *
     *      @return                 QVariant contains the data for the item.
	 */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*!
	 *  Save the data to the model for specified item.
	 *
	 *      @param [in] index   The model index of the item that's data is to be saved.
	 *      @param [in] value   The data that is to be saved.
	 *      @param [in] role    The role specifies what kind of data should be saved.
     *
     *      @return             True if saving happened successfully.
	 */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	/*!
	 *  Set the abstraction definition for the model.
	 *
	 *      @param [in] absDef      Pointer to the Abstraction definition.
	 */
	void setAbsDef(QSharedPointer<AbstractionDefinition> absDef);

	/*!
	 *  Write the ports from the table to the abstraction definition.
	 */
	void save();

public slots:

	/*!
	 *  Adds a new signal with mode as "any".
	 */
	void addSignal();

	/*!
	 *  Adds all signal modes for selected ports.
	 *
	 *      @param [in] indexes     The indexes of the selected ports.
	 */
	void addSignalOptions(const QModelIndexList& indexes);

	/*!
	 *  Remove the items with given indexes from the model.
	 *
	 *      @param [in] indexes     List of QModelIndexes that identify items to be removed.
	 */
	void removeIndexes(const QModelIndexList& indexes);

    /*!
     *  Remove the item with the given index.
     *
     *      @param [in] index       The index where to remove the item.
     */
    void onRemoveItem(QModelIndex const& index);

	/*!
	 *  Copy the items with given indexes in the model.
	 *
	 *      @param [in] indexes     List of QModelIndexes that identify items to copy.
	 */
	void copyIndexes(const QModelIndexList& indexes);

	/*!
	 *  Export the ports from the model to a csv-file.
	 *
	 *      @param [in] path    Path to the file the ports are exported to.
	 */
	void exportCSV(QString const& path);

	/*!
	 *  Import ports from a csv-file to the model.
	 *
	 *      @param [in] path    Path to the file to read the ports from.
	 */
	void importCSV(QString const& path);

   
signals:

	/*!
	 *  Inform that the state of the model has changed.
	 */
	void contentChanged();

	/*!
	 *  Sends error message to be printed for the user.
	 *
	 *      @param [in] msg     The error message.
	 */
	void errorMessage(QString const& msg);

	/*!
	 *  Sends notification to be printed to user.
	 *
	 *      @param [in] msg   The notification message.
	 */
	void noticeMessage(QString const& msg);

    /*!
     *  Inform that a port abstraction has been removed.
     *
     *      @param [in] oldName   Old name of the port.
     *      @param [in] newName   New name of the port.
     */
    void portRenamed(QString const& oldName, QString const& newName);

    /*!
     *  Inform that a port abstraction has been removed.
     *
     *      @param [in] portName    The port name.
     *      @param [in] mode        The interface mode.
     */
    void portRemoved(QString const& portName, const General::InterfaceMode mode);

private:
	//! No copying
	BusPortsModel(const BusPortsModel& other);

	//! No assignment
	BusPortsModel& operator=(const BusPortsModel& other);
    
	/*!
	 *  Save the port from table to a port abstraction.
	 *
	 *      @param [in] portAbs     Pointer to the port abstraction to save the port to.
	 *      @param [in] i           Index of the port in the table.
	 */
	void savePort(QSharedPointer<PortAbstraction> portAbs, int i);

	/*!
	 *  Read the ports from the abstraction defition to the table.
	 */
	void readPorts();

	/*!
	 *  Read a single port into table from port abstraction.
	 *
	 *      @param [in] portAbs         Pointer to the port abstraction of the port.
	 *      @param [in] modeSpesific    Pointer to the mode specific definitions of the port.
	 *      @param [in] mode            The mode of the port to be created.
	 */
	void readPort(QSharedPointer<PortAbstraction> portAbs, QSharedPointer<WirePort> modeSpesific,
		General::InterfaceMode mode);

	/*!
	 *  Rename port(s) with given name to another name.
	 *
	 *      @param [in] oldPort   The name of the port to rename.
	 *      @param [in] newPort   The new name for the port.
	 */
	void renamePort(QString const& oldPort, QString const& newPort);

	/*!
	 *  Set qualifier(s) for port(s) with given name.
	 *
	 *      @param [in] portName    Identifies the port that's qualifier is to be changed.
	 *      @param [in] qualifier   The value to set for the qualifier.
	 */
	void setQualifier(QString const& portName, QString const& qualifier);

	/*!
	 *  Set default value(s) for port(s) with the given name.
	 *
	 *      @param [in] portName        Identifies the port that's default value is to be set.
	 *      @param [in] defaultValue    The value to set as default value.
	 */
	void setDefaultValue(QString const& portName, QString const& defaultValue);

	/*!
	 *  Set the driver(s) for port(s) with given name.
	 *
	 *      @param [in] portName        Identifies the port that's driver is to be set.
	 *      @param [in] driverValue     The driver value to set.
	 */
	void setDriver(QString const& portName, QString const& driverValue);

	/*!
	 *  Set the description(s) for port(s) with given name.
	 *
	 *      @param [in] portName        Identifies the port that's comment is to be set.
	 *      @param [in] description     The comment to set for the port(s).
	 */
	void setDescription(QString const& portName, QString const& description);

    /*!
     *  Set the width for the port with the given name.
     *
     *      @param [in] portName    Identifies the port that's width is to be set.
     *      @param [in] width       The width to set for the port.
     *      @param [in] mode        The port mode (master, slave, system, any).
     */
    void setWidth(QString const& portName, const int width, General::InterfaceMode mode);
     
	/*! Convert PortQualifier to string.
	 *
	 *      @param [in]  qualifier The Qualifier to convert.
	 *
	 *      @return A string representation for the Qualifier.
	*/
	QString toString(Qualifier::Type qualifier) const;

	/*! Convert a string to PortQualifier.
	 *
	 *      @param [in] str The string to convert.
	 *
	 *      @return A qualifier that matches the string.
	*/
	Qualifier::Type toQualifier(QString const& str) const;

     //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the abstraction definition being edited.
	QSharedPointer<AbstractionDefinition> absDef_;

	//! The pointer to the list containing the ports
	QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports_;

	//! Contains the editable ports
	QList<Port> table_;
};

#endif // BUSPORTSMODEL_H
