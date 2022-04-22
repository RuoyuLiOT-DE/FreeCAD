#pragma once

#include <Mod/Part/App/AttachExtension.h>

#include "OTAttachableObject.h"

namespace Robot
{
    class RobotExport OTCoordinateSystemObject : public Robot::OTAttachableObject
    {
        PROPERTY_HEADER_WITH_EXTENSIONS(Robot::OTCoordinateSystemObject);

    public:
        OTCoordinateSystemObject();
        virtual ~OTCoordinateSystemObject();

        virtual const char *getViewProviderName(void) const override
        {
            return "RobotGui::ViewProviderOTCoordinateSystemObject";
        }

        Base::Vector3d getXAxis();
        Base::Vector3d getYAxis();
        Base::Vector3d getZAxis();

        virtual App::DocumentObject *getSubObject(const char *subname,
                                                  PyObject **pyObj, Base::Matrix4D *pmat, bool transform, int depth) const override;
    };
}