//-----------------------------------------------------------------------------
// File: MakeParametersDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 15.10.2015
//
// Description:
// Dialog for presenting parameters, which may be chosen for makefile generation.
//-----------------------------------------------------------------------------

#ifndef MAKEPARAMETERSDIALOG_H
#define MAKEPARAMETERSDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QTreeWidget>

#include "MakefileParser.h"
#include <Plugins/PluginSystem/GeneratorPlugin/FileOutputWidget.h>

class MakeConfiguration;

//-----------------------------------------------------------------------------
//! MakeParametersDialog class.
//-----------------------------------------------------------------------------
class MakeParametersDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     */
    MakeParametersDialog(QSharedPointer<MakeConfiguration> configuration,
        QSharedPointer<QList<QSharedPointer<MakeFileData> > > parsedData, QWidget* parent);

    /*!
     *  Destructor.
     */
    ~MakeParametersDialog();

public slots:

    virtual void accept();

private slots:

    void onItemChanged(QTreeWidgetItem *item, int column);

private:
    // Disable copying.
    MakeParametersDialog(MakeParametersDialog const& rhs);
    MakeParametersDialog& operator=(MakeParametersDialog const& rhs);

    QTreeWidget* createConflictTree(QSharedPointer<QList<QSharedPointer<MakeFileData> > > parsedData);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The configuration for the generation.
    QSharedPointer<MakeConfiguration> configuration_;

    //! Widget for output file information.
    FileOutputWidget* fileOutput_;

    //! General warnings are displayed here.
    QLabel* generalWarningLabel_;

    //! Make objects mapped to table items.
    QMap<QTreeWidgetItem*, QSharedPointer<MakeObjectData> > objectMapping_;
};

//-----------------------------------------------------------------------------

#endif // MAKEPARAMETERSDIALOG_H
