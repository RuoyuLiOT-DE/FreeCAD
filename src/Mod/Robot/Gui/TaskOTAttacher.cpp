#include "PreCompiled.h"

#include <Gui/BitmapFactory.h>

#include <Mod/Part/App/AttachExtension.h>
#include <Gui/Document.h>
#include <Gui/ViewProviderDocumentObject.h>

#include <App/OriginFeature.h>
#include <Mod/Part/App/DatumFeature.h> //For dealing with subnames

#include <Mod/Part/Gui/AttacherTexts.h>
#include <Mod/Part/App/Attacher.h> // For dealing with translatable mode text

#include <Gui/Application.h> 
#include "ui_TaskOTAttacher.h"
#include "TaskOTAttacher.h"

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
    } // end makeRefString
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
        connect(ui->lineParentFrame, SIGNAL(textEdited(QString)),
                this, SLOT(onLineParentFrame(QString)));

        // Connecting offset parameters
        connect(ui->attachmentOffsetX, SIGNAL(valueChanged(double)), this, SLOT(onAttachmentOffsetXChanged(double)));
        connect(ui->attachmentOffsetY, SIGNAL(valueChanged(double)), this, SLOT(onAttachmentOffsetYChanged(double)));
        connect(ui->attachmentOffsetZ, SIGNAL(valueChanged(double)), this, SLOT(onAttachmentOffsetZChanged(double)));
        connect(ui->attachmentOffsetYaw, SIGNAL(valueChanged(double)), this, SLOT(onAttachmentOffsetYawChanged(double)));
        connect(ui->attachmentOffsetPitch, SIGNAL(valueChanged(double)), this, SLOT(onAttachmentOffsetPitchChanged(double)));
        connect(ui->attachmentOffsetRoll, SIGNAL(valueChanged(double)), this, SLOT(onAttachmentOffsetRollChanged(double)));

        this->groupLayout()->addWidget(proxy);

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

        // Get the Attachment property
        Part::AttachExtension *pcAttach = ViewProvider->getObject()->getExtensionByType<Part::AttachExtension>();

        // Suppport of this Attachment is a PropertyLinkSubList type, refer to notes for image
        // getSubValues() returns the selected subelements of the Support shape, like a Face or Edge or Vertex
        std::vector<std::string> refnames = pcAttach->Support.getSubValues(); // Need to be converted to refstrings
        std::vector<QString> refstrings;                                      // Prepare for display in line editor

        // Generate names (in string) of references elements of the current targeting object
        // to display them in the line editor
        makeRefStrings(refstrings, refnames);
        //----------Debug
        Base::Console().Message("RefString first, %s\n", refstrings.front().toStdString().c_str());
        ui->lineParentFrame->setText(refstrings[0]);                                  // FIXME: CHECK result makeRefStrings??
        ui->lineParentFrame->setProperty("RefName", QByteArray(refnames[0].c_str())); // Use the subname to change the button display name
                                                                                      // FIXME: where is this RefName property defined?

        // By default, we use the current transformation as anchor offset
        // and deactivate the attachement
        ui->checkBoxAttachmentActivate->setChecked((bool)pcAttach->MapMode.getValue()); // mmDeactivated=0, mmObjectXY=2 FIXME: currently we only use these two modes
        ui->checkBoxUseCurrentTransform->setChecked(this->use_current);
        if (this->use_current)     // If use current transform as attachment offset
            useCurrentTransform();

        // FIXME: learn more about the freecad gui elements Gui::prefQuantitySpinBox and Gui::QuantitySpinBox
        // It can binds the property of an object with the Gui-element, so we can
        // already sync those properties with the user-input, without adding any control to that????

        ui->attachmentOffsetX->bind(App::ObjectIdentifier::parse(ViewProvider->getObject(), std::string("AttachmentOffset.Base.x")));
        ui->attachmentOffsetY->bind(App::ObjectIdentifier::parse(ViewProvider->getObject(), std::string("AttachmentOffset.Base.y")));
        ui->attachmentOffsetZ->bind(App::ObjectIdentifier::parse(ViewProvider->getObject(), std::string("AttachmentOffset.Base.z")));

        ui->attachmentOffsetYaw->bind(App::ObjectIdentifier::parse(ViewProvider->getObject(), std::string("AttachmentOffset.Rotation.Yaw")));
        ui->attachmentOffsetPitch->bind(App::ObjectIdentifier::parse(ViewProvider->getObject(), std::string("AttachmentOffset.Rotation.Pitch")));
        ui->attachmentOffsetRoll->bind(App::ObjectIdentifier::parse(ViewProvider->getObject(), std::string("AttachmentOffset.Rotation.Roll")));

        // Restore the blocked signals
        ui->checkBoxUseCurrentTransform->blockSignals(false);
        ui->checkBoxAttachmentActivate->blockSignals(false);
        ui->buttonParentFrame->blockSignals(false);
        ui->lineParentFrame->blockSignals(false);
        ui->attachmentOffsetX->blockSignals(false);
        ui->attachmentOffsetY->blockSignals(false);
        ui->attachmentOffsetZ->blockSignals(false);
        ui->attachmentOffsetYaw->blockSignals(false);
        ui->attachmentOffsetPitch->blockSignals(false);
        ui->attachmentOffsetRoll->blockSignals(false);
        // FIXME: Ignore the TaskAttacher::visibilityAutomation() for now, it is used in editing mode of document objects to show and hide models
        // FIXME: we don't use for now updateRefrencesUI()
        // FIXME: we don't use for now updateListOfModes()
        // FIXME: we don't use for now selectMapMode()

        updateAttachmentOffsetUI();
        updatePreview();
        // FIXME: learn the logic behind that connect object deletion with slot
        auto bnd1 = boost::bind(&TaskOTAttacher::objectDeleted, this, boost::placeholders::_1);
        auto bnd2 = boost::bind(&TaskOTAttacher::documentDeleted, this, boost::placeholders::_1);
        Gui::Document *document = Gui::Application::Instance->getDocument(ViewProvider->getObject()->getDocument());
        connectDelObject = document->signalDeletedObject.connect(bnd1);
        connectDelDocument = document->signalDeleteDocument.connect(bnd2);
    }

    void TaskOTAttacher::onButtonParentFrame(const bool clicked)
    {
        isButParFraChecked = (bool)(1 - (int)isButParFraChecked);
        if (clicked)
            Gui::Selection().clearSelection(); // FIXME: WHY???

        updateButtonParentFrame();

    } // end onButtonParentFrame

    void TaskOTAttacher::onLineParentFrame(const QString &text)
    {
        if (!ViewProvider)
            return;

        QLineEdit *line = ui->lineParentFrame;
        if (line == NULL)
            return;

        if (text.length() == 0) // In case user clear content inside line editor
        {
            // Reference was removed
            // Update the reference list
            Part::AttachExtension *pcAttach = ViewProvider->getObject()->getExtensionByType<Part::AttachExtension>();
            std::vector<App::DocumentObject *> refs = pcAttach->Support.getValues();
            std::vector<std::string> refnames = pcAttach->Support.getSubValues();

            std::vector<App::DocumentObject *> newrefs;
            std::vector<std::string> newrefnames;

            pcAttach->Support.setValues(newrefs, newrefnames); // Set empty to remove support
            pcAttach->MapMode.setValue(Attacher::eMapMode::mmDeactivated);
            pcAttach->AttachmentOffset.setValue(Base::Placement::Placement()); // Restore the transform
            
            updateAttachmentOffsetUI();
            updatePreview(); // get called after altering the attachment

            // Update the UI
            std::vector<QString> refstrings;
            makeRefStrings(refstrings, newrefnames);
            ui->lineParentFrame->setText(refstrings[0]);
            ui->lineParentFrame->setProperty("RefName", QByteArray(newrefnames[0].c_str()));
            return;
        }
        // Incase the line editor get editted
        QStringList parts = text.split(QChar::fromLatin1(':'));
        if (parts.length() < 2)
            parts.push_back(QString::fromLatin1(""));
        // Check whether this is the name of an App::Plane or Part::Datum feature
        App::DocumentObject *obj = ViewProvider->getObject()->getDocument()->getObject(parts[0].toLatin1());
        if (obj == NULL)
            return;

        std::string subElement;

        if (obj->getTypeId().isDerivedFrom(App::Plane::getClassTypeId()))
        {
            // everything is OK (we assume a Part can only have exactly 3 App::Plane objects located at the base of the feature tree)
            subElement = "";
        }
        else if (obj->getTypeId().isDerivedFrom(App::Line::getClassTypeId()))
        {
            // everything is OK (we assume a Part can only have exactly 3 App::Line objects located at the base of the feature tree)
            subElement = "";
        }
        else if (obj->getTypeId().isDerivedFrom(Part::Datum::getClassTypeId()))
        {
            subElement = "";
        }
        else // Incase this is a subelement of another shape
        {
            // TODO: check validity of the text that was entered: Does subElement actually reference to an element on the obj?

            // We must expect that "text" is the translation of "Face", "Edge" or "Vertex" followed by an ID.
            QRegExp rx;
            std::stringstream ss;

            rx.setPattern(QString::fromLatin1("^") + tr("Face") + QString::fromLatin1("(\\d+)$"));
            if (parts[1].indexOf(rx) >= 0)
            {
                int faceId = rx.cap(1).toInt();
                ss << "Face" << faceId;
            }
            else
            {
                rx.setPattern(QString::fromLatin1("^") + tr("Edge") + QString::fromLatin1("(\\d+)$"));
                if (parts[1].indexOf(rx) >= 0)
                {
                    int lineId = rx.cap(1).toInt();
                    ss << "Edge" << lineId;
                }
                else
                {
                    rx.setPattern(QString::fromLatin1("^") + tr("Vertex") + QString::fromLatin1("(\\d+)$"));
                    if (parts[1].indexOf(rx) >= 0)
                    {
                        int vertexId = rx.cap(1).toInt();
                        ss << "Vertex" << vertexId;
                    }
                    else
                    {
                        // none of Edge/Vertex/Face. May be empty string.
                        // Feed in whatever user supplied, even if invalid.
                        ss << parts[1].toLatin1().constData();
                    }
                }
            }

            // line->setProperty("RefName", QByteArray(ss.str().c_str()));
            subElement = ss.str();
        }

        Part::AttachExtension *pcAttach = ViewProvider->getObject()->getExtensionByType<Part::AttachExtension>();
        std::vector<App::DocumentObject *> refs = pcAttach->Support.getValues();
        std::vector<std::string> refnames = pcAttach->Support.getSubValues();

        refs.push_back(obj);
        refnames.push_back(subElement.c_str());
        pcAttach->Support.setValues(refs, refnames);
        pcAttach->MapMode.setValue(Attacher::eMapMode::mmDeactivated);
        updatePreview();
    }

    void TaskOTAttacher::onCheckBoxUseCurrentTransform(bool toggled)
    {
        // Always check viewprovider first FIXME: BUT WHY?? TO DEAL WITH OBJECT BEING DELETED DURING TASK?
        if (!ViewProvider)
            return;
        this->use_current = ui->checkBoxUseCurrentTransform->checkState();
        if (this->use_current)
            useCurrentTransform();
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
        updatePreview();
    }

    void TaskOTAttacher::onSelectionChanged(const Gui::SelectionChanges &msg)
    {
        if (!ViewProvider)
            return;                                          // FIXME: User should be able to open task without a valid viewprovider?
        if (msg.Type == Gui::SelectionChanges::AddSelection) // Exclude other seleciton changes
        {
            //----Debug
            Base::Console().Message("In selecting mode: %d\n", (int)isButParFraChecked);
            if (!isButParFraChecked) // If we are not in selecting mode, just do nothing for performance consideration
                return;
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
                if (this->use_current) // If use current transform option is enabled, calculate the relative transform and update the attachment offset UI
                    useCurrentTransform();
                // pcAttach->MapMode.setValue(Attacher::eMapMode::mmObjectXY);
                updatePreview(); //FIXME: Every time we change the actual attachment property, we should call this update method
            }
            catch (Base::Exception &e)
            {
                ui->message->setText(QString::fromLatin1(e.what()));
                ui->message->setStyleSheet(QString::fromLatin1("QLabel{color: red;}"));
            }

            QLineEdit *line = ui->lineParentFrame;
            if (line != NULL)
            {
                line->blockSignals(true);
                line->setText(makeRefString(selObj, subname));
                // line->setProperty("RefName", QByteArray(subname.c_str()));
                line->blockSignals(false);
            }

        } // endif messageType==msg.Type == Gui::SelectionChanges::AddSelection
    }     // end onSelectionChanged

    void TaskOTAttacher::updateButtonParentFrame()
    {
        // update the button to indicate we are selecting
        // !!!!and also to enable selection being registered to the task
        if (!ViewProvider)
            return;

        QAbstractButton *button;
        button = ui->buttonParentFrame;
        // Part::AttachExtension *pcAttach = ViewProvider->getObject()->getExtensionByType<Part::AttachExtension>();
        // std::vector<App::DocumentObject *> refs = pcAttach->Support.getValues();

        if (!isButParFraChecked)
        {
            button->setText(tr("ParentFrame"));
        }
        else
        {
            button->setText(tr("Selecting..."));
        }

    } // end updateButtonParentFrame()

    void TaskOTAttacher::useCurrentTransform()
    {
        // Make sure the parent frame is already selected
        Part::AttachExtension *pcAttach = ViewProvider->getObject()->getExtensionByType<Part::AttachExtension>();
        std::vector<App::DocumentObject *> refs = pcAttach->Support.getValues();
        if (!refs.empty())
        {
            App::PropertyPlacement *propla_parent = Base::freecad_dynamic_cast<App::PropertyPlacement>(refs.front()->getPropertyByName("Placement"));
            App::PropertyPlacement *propla_child = Base::freecad_dynamic_cast<App::PropertyPlacement>(
                ViewProvider->getObject()->getPropertyByName("Placement"));
            Base::Placement offset = propla_parent->getValue().inverse() * propla_child->getValue(); // Inverse returns a copy
            pcAttach->AttachmentOffset.setValue(offset);
            updateAttachmentOffsetUI(); //Get called after altering the attachment
        }
    }

    void TaskOTAttacher::updateAttachmentOffsetUI()
    {
        if (!ViewProvider)
            return;

        Part::AttachExtension *pcAttach = ViewProvider->getObject()->getExtensionByType<Part::AttachExtension>();
        Base::Placement pl = pcAttach->AttachmentOffset.getValue();
        Base::Vector3d pos = pl.getPosition();
        Base::Rotation rot = pl.getRotation();
        double yaw, pitch, roll;
        rot.getYawPitchRoll(yaw, pitch, roll);

        bool bBlock = true;
        ui->attachmentOffsetX->blockSignals(bBlock);
        ui->attachmentOffsetY->blockSignals(bBlock);
        ui->attachmentOffsetZ->blockSignals(bBlock);
        ui->attachmentOffsetYaw->blockSignals(bBlock);
        ui->attachmentOffsetPitch->blockSignals(bBlock);
        ui->attachmentOffsetRoll->blockSignals(bBlock);

        ui->attachmentOffsetX->setValue(Base::Quantity(pos.x, Base::Unit::Length));
        ui->attachmentOffsetY->setValue(Base::Quantity(pos.y, Base::Unit::Length));
        ui->attachmentOffsetZ->setValue(Base::Quantity(pos.z, Base::Unit::Length));
        ui->attachmentOffsetYaw->setValue(yaw);
        ui->attachmentOffsetPitch->setValue(pitch);
        ui->attachmentOffsetRoll->setValue(roll);

        auto expressions = ViewProvider->getObject()->ExpressionEngine.getExpressions();
        bool bRotationBound = false;
        bRotationBound = bRotationBound ||
                         expressions.find(App::ObjectIdentifier::parse(ViewProvider->getObject(), std::string("AttachmentOffset.Rotation.Angle"))) != expressions.end();
        bRotationBound = bRotationBound ||
                         expressions.find(App::ObjectIdentifier::parse(ViewProvider->getObject(), std::string("AttachmentOffset.Rotation.Axis.x"))) != expressions.end();
        bRotationBound = bRotationBound ||
                         expressions.find(App::ObjectIdentifier::parse(ViewProvider->getObject(), std::string("AttachmentOffset.Rotation.Axis.y"))) != expressions.end();
        bRotationBound = bRotationBound ||
                         expressions.find(App::ObjectIdentifier::parse(ViewProvider->getObject(), std::string("AttachmentOffset.Rotation.Axis.z"))) != expressions.end();

        ui->attachmentOffsetYaw->setEnabled(!bRotationBound);
        ui->attachmentOffsetPitch->setEnabled(!bRotationBound);
        ui->attachmentOffsetRoll->setEnabled(!bRotationBound);

        if (bRotationBound)
        {
            QString tooltip = tr("Not editable because rotation of AttachmentOffset is bound by expressions.");
            ui->attachmentOffsetYaw->setToolTip(tooltip);
            ui->attachmentOffsetPitch->setToolTip(tooltip);
            ui->attachmentOffsetRoll->setToolTip(tooltip);
        }

        bBlock = false;
        ui->attachmentOffsetX->blockSignals(bBlock);
        ui->attachmentOffsetY->blockSignals(bBlock);
        ui->attachmentOffsetZ->blockSignals(bBlock);
        ui->attachmentOffsetYaw->blockSignals(bBlock);
        ui->attachmentOffsetPitch->blockSignals(bBlock);
        ui->attachmentOffsetRoll->blockSignals(bBlock);
    } // End updateAttachmentOffsetUI()

    // FIXME: Think about where do we need to call this updatePreview to avoid unnecessary recompute
    bool TaskOTAttacher::updatePreview()
    {

        if (!ViewProvider)
            return false;

        Part::AttachExtension *pcAttach = ViewProvider->getObject()->getExtensionByType<Part::AttachExtension>();
        QString errMessage;
        bool attached = false;
        try
        {
            attached = pcAttach->positionBySupport(); // This step update the 3D view
        }
        catch (Base::Exception &err)
        {
            errMessage = QString::fromLatin1(err.what());
        }
        catch (Standard_Failure &err)
        {
            errMessage = tr("OCC error: %1").arg(QString::fromLatin1(err.GetMessageString()));
        }
        catch (...)
        {
            errMessage = tr("unknown error");
        }
        if (errMessage.length() > 0) // If got any error message, attached := false
        {
            ui->message->setText(tr("Attachment mode failed: %1").arg(errMessage));
            ui->message->setStyleSheet(QString::fromLatin1("QLabel{color: red;}"));
        }
        else // In case no errors attached could be true or false
        {
            if (!attached) // If the attachment is deactivated, attached := false
            {
                ui->message->setText(tr("Not attached"));
                ui->message->setStyleSheet(QString());
            }
            else // attachment will be true,
            {
                std::vector<QString> strs = AttacherGui::getUIStrings(pcAttach->attacher().getTypeId(), Attacher::eMapMode(pcAttach->MapMode.getValue()));
                ui->message->setText(tr("Attached with mode %1").arg(strs[0]));
                ui->message->setStyleSheet(QString::fromLatin1("QLabel{color: green;}"));
            }
        }

        // Update status message for attachment offset and activation state of offset group box
        QString splmLabelText = attached ? tr("Attachment Offset (in local coordinates):") : tr("Attachment Offset (inactive - not attached):");
        ui->groupBox_AttachmentOffset->setTitle(splmLabelText);
        ui->groupBox_AttachmentOffset->setEnabled(attached);

        return attached;
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

    void TaskOTAttacher::objectDeleted(const Gui::ViewProviderDocumentObject &view)
    {
        if (ViewProvider == &view)
        {
            ViewProvider = nullptr;
            this->setDisabled(true);
        }
    }

    void TaskOTAttacher::documentDeleted(const Gui::Document &)
    {
        ViewProvider = nullptr;
        this->setDisabled(true);
    }

    TaskOTAttacher::~TaskOTAttacher() {}
}

#include "moc_TaskOTAttacher.cpp"