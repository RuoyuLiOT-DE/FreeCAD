#pragma once
#include <Gui/TaskView/TaskView.h>
#include <Gui/Selection.h>

namespace Gui {class Document;}
namespace RobotGui
{
    class Ui_TaskOTAttacher;

    class RobotGuiExport TaskOTAttacher : public Gui::TaskView::TaskBox, public Gui::SelectionObserver
    {
        Q_OBJECT

    public:
        TaskOTAttacher(Gui::ViewProviderDocumentObject *ViewProvider, QWidget *parent = 0,
                       QString picture = QString(),
                       QString text = QString::fromLatin1("Attachment") /*, VisibilityFunction func = 0*/);
        ~TaskOTAttacher();

        /*member functions*/
    private Q_SLOTS:

        ///@brief Activating the parent frame selection feature provided by this tool
        ///       indicated by a internal bool variable
        ///@param clicked
        ///
        void onButtonParentFrame(const bool clicked);

        ///@brief
        ///
        ///@param text
        ///
        void onLineParentFrame(const QString &text);
        void onAttachmentOffsetChanged(int idx); // Reused by individual property changing spin box
        void onAttachmentOffsetXChanged(double);
        void onAttachmentOffsetYChanged(double);
        void onAttachmentOffsetZChanged(double);
        void onAttachmentOffsetYawChanged(double);
        void onAttachmentOffsetPitchChanged(double);
        void onAttachmentOffsetRollChanged(double);

        ///@brief Configure whether use current transformation as attachment offset
        ///
        ///@param toggled
        ///
        void onCheckBoxUseCurrentTransform(const bool toggled);

        ///@brief Configure wheter activate the attachment
        ///
        ///@param toggled
        ///
        void onCheckBoxAttachmentActivate(const bool toggled);

    private:

        ///@brief Update the text displayed on the button according to the checked state of the button 
        ///
        ///
        void updateButtonParentFrame();

        ///@brief updatePreview: calculate attachment, update 3d view, update status message (status mesage is on top of this task panel)
        ///
        ///@return true if attachment calculation was successful, false otherwise
        ///
        bool updatePreview();

        ///@brief Must override function inherited from SelectionObserver, this observer bedefault will be attached to a subject at construction
        ///       FIXME:Attention!!!! now with any selection change, this function is called 3 times
        ///@param msg
        ///
        void onSelectionChanged(const Gui::SelectionChanges &msg) override;

        ///@brief Update the attachment offset ui using parameter from model 
        ///
        ///
        void updateAttachmentOffsetUI();

        ///@brief Use the current transform as the default relative transformation from parent to the child 
        ///
        ///
        void useCurrentTransform();

        ///@brief
        ///
        ///@param refstrings
        ///@param refnames
        ///
        void makeRefStrings(std::vector<QString> &refstrings, std::vector<std::string> &refnames);

        void objectDeleted(const Gui::ViewProviderDocumentObject &);
        void documentDeleted(const Gui::Document &);

        /*member variable*/
    protected:
        Gui::ViewProviderDocumentObject *ViewProvider; // FIXME: For inheritance?, get from TaskDlg
        std::string ObjectName;                        // FIXME: For inheritance?

    private:
        QWidget *proxy; // Container for setup
        std::unique_ptr<Ui_TaskOTAttacher> ui;
        bool use_current;        // Wheter we use current transformation as default attachemnt offset
        bool isButParFraChecked; // Record button state of parent frame
        typedef boost::signals2::connection Connection;
        Connection connectDelObject;
        Connection connectDelDocument;
    };
}