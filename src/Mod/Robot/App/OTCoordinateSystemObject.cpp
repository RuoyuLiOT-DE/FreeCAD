#include "PreCompiled.h"

#include <BRepBuilderAPI_MakeFace.hxx>
#include <gp_Pln.hxx>
#include <Mod/Part/App/OCCError.h>
#include <Mod/Part/App/PartPyCXX.h>

#include "OTCoordinateSystemObject.h"

using namespace Robot;
PROPERTY_SOURCE_WITH_EXTENSIONS(Robot::OTCoordinateSystemObject, Robot::OTAttachableObject)

OTCoordinateSystemObject::OTCoordinateSystemObject()
{
    Part::AttachExtension::initExtension(this);
    this->Part::AttachExtension::setAttacher(new Attacher::AttachEngine3D);
    // TODO: Do we need a shape for the coordinate system?
    BRepBuilderAPI_MakeFace builder(gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));
    if (!builder.IsDone())
        return;
    Shape.setValue(builder.Shape());
}

OTCoordinateSystemObject::~OTCoordinateSystemObject() {}

Base::Vector3d OTCoordinateSystemObject::getXAxis()
{
    Base::Rotation rot = Placement.getValue().getRotation();
    Base::Vector3d normal;
    rot.multVec(Base::Vector3d(1,0,0), normal);
    return normal;
}

Base::Vector3d OTCoordinateSystemObject::getYAxis()
{
    Base::Rotation rot = Placement.getValue().getRotation();
    Base::Vector3d normal;
    rot.multVec(Base::Vector3d(0,1,0), normal);
    return normal;
}
 
Base::Vector3d OTCoordinateSystemObject::getZAxis()
{
    Base::Rotation rot = Placement.getValue().getRotation();
    Base::Vector3d normal;
    rot.multVec(Base::Vector3d(0,0,1), normal);
    return normal;
}

App::DocumentObject *OTCoordinateSystemObject::getSubObject(const char *subname, 
        PyObject **pyObj, Base::Matrix4D *pmat, bool transform, int) const
{
    if(pmat && transform)
        *pmat *= Placement.getValue().toMatrix();

    if(!pyObj)
        return const_cast<OTCoordinateSystemObject*>(this);

    gp_Dir dir(0,0,1);
    if(subname) {
        if(strcmp(subname,"X")==0)
            dir = gp_Dir(1,0,0);
        else if(strcmp(subname,"Y")==0)
            dir = gp_Dir(0,1,0);
    }

    Base::PyGILStateLocker lock;
    PY_TRY {
        BRepBuilderAPI_MakeFace builder(gp_Pln(gp_Pnt(0,0,0), dir));
        Part::TopoShape ts(builder.Shape());
        if(pmat)
            ts.transformShape(*pmat,false,true);
        *pyObj =  Py::new_reference_to(Part::shape2pyshape(ts));
        return const_cast<OTCoordinateSystemObject*>(this);
    } PY_CATCH_OCC
}