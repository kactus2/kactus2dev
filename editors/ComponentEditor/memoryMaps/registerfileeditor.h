#ifndef REGISTERFILEEDITOR_H
#define REGISTERFILEEDITOR_H


#include <QSharedPointer>
#include <QGroupBox>

class RegisterFile;
class Component;
class LibraryInterface;
class ParameterFinder;
class ExpressionFormatter;
class RegisterFileValidator;
class RegisterDataTableView;
class RegisterFileModel;
class RegisterFileEditor: public QGroupBox
{
  	Q_OBJECT
public:
  RegisterFileEditor(QSharedPointer<RegisterFile> registerFile,
    QSharedPointer<Component> component,
    LibraryInterface* handler,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<RegisterFileValidator> registerFileValidator,
    QWidget* parent = 0);


    virtual ~RegisterFileEditor();

    virtual void refresh();


signals:
  /*!
   *  Informs that the contents of the editor have changed.
   */
  void contentChanged();

  /*!
   *  Informs of a need to redraw the visualizer.
   */
  void graphicsChanged();

  /*!
   *  Sends an error message forward.
   *
   *      @param [in] msg     The error message.
   */
  void errorMessage(const QString& msg) const;

  /*!
   *  Sends a notification message forward.
   *
   *      @param [in] msg     The notification message.
   */
  void noticeMessage(const QString& msg) const;

  /*!
   *  Increase the amount of references made to the given parameter.
   *
   *      @param [in] id  The id of the given parameter.
   */
  void increaseReferences(QString id) const;

  /*!
   *  Decrease the amount of references made to the given parameter.
   *
   *      @param [in] id  The id the given parameter.
   */
  void decreaseReferences(QString id) const;

  /*!
   *  Informs that a new item has been created.
   *
   *      @param [in] index   The index of the new item.
   */
  void childAdded(int index);

  /*!
   *  Informs that an item has been removed.
   *
   *      @param [in] index   The index of the removed item.
   */
  void childRemoved(int index);

private:
  //! No copying.
	RegisterFileEditor(const RegisterFileEditor& other);

	//! No assignment.
	RegisterFileEditor& operator=(const RegisterFileEditor& other);

    //! The view to display the items.
    RegisterDataTableView* view_;

	//! Pointer to the model that manages the details of address block.
	RegisterFileModel* model_;
};

#endif // REGISTERFILEEDITOR_H
