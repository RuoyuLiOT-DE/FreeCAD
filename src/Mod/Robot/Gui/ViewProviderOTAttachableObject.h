#pragma once

#include "Gui/ViewProviderGeometryObject.h"
#include <Base/BoundBox.h>

#include <Mod/Part/Gui/ViewProviderAttachExtension.h>

class SoPickStyle;
class SbBox3f;
class SoGetBoundingBoxAction;

namespace RobotGui
{

    class RobotGuiExport ViewProviderOTAttachableObject : public Gui::ViewProviderGeometryObject, PartGui::ViewProviderAttachExtension
    {
        PROPERTY_HEADER_WITH_EXTENSIONS(RobotGui::ViewProviderOTAttachableObject);

    public:
        /// constructor
        ViewProviderOTAttachableObject();
        /// destructor
        virtual ~ViewProviderOTAttachableObject();

        /// grouping handling
        void setupContextMenu(QMenu *, QObject *, const char *) override;

        virtual void attach(App::DocumentObject *) override;
        virtual bool onDelete(const std::vector<std::string> &) override;
        virtual bool doubleClicked(void) override;
        std::vector<std::string> getDisplayModes(void) const override;
        void setDisplayMode(const char *ModeName) override;

        /// indicates if the ViewProvider use the new Selection model
        virtual bool useNewSelectionModel(void) const override { return true; }
        /// indicates if the ViewProvider can be selected //FIXME: in 3D View?
        virtual bool isSelectable(void) const override;

        // FIXME: Deleted FC code
        /* /// return a hit element to the selection path or 0
        virtual std::string getElement(const SoDetail *) const override;
        virtual SoDetail* getDetail(const char*) const override;
     */
        /**
         * Enable/Disable the selectability of the datum
         * This differs from the normal ViewProvider selectability in that, that with this enabled one
         * can pick through the datum and select stuff behind it.
         */
        bool isPickable();
        void setPickable(bool val);

        // FIXME: Deleted FC code auto adjusting views mb later
        //  /**
        //   * Update the visual size to match the given extents
        //   * @note should be reimplemented in the offspings
        //   * @note use FreeCAD-specific bbox here to simplify the math in derived classes
        //   */
        //  virtual void setExtents(Base::BoundBox3d /*bbox*/)
        //  {
        //  }

        // /// Update the visual sizes. This overloaded version of the previous function to allow pass coin type
        // void setExtents(const SbBox3f &bbox);

        // /// update size to match the guessed bounding box
        // void updateExtents();

        /*FIXME: Deleted FC CODE
        /// The datum type (Plane, Line or Point)
        // TODO remove this attribute (2015-09-08, Fat-Zer)
        QString datumType;
        QString datumText;
        */

        // FIXME: maybe later
        //  /**
        //   * Computes appropriate bounding box for the given list of objects to be passed to setExtents ()
        //   * @param bboxAction  a coin action for traverse the given objects views.
        //   * @param objs        the list of objects to traverse, due to we traverse the scene graph, the geo children
        //   *                    will likely be traversed too.
        //   */
        //  static SbBox3f getRelevantBoundBox(
        //      SoGetBoundingBoxAction &bboxAction,
        //      const std::vector<App::DocumentObject *> &objs);

        /// Default size used to produce the default bbox
        static const double defaultSize;
        // FIXME: maybe later
        /*         // Returned default bounding box if relevant is can't be used for some reason
                static SbBox3f defaultBoundBox(); */

        // Returns a default margin factor (part of size )
        static double marginFactor() { return 0.1; };

    protected:
        virtual bool setEdit(int ModNum) override;
        virtual void unsetEdit(int ModNum) override;
        
        //FIXME: maybe later
        // /**
        //  * Guesses the context this datum belongs to and returns appropriate bounding box of all
        //  *  visible content of the feature
        //  *
        //  * Currently known contexts are:
        //  *  - PartDesign::Body
        //  *  - App::DocumentObjectGroup (App::Part as well as subclass)
        //  *  - Whole document
        //  */
        // SbBox3f getRelevantBoundBox() const;

        // Get the separator to fill with datum content
        SoSeparator *getShapeRoot() { return pShapeSep; }

    private:
        SoSeparator *pShapeSep;
        SoPickStyle *pPickStyle;
        std::string oldWb;
        App::DocumentObject *oldTip;
    };

} // namespace RobotGui
