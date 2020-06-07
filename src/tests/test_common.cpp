/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-20 frank256
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

#include "test_common.h"
#include "lgui/platform/graphics.h"
#include "lgui/drawevent.h"
#include "lgui/mouseevent.h"

const char *mstatic_text =
"VerbumValdeLongumVerbumLongiusVerbumLongissimumValdeLongum "
"Cum defensionum laboribus senatoriisque muneribus aut "
"omnino aut magna ex parte essem aliquando liberatus,\nrettuli me, Brute, te "
"hortante maxime ad ea studia, quae retenta animo, remissa temporibus, longo "
"intervallo intermissa revocavi, et cum omnium artium, quae ad rectam"
"vivendi viam pertinerent, ratio et disciplina studio sapientiae, quae "
"philosophia dicitur, contineretur, hoc mihi Latinis litteris inlustrandum "
"putavi, non quia philosophia Graecis et litteris et doctoribus percipi non "
"posset, sed meum semper iudicium fuit omnia nostros aut invenisse per se "
"sapientius quam Graecos aut accepta ab illis fecisse meliora, quae quidem "
"digna statuissent, in quibus elaborarent.";


void HeightSlider::resized(const lgui::Size& old_size) {
    lgui::VerticalSlider::resized(old_size);
    set_max_value(height());
}

void WidthSlider::resized(const lgui::Size& old_size) {
    lgui::HorizontalSlider::resized(old_size);
    set_max_value(width());
}



BlockButtonDragRepr::BlockButtonDragRepr(lgui::Widget& src_widget, const lgui::Position& hotspot, const std::string& content_descr, const lgui::Size& size)
    : lgui::DragRepresentation(src_widget, hotspot, content_descr)
{
    set_size(size);
}

void BlockButtonDragRepr::draw(lgui::Graphics& gfx) const
{
    lgui::Color col;
    if(has_target_been_reached())
        col = lgui::rgb(1.0, 1.0, 1.0);
    else
        col = lgui::rgb(0.0, 0.5, 0.5);
    gfx.filled_rect(0.5, 0.5, size().w()+0.5, size().h()+0.5, col);
}


BlockButton::BlockButton(const std::string& str, int dim)
    : mstr(str), mhome(nullptr), mdisable(false)
{
    int w = font().text_width(mstr)+6;
    set_size(w, w);
    mwant_w = dim;
}

lgui::MeasureResults BlockButton::measure(lgui::SizeConstraint wc, lgui::SizeConstraint hc)
{
    return lgui::force_size_constraints(lgui::Size(mwant_w, mwant_w), wc, hc);
}

void BlockButton::draw(const lgui::DrawEvent& de) const
{
    lgui::Color col;
    if(mdisable || de.draw_disabled())
        col = lgui::rgba_premult(0.0, 0.0, 0.5, de.opacity());
    else if(is_hovered() && is_down())
        col = lgui::rgba_premult(1.0, 1.0, 0.0, de.opacity());
    else if(is_down())
        col = lgui::rgba_premult(1.0, 0.0, 0.0, de.opacity());
    else if(is_hovered())
        col = lgui::rgba_premult(0.0, 1.0, 0.0, de.opacity());
    else
        col = lgui::rgba_premult(0.0, 0.0, 1.0, de.opacity());
    de.gfx().filled_rect(size_rect(), col);
    de.gfx().draw_text(font(), moffs.x(), moffs.y(),
                       lgui::rgb(1.0, 1.0, 1.0), mstr);
    de.gfx().draw_visible_pixel(mtracking_pos.x(), mtracking_pos.y(), lgui::rgb(0.0, 0.0, 1.0));
}

void BlockButton::mouse_pressed(lgui::MouseEvent& event)
{
    mpressed_pos = event.pos();
    mpressed_time = event.timestamp();
    AbstractButton::mouse_pressed(event);
}

void BlockButton::mouse_dragged(lgui::MouseEvent& event)
{
    lgui::Position delta = event.pos() - mpressed_pos;
    double time_delta = event.timestamp()-mpressed_time;
    if(delta.lengthsq() > 25 && time_delta < 0.5) {
        auto drgr = new BlockButtonDragRepr(*this, mpressed_pos, std::string("BlockButton - ") + mstr, size());
        event.spawn_drag(drgr);
        mdisable = true;
    }
    event.consume();
}

void BlockButton::drag_ended(lgui::DragDropEvent& event)
{
    (void)event;
    mdisable = false;
}

void BlockButton::resized(const lgui::Size& old_size) {
    (void) old_size;
    moffs.set((width()-font().text_width(mstr))/2,
              (height()-font().line_height())/2);
}

void BlockButton::mouse_moved(lgui::MouseEvent& event) {
    mtracking_pos = event.pos();
}

Message::Message(const std::string& msg, const std::string& bttext)
    : mlabel(msg), mbutton(bttext)
{
    mlayout.add_item({mlabel, lgui::Align::HCenter});
    mlayout.add_spacing(10);
    mlayout.add_item({mbutton, lgui::Align::HCenter});
    set_layout(&mlayout);
    mbutton.on_activated.connect(std::bind(&Message::   close_popup, this));
    set_focus_child(&mbutton);
}

void Message::set_msg(const std::string& msg) {
    mlabel.set_text(msg);
}

void Message::focus_gained(lgui::FocusEvent& event)
{
    (void)event;
    mbutton.focus();
}
