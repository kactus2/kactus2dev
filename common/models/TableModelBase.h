//-----------------------------------------------------------------------------
// File: TableModelBase.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 30.03.2026
//
// Description:
// Base class for various table models found in Kactus2. Handles common functionality
// of table models.
//-----------------------------------------------------------------------------

#pragma once

#include <QAbstractTableModel>
#include <QObject>

class TableModelBase : public QAbstractTableModel
{
    Q_OBJECT

public:
    TableModelBase(QObject* parent = nullptr);
    virtual ~TableModelBase() = default;

    TableModelBase(TableModelBase const& other) = delete;
    TableModelBase& operator=(TableModelBase const& other) = delete;

    QVariant data(QModelIndex const& index, int role) const override;


protected:

    /*!
     *  Gets a black color for valid index and red color for invalid index.
     *
     *    @param [in] index   The index for which to get the color.
     *
     *    @return Black for valid index, red for invalid index.
     */
    QVariant blackForValidOrRedForInvalidIndex(QModelIndex const& index) const;

    /*!
     *  Validates the data in an index.
     *
     *    @param [in] index   The index whose data to validate
     *
     *    @return True, if the data in the index is valid, otherwise false.
     */
    virtual bool validateIndex(QModelIndex const& index) const = 0;

    /*!
     *  Checks if the given index is marked mandatory. Default implementation always returns false.
     *  Should be overridden by inheriting classes if needed.
     *
     *    @param[in] index   Model index to check.
     *
     *    @return True, if the index is a mandatory item, otherwise false.
     */
    virtual bool indexIsMandatory(QModelIndex const& index) const;

    /*!
     *  Checks if the given index corresponds to a table cell whose text should be grayed out. This could be the case for any item
     *  marked with non-true isPresent value (i.e. the item is editable, but not in use) or if editing of the value is disabled.
     *
     *  Default implementation always returns false. Should be overridden by inheriting classes if needed.
     *
     *    @param[in] index   Model index to check.
     *
     *    @return True, if the index is supposed to be greyed out, otherwise false.
     */
    virtual bool indexIsGreyedOut(QModelIndex const& index) const;
};