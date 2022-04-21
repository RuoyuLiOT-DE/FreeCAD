#pragma once

#include <Mod/Part/App/PartFeature.h>
#include <Mod/Part/App/AttachExtension.h>

namespace Robot
{
    class RobotExport OTCoordinateSystemObject : public Part::Feature, public Part::AttachExtension
    {
        PROPERTY_HEADER_WITH_EXTENSIONS(Robot::OTCoordinateSystemObject);

        public:
        OTCoordinateSystemObject();
        virtual ~OTCoordinateSystemObject();

        virtual const char* getViewProviderName(void) const override {
            return "RobotGui::ViewProviderOTCoordinateSystem";
        }
    };
}