//-----------------------------------------------------------------------------
// File: ApiFunctionParameterDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 18.4.2012
//
// Description:
// Delegate for editing API function parameters.
//-----------------------------------------------------------------------------

#ifndef APIFUNCTIONPARAMETERDELEGATE_H
#define APIFUNCTIONPARAMETERDELEGATE_H

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
// Constants defining which column represents what kind of information.
//-----------------------------------------------------------------------------
enum ApiFunctionParamColumn
{
    API_FUNC_PARAM_COL_NAME = 0,      //!< Column for the parameter name.
    API_FUNC_PARAM_COL_TYPE,          //!< Column for setting the value type for the parameter.
    API_FUNC_PARAM_COL_DESC,          //!< Column for adding a description for the parameter.
    API_FUNC_PARAM_COL_COUNT
};

//-----------------------------------------------------------------------------
// Delegate for editing API function parameters.
//-----------------------------------------------------------------------------
class ApiFunctionParameterDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	/*!
     *  Constructor.
     *
     *      @param [in] parent The parent object.
     */
	ApiFunctionParameterDelegate(QObject* parent = 0);

	/*!
     *  Destructor.
     */
	virtual ~ApiFunctionParameterDelegate();

    /*!
     *  Adds the given data types to the list of parameter type suggestions.
     *
     *      @param [in] dataTypes The list of data types to add.
     */
    void updateDataTypes(QStringList const& dataTypes);

    /*!
     *  Returns the list of all data type suggestions.
     */
    QStringList getDataTypesList() const;

	/*!
     *  Creates a new editor for the given item.
	 *
	 *      @param [in] parent  The parent widget.
     *      @param [in] option  Contains options for the editor.
     *      @param [in] index   Model index identifying the item.
	 *
	 *      @return Pointer to the editor to be used to edit the item.
	 */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
		                          QModelIndex const& index) const;

	/*!
     *  Sets the data for the editor.
	 *
	 *      @param [in] editor  Pointer to the editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
	 */
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*!
     *  Saves the data from the editor to the model.
	 *
	 *      @param [in] editor  Pointer to the editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   Model index identifying the item that's data is to be saved.
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		                      QModelIndex const& index) const;

private slots:
	/*! 
     *  Commits the data from the sending editor and close the editor.
	 */
	void commitAndCloseEditor();

private:
    // Disable copying.
    ApiFunctionParameterDelegate(ApiFunctionParameterDelegate const& rhs);
    ApiFunctionParameterDelegate& operator=(ApiFunctionParameterDelegate const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The list of built-in data types.
    QStringList builtInTypes_;

    //! The list of custom parameter data types.
    QStringList customTypes_;
};

//-----------------------------------------------------------------------------

#endif // APIFUNCTIONPARAMETERDELEGATE_H
