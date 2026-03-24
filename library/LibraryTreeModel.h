//-----------------------------------------------------------------------------
// File: LibraryTreeModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 20.3.2026
//
// Description:
// Wrapper of library model for Kactus2 GUI purposes.
//-----------------------------------------------------------------------------

#pragma once

#include <KactusAPI/include/LibraryModel.h>

#include <QObject>

class LibraryTreeModel : public LibraryModel
{
    Q_OBJECT
public:

    explicit LibraryTreeModel(LibraryInterface* handler, QObject *parent = nullptr);

    /*! Get the data associated with given object.
    *
    *    @param [in] index    Model index that identifies the object that's data is wanted.
    *    @param [in] role     Specifies the type of data wanted.
    *
    *    @return QVariant Containing the requested data.
    */
    virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const override;
};

