
#include "PreCompiled.h"

// #include <App/DocumentObjectGroup.h>
// #include <App/GeoFeatureGroupExtension.h>
// #include <Gui/Control.h>
// #include <Gui/Command.h>
// #include <Gui/MDIView.h>
// #include <Gui/View3DInventor.h>
// #include <Gui/View3DInventorViewer.h>
// #include <Gui/BitmapFactory.h>

// #include <Mod/PartDesign/App/DatumPoint.h>
// #include <Mod/PartDesign/App/DatumLine.h>
// #include <Mod/PartDesign/App/DatumPlane.h>
// #include <Mod/PartDesign/App/Body.h>
// #include <Mod/PartDesign/App/DatumCS.h>

// #include "TaskDatumParameters.h"
// #include "ViewProviderBody.h"
// #include "Utils.h"
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/SbBox3f.h>

#include <Gui/Application.h>
#include <Gui/ViewProviderOrigin.h>
#include <Gui/Control.h>
#include <Gui/Command.h>
#include <Gui/Document.h>

#include <QMenu>
#include <QAction>
#include <QMessagebox>

#include "ViewProviderOTAttachableObject.h"

#include "TaskDlgOTAttacher.h"
#include "TaskOTAttacher.h"

using namespace RobotGui;

PROPERTY_SOURCE_WITH_EXTENSIONS(RobotGui::ViewProviderOTAttachableObject, Gui::ViewProviderGeometryObject)

// static data
const double ViewProviderOTAttachableObject::defaultSize = Gui::ViewProviderOrigin::defaultSize();

ViewProviderOTAttachableObject::ViewProviderOTAttachableObject()
{
    PartGui::ViewProviderAttachExtension::initExtension(this);

    pShapeSep = new SoSeparator(); // root of scene graph
    pShapeSep->ref();
    pPickStyle = new SoPickStyle(); // FIXME: what is pick style?
    pPickStyle->ref();

    DisplayMode.setStatus(App::Property::Hidden, true);

    // FIXME: FC code which might not be useful
    /*
    // set default color for datums (golden yellow with 60% transparency)
    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath (
            "User parameter:BaseApp/Preferences/Mod/PartDesign");
    unsigned long shcol = hGrp->GetUnsigned ( "DefaultDatumColor", 0xFFD70099 );

    App::Color col ( (uint32_t) shcol );
    ShapeColor.setValue ( col );

    Transparency.setValue (col.a * 100);
    */
    oldWb = "";
    oldTip = NULL;
}

ViewProviderOTAttachableObject::~ViewProviderOTAttachableObject()
{
    pShapeSep->unref();
    pPickStyle->unref();
}

void ViewProviderOTAttachableObject::attach(App::DocumentObject *obj)
{
    ViewProviderGeometryObject::attach(obj);

    // FIXME: deleted FC Code
    /* // TODO remove this field (2015-09-08, Fat-Zer)
    App::DocumentObject* o = getObject();
    if (o->getTypeId() == PartDesign::Plane::getClassTypeId()) {
        datumType = QString::fromLatin1("Plane");
        datumText = QObject::tr("Plane");
    }
    else if (o->getTypeId() == PartDesign::Line::getClassTypeId()) {
        datumType = QString::fromLatin1("Line");
        datumText = QObject::tr("Line");
    }
    else if (o->getTypeId() == PartDesign::Point::getClassTypeId()) {
        datumType = QString::fromLatin1("Point");
        datumText = QObject::tr("Point");
    }
    else if (o->getTypeId() == PartDesign::CoordinateSystem::getClassTypeId()) {
        datumType = QString::fromLatin1("CoordinateSystem");
        datumText = QObject::tr("Coordinate System");
    } */

    SoShapeHints *hints = new SoShapeHints();
    hints->shapeType.setValue(SoShapeHints::UNKNOWN_SHAPE_TYPE);
    hints->vertexOrdering.setValue(SoShapeHints::COUNTERCLOCKWISE);
    SoDrawStyle *fstyle = new SoDrawStyle();
    fstyle->style = SoDrawStyle::FILLED;
    fstyle->lineWidth = 3;
    fstyle->pointSize = 5;
    pPickStyle->style = SoPickStyle::SHAPE;
    SoMaterialBinding *matBinding = new SoMaterialBinding;
    matBinding->value = SoMaterialBinding::OVERALL;

    SoSeparator *sep = new SoSeparator();
    sep->addChild(hints);
    sep->addChild(fstyle);
    sep->addChild(pPickStyle);
    sep->addChild(matBinding);
    sep->addChild(pcShapeMaterial);
    sep->addChild(pShapeSep);

    addDisplayMaskMode(sep, "Base");
}

