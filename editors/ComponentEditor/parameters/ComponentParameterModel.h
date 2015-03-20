//-----------------------------------------------------------------------------
// File: ComponentParameterModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.01.2015
//
// Description:
// Model for all parameter completions within a single component.
//-----------------------------------------------------------------------------

#ifndef COMPONENTPARAMETERMODEL_H
#define COMPONENTPARAMETERMODEL_H

#include <QAbstractItemModel>
#include <QSharedPointer>

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>

//-----------------------------------------------------------------------------
//! Model for all parameter completions within a single component.
//-----------------------------------------------------------------------------
class ComponentParameterModel : public ParameterizableTable
{
    Q_OBJECT
public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] parameterFinder     The parameter finder.
	 *      @param [in] parent              The owner of this model.
	 */
	ComponentParameterModel(QSharedPointer<ParameterFinder> parameterFinder, QObject *parent);

	//! The destructor.
	virtual ~ComponentParameterModel();

    /*!
     *  Finds the number of rows in the model.
     *
     *      @param [in] parent   The parent index whose row count to find.
     *
     *      @return The number of rows in the model.
     */
    virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

    /*!
     *  Finds the number of columns in the model.
     *
     *      @param [in] parent   The parent index whose column count to find.
     *
     *      @return The number of columns in the model.
     */
    virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

    /*!
     *  Finds the flags for the given index.
     *
     *      @param [in] index   The index whose flags to find.
     *
     *      @return Flags describing allowed actions for the index.
     */
    virtual Qt::ItemFlags flags(QModelIndex const& index) const;

    /*!
     *  Finds the parent index for the given child.
     *
     *      @param [in] child   The index whose parent index to find.
     *
     *      @return The index for the parent.
     */
    virtual QModelIndex parent(QModelIndex const& child) const;

    /*!
     *  Finds the data in the model for the given index.
     *
     *      @param [in] index   The index whose data to find.
     *      @param [in] role    The role describing which aspect of the data to find.
     *
     *      @return The data in the given index.
     */
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

public slots:

    /*!
     *  Reset the model to show the correct parameters.
     */
    void resetParameterModel();

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
     *  Gets the expression for the given index or the plain value if expression is not available.
     *
     *      @param [in] index   The index whose expression to get.
     *
     *      @return The expression for the index if available, otherwise the value for the given index.
     */
    virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const;
    
    /*!
     *  Validates the data in a parameter corresponding to a given column.
     *
     *      @param [in] index   The index whose data to validate
     *
     *      @return True, if the data in the parameter is valid, otherwise false.
     */
    virtual bool validateColumnForParameter(QModelIndex const& index) const;

    /*!
     *  Get all the references to the selected parameter from the selected row.
     *
     *      @param [in] row         The selected row.
     *      @param [in] valueID     The id of the referenced parameter.
     *
     *      @return The amount of references to the selected parameter.
     */
    virtual int getAllReferencesToIdInItemOnRow(const int& row, QString valueID) const;

private:

	// Disable copying.
	ComponentParameterModel(ComponentParameterModel const& rhs);
	ComponentParameterModel& operator=(ComponentParameterModel const& rhs);

    //! The parameter finder for searching all the parameters.
    QSharedPointer<ParameterFinder> parameterFinder_;
};

#endif // COMPONENTPARAMETERMODEL_H
