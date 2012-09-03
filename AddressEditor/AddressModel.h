//-----------------------------------------------------------------------------
// File: AddressModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.08.2012
//
// Description:
// Data model for the address editor.
//-----------------------------------------------------------------------------

#ifndef ADDRESSMODEL_H
#define ADDRESSMODEL_H

#include "AddressEntry.h"

#include <common/GenericEditProvider.h>

#include <QAbstractTableModel>
#include <QMap>
#include <QList>
#include <QString>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Data model for the address editor.
//-----------------------------------------------------------------------------
class AddressModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    /*!
     *  Constructor.
     *
     *      @param [in] parent The owner of this model.
     *
     */
    AddressModel(QObject* parent);
    
    /*!
     *  The destructor.
     */
    virtual ~AddressModel();

    /*!
     *  Sets the component being edited.
     *
     *      @param [in] component Pointer to the component being edited.
     */
    void setComponent(ComponentItem* component);

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
     *  Sets the data for specified index.
     *
     *      @param [in] index  Identifies the object that's data is to be saved.
     *      @param [in] value  Contains the data to be saved.
     *      @param [in] role   Identifies the type of the data to be saved.
     *
     *      @return True if data was saved successfully.
     */
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );

    /*!
     *  Retrieves info on what operations are possible for specified item.
     *
     *      @param [in] index Identifies the item that's operations are requested.
     *
     *      @return Item flags containing info on which operations are available for the given index.
     */
    virtual Qt::ItemFlags flags(const QModelIndex& index) const;

public slots:
    /*!
     *  Clears the model.
     */
    void clear();

    /*!
     *  Auto-assigns the addresses so that there are no address collisions.
     */
    void autoAssignAddresses();

    /*!
     *  Imports address entries from the given CSV file.
     */
    void importCSV(QString const& filename);

    /*!
     *  Exports address entries to the given CSV file.
     */
    void exportCSV(QString const& filename);

    /*!
     *  Refreshes the view.
     */
    void refresh();

    /*!
     *  Moves an item to a new position.
     *
     *      @param [in] originalPos  The original position of the item.
     *      @param [in] newPos       The new position for the item.
     */
    void onMoveItem(QModelIndex const& originalPos, QModelIndex const& newPos);


signals:
    //! \brief Emitted when contents of the model changes.
    void contentChanged();

private:
    // Disable copying.
    AddressModel(AddressModel const& rhs);
    AddressModel& operator=(AddressModel const& rhs);

    static bool addressEntrySortOp(QSharedPointer<AddressEntry> lhs, QSharedPointer<AddressEntry> rhs);

    /*!
     *  Returns true if the entry is valid (i.e. does not overlap with any other entry).
     *
     *      @param [in] index The index of the entry to validate.
     */
    bool checkRangeOverlaps(int index) const;
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! \brief Pointer to the component instance being edited.
    ComponentItem* component_;

    //! \brief Pointer to the generic edit provider that manages the undo/redo stack.
    GenericEditProvider* editProvider_;

    //! The model address entries.
    QList< QSharedPointer<AddressEntry> > addressEntries_;
};

//-----------------------------------------------------------------------------

#endif // ADDRESSMODEL_H
