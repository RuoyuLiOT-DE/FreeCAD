#pragma once

#include <Gui/ViewProviderGeometryObject.h>
#include <Mod/Part/Gui/ViewProviderAttachExtension.h>

namespace RobotGui {
    class RobotGuiExport ViewProviderAttachablePartObject : public Gui::ViewProviderGeometryObject, PartGui::ViewProviderAttachExtension
    {
        PROPERTY_HEADER_WITH_EXTENSIONS(RobotGui::ViewProviderAttachablePartObject);

        public:
        ViewProviderAttachablePartObject();
        virtual ~ViewProviderAttachablePartObject();

        protected:
        virtual bool setEdit(int ModNum) override;
        virtual void unsetEdit(int ModNum) override;

        private:
        std::string oldWb;

    };
}