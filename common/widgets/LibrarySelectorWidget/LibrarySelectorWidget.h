//-----------------------------------------------------------------------------
// File: LibrarySelectorWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 27.08.2013
//
// Description:
// Widget for selecting a path under library locations
//-----------------------------------------------------------------------------

#ifndef LIBRARYSELECTORWIDGET_H
#define LIBRARYSELECTORWIDGET_H

#include <QWidget>
#include <QSharedPointer>
#include <QPushButton>

class LibraryPathSelector;

//-----------------------------------------------------------------------------
//! Widget for selecting a path under library locations
//-----------------------------------------------------------------------------
class LibrarySelectorWidget : public QWidget
{
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

    /*!
     *  Get the edited path.
     *      
     *      @return Path in the edit field.
     */
    QString getPath() const;

    /*!
     *  Updates active libraries in combobox and enables path to be constructed using vlnv.
     */
    void reset();

    /*!
     *  Checks if the path is valid.
     *      
     *      @return True, if the path is in an active library, otherwise false.
     */
    bool isValid() const;

signals:

    //! \brief Emitted when the library path changes.
    void contentChanged();

public slots:

    //! Called when browse button is clicked.
    virtual void onBrowse();

    //! Called when vlnv is used to set the path. Path is appended to the active library path.
    virtual void updatePath(QString const& path);

private:
	
	//! \brief No copying
	LibrarySelectorWidget(const LibrarySelectorWidget& other);

	//! \brief No assignment
	LibrarySelectorWidget& operator=(const LibrarySelectorWidget& other);

    //! Creates the layout for the widget.
    void setupLayout();

	//! \brief Used to select the library path.
	LibraryPathSelector* librarySelector_;

    //! The browse button.
    QPushButton* browseButton_;

    //! Flag for indicating if the path set using browse or if vlnv should be used to set the path.
    bool directorySet_;

};

#endif // LIBRARYSELECTORWIDGET_H
