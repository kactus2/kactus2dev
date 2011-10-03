//-----------------------------------------------------------------------------
// File: EndpointEditDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.3.2011
//
// Description:
// Endpoint editor widget
//-----------------------------------------------------------------------------

#ifndef ENDPOINTEDITDIALOG_H
#define ENDPOINTEDITDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

#include "../MCAPI/MCAPIDesignerTypes.h"

class EndpointItem;

//-----------------------------------------------------------------------------
//! EndpointEditDialog class
//-----------------------------------------------------------------------------
class EndpointEditDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     *  Constructor which initializes the dialog with the data from an existing
     *  endpoint, if given.
     *
     *      @param [in] parent   The parent widget.
     *      @param [in] endpoint The existing endpoint.
     */
    EndpointEditDialog(QWidget* parent = 0, EndpointItem const* endpoint = 0);

    /*!
     *  Destructor.
     */
    ~EndpointEditDialog();

    /*!
     *  Returns the contents of the name field.
     */
    QString getName() const;

    /*!
     *  Returns the value of the endpoint type field.
     */
    MCAPIEndpointDirection getType() const;

    /*!
     *  Returns the value of the connection type field.
     */
    MCAPIDataType getConnectionType() const;

public slots:
    void accept();

    //! Called when the contents of the name field have changed.
    void onContentChanged();

private:
    // Disable copying.
    EndpointEditDialog(EndpointEditDialog const& rhs);
    EndpointEditDialog& operator=(EndpointEditDialog const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Line edit for the endpoint's name.
    QLineEdit* nameEdit_;

    //! Combo box for the endpoint's type.
    QComboBox* typeCombo_;

    //! Combo box for the connection type.
    QComboBox* connCombo_;

    //! The OK button.
    QPushButton* btnOK_;
};

//-----------------------------------------------------------------------------

#endif // ENDPOINTEDITDIALOG_H
