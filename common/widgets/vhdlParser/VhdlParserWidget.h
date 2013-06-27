//-----------------------------------------------------------------------------
// File: VhdlParserWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.06.2013
//
// Description:
// Widget for viewing and parsing a vhdl file.
//-----------------------------------------------------------------------------

#ifndef VHDLPARSERWIDGET_H
#define VHDLPARSERWIDGET_H

#include <common/widgets/vhdlParser/VhdlParser.h>

#include <QWidget>
#include <QSharedPointer>
#include <QString>
#include <QTextEdit>
#include <QSplitter>
#include <QPushButton>

class LibraryInterface;
class FileSelector;
class Component;
/*! \brief Used to parse VHDL files and generating IP-XACT packages of them.
 *
 */
class VhdlParserWidget : public QWidget {
	Q_OBJECT

public:

    /*!
     *  Constructor.
     *      
     *      @param [in] basePath Path to the component's xml file, used as base when operating on relative file paths.
     *		@param [in, out] component Pointer to the component being edited.
     *      @param [in] parent The parent widget.
     */
	VhdlParserWidget(const QString& basePath, QSharedPointer<Component> component,
	    QWidget *parent = 0);
	
	//! \brief The destructor.
	~VhdlParserWidget();

    /*!
     *  Initialize the combo box to select the top-level vhdl.
     */
	void initializeFileSelection();

signals:

    //! Emitted when a model parameter is selected.
    void addModelParameter(QSharedPointer<ModelParameter>);

    //! Emitted when a model parameter is deselected.
    void removeModelParameter(QSharedPointer<ModelParameter>);

    //! Emitted when a port is selected.
    void addPort(QSharedPointer<Port>);

    //! Emitted when a port is deselected.
    void removePort(QSharedPointer<Port>);

private slots:

    /*!
     *  Handler for changes in the file selector..
     *      
     *      @param [in] filePath The relative path of the selected file.
     */
	virtual void onFileSelected(const QString& filePath);

    /*!
     *  Opens a default editor for the selected vhdl file.
     */
    virtual void onOpenEditor();

    /*!
     *  Updates the model parameters and ports from the vhdl file.
     */
    virtual void onRefresh();

    /*!
     *  Called when outside editor changes a model parameter.
     */
    virtual void editorChangedModelParameter(QSharedPointer<ModelParameter> modelParam);

    /*!
     *  Called when outside editor removes a model parameter.
     */
    virtual void editorRemovedModelParameter(QSharedPointer<ModelParameter> modelParam);

    /*!
     *  Called when outside editor removes a port.
     */
    virtual void editorRemovedPort(QSharedPointer<Port> port);

private:
	
	//! \brief No copying
	VhdlParserWidget(const VhdlParserWidget& other);

	//! \brief No assignment
	VhdlParserWidget& operator=(const VhdlParserWidget& other);

    /*!
     *  Creates the layout for the page.
     */
    void setupLayout();

    //! \brief Base path for component xml file.
    QString basePath_;

	//! \brief Used to select the top-level vhdl file.
	FileSelector* fileSelector_;

    //! \brief Top-level vhdl file absolute path.
    QString vhdlPath_;

    //! \brief Button for opening an editor for vhdl file.
    QPushButton editButton_;

    //! \brief Button for refreshing ports and model parameters from file.
    QPushButton refreshButton_;

	//! \brief Displays and parses the vhdl code
	VhdlParser* vhdlParser_;

};

#endif // VHDLPARSERWIDGET_H
