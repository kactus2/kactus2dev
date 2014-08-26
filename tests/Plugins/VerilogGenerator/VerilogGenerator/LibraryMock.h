//-----------------------------------------------------------------------------
// File: LibraryMock.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 06.08.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef LIBRARYMOCK_H
#define LIBRARYMOCK_H

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/design.h>
#include <IPXACTmodels/librarycomponent.h>
#include <IPXACTmodels/vlnv.h>

#include <QObject>

class LibraryMock : public QObject, public LibraryInterface
{
    Q_OBJECT
public:

	//! The constructor.
	LibraryMock(QObject* parent);

	//! The destructor.
	~LibraryMock();

    /*!
     *  Method description.
     *
     *      @param [in]    Description.
     *
     *      @return Description.
     */
    void addComponent(QSharedPointer<LibraryComponent> component);

    /*!
     *  Method description.
     *
     *      @param [in]    Description.
     *
     *      @return Description.
     */
    void clear();

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual QSharedPointer<LibraryComponent> getModel(const VLNV& vlnv);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual QSharedPointer<LibraryComponent const> getModelReadOnly(const VLNV& vlnv);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual bool contains(const VLNV& vlnv);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual const QString getPath(const VLNV& vlnv) const;

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual QString getDirectoryPath(const VLNV& vlnv) const;

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual bool writeModelToFile(const QString path, QSharedPointer<LibraryComponent> model, bool printErrors = true);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual bool writeModelToFile(QSharedPointer<LibraryComponent> model, bool printErrors = true);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual void searchForIPXactFiles();

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual void getNeededVLNVs(const VLNV& vlnv, QList<VLNV>& list);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual void getDependencyFiles(const VLNV& vlnv, QStringList& list);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual void getHierarchicalDependencyFiles(const VLNV& vlnv, QStringList& list);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual LibraryItem const* getTreeRoot() const;

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual VLNV::IPXactType getDocumentType(const VLNV& vlnv);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual int referenceCount(const VLNV& vlnv) const;

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual int getOwners(QList<VLNV>& list, const VLNV& vlnvToSearch) const;

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual int getChildren(QList<VLNV>& list, const VLNV& vlnvToSearch) const;

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual VLNV getDesignVLNV(const VLNV& hierarchyRef);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual QSharedPointer<Design> getDesign(const VLNV& hierarchyRef);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual bool isValid(const VLNV& vlnv);

public slots:

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual void onCheckLibraryIntegrity();

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual void onSelectionChanged(const VLNV& vlnv);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual void onClearSelection();

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual void onEditItem(const VLNV& vlnv);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual void onOpenDesign(const VLNV& vlnv);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual void onCreateNewItem(const VLNV& vlnv);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual void onCreateDesign(const VLNV& vlnv);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual void removeObject(const VLNV& vlnv);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual void removeObjects(const QList<VLNV>& vlnvList);

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual void beginSave();

    /*!
    *  Method description.
    *
    *      @param [in]    Description.
    *
    *      @return Description.
    */
    virtual void endSave();

private:

    // Disable copying.
    LibraryMock(LibraryMock const& rhs);
    LibraryMock& operator=(LibraryMock const& rhs);

    QMap<VLNV, QSharedPointer<LibraryComponent> > components_;

    QMap<VLNV, QString > paths_;

};

#endif // LIBRARYMOCK_H
