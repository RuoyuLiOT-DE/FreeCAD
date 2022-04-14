#include "PreCompiled.h"

#include <Gui/BitmapFactory.h>

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
        : TaskBox(Gui::BitmapFactory().pixmap(picture.toLatin1()), text, true, parent), SelectionObserver(ViewProvider), ViewProvider(ViewProvider), ui(new Ui_TaskOTAttacher), use_current(true), isButParFraChecked(false)
    {
        // If the selected object is not attachable, throw warning
        if (!ViewProvider->getObject()->hasExtension(Part::AttachExtension::getExtensionClassTypeId()))
            throw Base::RuntimeError("Object is not attachable");

        // Seperate container widget to add all controls to
        proxy = new QWidget(this);
        ui->setupUi(proxy);
        QMetaObject::connectSlotsByName(this); // The collapsable ability of the task box seems come from here

        connect(ui->checkBoxUseCurrentTransform, SIGNAL(toggled(bool)),
                this, SLOT(onCheckBoxUseCurrentTransform(bool)));

        // Attachment activation mode
        connect(ui->checkBoxAttachmentActivate, SIGNAL(toggled(bool)),
                this, SLOT(onCheckBoxAttachmentActivate(bool)));

        // Connecting parent frame selection button and line edit
        connect(ui->buttonParentFrame, SIGNAL(clicked(bool)),
                this, SLOT(onButtonParentFrame(bool)));
        // Connecting offset parameters
        // connect(ui->attachmentOffsetX, SIGNAL(valueChanged(double)), this, SLOT(onAttachmentOffsetXChanged(double)));
        // connect(ui->attachmentOffsetY, SIGNAL(valueChanged(double)), this, SLOT(onAttachmentOffsetYChanged(double)));
        // connect(ui->attachmentOffsetZ, SIGNAL(valueChanged(double)), this, SLOT(onAttachmentOffsetZChanged(double)));
        // connect(ui->attachmentOffsetYaw, SIGNAL(valueChanged(double)), this, SLOT(onAttachmentOffsetYawChanged(double)));
        // connect(ui->attachmentOffsetPitch, SIGNAL(valueChanged(double)), this, SLOT(onAttachmentOffsetPitchChanged(double)));
        // connect(ui->attachmentOffsetRoll, SIGNAL(valueChanged(double)), this, SLOT(onAttachmentOffsetRollChanged(double)));

        // connect(ui->lineParentFrame, SIGNAL(textEdited(QString)),
        //         this, SLOT(onLineParentFrame(QString)));

        this->groupLayout()->addWidget(proxy);

        // Get the Attachment property
        Part::AttachExtension *pcAttach = ViewProvider->getObject()->getExtensionByType<Part::AttachExtension>();

        // Suppport of this Attachment is a PropertyLinkSubList type, refer to notes for image
        // getSubValues() returns the selected subelements of the Support shape, like a Face or Edge or Vertex
        std::vector<std::string> refnames = pcAttach->Support.getSubValues(); // Need to be converted to refstrings
        std::vector<QString> refstrings;                                      // Prepare for display in line editor

        // TODO: 
        makeRefStrings(refstrings, refnames);
        //----------Debug
        Base::Console().Message("RefString first, %s", refstrings.front().toStdString().c_str());
        // Temporarily prevent unnecessary feature recomputes

        ui->checkBoxAttachmentActivate->blockSignals(true);
        ui->checkBoxUseCurrentTransform->blockSignals(true);
        ui->buttonParentFrame->blockSignals(true);
        ui->lineParentFrame->blockSignals(true);
        ui->attachmentOffsetX->blockSignals(true);
        ui->attachmentOffsetY->blockSignals(true);
        ui->attachmentOffsetZ->blockSignals(true);
        ui->attachmentOffsetYaw->blockSignals(true);
        ui->attachmentOffsetPitch->blockSignals(true);
        ui->attachmentOffsetRoll->blockSignals(true);

        ui->checkBoxAttachmentActivate->setChecked(pcAttach->MapMode.getValue()); // mmDeactivated=0, mmObjectXY=2 FIXME: currently we only use these two modes
        ui->checkBoxUseCurrentTransform->setChecked(this->use_current);

        // Restore the blocked signals
        ui->checkBoxUseCurrentTransform->blockSignals(false);
        ui->checkBoxAttachmentActivate->blockSignals(false);
        ui->buttonParentFrame->blockSignals(false);
    }

    void TaskOTAttacher::onButtonParentFrame(const bool clicked)
    {
        if (clicked)
            Gui::Selection().clearSelection(); // FIXME: WHY???

        // update the button to indicate we are selecting
        // !!!!and also to enable selection being registered to the task
        if (!ViewProvider)
            return;

        QAbstractButton *button;
        button = ui->buttonParentFrame;

        Part::AttachExtension *pcAttach = ViewProvider->getObject()->getExtensionByType<Part::AttachExtension>();
        std::vector<App::DocumentObject *> refs = pcAttach->Support.getValues();

        if (this->isButParFraChecked)
        {
            button->setText(tr("ParentFrame"));
        }
        else
        {
            button->setText(tr("Selecting..."));
        }

        isButParFraChecked = 1 - int(isButParFraChecked);
    }

    void TaskOTAttacher::onCheckBoxUseCurrentTransform(bool toggled)
    {
        // Always check viewprovider first FIXME: BUT WHY?? TO DEAL WITH OBJECT BEING DELETED DURING TASK?
        if (!ViewProvider)
            return;
        this->use_current = ui->checkBoxUseCurrentTransform->checkState();
    }

    void TaskOTAttacher::onCheckBoxAttachmentActivate(bool toggled)
    {
        if (!ViewProvider)
            return;
        Part::AttachExtension *pcAttach = ViewProvider->getObject()->getExtensionByType<Part::AttachExtension>();
        auto mode = Attacher::eMapMode::mmDeactivated;
        if (ui->checkBoxAttachmentActivate->checkState())
            mode = Attacher::eMapMode::mmObjectXY;

        //----------Debug
        Base::Console().Message("Attaching mode: %d\n", mode);
        pcAttach->MapMode.setValue(mode);
        // TODO: updatepreview
    }

    void TaskOTAttacher::onSelectionChanged(const Gui::SelectionChanges &msg)
    {
        if (!ViewProvider)
            return;                                          // FIXME: User should be able to open task without a valid viewprovider?
        if (msg.Type == Gui::SelectionChanges::AddSelection) // Exclude other seleciton changes
        {
            // Note: The validity checking has already been done in ReferenceSelection.cpp
            // FIXME: HOW??????
            Part::AttachExtension *pcAttach = ViewProvider->getObject()->getExtensionByType<Part::AttachExtension>(); // FIXME: Data structure of Extension in DocumentObject???
            std::vector<App::DocumentObject *> refs = pcAttach->Support.getValues();
            std::vector<std::string> refnames = pcAttach->Support.getSubValues(); // FIXME: Learn more about PropertyLinkSubList

            // To get the document object under selection,
            App::DocumentObject *selObj = ViewProvider->getObject()->getDocument()->getObject(msg.pObjectName);

            if (!selObj || selObj == ViewProvider->getObject())
                return;                         // prevent self-referencing
            std::string subname = msg.pSubName; // Subname is like Face6 in Box::Face6
            // Base::Console().Message("Subname is: %s", subname.c_str());

            // FIXME: WHERE TO LIMIT THE AVAILABLE SELECTIONS GATE OR FILTER???

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
    } // end onSelectionChanged

    const QString makeRefString(const App::DocumentObject *obj, const std::string &sub)
    {
        if (obj == NULL)
            return QObject::tr("No reference selected");

        if (obj->getTypeId().isDerivedFrom(App::OriginFeature::getClassTypeId()) ||
            obj->getTypeId().isDerivedFrom(Part::Datum::getClassTypeId()))
            // App::Plane, Line or Datum feature
            return QString::fromLatin1(obj->getNameInDocument());

        if ((sub.size() > 4) && (sub.substr(0, 4) == "Face"))
        {
            int subId = std::atoi(&sub[4]);
            return QString::fromLatin1(obj->getNameInDocument()) + QString::fromLatin1(":") + QObject::tr("Face") + QString::number(subId);
        }
        else if ((sub.size() > 4) && (sub.substr(0, 4) == "Edge"))
        {
            int subId = std::atoi(&sub[4]);
            return QString::fromLatin1(obj->getNameInDocument()) + QString::fromLatin1(":") + QObject::tr("Edge") + QString::number(subId);
        }
        else if ((sub.size() > 6) && (sub.substr(0, 6) == "Vertex"))
        {
            int subId = std::atoi(&sub[6]);
            return QString::fromLatin1(obj->getNameInDocument()) + QString::fromLatin1(":") + QObject::tr("Vertex") + QString::number(subId);
        }
        else
        {
            // something else that face/edge/vertex. Can be empty string.
            return QString::fromLatin1(obj->getNameInDocument()) + (sub.length() > 0 ? QString::fromLatin1(":") : QString()) + QString::fromLatin1(sub.c_str());
        }
    }
    void TaskOTAttacher::makeRefStrings(std::vector<QString> &refstrings, std::vector<std::string> &refnames)
    {
        Part::AttachExtension *pcAttach = ViewProvider->getObject()->getExtensionByType<Part::AttachExtension>();
        std::vector<App::DocumentObject *> refs = pcAttach->Support.getValues();

        if (!refs.empty())
        {
            refstrings.push_back(makeRefString(refs[0], refnames[0]));
        }
        else
        {
            refstrings.push_back(QObject::tr("No reference selected"));
            refnames.push_back(""); // FIXME: WHY?????
        }
    } // end makeRefStrings

    TaskOTAttacher::~TaskOTAttacher() {}
}

#include "moc_TaskOTAttacher.cpp"