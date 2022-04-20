#include "PreCompiled.h"

#include "TaskDlgOTAttacher.h"
#include <Gui/Document.h>
#include <Gui/ViewProviderDocumentObject.h>
#include <Gui/Application.h>
#include <Gui/DocumentObserver.h>
#include <Mod/Part/App/AttachExtension.h>
#include <Gui/CommandT.h>
#include <QMessageBox>

namespace RobotGui
{

    TaskDlgOTAttacher::TaskDlgOTAttacher(Gui::ViewProviderDocumentObject *ViewProvider, bool createBox)
        : TaskDialog(), ViewProvider(ViewProvider), otAttacherTask(nullptr)
    {
        assert(ViewProvider);
        setDocumentName(ViewProvider->getDocument()->getDocument()->getName()); // FIXME: Probably for inter-document tasks checking
        if (createBox)
        {
            otAttacherTask = new TaskOTAttacher(ViewProvider); // Local member variable
            Content.push_back(otAttacherTask);                 // Inherited varaible from TaskView::TaskDlg
        }
    }
    TaskDlgOTAttacher::~TaskDlgOTAttacher() {}

    void TaskDlgOTAttacher::open()
    {
        // Open a new transaction
        Gui::Document *document = Gui::Application::Instance->getDocument(ViewProvider->getObject()->getDocument());
        document->openCommand(QT_TRANSLATE_NOOP("Command", "Edit attachment"));
    } // end open()
    void TaskDlgOTAttacher::clicked(int)
    {

    } // end clicked()
    bool TaskDlgOTAttacher::accept()
    {
        try
        {
            // In case the document is not accessable
            Gui::DocumentT doc(getDocumentName());
            Gui::Document *document = doc.getDocument();
            if (!document || !ViewProvider)
                return true;

            Part::AttachExtension *pcAttach = ViewProvider->getObject()->getExtensionByType<Part::AttachExtension>();
            auto obj = ViewProvider->getObject();

            // FIXME: FreeCAD comment
            // DeepSOIC: changed this to heavily rely on dialog constantly updating feature properties
            // if (pcAttach->AttachmentOffset.isTouched()){
            Base::Placement plm = pcAttach->AttachmentOffset.getValue();
            double yaw, pitch, roll;
            plm.getRotation().getYawPitchRoll(yaw, pitch, roll);
            Gui::cmdAppObjectArgs(obj, "AttachmentOffset = App.Placement(App.Vector(%.10f, %.10f, %.10f),  App.Rotation(%.10f, %.10f, %.10f))",
                                  plm.getPosition().x, plm.getPosition().y, plm.getPosition().z, yaw, pitch, roll);
            //}

            Gui::cmdAppObjectArgs(obj, "MapReversed = %s", pcAttach->MapReversed.getValue() ? "True" : "False");

            Gui::cmdAppObjectArgs(obj, "Support = %s", pcAttach->Support.getPyReprString().c_str());

            Gui::cmdAppObjectArgs(obj, "MapPathParameter = %f", pcAttach->MapPathParameter.getValue());

            Gui::cmdAppObjectArgs(obj, "MapMode = '%s'", Attacher::AttachEngine::getModeName(Attacher::eMapMode(pcAttach->MapMode.getValue())).c_str());
            Gui::cmdAppObject(obj, "recompute()");

            Gui::cmdGuiDocument(obj, "resetEdit()"); // FIXME:Why this does not erase the change?because the recompute()?
            document->commitCommand();
        }
        catch (const Base::Exception &e)
        {
            QMessageBox::warning(otAttacherTask, tr("Datum dialog: Input error"), QString::fromLatin1(e.what()));
            return false;
        }

        return true;

    } // end accept()
    bool TaskDlgOTAttacher::reject()
    {
        Gui::DocumentT doc(getDocumentName());
        Gui::Document *document = doc.getDocument();
        if (document)
        {
            // roll back the done things
            document->abortCommand();
            Gui::Command::doCommand(Gui::Command::Gui, "%s.resetEdit()", doc.getGuiDocumentPython().c_str());
            Gui::Command::doCommand(Gui::Command::Doc, "%s.recompute()", doc.getAppDocumentPython().c_str());
        }

        return true;
    } // end reject()
}

#include "moc_TaskDlgOTAttacher.cpp"