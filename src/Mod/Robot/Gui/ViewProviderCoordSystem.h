#ifndef ROBOT_VIEWPROVIDERCOORDSYSTEM_H
#define ROBOT_VIEWPROVIDERCOORDSYSTEM_H

#include <Gui/ViewProviderGeometryObject.h>
#include <Gui/SoFCSelection.h>
#include <Base/Placement.h>

class SoDragger;
class SoJackDragger;
class SoTrackballDragger;

namespace RobotGui
{

	class RobotGuiExport ViewProviderCoordSystem : public Gui::ViewProviderGeometryObject
	{
		PROPERTY_HEADER(RobotGui::ViewProviderCoordSystem);

	public:
		/// constructor.
		ViewProviderCoordSystem();

		/// destructor.
		~ViewProviderCoordSystem();

		void attach(App::DocumentObject *pcObject);
		void setDisplayMode(const char* ModeName);
		std::vector<std::string> getDisplayModes() const;
		void updateData(const App::Property*);

	protected:
		Gui::SoFCSelection    * pcRobotRoot;
		Gui::SoFCCSysDragger  * pCS;

	};

} //namespace RobotGui


#endif // ROBOT_VIEWPROVIDERCOORDSYSTEM_H
