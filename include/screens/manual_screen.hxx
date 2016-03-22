#ifndef MANUAL_SCREEN_HXX
#define MANUAL_SCREEN_HXX

#include <memory>

#include "../events.hxx"
#include "../utility.hxx"
#include "../options.hxx"

namespace kin
{

class ManualScreen : public Widget
{
public:

    template <typename... Args>
    ManualScreen(
            Args... args
    )
        :
          Widget(args...),
          active_(0)
    {
        // Get the image filenames for the instructions. The kinect has 2 additional images.
        std::vector<std::string> filenames = {
            "images/instructions/instructions_0.jpg",
            "images/instructions/instructions_1.jpg",
            "images/instructions/instructions_2.jpg",
            "images/instructions/instructions_3.jpg",
            "images/instructions/instructions_4.jpg"
        };
        if (opts.use_kinect_)
        {
            filenames.push_back("images/instructions/instructions_5.jpg");
            filenames.push_back("images/instructions/instructions_6.jpg");
        }

        // Create the widgets.
        for (auto const & f : filenames)
        {
            auto w = std::make_shared<ImageWidget>(f);
            w->scale_ = ScaleInX;
            w->align_x_ = Center;
            w->set_height(0.7);
            w->set_y(0.05);
            w->hide();
            add_widget(w);
            instructions_.push_back(w);

            auto shadow = std::make_shared<ImageWidget>("images/instructions/frame.png");
            shadow->set_x(-0.0165);
            shadow->set_y(-0.0294);
            shadow->set_width(1.0428);
            shadow->set_height(1.0761);
            w->add_widget(shadow);
        }
        instructions_[active_]->show();

        // Create the grid for the controls.
        auto grid_ptr = std::make_shared<GridWidget>(3, 1);
        auto & grid = *grid_ptr;
        add_widget(grid_ptr);
        grid.set_height(0.15);
        grid.set_y(0.05);
        grid.align_y_ = Bottom;

        // Create the left arrow.
        auto arrow_left = std::make_shared<HoverclickWidget<ImageWidget> >("images/arrow_left.png");
        arrow_left->scale_ = ScaleInX;
        arrow_left->align_x_ = Center;
        attach_mouse_events(opts.mouse_, arrow_left);
        arrow_left->handle_click_ = [&](DiffType x, DiffType y){
            instructions_[active_]->hide();
            active_ -= 1; // --active_ yields compiler error in gcc 4.6
            if (active_ < 0)
                active_ += instructions_.size();
            instructions_[active_]->show();
        };
        grid(0) = arrow_left;

        // Create the back button.
        auto back_button = std::make_shared<HoverclickWidget<ImageWidget> >("images/back_button.png");
        back_button->scale_ = ScaleInX;
        back_button->align_x_ = Center;
        attach_mouse_events(opts.mouse_, back_button);
        back_button->handle_click_ = [&](DiffType x, DiffType y){
            event_manager.post(Event(Event::MainMenuScreen));
        };
        grid(1) = back_button;

        // Create the right arrow.
        auto arrow_right = std::make_shared<HoverclickWidget<ImageWidget> >("images/arrow_right.png");
        arrow_right->scale_ = ScaleInX;
        arrow_right->align_x_ = Center;
        attach_mouse_events(opts.mouse_, arrow_right);
        arrow_right->handle_click_ = [&](DiffType x, DiffType y){
            instructions_[active_]->hide();
            active_ += 1; // ++active_ yields compiler error in gcc 4.6
            if (active_ >= instructions_.size())
                active_ = 0;
            instructions_[active_]->show();
        };
        grid(2) = arrow_right;
    }

private:

    void decrement()
    {
        --active_;
    }
    void increment()
    {
        ++active_;
    }

    std::vector<std::shared_ptr<ImageWidget> > instructions_;
    int active_;
};

} // namespace kin

#endif
