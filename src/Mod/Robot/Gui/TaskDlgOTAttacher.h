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

    protected:
        Gui::ViewProviderDocumentObject *ViewProvider;
        TaskOTAttacher *otAttacherTask;
    };
}