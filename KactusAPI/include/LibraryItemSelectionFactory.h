#ifndef LIBRARYITEMSELECTIONFACTORY_H
#define LIBRARYITEMSELECTIONFACTORY_H

#include "LibraryInterface.h"

#include <KactusAPI/include/ObjectSelectionDialog.h>

#include <IPXACTmodels/common/VLNV.h>

#include <QList>

namespace LibraryItemSelectionFactory
{
    /*!
     *  Construct the items for the selection dialog.
     *
     *      @param [in] dialog    The selection dialog.
     *      @param [in] vlvns     List of selectable VLNVs.
     */
   void constructItemsForSelectionDialog(LibraryInterface* library, ObjectSelectionDialog* dialog,
                                         QList<VLNV> const& vlvns);

   namespace Details
   {
        void addComponentFilesAndDesignItems(LibraryInterface *library, ObjectSelectionDialog* dialog,
            VLNV const& componentVLNV);

        void addDesignItemForConfiguration(LibraryInterface* library, ObjectSelectionDialog* dialog,
            VLNV const& configurationVLNV);

        void addAbstractionDefinitionItems(LibraryInterface* library, ObjectSelectionDialog* dialog,
            VLNV const& busDefinitionVLNV);

        void addBusDefintionItemIfOnlyAbstraction(LibraryInterface* library, ObjectSelectionDialog* dialog,
            VLNV const& abstractionVLNV);
   }
}

#endif // LIBRARYITEMSELECTIONFACTORY_H
