//-----------------------------------------------------------------------------
// File: LibrarySelectorWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 27.08.2013
//
// Description:
// Widget for viewing and selecting a library path.
//-----------------------------------------------------------------------------

#ifndef LIBRARYSELECTORWIDGET_H
#define LIBRARYSELECTORWIDGET_H

#include <QWidget>
#include <QSharedPointer>
#include <QPushButton>

class LibraryInterface;
class LibraryPathSelector;
class Component;
/*! \brief Used to parse VHDL files and generating IP-XACT packages of them.
 *
 */
class LibrarySelectorWidget : public QWidget {
	Q_OBJECT

public:

    /*!
     *  Constructor.
     *      
     *      @param [in] parent The parent widget.
     */
	LibrarySelectorWidget(QWidget *parent = 0);
	
	//! \brief The destructor.
	~LibrarySelectorWidget();

    QString getDirectory() const;

    void reset();

    bool isValid() const;

public slots:

    
    void onBrowse();


    void updateDirectory(QString const& path);

private:
	
	//! \brief No copying
	LibrarySelectorWidget(const LibrarySelectorWidget& other);

	//! \brief No assignment
	LibrarySelectorWidget& operator=(const LibrarySelectorWidget& other);

    /*!
     *  Creates the layout for the widget.
     */
    void setupLayout();

	//! \brief Used to select the library path.
	LibraryPathSelector* librarySelector_;

    QPushButton* browseButton_;

    bool directorySet_;

};

#endif // LIBRARYSELECTORWIDGET_H
