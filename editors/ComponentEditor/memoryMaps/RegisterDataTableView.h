#ifndef REGISTERDATATABLEVIEW_H
#define REGISTERDATATABLEVIEW_H
#include "common/views/EditableTableView/editabletableview.h"

class QSortFilterProxyModel;
class RegisterDataTableView : public EditableTableView
{
  Q_OBJECT
public:
    RegisterDataTableView(QWidget *parent);


signals:
    void addRegister(const QModelIndex& index);
    void addRegisterFile(const QModelIndex& index);

protected:
  QAction addRFAction_;
  virtual void contextMenuEvent(QContextMenuEvent* event);

protected slots:
  void onAddRegisterAction();
  void onAddRegisterFileAction();

private:
  //! No copying
  RegisterDataTableView(const RegisterDataTableView& other);
  RegisterDataTableView& operator=(const RegisterDataTableView& other);


  QModelIndex getPosToAdd(QModelIndexList &indexes, QSortFilterProxyModel* sortProxy);
  void finalizeAddition(QModelIndex posToAdd,QSortFilterProxyModel* sortProxy);
};

#endif // REGISTERDATATABLEVIEW_H
