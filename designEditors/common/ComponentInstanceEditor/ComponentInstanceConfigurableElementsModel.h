//-----------------------------------------------------------------------------
// File: ComponentInstanceConfigurableElementsModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 17.08.2017
//
// Description:
// Model class to manage the configurable element values of component instances.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANCECONFIGURABLEELEMENTSMODEL_H
#define COMPONENTINSTANCECONFIGURABLEELEMENTSMODEL_H

#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ConfigurableElementFinder.h>

#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <QAbstractTableModel>
#include <QList>
#include <QString>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

class Parameter;
class ModuleParameter;
class Choice;
class ComponentInstance;
class Component;
class ConfigurableElementsModel;

//-----------------------------------------------------------------------------
//! Model class to manage the configurable element values of component instances.
//-----------------------------------------------------------------------------
class ComponentInstanceConfigurableElementsModel : public QAbstractTableModel
{
	Q_OBJECT

public:

    //! Root item.
    struct rootModel
    {
        //! The model containing the configurable elements.
        ConfigurableElementsModel* elementModel_;

        //! Filter for the model.
        QSortFilterProxyModel* elementFilter_;

        //! Name of the root item.
        QString name_;
    };


	/*!
	 *  The constructor.
	 *
	 *      @param [in] parameterFinder             The parameter finder.
	 *      @param [in] elementFinder               Configurable elements finder.
	 *      @param [in] elementExpressionFormatter  Expression formatter for configurable element values.
	 *      @param [in] defaultValueFormatter       Expression fromatter for default values.
	 *      @param [in] elementExpressionParser     Parser for configurable element values.
	 *      @param [in] defaultValueParser          Parser for default values.
	 *      @param [in] parent                      The parent object.
	 */
	ComponentInstanceConfigurableElementsModel(QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ConfigurableElementFinder> elementFinder,
        QSharedPointer<ExpressionFormatter> elementExpressionFormatter,
        QSharedPointer<ExpressionFormatter> defaultValueFormatter,
        QSharedPointer<ExpressionParser> elementExpressionParser,
        QSharedPointer<ExpressionParser> defaultValueParser, QObject* parent);

	/*!
	 *  The destructor.
	 */
    virtual ~ComponentInstanceConfigurableElementsModel();

	/*!
	 *  Set the parameters for configurable element values.
	 *
	 *      @param [in] component           Pointer to the component referenced by the component instance.
     *      @param [in] instance            Pointer to the component instance being edited.
     *      @param [in] viewConfiguration   Pointer to the active view of the instanced component.
	 */
    void setParameters(QSharedPointer<Component> component, QSharedPointer<ComponentInstance> instance,
        QSharedPointer<ViewConfiguration> viewConfiguration);

	/*!
	 *  Get the number of rows to be displayed.
	 *
	 *      @param [in] parent  Identifies the parent, must always be invalid index.
     *
     *      @return The number of rows to display.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
	 *  Get the number of columns to display.
	 *
	 *      @param [in] parent  Identifies the parent, must always be invalid index.
     *
     *      @return The number of columns to display.
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
	 *  Get the data for specified index in given data role.
	 *
	 *      @param [in] index   Identifies the object that's data is wanted.
	 *      @param [in] role    Identifies the type of data being requested.
     *
     *      @return QVariant that contains the requested data.
	 */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*!
	 *  Get the data for the given header in the given data role.
	 *
	 *      @param [in] section         Identifies the header section.
	 *      @param [in] orientation     Only horizontal header is supported.
	 *      @param [in] role            Identifies the type of the data being requested.
     *
     *      @return QVariant that contains the requested data.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/*!
	 *  Set the data for the specified index.
	 *
	 *      @param [in] index   Identifies the object which data is to be saved.
	 *      @param [in] value   Contains the data to be saved.
	 *      @param [in] role    Identifies the type of the data to be saved.
     *
     *      @return True if data was saved successfully.
	 */
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );

	/*!
	 *  Get info on what operations are possible for specified item.
	 *
	 *      @param [in] index   Identifies the item that's operations are requested.
     *
     *      @return Flags containing info on which operations are available for given index.
	 */
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

    /*!
     *  Get the model index of the specified object.
     *
     *      @param [in] row     Row number of the object.
     *      @param [in] column  Column number of the object.
     *      @param [in] parent  Model index of the parent of the object.
     *
     *      @return QModelIndex that identifies the object.
     */
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

    /*!
     *  Get the model index of the parent of the object.
     *
     *      @param [in] child   Model index that identifies the child of the object.
     *
     *      @return QModelindex that identifies the parent of the given object.
     */
    QModelIndex parent(const QModelIndex& child) const;

