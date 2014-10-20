//-----------------------------------------------------------------------------
// File: ModelParameterComparator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.10.2014
//
// Description:
// Comparator for finding differences in lists of model parameters.
//-----------------------------------------------------------------------------

#ifndef MODELPARAMETERCOMPARATOR_H
#define MODELPARAMETERCOMPARATOR_H

#include <QList>
#include <QSharedPointer>

#include <wizards/common/IPXactDiff.h>
#include <wizards/common/IPXactElementComparator.h>

class ModelParameter;

//-----------------------------------------------------------------------------
//! Comparator for finding differences in lists of model parameters.
//-----------------------------------------------------------------------------
class ModelParameterComparator : public IPXactElementComparator<ModelParameter>
{
public:
    ModelParameterComparator();
    ~ModelParameterComparator();
    
    /*!
     *  Compares the two lists of model parameters.
     *
     *      @param [in] first    The first lists.
     *      @param [in] second   The second lists.
     *
     *      @return True, if the lists are similar, otherwise false.
     */
    bool compare(QList<QSharedPointer<ModelParameter> > const first, 
        QList<QSharedPointer<ModelParameter> > const second) ;

    bool compare(QSharedPointer<const ModelParameter> first, QSharedPointer<const ModelParameter> second) const;

    /*!
     *  Finds the differences between the two given lists.
     *
     *      @param [in] reference   The reference lists to compare to.
     *      @param [in] subject     The lists to compare against the reference.
     *
     *      @return Set of differences between the reference and subject.
     */
    QList<QSharedPointer<IPXactDiff> > diff(QList<QSharedPointer<ModelParameter> > reference, 
        QList<QSharedPointer<ModelParameter> > subject);

    virtual QList<QSharedPointer<IPXactDiff> > diff(QSharedPointer<const ModelParameter> reference, 
        QSharedPointer<const ModelParameter> subject) const;

private:

    //! Disable copying.
    ModelParameterComparator(ModelParameterComparator const& rhs);
    ModelParameterComparator& operator=(ModelParameterComparator const& rhs);

    /*!
     *  Creates a map of model parameters where the model parameter name is the key.
     *
     *      @param [in] list   The model parameters to include in the map.
     *
     *      @return Map of model parameters.
     */
    QMap<QString, QSharedPointer<const ModelParameter> > mapById(QList<QSharedPointer<ModelParameter> > const list);

    bool hasElementForId(QMap<QString, QSharedPointer<const ModelParameter> > mappedElements, QString const& id);

    QSharedPointer<const ModelParameter> elementForId(QMap<QString, QSharedPointer<const ModelParameter> > const& mappedElements, 
        QString const& id);
};

#endif // MODELPARAMETERCOMPARATOR_H
