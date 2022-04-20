#include <Mod/Part/App/PartFeature.h>
#include <Mod/Part/App/AttachExtension.h>

namespace Robot{

class RobotExport AttachablePartObject : public Part::Feature, public Part::AttachExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(Robot::AttachablePartObject);

    public:
    AttachablePartObject();

    // const char* getViewProviderName(void) const override {
    //     return "RobotGui::ViewProviderAttachablePartObject";
    // }
    virtual ~AttachablePartObject();
};

}