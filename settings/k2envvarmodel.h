//-----------------------------------------------------------------------------
// File: k2envvarmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 05.06.2013
//
// Description:
// Manages the K2 environment variables displayed in the settings dialog.
//-----------------------------------------------------------------------------

#ifndef K2ENVVARMODEL_H
#define K2ENVVARMODEL_H

#include <Plugins/PluginSystem/PluginManager.h>

#include <QAbstractTableModel>
#include <QSettings>
#include <QList>

//-----------------------------------------------------------------------------
//!  Manages the K2 environment variables displayed in the settings dialog.
//-----------------------------------------------------------------------------
class K2EnvVarModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    //! Defines the used columns
    enum Columns
    {
        NAME_COLUMN    = 0,
        VALUE_COLUMN,
        FILE_SUFFIX_COLUMN,
        DESCRIPTION_COLUMN,
        COLUMN_COUNT
    };

    K2EnvVarModel(QSettings& settings, QObject *parent);
    
    //! The destructor.
    virtual ~K2EnvVarModel() = default;

    //! No copying
    K2EnvVarModel(const K2EnvVarModel& other) = delete;

    //! No assignment
    K2EnvVarModel& operator=(const K2EnvVarModel& other) = delete;

    /*!
     *  Save the model to settings.
     *
     *      @param [in] settings    The settings to store to.
     */
    void apply(QSettings& settings);

    /*! Get the number of rows an item contains.
     *
     *      @param [in] parent Identifies the parent that's row count is requested.
     *
     *      @return Number of rows the item has.
    */
    virtual int rowCount(QModelIndex const& parent = QModelIndex()) const override final;

    /*! Get the number of columns the item has to be displayed.
     *
     *      @param [in] parent Identifies the parent that's column count is requested.
     *
     *      @return The number of columns to be displayed.
    */
    virtual int columnCount(QModelIndex const& parent = QModelIndex()) const override final;

    /*! Get the item flags that defines the possible operations for the item.
     *
     *      @param [in] index Model index that identifies the item.
     *
     *      @return Flags specify the possible operations for the item.
    */
    virtual Qt::ItemFlags flags(QModelIndex const& index) const override final;

    /*! Get the header data for specified header.
     *
     *      @param [in] section The section specifies the row/column number for the header.
     *      @param [in] orientation Specified if horizontal or vertical header is wanted.
     *      @param [in] role Specifies the type of the requested data.
     *
     *      @return The requested data.
    */
    virtual QVariant headerData(int section, Qt::Orientation orientation, 
        int role = Qt::DisplayRole) const override final;

    /*! Get the data for specified item.
     *
     *      @param [in] index Specifies the item that's data is requested.
     *      @param [in] role The role that defines what kind of data is requested.
     *
     *      @return The data for the item.
    */
    virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const override final;

    /*! Save the data to the model for specified item
     *
     *      @param [in] index The model index of the item that's data is to be saved.
     *      @param [in] value The data that is to be saved.
     *      @param [in] role The role specifies what kind of data should be saved.
     *
     *      @return True if saving happened successfully.
    */
    virtual bool setData(QModelIndex const& index, const QVariant& value, 
        int role = Qt::EditRole) override final;
    /*!
     *  Check that all variables have at least the name defined.
     *
     *      @return True if all defined variables contain a name, otherwise false.
     */
    bool containsEmptyVariables() const;

public slots:

    /*! Add a new item to the given index.
     *
     *      @param [in] index The index identifying the position for new item.
     *
    */
    virtual void onAddItem(QModelIndex const& index);

    /*! Remove the item in the given index.
     *
     *      @param [in] index The index identifying the item to remove.
     */
    virtual void onRemoveItem(QModelIndex const& index);

private:
    

    // Loads the model data in place.
    void loadData(QSettings& settings);

    //! The settings for a single environment variable
    struct EnvSetting {

        //! The name of the variable.
        QString name_;

        //! The value for the variable.
        QString value_;

        //! If the variable if a file path then this is used to define the file suffix.
        QString fileSuffix_;

        //! Textual description of the variable.
        QString description_;

        //! Set to true if the variable is requested by the pluging.
        bool fromPlugin_;

        /*! The constructor.
         *
         *      @param [in] name        The name for the variable.
         *      @param [in] value       The value for the variable.
         *      @param [in] fileSuffix  The file suffix.
         *      @param [in] description Textual description for the variable.
         *      @param [in] fromPlugin  Defines whether the variable is requested by the plugins.
         *
        */
        explicit EnvSetting(QString const& name, 
            QString const& value,
            QString const& fileSuffix,
            QString const& description,
            bool fromPlugin);

        //! The default constructor.
        EnvSetting();

        //! The destructor.
        ~EnvSetting() = default;
        
        //! Copy constructor
        EnvSetting(const EnvSetting& other) = default;

        //! Assignment operator
        EnvSetting& operator=(const EnvSetting& other) = default;

        //! Operator <
        bool operator<(const EnvSetting& other) const;

        //! Operator ==
        bool operator==(const EnvSetting& other) const;

        //! Operator !=
        bool operator!=(const EnvSetting& other) const;
    };

    //! The list of the environment variables which is displayed/edited.
    QList<EnvSetting> table_;
};

#endif // K2ENVVARMODEL_H
