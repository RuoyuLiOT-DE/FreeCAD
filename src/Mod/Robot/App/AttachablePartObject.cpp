#include "PreCompiled.h"

#include "AttachablePartObject.h"

using namespace Robot;

PROPERTY_SOURCE_WITH_EXTENSIONS(Robot::AttachablePartObject, Part::Feature)

AttachablePartObject::AttachablePartObject()
{
    AttachExtension::initExtension(this);
    touch();   
}

AttachablePartObject::~AttachablePartObject()
{

}