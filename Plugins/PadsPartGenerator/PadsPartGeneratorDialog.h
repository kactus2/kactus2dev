//-----------------------------------------------------------------------------
// File: PadsPartGeneratorDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 5.11.2013
//
// Description:
// Dialog for PADS part generation settings.
//-----------------------------------------------------------------------------

#ifndef PADSPARTGENERATORDIALOG_H
#define PADSPARTGENERATORDIALOG_H

#include <models/component.h>

#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRadioButton>

class LibraryInterface;
class PadsPartGenerator;
//-----------------------------------------------------------------------------
// class PadsPartGeneratorDialog.
//-----------------------------------------------------------------------------
class PadsPartGeneratorDialog : public QDialog
{	
    Q_OBJECT

public:

    //! Selection type for gate generation.
    enum gateGenerationType
    {
        MULTIPLEGATES = 0,  //!< Generate one gate per interface.
        SINGLEGATE          //!< Generate exactly one gate.
    };

	/*!
	 *  The constructor.
	 *
	 *      @param [in] libIf               The library interface.
	 *      @param [in] component           The component for which generation is done.
	 *      @param [in] generatorName       The utilizing generator name.
     *      @param [in] generatorVersion    The utilizing generator version.
	 *      @param [in] parent              The parent widget.
	 *
	 *      @return 
	 */
	PadsPartGeneratorDialog(LibraryInterface* libIf, QSharedPointer<const Component> component, 
        const QString& generatorName, const QString& generatorVersion, QWidget* parent);

	/*!
    *  The destructor.
    */
    ~PadsPartGeneratorDialog();

    /*!
     *  Returns the name for the generated part.
     *
     *      @return The name of the part.
     */
    QString getPartName() const;

    /*!
     *  Returns the name of the target fileset.
     *
     *      @return Name of the selected fileset.
     */
    QString getFileSetName() const;

    /*!
     *  Returns the text in the preview.     
     *
     *      @return the preview text.
     */
    QString getPreviewText();

    /*!
     *  Returns the selected gate generation type (single/multiple).
     *     
     *      @return The selected type.
     */
    gateGenerationType getGategenerationType() const;

public slots:

    /*!
     *  Generates the two-line header for PADS part file.
     */
    virtual void generateHeader();

    /*!
     *  Generates the gates in PADS part file.
     */
    virtual void generateGates();

    /*!
     *  Handler for events when user clicks run.
     */
    virtual void onRunClicked();

    /*!
     *  Handler for events when user edits the part name.
     */
    virtual void onNameChanged();

    /*!
     *  Handler for events when user changes the measurement unit.
     */
    virtual void onUnitChanged();

    /*!
     *  Handler for events when user changes the logical family.
     */
    virtual void onFamilyChanged();

    /*!
     *  Handler for events when user selects single/multiple gates.
     */
    virtual void onGateSelectionChanged();

    /*!
     *  Handler for events when user edits the gate name.
     */
    virtual void onGateNameChanged();

protected:
	
private slots:

        /*!
         *  Re-generates the part preview.
         */
        virtual void refreshPreview();

private:
	// Disable copying.
	PadsPartGeneratorDialog(PadsPartGeneratorDialog const& rhs);

	// Disable assignment.
	PadsPartGeneratorDialog& operator=(PadsPartGeneratorDialog const& rhs);

    /*!
     *  Sets the dialog layout.
     */
    void setupLayout();

    /*!
     *  Adds pin declarations for interface ports.
     *
     *      @param [in] busInterface    The interface whose ports to add.
     *      @param [in] cursor          Cursor to the correct position in preview.
     */
    void insertPins(QSharedPointer<BusInterface> busInterface, QTextCursor &cursor); 

    /*!
     *  Adds a gate declaration.
     *
     *      @param [in] name        The name of the gate.
     *      @param [in] decals      Number of decals for the gate.
     *      @param [in] pins        Number of pins in the gate.
     *      @param [in] gateswap    Gate swap type.
     *      @param [in] cursor      Cursor to the correct position in preview.
     */
    void insertGate(QString const& name, int decals, int pins, int gateswap, QTextCursor &cursor); 

    /*!
     *  Inserts a line of text to the preview.
     *
     *      @param [in] line            The line to insert.
     *      @param [in] cursor          Cursor to the correct position in preview.
     *      @param [in] validatingExp   RegExp for checking the line correctness.
     */
    void insertLine(QString const& line,  QTextCursor cursor, QRegExp const validatingExp = QRegExp() );

    /*!
     *  Inserts part attributes to the preview.
     *
     *      @param [in] cursor   Cursor to the correct position in preview.
     */
    void insertAttributes( QTextCursor& cursor );

	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* libHandler_;

    //! The component for which the generator is run.
    QSharedPointer<const Component> component_;

    //! The generator name.
    QString generatorName_;

    //! The generator version.
    QString generatorVersion_;

    //! Selector for fileset.
    QComboBox* fileSetSelector_;

    //! Editor for part name.
    QLineEdit* nameEditor_;

    //! Selector for pcb decal.
    QComboBox* decalSelector_;

    //! Selector for measurement unit.
    QComboBox* unitSelector_;

    //! Editor for logical family of the part.
    QLineEdit* familyEditor_;

    //! Button for selecting multiple gate generation.
    QRadioButton* multiGateButton_; 

    //! Button for selecting single gate generation.
    QRadioButton* singleGateButton_;

    //! Editor for gate name when single gate generation is selected.
    QLineEdit* gateNameEditor_;

    //! Editor for displaying preview of the generated part.
    QPlainTextEdit* preview_;

    //! Selected gate generation type.
    gateGenerationType gateGenerationType_;    

    //! Map of gate names and their line numbers in preview.
    QMap<int, QString> gates_;

    //! Formatting for valid lines.
    QTextCharFormat okFormat_;

    //! Formatting for invalid lines.
    QTextCharFormat errorFormat_;
};
#endif // PADSPARTGENERATORDIALOG_H

