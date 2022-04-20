#pragma once
#include <Gui/TaskView/TaskDialog.h>
#include "TaskOTAttacher.h"

namespace RobotGui
{

    class RobotGuiExport TaskDlgOTAttacher : public Gui::TaskView::TaskDialog
    {
        Q_OBJECT
    public:
        TaskDlgOTAttacher(Gui::ViewProviderDocumentObject *ViewProvider, bool createBox = true);
        ~TaskDlgOTAttacher();
        // FIXME:Why is this not a virtual function and still can be inherited by the parent class?
        Gui::ViewProviderDocumentObject *getViewProvider() const
        {
            return ViewProvider;
        }

        /// is called the TaskView when the dialog is opened
        virtual void open();
        /// is called by the framework if an button is clicked which has no accept or reject role
        virtual void clicked(int);
        /// is called by the framework if the dialog is accepted (Ok)
        virtual bool accept();
        /// is called by the framework if the dialog is rejected (Cancel)
        virtual bool reject();
        /// is called by the framework if the user presses the help button
        virtual bool isAllowedAlterDocument(void) const
        {
            return false;
        }

        /// returns for Close and Help button
        virtual QDialogButtonBox::StandardButtons getStandardButtons(void) const
        {
            return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
        }

    protected:
        Gui::ViewProviderDocumentObject *ViewProvider;
        TaskOTAttacher *otAttacherTask;
    };
}