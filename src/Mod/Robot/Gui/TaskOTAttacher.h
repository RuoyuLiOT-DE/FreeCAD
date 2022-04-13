#pragma once
#include <Gui/TaskView/TaskView.h>
#include <Gui/Selection.h>

namespace RobotGui
{
    class Ui_TaskOTAttacher;

    class RobotGuiExport TaskOTAttacher : public Gui::TaskView::TaskBox, public Gui::SelectionObserver
    {
        Q_OBJECT

    public:
        TaskOTAttacher(Gui::ViewProviderDocumentObject *ViewProvider, QWidget *parent = 0,
                       QString picture = QString(),
                       QString text = QString::fromLatin1("Attachment") /*, VisibilityFunction func = 0*/);
        ~TaskOTAttacher();

    private:
        ///@brief Must override function inherited from SelectionObserver, this observer bedefault will be attached to a subject at construction
        ///       FIXME:Attention!!!! now with any selection change, this function is called 3 times
        ///@param msg 
        ///
        void onSelectionChanged(const Gui::SelectionChanges &msg) override;

        /*member variable*/
    protected:
        Gui::ViewProviderDocumentObject *ViewProvider; // FIXME: For inheritance?, get from TaskDlg
        std::string ObjectName;                        // FIXME: For inheritance?

    private:
        QWidget *proxy; // Container for setup
        std::unique_ptr<Ui_TaskOTAttacher> ui; 
    };
}