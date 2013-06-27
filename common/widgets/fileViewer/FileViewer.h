//-----------------------------------------------------------------------------
// File: FileViewer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 26.06.2013
//
// Description:
// Widget for viewing files in a component fileset.
//-----------------------------------------------------------------------------

#ifndef FILEVIEWER_H
#define FILEVIEWER_H

#include <QWidget>
#include <QSharedPointer>
#include <QTextEdit>
#include <QPushButton>
#include <QString>

class Component;
class LibraryInterface;
class FileSelector;

//-----------------------------------------------------------------------------
//! Viewer widget for browsing files in component's fileset.
//-----------------------------------------------------------------------------
class FileViewer : public QWidget
{
    Q_OBJECT

public:

    /*!
    *  Constructor.
    *
    *		@param [in, out] component Pointer to the component whose fileset is viewed.
    */
    FileViewer(QSharedPointer<Component> component, QString const& basePath_, QWidget* parent = 0);

    /*!
    *  Destructor.
    */
    ~FileViewer();

    void refresh();

private slots:

    void onFileSelected(const QString&);

private:
    // Disable copying.
	FileViewer(FileViewer const& rhs);
	FileViewer& operator=(FileViewer const& rhs);

    void setupLayout();

    QString basePath_;

    FileSelector* fileSelector_;
    
    QTextEdit* editor_;
};
#endif //FILEVIEWER_H