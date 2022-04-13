#include "PreCompiled.h"

#include <Mod/Part/App/AttachExtension.h>
#include <Gui/Document.h>
#include <Gui/ViewProviderDocumentObject.h>

#include <App/OriginFeature.h>
#include <Mod/Part/App/DatumFeature.h> //For dealing with subnames

#include "ui_TaskOTAttacher.h"
#include "TaskOTAttacher.h"

namespace RobotGui
{
    TaskOTAttacher::TaskOTAttacher(Gui::ViewProviderDocumentObject *ViewProvider, QWidget *parent,
                                   QString picture,
                                   QString text /*, VisibilityFunction func*/)
        : TaskBox(), SelectionObserver(ViewProvider), ViewProvider(ViewProvider), ui(new Ui_TaskOTAttacher)
    {
        proxy = new QWidget(this);
        ui->setupUi(proxy);
        this->groupLayout()->addWidget(proxy);
    }

    void TaskOTAttacher::onSelectionChanged(const Gui::SelectionChanges &msg)
    {
        if (!ViewProvider)
            return;                                          // TODO: User should be able to open task without a valid viewprovider?
        if (msg.Type == Gui::SelectionChanges::AddSelection) // Exclude other seleciton changes
        {
            // Note: The validity checking has already been done in ReferenceSelection.cpp
            // FIXME: HOW??????
            Part::AttachExtension *pcAttach = ViewProvider->getObject()->getExtensionByType<Part::AttachExtension>(); // TODO: Data structure of Extension in DocumentObject???
            std::vector<App::DocumentObject *> refs = pcAttach->Support.getValues();
            std::vector<std::string> refnames = pcAttach->Support.getSubValues(); // TODO: Learn more about PropertyLinkSubList

            // To get the document object under selection,
            App::DocumentObject *selObj = ViewProvider->getObject()->getDocument()->getObject(msg.pObjectName);

            if (!selObj || selObj == ViewProvider->getObject())
                return;                         // prevent self-referencing
            std::string subname = msg.pSubName; // Subname is like Face6 in Box::Face6
            // Base::Console().Message("Subname is: %s", subname.c_str());

            // TODO: WHERE TO LIMIT THE AVAILABLE SELECTIONS GATE OR FILTER???

            // FIXME: Just reuse the way they deal with the sub-name and multiple references,
            //  Remove subname for planes and datum features
            if (selObj->getTypeId().isDerivedFrom(App::OriginFeature::getClassTypeId()) ||
                selObj->getTypeId().isDerivedFrom(Part::Datum::getClassTypeId()))
                subname = "";

            // eliminate duplicate selections
            for (size_t r = 0; r < refs.size(); r++)
                if ((refs[r] == selObj) && (refnames[r] == subname))
                    return;
            if (refs.empty())
            {
                refs.push_back(selObj);
                refnames.push_back(subname);
            }
            else
            {
                refs.front() = selObj;
                refnames.front() = subname;
            }

            try // FIXME: How do we know where exception can appear and where not????
            {
                pcAttach->Support.setValues(refs, refnames);
                pcAttach->MapMode.setValue(Attacher::eMapMode::mmObjectXY);
            }
            catch (Base::Exception &e)
            {
                ui->message->setText(QString::fromLatin1(e.what()));
                ui->message->setStyleSheet(QString::fromLatin1("QLabel{color: red;}"));
            }
        }
    }

    TaskOTAttacher::~TaskOTAttacher() {}
}

#include "moc_TaskOTAttacher.cpp"