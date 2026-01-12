//-----------------------------------------------------------------------------
// File: LockableParametersModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 10.11.2025
//
// Description:
// Implementation of parameters model that allows locking editing of parameters
//-----------------------------------------------------------------------------

#ifndef LOCKABLEPARAMETERSMODEL_H
#define LOCKABLEPARAMETERSMODEL_H

#include "parametersmodel.h"
#include "ParameterColumns.h"

class LockableParametersModel : public ParametersModel
{
    Q_OBJECT
public:

    /*!
     *  The constructor
     *
     *    @param [in] parameterInterface  Interface for accessing parameters.
     *    @param [in] expressionParser    Expression parser for configurable elements.
     *    @param [in] parameterFinder     The parameter finder.
     *    @param [in] parent              The owner of this model.
     */
    LockableParametersModel(ParametersInterface* parameterInterface,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ParameterFinder> parameterFinder,
        QObject* parent);

	~LockableParametersModel() final = default;

    /*!
     *  Get the data for the specified item for specified role.
     *  Overrides background color for locked parameters.
     *
     *    @param [in]  index  Identifies the item that's data is wanted.
     *    @param [in]  role   Specifies what kind of data is wanted.
     *
     *    @return  The data for the given index.
     */
    QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const override;

    /*!
     *  Get information on how specified item can be handled. Disable editing of indices in indicesToLock_.
     *
     *    @param [in]  index Specifies the item that's flags are wanted.
     *
     *    @return  Qt::ItemFlags that define how object can be handled.
     */
    Qt::ItemFlags flags(QModelIndex const& index) const override;

    /*!
     *	Lock editing of a given column/cell/index of a given parameter.
     *  
     *    @param [in] parameterName     Parameter whose cell to lock.
     *    @param [in] column            The column to lock.
     */
    void lockParameterColumn(QString const& parameterName, ParameterColumns::columns column);

    /*!
     *  Reset the model. Also clears the indices to lock.
     */
    void resetModelItems() override;

public slots:

    /*!
     *  An item should be removed from the model.
     *
     *    @param [in]  index Identifies the item that should be removed.
     */
    virtual void onRemoveItem(QModelIndex const& index) override;


private:

    //! Stores the indices of the model that should be locked.
    QHash<int, QSet<ParameterColumns::columns> > indicesToLock_;

};


#endif // LOCKABLEPARAMETERSMODEL_H
