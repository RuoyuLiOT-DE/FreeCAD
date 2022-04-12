#pragma once

#include "PreCompiled.h"
#include <App/GeoFeature.h>
#include <App/PropertyFile.h>
#include <App/PropertyGeo.h>
#include <App/PropertyLinks.h>

namespace Robot
{

	class RobotExport RobCoordSystem : public App::GeoFeature
	{
		PROPERTY_HEADER(Robot::RobCoordSystem);

	public:
		/// Constructor
		RobCoordSystem(void);
		virtual ~RobCoordSystem();

		/// returns the type name of the ViewProvider
		virtual const char* getViewProviderName(void) const {
			return "RobotGui::ViewProviderCoordSystem";
		}
		virtual App::DocumentObjectExecReturn *execute(void) {
			return App::DocumentObject::StdReturn;
		}
		virtual short mustExecute(void) const;
		virtual PyObject *getPyObject(void);

		virtual void Save(Base::Writer &/*writer*/) const;
		virtual void Restore(Base::XMLReader &/*reader*/);

		App::PropertyPlacement Base;


	protected:
		/// get called by the container when a property has changed
		virtual void onChanged(const App::Property* prop);


		bool block;

	};

} //namespace Robot



