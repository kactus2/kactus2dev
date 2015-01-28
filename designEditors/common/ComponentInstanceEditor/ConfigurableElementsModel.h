/* 
 *  	Created on: 12.8.2011
 *      Author: Antti Kamppi
 * 		filename: componentinstancemodel.h
 *		Project: Kactus 2
 */

#ifndef CONFIGURABLEELEMENTSMODEL_H
#define CONFIGURABLEELEMENTSMODEL_H

#include <common/GenericEditProvider.h>
#include <designEditors/HWDesign/HWComponentItem.h>
#include <IPXACTmodels/parameter.h>
#include <IPXACTmodels/modelparameter.h>
#include <editors/ComponentEditor/common/ParameterizableTable.h>

#include <QAbstractTableModel>
#include <QMap>
#include <QList>
#include <QString>
#include <QSharedPointer>

class ComponentItem;
//-----------------------------------------------------------------------------
//! Model class to manage the configurable element values being edited.
//-----------------------------------------------------------------------------
class ConfigurableElementsModel : public QAbstractTableModel, public ParameterizableTable
{
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the owner of this model.
	 *
	*/
	ConfigurableElementsModel(QObject *parent);
	
	//! \brief The destructor
	virtual ~ConfigurableElementsModel();

	/*! \brief Set the component being edited.
	 *
	 * \param component Pointer to the component being edited.
	 *
	*/
	void setComponent(ComponentItem* component);

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

	/*! \brief Remove the specified item from the model.
	 *
	 * \param index Identifies the configurable element to remove.
	 *
	*/
	void onRemove(const QModelIndex& index);

	/*! \brief An item should be removed from the model.
	 * 
	 * \param index Identifies the item that should be removed.
	 *
	*/
	void onRemoveItem(const QModelIndex& index);

	/*! \brief Clear the model.
	 *
	*/
	void clear();

	//! \brief Change the configurable elements of this model.
	void changeElements(const QMap<QString, QString>& confElements);

signals:

	//! \brief Emitted when contents of the model changes.
	void contentChanged();

protected:

    /*!
     *  Check if the column index is valid for containing expressions.
     *
     *      @param [in] index   The index being evaluated.
     *
     *      @return     True, if column can have expressions, false otherwise.
     */
    virtual bool isValidExpressionColumn(QModelIndex const& index) const;

    /*!
     *  Gets the expression for the given index, or plain value if there is no expression.
     *
     *      @param [in] index   The index of target data.
     *
     *      @return     Expression in the given index if there is one, or plain value.
     */
    virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const;

    /*!
     *  Validates the data in a parameter corresponding to a given column.
     *
     *      @param [in] index   The index whose data to validate.
     *
     *      @return True, if the data in the parameter is valid, otherwise false.
     */
    virtual bool validateColumnForParameter(QModelIndex const& index) const;

private:
	//! No copying
	ConfigurableElementsModel(const ConfigurableElementsModel& other);

	//! No assignment
	ConfigurableElementsModel& operator=(const ConfigurableElementsModel& other);

    /*!
     *  Gets the value for the given index.
     *
     *      @param [in] index   The index of target data.
     *
     *      @return     The data in the given index.
     */
    QVariant valueForIndex(QModelIndex const& index) const;

    //! The values for the model columns.
    enum modelColumns
    {
        NAME = 0,
        VALUE,
        DEFAULT_VALUE
    };

	//! Struct that contains the name-value pair for configurable element.
	struct ConfigurableElement {

		//! The name of the configurable element.
		QString name_;

        //! The unique id of the configurable element
        QString uuID_;

		//! The value of the configurable element.
		QString value_;

        //! The default value of the configurable element.
        QString defaultValue_;

        //! True if item is editable.
        bool isEditable_;

		/*! \brief Struct constructor
		 *
		 * \param name Name of the configurable element
		 * \param value Value of the configurable element.
		 *
		*/
		ConfigurableElement(const QString& name, const QString& uuID, 
                            const QString& value, const QString& defaultValue, const bool& isEditable);
        //ConfigurableElement(const QSharedPointer <Parameter> parameter, const QString& value);

		/*! \brief Constructs struct with empty value
		 * 
		 * \param name Name of the configurable element
		 *
		*/
		ConfigurableElement(const QString& name, const QString& value);
        //ConfigurableElement(const QSharedPointer <Parameter> parameter);

		/*! \brief Operator ==
		 *
		 * \param other Reference to the Configurable element to compare to.
		 *
		 * \return bool True if the names of the configurable elements match.
		*/
		bool operator==(const ConfigurableElement& other) const;
	};

	//! Save the elements from the table to values_ map.
	void save();

	/*!
	 *  Read the configurable elements from the component and from the saved values.
	 */
	void readValues();

    /*!
     *  Read the configurable elements from the component.
     */
    void readComponentConfigurableElements();

    /*!
     *  Read the configurable parameters from the component.
     *
     *      @param [in] componentModel      The component pointer.
     *      @param [in] unEditableResolve   The text for uneditable resolve value.
     */
    void readConfigurableParameters(QSharedPointer <Component> componentModel, QString unEditableResolve);

    /*!
     *  Read the configurable model parameters from the component.
     *
     *      @param [in] componentModel      The component pointer.
     *      @param [in] unEditableResolve   The text for uneditable resolve value.
     */
    void readConfigurableModelParameters(QSharedPointer <Component> componentModel, QString unEditableResolve);

    /*!
     *  Add parameter to the table, if it has an id.
     *
     *      @param [in] parameterPointer        Pointer to the parameter or model parameter.
     *      @param [in] unEditableResolveValue  The text for uneditable resolve value.
     */
    void addParameterWithIDToVisibleElements(QSharedPointer <Parameter> parameterPointer,
        QString unEditableResolveValue);

	//! Pointer to the component instance being edited.
	ComponentItem* component_;

	//! Reference to the map containing the actual configurable elements.
	QMap<QString, QString> currentElementValues_;

	//! The list that is used to display the elements in a table form.
	QList<ConfigurableElement> visibleConfigurableElements_;

	//! Pointer to the generic edit provider that manages the undo/redo stack.
	GenericEditProvider* editProvider_;
};

#endif // CONFIGURABLEELEMENTSMODEL_H
