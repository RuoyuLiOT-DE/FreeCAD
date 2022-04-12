#include "PreCompiled.h"

#ifndef _PreComp_
# include <Inventor/SoDB.h>
# include <Inventor/SoInput.h>
# include <Inventor/SbVec3f.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoTransform.h>
# include <Inventor/nodes/SoSphere.h>
# include <Inventor/nodes/SoRotation.h>
# include <Inventor/actions/SoSearchAction.h>
# include <Inventor/draggers/SoJackDragger.h>
# include <Inventor/draggers/SoTrackballDragger.h>
# include <Inventor/VRMLnodes/SoVRMLTransform.h>
# include <QFile>
#endif

#include "ViewProviderCoordSystem.h"

# include <Inventor/nodekits/SoShapeKit.h>
#include <Gui/SoAxisCrossKit.h>
#include <Gui/SoFCBoundingBox.h>

#include <Mod/Robot/App/RobCoordSystem.h>

//#include <Mod/Part/App/PartFeature.h>
//#include <Mod/Part/Gui/ViewProvider.h>
#include <App/Document.h>
//#include <App/VRMLObject.h>
//#include <Gui/Application.h>
//#include <Base/FileInfo.h>
//#include <Base/Stream.h>
//#include <Base/Console.h>
//#include <sstream>

#include <Gui/SoFCCSysDragger.h>


using namespace Gui;
using namespace RobotGui;

PROPERTY_SOURCE(RobotGui::ViewProviderCoordSystem, Gui::ViewProviderGeometryObject)

ViewProviderCoordSystem::ViewProviderCoordSystem()
{
	pcRobotRoot = new Gui::SoFCSelection();
	pCS = new SoFCCSysDragger();
	pCS->ref();
	pCS->hideRotationX();
	pCS->hideRotationY();
	pCS->hideRotationZ();
	
	pCS->translationIncrement = 0;
	pCS->draggerSize = 30;
	
	pcRobotRoot->addChild(pCS);	
}

ViewProviderCoordSystem::~ViewProviderCoordSystem()
{
	pCS->unref();
	pcRobotRoot->unref();
}

void ViewProviderCoordSystem::attach(App::DocumentObject *pcObj)
{
	ViewProviderGeometryObject::attach(pcObj);

	addDisplayMaskMode(pcRobotRoot, "VRML");
	pcRobotRoot->objectName = pcObj->getNameInDocument();
	pcRobotRoot->documentName = pcObj->getDocument()->getName();
	pcRobotRoot->subElementName = "Main";
}

void ViewProviderCoordSystem::setDisplayMode(const char* ModeName)
{
	if (strcmp("VRML", ModeName) == 0)
		setDisplayMaskMode("VRML");
	ViewProviderGeometryObject::setDisplayMode(ModeName);
}

std::vector<std::string> ViewProviderCoordSystem::getDisplayModes(void) const
{
	std::vector<std::string> StrList;
	StrList.push_back("VRML");
	return StrList;
}

void ViewProviderCoordSystem::updateData(const App::Property* prop)
{
	Robot::RobCoordSystem* robObj = static_cast<Robot::RobCoordSystem*>(pcObject);
	if (prop == &robObj->Placement) {
		double matrix[16] = { 0 };
		robObj->Placement.getValue().toMatrix().getMatrix(matrix);
	
		setTransformation(robObj->Placement.getValue().toMatrix());
	}
	
}


