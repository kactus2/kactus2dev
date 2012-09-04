//-----------------------------------------------------------------------------
// File: LibraryErrorModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 04.09.2012
//
// Description:
// Library error model.
//-----------------------------------------------------------------------------

#ifndef LIBRARYERRORMODEL_H
#define LIBRARYERRORMODEL_H

#include <QAbstractTableModel>
#include <QMap>
#include <QList>
#include <QString>
#include <QSharedPointer>
#include <QStringList>

//-----------------------------------------------------------------------------
//! Library error model.
//-----------------------------------------------------------------------------
class LibraryErrorModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The owner of this model.
     */
    LibraryErrorModel(QObject* parent);
    
    /*!
     *  The destructor.
     */
    virtual ~LibraryErrorModel();

    /*!
     *  Adds errors to the model.
     */
    void addErrors(QStringList const& errorList);

    /*!
     *  Returns the number of rows to be displayed.
     *
     *      @param [in] parent Identifies the parent. Must always be a invalid index.
     */
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    /*!
     *  Returns the number of columns to display.
     *
     *      @param [in] parent Identifies the parent. Must always be a invalid index.
     */
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

    /*!
     *  Returns that data for specified index in given data role.
     *
     *      @param [in] index  Identifies the object that's data is wanted.
     *      @param [in] role   Identifies the type of data being requested.
     *
     *      @return QVariant containing the requested data.
     */
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    /*!
     *  Returns the data for given header in given data role.
     *
     *      @param [in] section      Identifies the header section.
     *      @param [in] orientation  Only Qt::Horizontal is supported.
     *      @param [in] role         Identifies the type of the data being requested.
     *
     *      @return QVariant contains the requested data.
     */
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    /*!
     *  Retrieves info on what operations are possible for specified item.
     *
     *      @param [in] index Identifies the item that's operations are requested.
     *
     *      @return Item flags containing info on which operations are available for the given index.
     */
    virtual Qt::ItemFlags flags(const QModelIndex& index) const;

private:
    // Disable copying.
    LibraryErrorModel(LibraryErrorModel const& rhs);
    LibraryErrorModel& operator=(LibraryErrorModel const& rhs);

    enum Column
    {
        COLUMN_NUMBER = 0,
        COLUMN_MESSAGE,
        COLUMN_COUNT
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The list of errors.
    QStringList errors_;
};

//-----------------------------------------------------------------------------

#endif // LIBRARYERRORMODEL_H