bool ViewProviderOTAttachableObject::onDelete(const std::vector<std::string> &)
{
    // FIXME: Fine for us to not consider the attachable object getting deleted, because we are not building geometry feature based on attachable object

    /* // FCTODO Ask user what to do about dependent objects, e.g. Sketches that have this feature as their support
    // 1. Delete
    // 2. Suppress
    // 3. Re-route
 */
    return true;
}

std::vector<std::string> ViewProviderOTAttachableObject::getDisplayModes(void) const
{
    return {"Base"};
}

void ViewProviderOTAttachableObject::setDisplayMode(const char *ModeName)
{
    if (strcmp(ModeName, "Base") == 0)
        setDisplayMaskMode("Base");
    ViewProviderGeometryObject::setDisplayMode(ModeName);
}

// FIXME: Deleted FC code
/* std::string ViewProviderOTAttachableObject::getElement(const SoDetail* detail) const
{
    if (detail) {
        int element = 1;

        if (detail->getTypeId() == SoLineDetail::getClassTypeId()) {
            const SoLineDetail* line_detail = static_cast<const SoLineDetail*>(detail);
            element = line_detail->getLineIndex();
        } else if (detail->getTypeId() == SoFaceDetail::getClassTypeId()) {
            const SoFaceDetail* face_detail = static_cast<const SoFaceDetail*>(detail);
            element = face_detail->getFaceIndex();
        } else if (detail->getTypeId() == SoPointDetail::getClassTypeId()) {
            const SoPointDetail* point_detail = static_cast<const SoPointDetail*>(detail);
            element = point_detail->getCoordinateIndex();
        }

        if (element == 0)
            return datumType.toStdString();
    }

    return std::string("");
}

SoDetail* ViewProviderOTAttachableObject::getDetail(const char* subelement) const
{
    QString subelem = QString::fromLatin1(subelement);

    if (subelem == QObject::tr("Line")) {
         SoLineDetail* detail = new SoLineDetail();
         detail->setPartIndex(0);
         return detail;
    } else if (subelem == QObject::tr("Plane")) {
        SoFaceDetail* detail = new SoFaceDetail();
        detail->setPartIndex(0);
        return detail;
   } else if (subelem == QObject::tr("Point")) {
        SoPointDetail* detail = new SoPointDetail();
        detail->setCoordinateIndex(0);
        return detail;
   }

    return NULL;
}
 */
bool ViewProviderOTAttachableObject::isSelectable(void) const
{
    return true;
}

void ViewProviderOTAttachableObject::setupContextMenu(QMenu *menu, QObject *receiver, const char *member)
{
    QAction *act;
    act = menu->addAction(QObject::tr("Edit OTAttachable"), receiver, member);
    act->setData(QVariant((int)ViewProvider::Default));
}

