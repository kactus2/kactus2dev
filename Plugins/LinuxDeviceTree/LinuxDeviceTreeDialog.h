//-----------------------------------------------------------------------------
// File: LinuxDeviceTreeDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.05.2018
//
// Description:
// Dialog for setting linux device tree generation options.
//-----------------------------------------------------------------------------

#ifndef LINUXDEVICETREEDIALOG_H
#define LINUXDEVICETREEDIALOG_H

class FileSet;
class View;
class VLNV;
class Component;
class Design;

#include <QDialog>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Dialog for setting linux device tree generation options.
//-----------------------------------------------------------------------------
class LinuxDeviceTreeDialog: public QDialog
{
    Q_OBJECT
public:
    
    /*!
     *	The constructor.
     *
     *		@param [in]	defaultPath     The default path for the Linux Device Tree file.
     *		@param [in]	component       The top component of the selected design.
     *		@param [in]	design          The selected design.
     *		@param [in]	parent          Parent item for the dialog.
     */
    LinuxDeviceTreeDialog(QString const& defaultPath, QSharedPointer<Component> component,
        QSharedPointer<Design> design, QWidget* parent);

	/*!
     *  The destructor.
     */
    virtual ~LinuxDeviceTreeDialog();

    /*!
     *	Get the output path for the device tree file.
     *
     *		@return	The output path of the device tree file.
     */
    QString getOutputPath() const;

    /*!
     *  Get the top component view from which the device tree file is constructed from.
     *
     *      @return	The name of the selected view.
     */
    QString getSelectedView() const;

    /*!
     *  Check if the file should be saved to a file set.
     *
     *      @return	True, if the file should be saved to a file set, false otherwise.
     */
    bool saveFileToFileSet() const;

    /*!
     *  Get the name of the file set where to save the file to.
     *
     *      @return	The name of the selected file set.
     */
    QString getTargetFileSet() const;

public slots:

    /*!
     *	Handles the accepting of the dialog.
     */
    void accept();

private slots:

    /*!
     *	Handles the target file browsing.
     */
    void onBrowse();

private:

	// Disable copying. Disable assignment.
    LinuxDeviceTreeDialog(LinuxDeviceTreeDialog const& rhs);
    LinuxDeviceTreeDialog& operator=(LinuxDeviceTreeDialog const& rhs);

    /*!
     *	Setup the view selector for the selected component. Set the current view according to the selected design.
     *
     *		@param [in]	component   The selected component.
     *		@param [in]	design      The selected design.
     */
    void setupViewSelector(QSharedPointer<Component> component, QSharedPointer<Design> design);

    /*!
     *	Setup the file set selector for the selected component.
     *
     *		@param [in]	componentFileSets   File sets of the selected component.
     */
    void setupFileSetSelector(QSharedPointer<QList<QSharedPointer<FileSet> > > componentFileSets);

    /*!
     *	Setup the dialog layout.
     */
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Selects the target view.
    QComboBox* viewSelector_;

    //! Groups the desired items for file set selection.
    QGroupBox* fileSetGroup_;

    //! Selects the target file set.
    QComboBox* fileSetSelector_;

    //! Editor for selecting the path for the device tree file.
    QLineEdit* fileEditor_;
};

#endif // LINUXDEVICETREEDIALOG_H
