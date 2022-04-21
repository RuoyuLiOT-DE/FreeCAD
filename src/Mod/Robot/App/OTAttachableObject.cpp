#include "PreCompiled.h"

#include <Mod/Part/App/OCCError.h>
#include <Mod/Part/App/PartPyCXX.h>
#include <Mod/Part/App/TopoShape.h>

#include "OTAttachableObject.h"

using namespace Robot;
PROPERTY_SOURCE_ABSTRACT_WITH_EXTENSIONS(Robot::OTAttachableObject, Part::Feature)

OTAttachableObject::OTAttachableObject(void)
{
    Part::AttachExtension::initExtension(this);
    touch();
}

OTAttachableObject::~OTAttachableObject() {}

void OTAttachableObject::onDocumentRestored()
{
    // FIXME: comment from FC developer here
    // This seems to be the only way to make the ViewProvider display the OTAttachableObject feature
    Support.touch();
    Part::Feature::onDocumentRestored();
}

TopoDS_Shape OTAttachableObject::getShape() const
{
    Part::TopoShape sh = Shape.getShape();
    sh.setPlacement(Placement.getValue());
    return sh.getShape();
}

App::DocumentObject *OTAttachableObject::getSubObject(const char *subname,
                                                      PyObject **pyObj, Base::Matrix4D *pmat, bool transform, int depth) const
{
    // FIXME: comment form FC developer here, which no longer holds true
    // For the sake of simplicity, we don't bother to check for subname, just
    // return the shape as it is, because a OTAttachableObject object only holds shape with
    // one single geometry element.
    // TODO: compare with the getSubObject form Part/App/PartFeature.h

    (void)subname;
    (void)depth;

    if (pmat && transform)
        *pmat *= Placement.getValue().toMatrix();

    if (!pyObj)
        return const_cast<OTAttachableObject *>(this);

    Base::PyGILStateLocker lock;
    PY_TRY
    {
        Part::TopoShape ts(getShape().Located(TopLoc_Location()));
        if (pmat && !ts.isNull())
            ts.transformShape(*pmat, false, true);
        *pyObj = Py::new_reference_to(Part::shape2pyshape(ts.getShape()));
        return const_cast<OTAttachableObject *>(this);
    }
    PY_CATCH_OCC
}

Base::Vector3d OTAttachableObject::getBasePoint() const
{
    return Placement.getValue().getPosition();
}

void OTAttachableObject::handleChangedPropertyName(Base::XMLReader &reader, const char *TypeName, const char *PropName)
{
    extHandleChangedPropertyName(reader, TypeName, PropName); // AttachExtension
}
