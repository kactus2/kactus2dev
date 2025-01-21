//-----------------------------------------------------------------------------
// File: RenodeSettingsWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.06.2023
//
// Description:
// Widget for renode generator settings.
//-----------------------------------------------------------------------------

#ifndef RENODESETTINGSWIDGET_H
#define RENODESETTINGSWIDGET_H

#include <QWidget>
#include <QLineEdit>

class RenodeSettingsModel;

//-----------------------------------------------------------------------------
//! Widget for renode generator settings.
//-----------------------------------------------------------------------------
class RenodeSettingsWidget : public QWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] settingsModel   Model for renode peripheral template settings.
     *    @param [in] parent          Pointer to the owner of this widget.
     */
    RenodeSettingsWidget(RenodeSettingsModel* settingsModel, QWidget *parent = 0);

    /*!
     *  The destructor.
     */
    ~RenodeSettingsWidget() final = default;

    //! No copying. No assignment.
    RenodeSettingsWidget(const RenodeSettingsWidget& other) = delete;
    RenodeSettingsWidget& operator=(const RenodeSettingsWidget& other) = delete;

private slots:

    /*!
     *  Open file browser for peripheral template configuration.
     */
    void onBrowseTemplatePath();

    /*!
     *  Handle the change in the peripheral template configuration path.
     *	
     *    @param [in] newPath     New path for peripheral template configuration file.
     */
    void onPathChanged(QString const& newPath);

private:

    /*!
     *  Setup the layout.
     */
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Model for renode peripheral template settings.
    RenodeSettingsModel* settingsModel_;

    //! Editor for peripheral template configuration file path.
    QLineEdit* templatePathEditor_;
};

#endif // RENODESETTINGSWIDGET_H
