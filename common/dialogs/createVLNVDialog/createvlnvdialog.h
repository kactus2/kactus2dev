/* 
 *
 *  Created on: 9.10.2010
 *      Author: kamppia
 */

#ifndef CREATEVLNVDIALOG_H_
#define CREATEVLNVDIALOG_H_

#include <LibraryManager/vlnv.h>

#include <QDialog>
#include <QWidget>
#include <QLineEdit>
#include <QString>
#include <QRadioButton>
#include <QGridLayout>

class CreateVLNVDialog: public QDialog {

	Q_OBJECT

public:

	CreateVLNVDialog(QWidget* parent, const VLNV* const vlnv = 0);

	virtual ~CreateVLNVDialog();

        VLNV* exec();

public slots:

	virtual void accept();

	virtual void reject();

	void onAbstractorSelected();

	void onAbstractionDefSelected();

	void onBusDefSelected();

	void onComponentSelected();

	void onDesignSelected();

	void onDesignConfSelected();

	void onGeneratorChainSelected();

private:

	VLNV* createVLNV();

	void setVendor(const VLNV* const vlnv);

	void setLibrary(const VLNV* const vlnv);

	void setName(const VLNV* const vlnv);

	void setVersion(const VLNV* const vlnv);

	void setTypeBox(const VLNV* const vlnv);

	VLNV::IPXactType type_;

	QLineEdit* vendorEdit_;

	QLineEdit* libraryEdit_;

	QLineEdit* nameEdit_;

	QLineEdit* versionEdit_;

	QGridLayout* layout_;
};

#endif /* CREATEVLNVDIALOG_H_ */
