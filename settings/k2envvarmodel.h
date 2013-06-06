/* 
 *	Created on:	5.6.2013
 *	Author:		Antti Kamppi
 *	File name:	k2envvarmodel.h
 *	Project:		Kactus 2
*/

#ifndef K2ENVVARMODEL_H
#define K2ENVVARMODEL_H

#include <PluginSystem/PluginManager.h>

#include <QAbstractTableModel>
#include <QSettings>
#include <QList>

/*! \brief Manages the K2 environment variables displayed in the settings dialog.
 *
 */
class K2EnvVarModel : public QAbstractTableModel {
	Q_OBJECT

public:

	//! \brief Defines the used columns
	enum Columns {
		NAME_COLUMN	= 0,
		VALUE_COLUMN,
		FILE_SUFFIX_COLUMN,
		DESCRIPTION_COLUMN,
		COLUMN_COUNT
	};

	K2EnvVarModel(QSettings& settings, PluginManager& pluginMgr, QObject *parent);
	
	//! \brief The destructor.
	virtual ~K2EnvVarModel();

	/*! \brief Save the model to settings.
	 *
	 * Method: 		apply
	 * Full name:	K2EnvVarModel::apply
	 * Access:		public 
	 *
	 * \param settings The settings to store to.
	 *
	*/
	void apply(QSettings& settings);

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

	/*! \brief Get the item flags that defines the possible operations for the item.
	 *
	 * \param index Model index that identifies the item.
	 *
	 * \return Qt::ItemFlags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(const QModelIndex& index) const;

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

	/*! \brief Get the data for specified item.
	 *
	 * \param index Specifies the item that's data is requested.
	 * \param role The role that defines what kind of data is requested.
	 *
	 * \return QVariant Contains the data for the item.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

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

	/*! \brief Check that all variables have at least the name defined.
	 *
	 * Method: 		containsEmptyVariables
	 * Full name:	K2EnvVarModel::containsEmptyVariables
	 * Access:		public 
	 *
	 *
	 * \return True if all defined variables contain a name.
	*/
	bool containsEmptyVariables() const;

public slots:

	/*! \brief Add a new item to the given index.
	 *
	 * \param index The index identifying the position for new item.
	 *
	*/
	virtual void onAddItem(const QModelIndex& index);

	/*! \brief Remove the item in the given index.
	 *
	 * \param index The index identifying the item to remove.
	 *
	*/
	virtual void onRemoveItem(const QModelIndex& index);

private:
	
	//! \brief No copying
	K2EnvVarModel(const K2EnvVarModel& other);

	//! \brief No assignment
	K2EnvVarModel& operator=(const K2EnvVarModel& other);

	//! \brief The settings for a single environment variable
	struct EnvSetting {

		//! \brief The name of the variable.
		QString name_;

		//! \brief The value for the variable.
		QString value_;

		//! \brief If the variable if a file path then this is used to define the file suffix.
		QString fileSuffix_;

		//! \brief Textual description of the variable.
		QString description_;

		//! \brief Set to true if the variable is requested by the pluging.
		bool fromPlugin_;

		/*! \brief The constructor.
		 *
		 * Method: 		EnvSetting
		 * Full name:	K2EnvVarModel::EnvSetting::EnvSetting
		 * Access:		public 
		 *
		 * \param name The name for the variable.
		 * \param value The value for the variable.
		 * \param fileSuffix The file suffix.
		 * \param description Textual description for the variable.
		 * \param fromPlugin Defines whether the variable is requested by the plugins.
		 *
		*/
		EnvSetting(const QString& name, 
			const QString& value,
			const QString& fileSuffix,
			const QString& description,
			bool fromPlugin);

		/*! \brief The constructor.
		 *
		 * Method: 		EnvSetting
		 * Full name:	K2EnvVarModel::EnvSetting::EnvSetting
		 * Access:		public 
		 *
		 * \param name The name for the variable.
		 *
		*/
		EnvSetting(const QString& name);

		//! \brief The default constructor.
		EnvSetting();

		//! \brief Copy constructor
		EnvSetting(const EnvSetting& other);

		//! \brief Assignment operator
		EnvSetting& operator=(const EnvSetting& other);

		//! \brief Operator <
		bool operator<(const EnvSetting& other) const;

		//! \brief Operator ==
		bool operator==(const EnvSetting& other) const;

		//! \brief Operator !=
		bool operator!=(const EnvSetting& other) const;
	};

	//! \brief The list of the environment variables which is displayed/edited.
	QList<EnvSetting> table_;
};

#endif // K2ENVVARMODEL_H
