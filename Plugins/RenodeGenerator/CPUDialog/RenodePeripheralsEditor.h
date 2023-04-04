//-----------------------------------------------------------------------------
// File: RenodePeripheralsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.02.2023
//
// Description:
// Editor for CPU peripheral details in renode generation.
//-----------------------------------------------------------------------------

#ifndef RENODEPERIPHERALSEDITOR_H
#define RENODEPERIPHERALSEDITOR_H

#include <QWidget>
#include <QObject>
#include <QTableView>
#include <QGroupBox>

#include <Plugins/RenodeGenerator/CPUDialog/RenodeStructs.h>

class RenodePeripheralsModel;
class RenodePeripheralsDelegate;

//-----------------------------------------------------------------------------
//! Editor for CPU peripheral details in renode generation.
//-----------------------------------------------------------------------------
class RenodePeripheralsEditor : public QWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  Pointer to the owner of this widget.
     */
    RenodePeripheralsEditor(QWidget *parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~RenodePeripheralsEditor() = default;

    /*!
     *  Setup the selected CPUs.
     *
     *      @param [in] library     Interface for accessing the library.
     *      @param [in] component   Top component of the selected design.
     *      @param [in] activeView  Active view of the design.
     */
    void setupPeripherals(QVector<QSharedPointer<RenodeStructs::cpuPeripherals> > peripherals);

    /*!
     *  Setup the folder path.
     *
     *      @param [in] newFolderPath   The new folder path.
     */
    void setupFolderPath(QString const& newFolderPath);

    //! No copying. No assignment.
    RenodePeripheralsEditor(const RenodePeripheralsEditor& other) = delete;
    RenodePeripheralsEditor& operator=(const RenodePeripheralsEditor& other) = delete;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The view for the table.
    QTableView* view_;

    //! The containing model.
    RenodePeripheralsModel* model_;

    //! The delegate.
    RenodePeripheralsDelegate* peripheralDelegate_;
};

#endif // RENODEPERIPHERALSEDITOR_H
