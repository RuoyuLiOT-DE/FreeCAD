#pragma once

#include <Gui/ViewProviderGeometryObject.h>
#include <Mod/Part/Gui/ViewProviderAttachExtension.h>

// Forward declarations
class SoCoordinate3;
class SoFont;
class SoTranslation;
namespace Gui
{
    class SoAutoZoomTranslation;
}

namespace RobotGui
{
    class RobotGuiExport ViewProviderOTCoordinateSystemObject : public Gui::ViewProviderGeometryObject, PartGui::ViewProviderAttachExtension
    {
        PROPERTY_HEADER(RobotGui::ViewProviderOTCoordinateSystemObject);

    public:
        /*attributes*/
        App::PropertyFloatConstraint Zoom;
        App::PropertyIntegerConstraint FontSize;
        App::PropertyBool ShowLabel;

        /*constructor*/
        ViewProviderOTCoordinateSystemObject();
        virtual ~ViewProviderOTCoordinateSystemObject();

        /*methods*/
        virtual void attach(App::DocumentObject *obj);
        virtual void updateData(const App::Property *);
        virtual void onChanged(const App::Property *);

        virtual void setExtents(Base::BoundBox3d bbox);

        virtual SoDetail *getDetail(const char *subelement) const;
        virtual std::string getElement(const SoDetail *detail) const;

    private:
        /*attributes*/
        SoCoordinate3 *coord;
        SoTranslation *axisLabelXTrans;
        SoTranslation *axisLabelXToYTrans;
        SoTranslation *axisLabelYToZTrans;
        SoFont *font;
        SoSwitch *labelSwitch;
        Gui::SoAutoZoomTranslation *autoZoom;

        /*methods*/
        void setupLabels();
    };
}