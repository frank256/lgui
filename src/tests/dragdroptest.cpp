/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015 frank256
*
* License (BSD):
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice, this
*    list of conditions and the following disclaimer in the documentation and/or
*    other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "dragdroptest.h"
#include "platform/graphics.h"
#include "lgui/drawevent.h"
#include "lgui/dragdropevent.h"
#include "lgui/style/style.h"

const char* DragDropTest::BUTTON_STRINGS[6] =
{
    "Veggies: Carrot",
    "Veggies: Pepper",
    "Veggies: Onion",
    "Fruit: Peach",
    "Fruit: Apple",
    "Fruit: Cherry"
};


DragDropTest::DragDropTest(const lgui::Font& font)
: mbc1("Fruit"),
  mbc2("Veggies"),
  mbc3("Veggies & Fruit"),
  mreset_button("Reset")
{
    mbc1.set_font(&font);
    mbc2.set_font(&font);
    mbc3.set_font(&font);

    int dim = 0;
    for(unsigned int i = 0; i < NBUTTONS; i++) {
        dim = std::max(font.text_width(BUTTON_STRINGS[i]), dim);
    }

    int maxw = 0;
    for(unsigned int i = 0; i < NBUTTONS; i++) {
        const char* init_str = BUTTON_STRINGS[i];
        mbb[i] = std::unique_ptr <BlockButton> (new BlockButton(init_str, dim));
        mbb[i]->set_font(&font);
        maxw = std::max(mbb[i]->measure(lgui::SizeConstraint(0, lgui::SizeConstraintMode::NoLimits),
        lgui::SizeConstraint(0, lgui::SizeConstraintMode::NoLimits)).w(), maxw);
        //maxw = std::max(mbb[i]->width(), maxw);
    }

    for(unsigned int i = 0; i < NBUTTONS; i++) {
        mbb[i]->set_size(maxw, maxw);
        mbc3.insert(*mbb[i].get());
        mbb[i]->on_activated_src.connect(show_message);
    }
    // 4, 8 <- FlowLayout spacing
    mbc1.set_static_size(lgui::Size(maxw*3+8, maxw));
    mbc2.set_static_size(lgui::Size(maxw*3+8, maxw));
    mbc3.set_static_size(lgui::Size(maxw*3+8, 2*maxw+4));


    mlayout.add(mbc1, 0);
    mlayout.add_spacing(40);
    mlayout.add(mbc2, 0);
    mlayout.add_spacing(40);
    mlayout.add(mbc3, 0);
    mlayout.add_spacing(40);
    mlayout.add(mreset_button);
    //mlayout.set_resize_target(true);
    set_layout(&mlayout);
    mreset_button.on_activated.connect([this]() -> void { reset(); });
}

void DragDropTest::reset() {
    for(unsigned int i = 0; i < NBUTTONS; i++) {
        mbb[i]->home()->remove(*mbb[i].get());
        mbc3.insert(*mbb[i].get());
    }
}


BlockContainer::BlockContainer(const std::string& pattern)
    : mpattern(pattern), mdrag_active(false),
      mline_color(lgui::rgb(1.0, 1.0, 1.0))
{
    set_padding(lgui::Padding(3, 3, 3, 3+3+font().line_height()));
    add_widget_listener(this);
    //            mlayout.set_resize_height_to_contents(true);
    set_layout(&mlayout);
    //mlayout.set_bottom_to_top(true);
    //mlayout.set_right_to_left(true);
    //mlayout.set_horizontally_centered(true);
}

void BlockContainer::draw(const lgui::DrawEvent& de) const
{
    lgui::Rect r = size_rect();
    if(mdrag_active) {
        de.gfx().filled_rect(r, lgui::col_mult_alpha(mline_color, 0.3*de.opacity()));
    }
    if(is_hovered()) {
        de.gfx().rect(r, lgui::rgba_premult(0.0, 1.0, 0.0, de.opacity()), 1);
    }
    else {
        de.gfx().rect(r, lgui::col_mult_alpha(mline_color, de.opacity()), 1);
    }
    de.gfx().draw_textr(font(), width()-4, height()-4-font().line_height(),
                        lgui::col_mult_alpha(mline_color, de.opacity()), mpattern);
    draw_children(de);
}

void BlockContainer::drag_entered(lgui::DragDropEvent& event)
{
    if(event.drag_representation().content_descr().find("BlockButton") != std::string::npos) {
        BlockButton* bt = static_cast <BlockButton*> (event.drag_representation().source_widget());
        if(bt->home() != this &&
                event.drag_representation().content_descr().find(mpattern) != std::string::npos) {
            event.set_accept_drag(true);
            mdrag_active = true;
        }
        event.consume();
    }

}

void BlockContainer::drag_left(lgui::DragDropEvent& event)
{
    mdrag_active = false;
    event.consume();
}

void BlockContainer::dropped(lgui::DragDropEvent& event)
{
    if(event.drag_representation().content_descr().find("BlockButton") != std::string::npos) {
        BlockButton* bt = static_cast <BlockButton*> (event.drag_representation().source_widget());
        ASSERT(bt->home());
        bt->home()->remove(*bt);
        mlayout.add_item(*bt);
        mdrag_active = false;
    }
    event.consume();
}

void BlockContainer::child_added_wl(lgui::Widget& w, lgui::Widget& child)
{
    (void) w;
    BlockButton& bt = static_cast <BlockButton&> (child);
    bt.set_home(this);
}

void BlockContainer::child_removed_wl(lgui::Widget& w, lgui::Widget& child)
{
    (void) w;
    BlockButton& bt = static_cast <BlockButton&> (child);
    bt.set_home(nullptr);
}

lgui::MeasureResults BlockContainer::measure(lgui::SizeConstraint wc, lgui::SizeConstraint hc)
{
    return lgui::force_size_constraints(mmy_size, wc, hc);
    /*lgui::MeasureResult mr = BasicContainer::measure(wc, hc);
            debug("\nMeasure BlockContainer %d, %d, -> %d,%d", wc.value(), hc.value(), mr.w(), mr.h());
            return mr;*/
}

void BlockContainer::set_static_size(const lgui::Size& s) {
    mmy_size = s + padding();
}

void BlockContainer::insert(BlockButton& bt) {
    mlayout.add_item(bt);
}

void BlockContainer::style_changed()
{
    lgui::PaddedContainer::style_changed();
    mline_color = style().button_text_color(lgui::WidgetState(false, false, false), 1.0);
}
