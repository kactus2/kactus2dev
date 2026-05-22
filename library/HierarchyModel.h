//-----------------------------------------------------------------------------
// File: HierarchyModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 07.04.2026
//
// Description:
// Extended hierarchy model containing GUI functionality for Kactus2
//-----------------------------------------------------------------------------
#pragma once

#include <KactusAPI/include/HierarchyModelBase.h>

class HierarchyModel : public HierarchyModelBase
{
    Q_OBJECT
public:
    
    /*! The constructor
     * 
     *    @param [in] handler         The instance that manages the library.
     *    @param [in] parent          The owner of this model.
     */
    HierarchyModel(LibraryInterface* handler, QObject* parent);
    ~HierarchyModel() = default;

    HierarchyModel(HierarchyModel const& other) = delete;
    HierarchyModel& operator=(HierarchyModel const& other) = delete;

    /*! Get the data associated with given object.
     *
     *    @param [in] index Model index that identifies the object that's data is wanted.
     *    @param [in] role Specifies the type of data wanted.
     *
     *    @return QVariant Containing the requested data.
    */
    QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const override;
};
