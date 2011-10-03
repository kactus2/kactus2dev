/* 
 *
 *  Created on: 9.10.2010
 *      Author: kamppia
 */

#ifndef FILERENAMEDIALOG_H_
#define FILERENAMEDIALOG_H_

#include <QMessageBox>
#include <QWidget>
#include <QPushButton>

class RenameDialog: public QMessageBox {

	Q_OBJECT

public:

	enum ClickedButton {
		YES,
		CANCEL,
		RENAME
	};

	RenameDialog(QWidget* parent);

	virtual ~RenameDialog();

	ClickedButton clickedButton();

public slots:

	void onYesClicked();

	void onCancelClicked();

	void onRenameClicked();

private:

	QPushButton* yesButton_;

	QPushButton* cancelButton_;

	QPushButton* renameButton_;

	ClickedButton selected_;

};

#endif /* FILERENAMEDIALOG_H_ */