bool ViewProviderOTAttachableObject::setEdit(int ModNum)
{
    if (!ViewProvider::setEdit(ModNum))
        return false;
    // FIXME: FC comment seems useful to us as well
    // TODO Share this code with Features view providers somehow (2015-09-08, Fat-Zer)
    if (ModNum == ViewProvider::Default)
    {

        // Check the currently activated dialog in Document first
        Gui::TaskView::TaskDialog *dlg = Gui::Control().activeDialog();          // Get the currently activated Dialog
        TaskDlgOTAttacher *attacherDlg = qobject_cast<TaskDlgOTAttacher *>(dlg); // Check the viewprovider of opening task dialog
        if (attacherDlg && attacherDlg->getViewProvider() != this)
            attacherDlg = 0; // another attachable left open its task panel
        if (dlg && !attacherDlg)
        { // ask for user interaction
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

        // FIXME: We need to ensure at this time all the needed module should be imported, but in our case does it have to be the PartDesignWorkbench?
        //  oldWb = Gui::Command::assureWorkbench("PartDesignWorkbench");

        // When double-clicking on the item for this datum feature the
        // object unsets and sets its edit mode without closing
        // the task panel
        // Here is the case, dialog is active, but we switch to another view, mybe model view, and now if we double click the viewprovider again
        // we will show the dialog
        if (attacherDlg) // In case there is a activated dialog
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

bool ViewProviderOTAttachableObject::doubleClicked(void)
{
    // TODO: Consider a appropriate double click behaviour for attachable object
    /* auto activeDoc = Gui::Application::Instance->activeDocument();
    if (!activeDoc)
        activeDoc = getDocument();
    auto activeView = activeDoc->getActiveView();
    if (!activeView)
        return false;

    std::string Msg("Edit ");
    Msg += this->pcObject->Label.getValue();
    Gui::Command::openCommand(Msg.c_str());
 */
    // FIXME: Deleted FC code, we don't need body, we are not desgining geometry features using this attachable object
    /* Part::Datum *pcDatum = static_cast<Part::Datum *>(getObject());
    PartDesign::Body *activeBody = activeView->getActiveObject<PartDesign::Body *>(PDBODYKEY);
    auto datumBody = PartDesignGui::getBodyFor(pcDatum, false);

    if (datumBody != NULL)
    {
        if (datumBody != activeBody)
        {
            Gui::Command::doCommand(Gui::Command::Gui,
                                    "Gui.ActiveDocument.ActiveView.setActiveObject('%s',%s)",
                                    PDBODYKEY, Gui::Command::getObjectCmd(datumBody).c_str());
            activeBody = datumBody;
        }
    }
    return PartDesignGui::setEdit(pcObject, activeBody);
    */
}

void ViewProviderOTAttachableObject::unsetEdit(int ModNum)
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

//FIXME: Deleted FC code regarding the view auto adjustment feature, maybe useful later
/* void ViewProviderOTAttachableObject::updateExtents()
{
    setExtents(getRelevantBoundBox());
}

void ViewProviderOTAttachableObject::setExtents(const SbBox3f &bbox)
{
    const SbVec3f &min = bbox.getMin();
    const SbVec3f &max = bbox.getMax();
    setExtents(Base::BoundBox3d(min.getValue()[0], min.getValue()[1], min.getValue()[2],
                                max.getValue()[0], max.getValue()[1], max.getValue()[2]));
}

SbBox3f ViewProviderOTAttachableObject::getRelevantBoundBox() const
{
    std::vector<App::DocumentObject *> objs;

    // Probe body first
    PartDesign::Body *body = PartDesign::Body::findBodyOf(this->getObject());
    if (body)
    {
        objs = body->getFullModel();
    }
    else
    {
        // Probe if we belongs to some group
        App::DocumentObject *group = App::DocumentObjectGroup::getGroupOfObject(this->getObject());

        if (group)
        {
            auto *ext = group->getExtensionByType<App::GroupExtension>();
            if (ext)
                objs = ext->getObjects();
        }
        else
        {
            // Fallback to whole document
            objs = this->getObject()->getDocument()->getObjects();
        }
    }

    Gui::View3DInventor *view = dynamic_cast<Gui::View3DInventor *>(this->getActiveView());
    if (view)
    {
        Gui::View3DInventorViewer *viewer = view->getViewer();
        SoGetBoundingBoxAction bboxAction(viewer->getSoRenderManager()->getViewportRegion());
        SbBox3f bbox = getRelevantBoundBox(bboxAction, objs);

        if (bbox.getVolume() < Precision::Confusion())
        {
            bbox.extendBy(defaultBoundBox());
        }
        return bbox;
    }
    else
    {
        return defaultBoundBox();
    }
}

SbBox3f ViewProviderOTAttachableObject::getRelevantBoundBox(
    SoGetBoundingBoxAction &bboxAction, const std::vector<App::DocumentObject *> &objs)
{
    SbBox3f bbox = defaultBoundBox();

    // Adds the bbox of given feature to the output
    for (auto obj : objs)
    {
        ViewProvider *vp = Gui::Application::Instance->getViewProvider(obj);
        if (!vp)
        {
            continue;
        }
        if (!vp->isVisible())
        {
            continue;
        }

        if (obj->isDerivedFrom(Part::Datum::getClassTypeId()))
        {
            // Treat datums only as their basepoint
            // I hope it's ok to take FreeCAD's point here
            Base::Vector3d basePoint = static_cast<Part::Datum *>(obj)->getBasePoint();
            bbox.extendBy(SbVec3f(basePoint.x, basePoint.y, basePoint.z));
        }
        else
        {
            bboxAction.apply(vp->getRoot());
            SbBox3f obj_bbox = bboxAction.getBoundingBox();

            if (obj_bbox.getVolume() < Precision::Infinite())
            {
                bbox.extendBy(obj_bbox);
            }
        }
    }

    // TODO: shrink bbox when all other elements are too small
    return bbox;
}

SbBox3f ViewProviderOTAttachableObject::defaultBoundBox()
{
    return SbBox3f(-defaultSize, -defaultSize, -defaultSize,
                   defaultSize, defaultSize, defaultSize);
}
 */
bool ViewProviderOTAttachableObject::isPickable()
{

    return bool(pPickStyle->style.getValue() == SoPickStyle::SHAPE);
}

void ViewProviderOTAttachableObject::setPickable(bool val)
{

    if (val)
        pPickStyle->style = SoPickStyle::SHAPE;
    else
        pPickStyle->style = SoPickStyle::UNPICKABLE;
}
