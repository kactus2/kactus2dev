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

#include <Plugins/RenodeGenerator/CPUDialog/RenodeStructs.h>

#include <QWidget>
#include <QObject>
#include <QTableView>

class RenodePeripheralsModel;
class RenodePeripheralsDelegate;
class PeripheralTemplateConfigurer;

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
     *    @param [in] templateConfigurer  Handles configuration of peripheral templates.
     *    @param [in] parent              Pointer to the owner of this widget.
     */
    RenodePeripheralsEditor(PeripheralTemplateConfigurer* templateConfigurer, QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~RenodePeripheralsEditor() = default;

    /*!
     *  Setup the selected peripherals.
     *
     *    @param [in] peripherals     The selected peripherals.
     */
    void setupPeripherals(QVector<QSharedPointer<RenodeStructs::cpuPeripheral> > peripherals);

    /*!
     *  Setup the folder path.
     *
     *    @param [in] newFolderPath   The new folder path.
     */
    void setupFolderPath(QString const& newFolderPath);

    //! No copying. No assignment.
    RenodePeripheralsEditor(const RenodePeripheralsEditor& other) = delete;
    RenodePeripheralsEditor& operator=(const RenodePeripheralsEditor& other) = delete;

private slots:

	/*!
	 *  Open the peripherals template dialog.
	 */
	void openTemplatesDialog();

private:

    /*!
     *  Setup the peripheral templates.
     *	
     *    @param [in] templates   List of new peripheral templates.
     */
    void setupPeripheralTemplates(QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > templates);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The view for the table.
    QTableView* view_;

    //! The containing model.
    RenodePeripheralsModel* model_;

    //! The delegate.
    RenodePeripheralsDelegate* peripheralDelegate_;

    //! Target folder for the Renode generator.
    QString generationFolder_;

    //! Configurer for peripheral templates.
    PeripheralTemplateConfigurer* templateConfigurer_;
};

#endif // RENODEPERIPHERALSEDITOR_H
