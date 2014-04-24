//-----------------------------------------------------------------------------
// File: QuartusPinImportDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.11.2013
//
// Description:
// Dialog for file selection and pin import.
//-----------------------------------------------------------------------------

#ifndef QUARTUSPINIMPORTDIALOG_H
#define QUARTUSPINIMPORTDIALOG_H

#include <QTextCharFormat>
#include <QDialog>
#include <QPushButton>
#include <QPlainTextEdit>

#include <IPXACTmodels/generaldeclarations.h>

class Component;
class FileSelector;
class LibraryInterface;
class Port;
//-----------------------------------------------------------------------------
// class QuartusPinImportDialog.
//-----------------------------------------------------------------------------
class QuartusPinImportDialog : public QDialog
{
	Q_OBJECT

public:
	/*!
	 *  The constructor.
	 *
	 *      @param [in] parent   The parent widget.
	 */
    QuartusPinImportDialog(QSharedPointer<Component> component, LibraryInterface* libraryHandler,
        QWidget *parent);

	/*!
	 *  The destructor.
	*/
	~QuartusPinImportDialog();

    /*!
     *  Gets the list of imported ports.
     *     
     *      @return List of imported ports.
     */
    QList<QSharedPointer<Port> > getPorts() const;
	
signals:

    void printMessage(QString const& message);

private slots:

    //! Called when save is clicked.
    virtual void onSaveClicked();

    //! Called when a new file is selected.
    virtual void onFileSelected();

private:
	// Disable copying.
	QuartusPinImportDialog(QuartusPinImportDialog const& rhs);

	// Disable assignment.
	QuartusPinImportDialog& operator=(QuartusPinImportDialog const& rhs);

    /*!
     *  Prints a notice to the output widget.
     *
     *      @param [in] text   The notice to print.
     */
    void printNotice(QString const& text);
    
    /*!
     *  Prints an error to the output widget.
     *
     *      @param [in] text   The error to print.
     */
    void printError(QString const& text);

    //! Statistics for port import.
    struct PortImportStatistics 
    {
        unsigned int inPorts;       //<! Number of in ports.
        unsigned int outPorts;      //<! Number of out ports.        
        unsigned int powerPorts;    //<! Number of power ports.
        unsigned int gndPorts;      //<! Number of ground ports.
        unsigned int otherPorts;    //<! Number of other ports.
        unsigned int portCount;     //<! Total number of ports.
        unsigned int modifiedPorts; //<! Number of changed ports.
        unsigned int newPorts;      //<! Number of new ports.
        unsigned int removedPorts;  //<! Number of removed ports in component.

        // The constructor.
        PortImportStatistics() : inPorts(0), outPorts(0), powerPorts(0), gndPorts(0), otherPorts(0), 
            portCount(0), modifiedPorts(0), newPorts(0), removedPorts(0) {}
    };

    /*!
     *  Reads a pin file stream parsing the ports for the component and writes statistics of the parsing.
     *
     *      @param [in] fileStream   The filestream to read.
     *      @param [out] statistics   The statistics to write.
     */
    void readFile(QTextStream &fileStream, PortImportStatistics& statistics);

    /*!
     *  Prints a summary of the import.
     *
     *      @param [in] statistics   The statistics of the import.
     */
    void printSummary( PortImportStatistics &statistics );

    /*!
     *  Checks, if importing modified a port.
     *
     *      @param [in] portName        The name of the port to check.
     *      @param [in] portDirection   The imported direction of the port.
     *      @param [in] usage           The imported pin usage of the port.
     *
     *      @return True, if port was modified in import, otherwise false.
     */
    bool isPortModified(QString const& portName, General::Direction portDirection, QString const& usage);
    
    /*!
     *  Counts the ports that were removed in the import.
     *
     *      @return The number of removed ports.
     */
    unsigned int removedPortCount();

    //! Sets the dialog layout.
    void setupLayout();

	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! The library handler.
    LibraryInterface* handler_;

    //! The component being edited.
    QSharedPointer<Component> component_;

    //! Used to select the import file file.
    FileSelector* fileSelector_;    

    //! Widget for displaying notifications and errors to the user.
    QPlainTextEdit* output_;

    //! Contains the imported ports.
    QList<QSharedPointer<Port> > ports_;

    //! Text formatting for notice messages.
    QTextCharFormat noticeFormat_;

    //! Text formatting for error messages.
    QTextCharFormat errorFormat_;
};
#endif // QUARTUSPINIMPORTDIALOG_H
