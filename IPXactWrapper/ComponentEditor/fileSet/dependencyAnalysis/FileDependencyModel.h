//-----------------------------------------------------------------------------
// File: FileDependencyModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.01.2013
//
// Description:
// The data model for the file dependencies.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYMODEL_H
#define FILEDEPENDENCYMODEL_H

#include <QAbstractItemModel>
#include <QTimer>
#include <QMap>
#include <QSharedPointer>

class FileDependencyItem;
class FileDependency;
class ISourceAnalyzerPlugin;
class Component;
class PluginManager;

//-----------------------------------------------------------------------------
//! Dependency headers.
//-----------------------------------------------------------------------------
enum FileDependencyColumn
{
    FILE_DEPENDENCY_COLUMN_TREE = 0,
    FILE_DEPENDENCY_COLUMN_STATUS,
    FILE_DEPENDENCY_COLUMN_PATH,
    FILE_DEPENDENCY_COLUMN_FILESETS,
    FILE_DEPENDENCY_COLUMN_CREATE,
    FILE_DEPENDENCY_COLUMN_DEPENDENCIES,
    FILE_DEPENDENCY_COLUMN_COUNT
};

//-----------------------------------------------------------------------------
//! The data model for the file dependencies.
//-----------------------------------------------------------------------------
class FileDependencyModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] pluginMgr  The plugin manager.
     *      @param [in] component  The component to which this model is made.
     *      @param [in] basePath   The component's base path.
     */
    FileDependencyModel(PluginManager& pluginMgr, QSharedPointer<Component> component,
                        QString const& basePath);

    /*!
     *  Destructor.
     */
    ~FileDependencyModel();

    /*!
     *  Adds a new dependency to the model.
     *
     *      @param [in] dependency The file dependency to add.
     */
    void addDependency(QSharedPointer<FileDependency> dependency);

    /*!
     *  Removes a dependency from the model.
     *
     *      @param [in] dependency The file dependency to remove.
     */
    void removeDependency(FileDependency* dependency);

    /*!
     *  Defines the location for the given item.
     *
     *      @param [in] item  The item whose location to define.
     *      @param [in] path  The path where the file resides.
     */
    void defineLocation(FileDependencyItem* item, QString const& path);

    /*!
     *  Resets the location of the given item.
     *
     *      @param [in] item  The item whose location to reset.
     */
    void resetLocation(FileDependencyItem* item);

    /*!
     *  Searches for a file item with the given path.
     *
     *      @param [in] path The path.
     *
     *      @return The corresponding item of null if not found in the model.
     */
    FileDependencyItem* findFileItem(QString const& path);

    /*!
     *  Searches for an external file item with the given path.
     *
     *      @param [in,out] path The base path as an input. The found path if the item was found.
     *
     *      @return The corresponding item of null if not found in the model.
     */
    FileDependencyItem* findExternalFileItem(QString& path);

    /*!
     *  Searches for a folder item with the given path.
     *
     *      @param [in] path The path.
     *
     *      @return The corresponding item of null if not found in the model.
     */
    FileDependencyItem* findFolderItem(QString const& path);

    /*!
     *  Searches for a dependency between the given files.
     *
     *      @param [in] file1 The first file.
     *      @param [in] file2 The second file.
     */
    FileDependency* findDependency(QString const& file1, QString const& file2) const;

    /*!
     *  Returns the model index of the given file dependency item.
     *
     *      @param [in] item The file dependency item.
     */
    QModelIndex getItemIndex(FileDependencyItem* item, int column) const;

    /*!
     *  Begins reset.
     */
    void beginReset();

    /*!
     *  Ends reset.
     */
    void endReset();

    /*!
     *  Starts the dependency analysis.
     */
    void startAnalysis();

    /*!
     *  Stops the dependency analysis.
     */
    void stopAnalysis();

    /*!
     *  Adds a new folder item to the model.
     *
     *      @param [in] path The folder path.
     */
    FileDependencyItem* addFolder(QString const& path);

    /*!
     *  Return header data for the given header column.
     *
     *      @param [in] section      The index of the header column.
	 *      @param [in] orientation  The orientation of the header, only Qt::Horizontal supported.
     *      @param [in] role         The role of the data.
     *
     *      @return QVariant containing the data.
    */
    virtual QVariant headerData(int section, Qt::Orientation orientation,
    		                    int role = Qt::DisplayRole) const;

    /*!
     *  Returns the number of columns in this model.
     *
     *      @param [in] parent Model index identifying the item whose column count is requested.
     *
     *      @return The number of columns.
     */
    virtual int columnCount(const QModelIndex & parent = QModelIndex() ) const;

    /*! 
     *  Returns the number of rows an item has.
     *
     *      @param [in] parent Model index identifying the item whose row count is wanted.
     *
     *      @return The number of rows the item has.
    */
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /*! \brief Get the model index of the specified object.
     *
     * \param row Row number of the object.
     * \param column Column number of the object.
     * \param parent Model index of the parent of the object.
     *
     * \return QModelIndex that identifies the object.
    */
    virtual QModelIndex index(int row, int column,
    		const QModelIndex &parent = QModelIndex()) const;

    /*! \brief Get the model index of the parent of the object
     *
     * \param child Model index that identifies the child of the object.
     *
     * \return QModelIndex that identifies the parent of the given object.
    */
    virtual QModelIndex parent(const QModelIndex &child) const;

    /*! \brief Get the data associated with given object.
     *
     * \param index Model index that identifies the object that's data is wanted.
     * \param role Specifies the type of data wanted.
     *
     * \return QVariant Containing the requested data.
    */
    virtual QVariant data(const QModelIndex& index,
    		              int role = Qt::DisplayRole) const;

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role /* = Qt::EditRole */);

    /*! \brief Does the specified item have child items or not.
     *
     * \param parent Model index identifying the object that's children are asked.
     *
     * \return True if object has child objects.
    */
    virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const;

    /*! \brief Get the flags that identify possible methods for given object.
     *
     * \param index Model index identifying the object that's flags are requested.
     *
     * \return Qt::ItemFlags that specify how the object can be handled.
    */
    Qt::ItemFlags flags(const QModelIndex& index) const;

    /*!
     *  Returns the number of files in the model.
     */
    int getTotalStepCount() const;

    /*!
     *  Returns the list of dependencies.
     */
    QList< QSharedPointer<FileDependency> > getDependencies() const;


