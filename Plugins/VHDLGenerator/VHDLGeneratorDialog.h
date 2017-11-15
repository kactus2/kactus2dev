//-----------------------------------------------------------------------------
// File: VHDLGeneratorDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.11.2017
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef VHDLGENERATORDIALOG_H
#define VHDLGENERATORDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>

#include <Plugins/PluginSystem/GeneratorPlugin/ViewSelectionWidget.h>

class VHDLGeneratorDialog: public QDialog
{
    Q_OBJECT
public:
    

	//! The constructor.
	VHDLGeneratorDialog(QSharedPointer<ViewSelection> viewConfiguration,
            QString const& defaultPath,
            QWidget* parent);

	//! The destructor.
	virtual ~VHDLGeneratorDialog();

    QString getOutputPath() const;
public slots:

    void accept();

private slots:

    void onBrowse();

private:

	// Disable copying.
	VHDLGeneratorDialog(VHDLGeneratorDialog const& rhs);
	VHDLGeneratorDialog& operator=(VHDLGeneratorDialog const& rhs);

    void setupLayout();

    ViewSelectionWidget* viewSelector_;

    QLineEdit* fileEditor_;

};

#endif // VHDLGENERATORDIALOG_H
