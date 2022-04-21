#pragma once

#include <QString>
#include <App/PropertyLinks.h>

#include <Mod/Part/App/PartFeature.h>
#include <Mod/Part/App/AttachExtension.h>

namespace Robot
{

class RobotExport OTAttachableObject : public Part::Feature, public Part::AttachExtension
{
    // Attention!! this is a abstract class
    PROPERTY_HEADER_WITH_EXTENSIONS(Robot::OTAttachableObject);

public:
    OTAttachableObject();
    virtual ~OTAttachableObject();
    //short mustExecute();

    /// returns the type name of the view provider
    virtual const char* getViewProviderName(void) const override = 0;

    /// Return a shape including Placement representing the OTAttachableObject feature
    virtual TopoDS_Shape getShape() const;

    /// Returns a point of the feature it counts as it's base
    virtual Base::Vector3d getBasePoint () const;

    virtual App::DocumentObject *getSubObject(const char *subname, PyObject **pyObj, 
            Base::Matrix4D *mat, bool transform, int depth) const override;
protected:
    void onDocumentRestored() override;
    void handleChangedPropertyName(Base::XMLReader &reader, const char* TypeName, const char* PropName) override;
};

} //namespace Robot