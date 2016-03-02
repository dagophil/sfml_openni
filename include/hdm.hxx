#ifndef HDM_HXX
#define HDM_HXX

#include <SFML/Graphics.hpp>

#include "widgets.hxx"

namespace kin
{

class HDMGame : public Widget
{
public:

    HDMGame(size_t width, size_t height);

private:

    std::shared_ptr<AnimatedWidget> mouse_; // the mouse widget

};

HDMGame::HDMGame(
        size_t width,
        size_t height
)   :
      Widget(0, 0, width, height, 0)
{
    // Load the mouse.
    mouse_ = std::make_shared<AnimatedWidget>(
                "animations/hand_load.pf",
                width, height,
                75, 75,
                999
    );
    mouse_->hoverable_ = false;
    mouse_->stop();
    add_widget(mouse_);
    handle_hover_ = [&](DiffType x, DiffType y)
    {
        auto old_x = mouse_->rect_.Left;
        auto old_y = mouse_->rect_.Top;
        auto w = mouse_->rect_.GetWidth();
        auto h = mouse_->rect_.GetHeight();
        mouse_->rect_.Offset(x-old_x - 0.45*w, y-old_y - 0.17*h);
    };
}



}

#endif
