#include "PreCompiled.h"

#include "OTCoordinateSystemObject.h"

using namespace Robot;
PROPERTY_SOURCE_WITH_EXTENSIONS(Robot::OTCoordinateSystemObject, Part::Feature)

OTCoordinateSystemObject::OTCoordinateSystemObject()
{
    Part::AttachExtension::initExtension(this);
    this->setAttacher(new Attacher::AttachEngine3D);
    //TODO: Do we need a shape for the coordinate system?

}

OTCoordinateSystemObject::~OTCoordinateSystemObject(){}

