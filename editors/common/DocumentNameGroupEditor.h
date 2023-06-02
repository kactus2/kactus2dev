//-----------------------------------------------------------------------------
// File: DocumentNameGroupEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 23.05.2023
//
// Description:
// Editor for document name group.
//-----------------------------------------------------------------------------

#ifndef DOCUMENTNAMEGROUPEDITOR_H
#define DOCUMENTNAMEGROUPEDITOR_H

#include <QGroupBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QLineEdit>

class BusDefinition;
class LibraryInterface;
class VLNVDisplayer;
class VLNV;
class Document;

//-----------------------------------------------------------------------------
//! Editor for document name group.
//-----------------------------------------------------------------------------
class DocumentNameGroupEditor : public QGroupBox
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] library     Interface to the library.
     *      @param [in] parent      The owner of this widget.
     */
    DocumentNameGroupEditor(QWidget *parent = nullptr);

    //! No copying. No assignment.
    DocumentNameGroupEditor(const DocumentNameGroupEditor& other) = delete;
    DocumentNameGroupEditor& operator=(const DocumentNameGroupEditor& other) = delete;

    /*!
     *  The destructor.
     */
    virtual ~DocumentNameGroupEditor() = default;
    
    /*!
     *  Sets the document name group information of a document.
     *
     *      @param [in] document        The document whose information is set.
     *      @param [in] documentPath    The path to the document.
     */
    void setDocumentNameGroup(QSharedPointer<Document> document, QString const& documentPath);

signals:
    
    void contentChanged();

private slots:

    void onDisplayNameChanged();

    void onShortDescriptionChanged();

    void onDescriptionChanged();

private:

    /*
     *  Set the VLNV information to their respective child widgets.
     */
    void setVlnv(VLNV const& vlnv);
    
    /*!
     *  Set the layout of the widget
     */
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! The document for this editor.
    QSharedPointer<Document> document_;

    //! The label to display the vendor.
    QLabel vendor_;

    //! The label to display the library.
    QLabel library_;

    //! The label to display the name.
    QLabel name_;

    //! The label to display the version.
    QLabel version_;

    //! The label to display the document path.
    QLabel path_;

    //! The label to display "Path:".
    QLabel pathTitleLabel_ = QLabel(QStringLiteral("Path:"));

    //! The label to display "Display name:".
    QLabel displayNameTitleLabel_ = QLabel(QStringLiteral("Display name:"));
    
    //! The label to display "Short description:".
    QLabel shortDescriptionTitleLabel_ = QLabel(QStringLiteral("Short description:"));

    //! The label to display IP-XACT standard version compatibility.
    QLabel compatibility_;

    //! Editor for document display name.
    QLineEdit displayNameEditor_;

    //! Editor for document short description.
    QLineEdit shortDescriptionEditor_;
    
    //! Editor for document description.
    QPlainTextEdit descriptionEditor_;
};

#endif // DOCUMENTNAMEGROUPEDITOR_H