signals:
    /*!
     *  Emitted when the analysis progress has changed.
     */
    void analysisProgressChanged(int value);

    /*!
     *  Emitted when a file dependency has been added to the model.
     */
    void dependencyAdded(FileDependency* dependency);

    /*!
     *  Emitted when a file dependency has been removed from the model.
     */
    void dependencyRemoved(FileDependency* dependency);

    /*!
     *  Emitted when a file dependency has changed.
     */
    void dependencyChanged(FileDependency* dependency);

    /*!
     *  Emitted when the dependencies have been reset.
     */
    void dependenciesReset();

    /*!
     *  Emitted when the contents have changed.
     */
    void contentChanged();

    /*!
     *  Emitted when the dependencies have changed.
     */
    void dependenciesChanged();

private slots:
    /*!
     *  Performs one step of the dependency analysis.
     */
    void performAnalysisStep();

private:
    // Disable copying.
    FileDependencyModel(FileDependencyModel const& rhs);
    FileDependencyModel& operator=(FileDependencyModel const& rhs);

    /*!
     *  Resolves plugins for each file type.
     */
    void resolvePlugins();

    /*!
     *  Analyzes the given file item.
     *
     *      @param [in] fileItem The file item to analyze.
     */
    void analyze(FileDependencyItem* fileItem);

    /*!
     *  Searches for a dependency between the given files in the given container.
     *
     *      @param [in] dependencies  The list of dependencies.
     *      @param [in] file1         File path of the first file.
     *      @param [in] file2         File path of the second file.
     */
    FileDependency* findDependency(QList<FileDependency*> const& dependencies,
                                   QString const& file1, QString const& file2) const;

    /*!
     *  Searches for all dependencies that reference the given file.
     *
     *      @param [in]  file          File path of the file.
     *      @param [out] dependencies  The found dependencies.
     */
    void findDependencies(QString const& file, QList<FileDependency*>& dependencies) const;

    /*!
     *  Moves the item to the new parent.
     *
     *      @param [in] item    The item to move.
     *      @param [in] parent  The new parent for the item.
     */
    void moveItem(FileDependencyItem* item, FileDependencyItem* parent);

    /*!
     *  Updates dependencies due to file relocation.
     */
    void onExternalRelocated(FileDependencyItem* item, QString const& oldPath);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The plugin manager.
    PluginManager& pluginMgr_;

    //! The parent component.
    QSharedPointer<Component> component_;

    //! The component's base path.
    QString basePath_;

    //! The dependency tree root.
    FileDependencyItem* root_;

    //! The externals item (for fast access).
    FileDependencyItem* unspecifiedLocation_;

    //! The timer for running the analysis.
    QTimer* timer_;

    //! The current folder scan index.
    int curFolderIndex_;

    //! The current file scan index.
    int curFileIndex_;

    //! The current analysis progress.
    int progressValue_;

    //! The list of used plugins on the current run.
    QList<ISourceAnalyzerPlugin*> usedPlugins_;

    //! Analyzer plugin map for fast access to correct plugin for each file type.
    QMap<QString, ISourceAnalyzerPlugin*> analyzerPluginMap_;

    //! Dependency list.
    QList< QSharedPointer<FileDependency> >& dependencies_;
};

//-----------------------------------------------------------------------------

#endif // FILEDEPENDENCYMODEL_H