public slots:

	/*!
	 *  Clear the model.
	 */
	void clear();

    /*!
     *  Handles the signals for data change in the selected configurable element.
     *
     *      @param [in] parameterID     ID of the parameter referenced by the selected configurable element.
     *      @param [in] newDataValue    New value for the configurable element.
     */
    void onDataChangedInID(QString const& parameterID, QString const& newDataValue);

    /*!
     *  Handles the adding of configurable elements.
     *
     *      @param [in] elementID       ID of the parameter referenced by the configurable element.
     *      @param [in] elementValue    Value of the configurable element.
     *      @param [in] parentName      Name of the root item containing the configurable element.
     *      @param [in] elementRow      Index of the configurable element.
     */
    void onAddElement(QString const& elementID, QString const& elementValue, QString const& parentName,
        int elementRow);

    /*!
     *  Handles the removing of configurable elements.
     *
     *      @param [in] elementID   ID of the parameter referenced by the configurable element.
     *      @param [in] parentName  Name of the root item contianing the configurable element.
     *      @param [in] elementRow  Index of the configurable element.
     */
    void onRemoveElement(QString const& elementID, QString const& parentName, int elementRow);

signals:

	/*!
     *  Emitted when contents of the model changes.
     */
	void contentChanged();

    /*!
     *  Signal for connecting the root item models to the flag for displaying immediate values.
     *
     *      @param [in] show    Flag for displaying the immediate values.
     */
    void showImmediateValuesInModels(bool show);

    /*!
     *  Emitted to invalidate the assigned filter.
     */
    void invalidateFilter();

private:

    //! No copying. //! No assignment.
    ComponentInstanceConfigurableElementsModel(const ComponentInstanceConfigurableElementsModel& other);
    ComponentInstanceConfigurableElementsModel& operator=(const ComponentInstanceConfigurableElementsModel& other);

    /*!
     *  Get the list of contained module parameters.
     *
     *      @param [in] component           The component containing the module parameters.
     *      @param [in] viewConfiguration   View configuration assigned to the component instance.
     *
     *      @return Module parameters contained within the selected view configuration.
     */
    QSharedPointer<QList<QSharedPointer<Parameter> > > getModuleParameters(
        QSharedPointer<Component> component, QSharedPointer<ViewConfiguration> viewConfiguration);

    /*!
     *  Check if the selected index is valid.
     *
     *      @param [in] index   The selected index.
     *
     *      @return True, if the index is valid, false otherwise.
     */
    bool isIndexValid(QModelIndex const& index) const;

    /*!
     *  Get a list of all the parameters accessed through the view configuration.
     *
     *      @param [in] moduleParameters    List of contained module parameters.
     *
     *      @return A Pointer to a list containing parameters accessed through the view configuration.
     */
    QSharedPointer<QList<QSharedPointer<Parameter> > > gatherAllViewConfigurationParameters(
        QSharedPointer<QList<QSharedPointer<Parameter> > > moduleParameters);

    /*!
     *  Find the root item containing a configurable element with a reference to the selected parameter ID.
     *
     *      @param [in] referencedID    The selected parameter ID.
     *
     *      @return The root item containing the selected configurable element.
     */
    QSharedPointer<rootModel> getRootModelContainingReferencedID(QString const& referencedID) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of all the root items.
    QList<QSharedPointer<rootModel> > rootItems_;

    //! Root item containing component parameters.
    QSharedPointer<rootModel> parametersRoot_;

    //! Root item containing module parameters.
    QSharedPointer<rootModel> moduleParametersRoot_;

    //! Root item containing missing parameters from the component instance configurable element values.
    QSharedPointer<rootModel> missingParametersRoot_;

    //! Root item containing missing parameters from the view configuration configurable element values.
    QSharedPointer<rootModel> unknownParametersRoot_;

    //! The configurable element finder.
    QSharedPointer<ConfigurableElementFinder> elementFinder_;
};

#endif // COMPONENTINSTANCECONFIGURABLEELEMENTSMODEL_H
