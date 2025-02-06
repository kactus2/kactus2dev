//-----------------------------------------------------------------------------
// File: FileOutputWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 15.11.2016
//
// Description:
// Widget for listing generated files and some details about them.
//-----------------------------------------------------------------------------

#ifndef FILEOUTPUTWIDGET_H
#define FILEOUTPUTWIDGET_H

#include <QLineEdit>
#include <QLabel>
#include <QTableWidget>

#include "OutputControl.h"

class FileOutputWidget : public QWidget
{
    Q_OBJECT
public:

    //! The constructor.
	FileOutputWidget(QSharedPointer<OutputControl> model);

	//! The destructor.
    virtual ~FileOutputWidget() = default;

    /*!
     * Checks if the output setting can be accepted.    
     *
     *     @return  True, if acceptable, otherwise false. 
     */
    bool canAccept() const;

signals:
	
    // Emitted when the selected file is changed.
    void selectedFileChanged(QSharedPointer<GenerationOutput> newSelection) const;

    // Emitted when the output path is changed.
    void outputPathChanged() const;

    /*!
     *  Informs of filename change.
     *
     *      @param [in] fileIndex   Index of the changed file.
     */
    void fileNameChanged(int const& fileIndex);

public slots:

    // Informs that the output files have changes. Will query new ones from the model and reconstruct the table.
    void onOutputFilesChanged();

private slots:

    // Informs that the user chosen new output path.
    // Will tell to model and call checkExistence().
    void onPathEdited();

    // Received when user click the browse button. Opens a dialog and updates the changes to the path editor.
    void onBrowse();

    // Received when user makes a change to the table.
    void onItemChanged(QTableWidgetItem *item);

    // Received when user selects an item on the table.
    void onItemSelectionChanged();

private:

    // Disable copying.
    FileOutputWidget(FileOutputWidget const& rhs);
    FileOutputWidget& operator=(FileOutputWidget const& rhs);

    // Goes through the fileTable_ and checks if there are files that already exists.
    // Will set appropriate warnings.
    void checkExistence();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	// The "model" for the widget.
    QSharedPointer<OutputControl> model_;

    //! Editor for the path of the generated file.
    QLineEdit* pathEditor_;

    //! Warning is display here, namely for an existing file being overwritten.
    QLabel* generalWarningLabel_;

    //! Table used to display all needed files.
    QTableWidget* fileTable_;
};

#endif //FILEOUTPUTWIDGET_H