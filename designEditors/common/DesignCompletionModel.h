//-----------------------------------------------------------------------------
// File: DesignCompletionModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.03.2015
//
// Description:
// Completion model for parameter names in a design.
//-----------------------------------------------------------------------------

#ifndef DESIGNCOMPLETIONMODEL_H
#define DESIGNCOMPLETIONMODEL_H

#include <QAbstractItemModel>
#include <QSharedPointer>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

//-----------------------------------------------------------------------------
//! Completion model for parameter names in a design.
//-----------------------------------------------------------------------------
class DesignCompletionModel : public ComponentParameterModel
{
    Q_OBJECT
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] topComponentFinder      Finder for top component parameters.
	 *      @param [in] parameterFinder         Finder for all available parameters.
	 *      @param [in] parent                  The parent object.
	 */
	DesignCompletionModel(QSharedPointer<ParameterFinder> topComponentFinder, 
        QSharedPointer<ParameterFinder> parameterFinder, QObject *parent);

	//! The destructor.
	~DesignCompletionModel();

    /*!
     *  Finds the data in the model for the given index.
     *
     *      @param [in] index   The index whose data to find.
     *      @param [in] role    The role describing which aspect of the data to find.
     *
     *      @return The data in the given index.
     */
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private:

	// Disable copying.
	DesignCompletionModel(DesignCompletionModel const& rhs);
	DesignCompletionModel& operator=(DesignCompletionModel const& rhs);

    //! The finder for top component parameters.
    QSharedPointer<ParameterFinder> topFinder_;
};

#endif // DESIGNCOMPLETIONMODEL_H
