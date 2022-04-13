#include "PreCompiled.h"


#include "TaskDlgOTAttacher.h"
#include <Gui/Document.h>
#include <Gui/ViewProviderDocumentObject.h>

namespace RobotGui{

    TaskDlgOTAttacher::TaskDlgOTAttacher(Gui::ViewProviderDocumentObject *ViewProvider, bool createBox)
    : TaskDialog(), ViewProvider(ViewProvider), otAttacherTask(nullptr)
    {
        assert(ViewProvider);
        setDocumentName(ViewProvider->getDocument()->getDocument()->getName()); // FIXME: Probably for inter-document tasks checking
        if (createBox)
        {
            otAttacherTask = new TaskOTAttacher(ViewProvider); // Local member variable
            Content.push_back(otAttacherTask); // Inherited varaible from TaskView::TaskDlg
        }
        
    }
    TaskDlgOTAttacher::~TaskDlgOTAttacher(){}
}

#include "moc_TaskDlgOTAttacher.cpp"