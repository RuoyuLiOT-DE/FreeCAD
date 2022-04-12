#include "PreCompiled.h"

#include "RobCoordSystem.h"
#include <App/DocumentObjectPy.h>
#include <Base/Placement.h>

#include <Base/Writer.h>
#include <Base/Reader.h>
#include <Base/Console.h>


using namespace Robot;
using namespace App;


PROPERTY_SOURCE(Robot::RobCoordSystem, App::GeoFeature)


RobCoordSystem::RobCoordSystem()
	:block(false)
{
	ADD_PROPERTY_TYPE(Base, (Base::Placement()), "Coord Desc", Prop_None, "the data of Coord");

}

RobCoordSystem::~RobCoordSystem()
{
}

short RobCoordSystem::mustExecute(void) const
{
	return 0;
}

PyObject *RobCoordSystem::getPyObject()
{
	if (PythonObject.is(Py::_None())) {
		// ref counter is set to 1
		PythonObject = Py::Object(new DocumentObjectPy(this), true);
	}
	return Py::new_reference_to(PythonObject);
}


void RobCoordSystem::onChanged(const Property* prop)
{

	if (prop == &Base) {
		//robot.readKinematic(RobotKinematicFile.getValue());
		//Tcp.setValue(robot.getTcp());
		//FlangeFrame.setValue(robot.getFlangFrme());
	}


	App::GeoFeature::onChanged(prop);
}

void RobCoordSystem::Save(Base::Writer &writer) const
{
	App::GeoFeature::Save(writer);
	//robot.Save(writer);
}

void RobCoordSystem::Restore(Base::XMLReader &reader)
{
	block = true;
	App::GeoFeature::Restore(reader);
	//robot.Restore(reader);

	//// set up the robot with the loaded axis position 
	//robot.setAxis(0, Axis1.getValue());
	//robot.setAxis(1, Axis2.getValue());
	//robot.setAxis(2, Axis3.getValue());
	//robot.setAxis(3, Axis4.getValue());
	//robot.setAxis(4, Axis5.getValue());
	//robot.setAxis(5, Axis6.getValue());
	//robot.setTo(Tcp.getValue());
	//Tcp.setValue(robot.getTcp());
	//FlangeFrame.setValue(robot.getFlangFrme());
	//block = false;
}

