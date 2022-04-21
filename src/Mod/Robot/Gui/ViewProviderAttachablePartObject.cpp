#include "PreCompiled.h"

#include "ViewProviderAttachablePartObject.h"
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/Control.h>
#include "TaskDlgOTAttacher.h"
#include <QMessageBox>
#include <Gui/Command.h>

namespace RobotGui
{

    PROPERTY_SOURCE_WITH_EXTENSIONS(RobotGui::ViewProviderAttachablePartObject, Gui::ViewProviderGeometryObject)

    ViewProviderAttachablePartObject::ViewProviderAttachablePartObject()
        : Gui::ViewProviderGeometryObject::ViewProviderGeometryObject()
    {
        PartGui::ViewProviderAttachExtension::initExtension(this);
    }

    ViewProviderAttachablePartObject::~ViewProviderAttachablePartObject()
    {
        // FIXME: Extension need deconstructor?
        Gui::ViewProviderGeometryObject::~ViewProviderGeometryObject();
    }

    bool ViewProviderAttachablePartObject::setEdit(int ModNum)
    {
        if (!ViewProvider::setEdit(ModNum))
            return false;
        // FIXME: FreeCAD commnent
        // TODO Share this code with Features view providers somehow (2015-09-08, Fat-Zer)
        if (ModNum == ViewProvider::Default)
        {
            // FIXME: FreeCAD comment
            //  When double-clicking on the item for this datum feature the
            //  object unsets and sets its edit mode without closing
            //  the task panel
            Gui::TaskView::TaskDialog *dlg = Gui::Control().activeDialog();
            TaskDlgOTAttacher *attacherDlg = qobject_cast<TaskDlgOTAttacher *>(dlg);
            if (attacherDlg && attacherDlg->getViewProvider() != this)
                attacherDlg = 0; // another datum feature left open its task panel
            if (dlg && !attacherDlg)
            {
                QMessageBox msgBox;
                msgBox.setText(QObject::tr("A dialog is already open in the task panel"));
                msgBox.setInformativeText(QObject::tr("Do you want to close this dialog?"));
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::Yes);
                int ret = msgBox.exec();
                if (ret == QMessageBox::Yes)
                    Gui::Control().closeDialog();
                else
                    return false;
            }

            // clear the selection (convenience)
            Gui::Selection().clearSelection();

            oldWb = Gui::Command::assureWorkbench("PartDesignWorkbench");

            // start the edit dialog
            if (attacherDlg)
                Gui::Control().showDialog(attacherDlg);
            else
                Gui::Control().showDialog(new TaskDlgOTAttacher(this));

            return true;
        }
        else
        {
            return ViewProvider::setEdit(ModNum);
        }
    }

    void ViewProviderAttachablePartObject::unsetEdit(int ModNum)
    {
        // return to the WB we were in before editing the PartDesign feature
        Gui::Command::assureWorkbench(oldWb.c_str());

        if (ModNum == ViewProvider::Default)
        {
            // when pressing ESC make sure to close the dialog
            Gui::Control().closeDialog();
        }
        else
        {
            Gui::ViewProviderGeometryObject::unsetEdit(ModNum);
        }
    }

}