/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-19 frank256
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

#ifndef LGUI_H
#define LGUI_H

namespace lgui {

enum Orientation { Vertical, Horizontal };


/** A basic point type. */
class Point {
    public:
        Point(int x, int y)
            : mx(x), my(y) {}
        Point()
            : mx(0), my(0) {}

        int x() const { return mx; }
        int y() const { return my; }
        void set_x(int x) { mx = x; }
        void set_y(int y) { my = y; }
        void set(int x, int y) { mx = x; my = y; }

        void operator+=(const Point& other) {
            mx+= other.mx;
            my+= other.my;
        }
        void operator-=(const Point& other) {
            mx-=other.mx;
            my-=other.my;
        }

        Point operator-() const {
            return Point(-mx, -my);
        }

        bool operator==(const Point& other) const
        {
            return (mx == other.mx) && (my == other.my);
        }
        bool operator!=(const Point& other) const
        {
            return !operator==(other);
        }

        int lengthsq()
        {
            return mx*mx+my*my;
        }

        friend inline Point operator+(const Point& a, const Point& b){
            return Point(a.x()+b.x(), a.y()+b.y());
        }

        friend inline Point operator-(const Point& a, const Point& b){
            return Point(a.x()-b.x(), a.y()-b.y());
        }

    private:
        int mx, my;
};


using Position = Point;

class Padding;

/** A basic size class. */
class Size {
    public:
        Size()
            : mw(0), mh(0) {}
        Size(int w, int h)
            : mw(w), mh(h) {}

        int w() const { return mw; }
        int h() const { return mh; }

        void set_w(int w) { mw = w; }
        void set_h(int h) { mh = h; }
        void set(int w, int h) { mw = w; mh = h; }

        bool operator==(const Size& other) const
        {
            return (mw == other.mw) && (mh == other.mh);
        }
        bool operator!=(const Size& other) const
        {
            return !operator==(other);
        }

        void operator+=(const Padding& p);
        void operator-=(const Padding& p);

    private:
        int mw, mh;
};

/** A basic rectangle class. */
class Rect
{
    public:
        Rect()
            : mpos(0, 0), msize(0, 0)
        {}
        Rect(int x, int y, int w, int h)
            : mpos(x, y), msize(w, h)
        {}
        Rect(int x, int y, Size s)
            : mpos(x, y), msize(s)
        {}
        Rect(Position p, int w, int h)
            : mpos(p), msize(w, h)
        {}
        Rect(Position p, Size s)
            : mpos(p), msize(s)
        {}

        int x() const { return mpos.x(); }
        int y() const { return mpos.y(); }
        int w() const { return msize.w(); }
        int h() const { return msize.h(); }

        int x1() const { return x(); }
        int y1() const { return y(); }
        int x2() const { return x()+msize.w()-1; }
        int y2() const { return y()+msize.h()-1; }

        Position pos() const { return mpos; }
        Size size() const { return msize; }

        bool contains(Position p)  const
        {
            return contains(p.x(), p.y());
        }

        bool contains(int x, int y)  const
        {
            return (x >= mpos.x() && x < mpos.x()+msize.w()) &&
                   (y >= mpos.y() && y < mpos.y()+msize.h());
        }

        bool overlaps(const Rect& other) const {
            if(x() >= other.x()+other.w()) return false;
            if(y() >= other.y()+other.h()) return false;
            if(x()+w() < other.x()) return false;
            if(y()+h() < other.y()) return false;
            return true;
        }

        void translate(int x, int y)
        {
            mpos.set(mpos.x()+x, mpos.y()+y);
        }

        void translate(Position p) {
            mpos.set(mpos.x()+p.x(), mpos.y()+p.y());
        }

        Rect translated(Position p) const {
            return Rect(mpos + p, msize);
        }

        Rect translated(int x, int y) const {
            return Rect(mpos.x() + x, mpos.y() + y, msize);
        }

        /** Ensure that the rect is within `frame`, cutting it off in all directions if it isn't. */
        void clip_to(const Rect& frame);

        /** Ensure that p is within the rectangle. */
        void clip_point(Point& p) const;

        void set_pos(Position pos) { mpos = pos; }
        void set_pos(int x, int y) { mpos = Position(x, y); }
        void set_pos_x(int x) { mpos.set_x(x); }
        void set_pos_y(int y) { mpos.set_y(y); }
        void set_size(Size size) { msize = size; }
        void set_size(int w, int h) { msize = Size(w, h); }
        void set_width(int w) { msize.set_w(w); }
        void set_height(int h) { msize.set_h(h); }

        /** Return a rectangle that is grown exactly s pixels into all four directions. */
        Rect grown(int s) const {
            return Rect(mpos.x()-s, mpos.y()-s, msize.w()+2*s, msize.h()+2*s);
        }

        /** Return a rectangle that is shrunk exactly s pixels into all four directions. */
        Rect shrunk(int s) const {
            return Rect(mpos.x()+s, mpos.y()+s, msize.w()-2*s, msize.h()-2*s);
        }

        bool operator==(const Rect& other) const
        {
            return mpos == other.mpos && msize == other.msize;
        }
        bool operator!=(const Rect& other) const
        {
            return !operator==(other);
        }

    private:
        Position mpos;
        Size msize;
};

/** A class representing alignment in X and Y directions.
 *  Apart from others, this is used by LayoutItem behind the scenes. Behavior will vary depending on the layout used.
 *  Note that HStrech and VStretch are different from HTakeAll and VTakeAll: the TakeAll variants
 *  should lead to the LayoutItem greedily grabbing all the space it is offered by the layout, but not necessarily
 *  distributing it to the actual child layout element. The Stretch variants, however, make the LayoutItem distribute
 *  all the space assigned by the layout to the child element, but do not make the item greedily request all space it can
 *  get. Concrete usage depends on the layout (e.g. whether space is allocated more in a bottom-up or top-down way).
 * */
class Alignment {
    public:
        using align_bits_t = unsigned int;

        enum {
            Default = 0u, /**< no alignment specified; might trigger default alignment depending on layout and settings */
            Left = 1u,    /**< align left horizontally */
            Right = 2u,   /**< align right horizontally */
            HStretch = 3u, /**< stretch to fill horizontal space assigned by the layout; applicability depends on
                                 * the layout */
            HCenter = 4u, /**< center horizontally */
            HTakeAll=8u,  /**< greedily grab all horizontal space offered by the layout; applicability depends on the layout */

            Top = 16u,    /**< align top vertically */
            Bottom = 32u, /**< align bottom vertically */
            VStretch = 48u, /**< stretch to fill vertical space assigned by the layout; applicability depends on the
                                  *  layout; */
            VCenter = 64u, /**< center vertically */
            VTakeAll=128u, /**< greedily grab all vertical space offered by the layout; applicability depends on the layout */

            HVCenter = HCenter | VCenter,

            HVTakeAll = HTakeAll | VTakeAll,
            HVStretch = HStretch | VStretch,

            HMask = HCenter | HStretch | HTakeAll ,
            VMask = VCenter | VStretch | VTakeAll
        };

        Alignment(align_bits_t align)
            : malign(align) {}

        Alignment() : malign(Default) {}

        /** Return as bitfield. */
        align_bits_t align() const { return malign; }

        align_bits_t horz() const { return malign & HMask; }
        align_bits_t vert() const { return malign & VMask; }

        bool is(align_bits_t align) const { return (malign & align) == align; }

        bool operator==(const Alignment& other) const {
            return malign == other.malign;
        }

        bool operator!=(const Alignment& other) const {
            return malign != other.malign;
        }

    private:
        align_bits_t malign;
};

using Align = Alignment;

enum SizeConstraintMode { Maximum = 0, NoLimits = 1<<31, Exactly = 1 <<30 };

/** Represents a size constraint that is passed to measure().
 *  Each %SizeConstraint describes only one dimension. It consists of
 *  a value and a mode. The mode describes how the value is understood.
 *  Typically, it will be a maximum size.
 */
class SizeConstraint {
    public:
        explicit SizeConstraint(int value, SizeConstraintMode mode)
            : mvalue(mode == NoLimits ? mode : (value <= 0 ? 0 : value) | mode)
        {}

        SizeConstraint() : mvalue(0) {}

        int value() const { return mvalue & VALUE_MASK ; }

        SizeConstraintMode mode() const {
            return SizeConstraintMode(mvalue & (~VALUE_MASK));
        }

        SizeConstraint add(int value) const {
            return SizeConstraint(this->value() + value, mode());
        }

        SizeConstraint sub(int value) const {
            return SizeConstraint(this->value() - value, mode());
        }

        SizeConstraint adapted_for_child(int space_used=0) const;

        static inline SizeConstraint no_limits()
        {
            return SizeConstraint(0, SizeConstraintMode::NoLimits);
        }

        bool operator==(const SizeConstraint& other) const {
            return mvalue == other.mvalue;
        }

        bool operator!=(const SizeConstraint& other) const {
            return mvalue != other.mvalue;
        }

    private:
        static const int VALUE_MASK = (1<<30)-1;

        int mvalue;
};

/** Helper class to return the result of a measure operation for one dimension.
 *  Basically, this is a size (width or height) combined with an optional flag
 *  that indicates whether the constraint passed was found to be insufficient. */
class MeasureResult {
    public:
        MeasureResult()
            : mvalue(0)
        {}

        MeasureResult(int value, bool too_small=false)
            : mvalue(too_small ? (value | TOO_SMALL) : value & MASK)
        {}

        int value() const { return mvalue & MASK; }
        bool is_too_small() const { return (mvalue & TOO_SMALL) > 0; }

        void set(int value, int too_small=false) {
            mvalue = too_small ? (value | TOO_SMALL) : value & MASK;
        }

        void set_too_small(bool too_small=true) {
            if (too_small)
                mvalue = mvalue | TOO_SMALL;
            else
                mvalue &= MASK;
        }

    private:
        int mvalue;
        static const int TOO_SMALL = 1<<31,
                         MASK = ~TOO_SMALL;
 };

/** Helper class to return the result of a measure operation for both dimensions. */
class MeasureResults {
    public:
        MeasureResults()
            : mwidth(), mheight()
        {}

        MeasureResults(int w, int h)
            : mwidth(w), mheight(h)
        {}

        MeasureResults(Size s)
            : mwidth(s.w()), mheight(s.h()) {}

        MeasureResults(int w, int h, bool w_too_small, bool h_too_small)
            : mwidth(w, w_too_small), mheight(h, h_too_small)
        {}

        MeasureResults(MeasureResult w, MeasureResult h)
            : mwidth(w), mheight(h)
        {}

        int w() const { return mwidth.value(); }
        int h() const { return mheight.value();; }

        Size to_size() const {
            return Size(mwidth.value(), mheight.value());
        }

        operator Size() const
        {
            return to_size();
        }

        bool is_width_too_small() const {
            return mwidth.is_too_small();
        }

        bool is_height_too_small() const {
            return mheight.is_too_small();
        }

        void set_width(int w, bool w_too_small=false) {
            mwidth.set(w, w_too_small);
        }

        void set_height(int h, bool h_too_small=false) {
            mheight.set(h, h_too_small);
        }

    private:
        MeasureResult mwidth, mheight;
};

/** Helper class to accumulate the too-small bits of different MeasureResults. */
class TooSmallAccumulator {
    public:
        TooSmallAccumulator()
            : mdata(0) {}

        TooSmallAccumulator(const MeasureResults& mr)
            : mdata(0)
        {
            or_bits(mr);
        }

        TooSmallAccumulator(const MeasureResults& mr1,
                            const MeasureResults& mr2)
            : mdata(0)
        {
            or_bits(mr1);
            or_bits(mr2);
        }

        Size consider(const MeasureResults& mr) {
            or_bits(mr);
            return mr.to_size();
        }

        int consider_width(MeasureResult w) {
            mdata |= w.is_too_small() ? 1 : 0;
            return w.value();
        }

        int consider_height(MeasureResult h) {
            mdata |= h.is_too_small() ? 2 : 0;
            return h.value();
        }

        bool width_too_small() const { return (mdata & 1) > 0; }
        bool height_too_small() const { return (mdata & 2) > 0; }

    private:
        void or_bits(const MeasureResults& mr)
        {
            mdata |= mr.is_width_too_small() ? 1 : 0;
            mdata |= mr.is_height_too_small() ? 2 : 0;
        }
        int mdata;
};

/** A class representing basic padding in four directions. */
class Padding {
    public:
        Padding(int l, int t, int r, int b)
            : mleft(l), mtop(t), mright(r), mbottom(b)
        {}

        Padding(int horz, int vert)
            : mleft(horz), mtop(vert), mright(horz), mbottom(vert)
        {}

        explicit Padding(int everywhere)
            : mleft(everywhere), mtop(everywhere),
              mright(everywhere), mbottom(everywhere)
        {}

        Padding()
            : mleft(0), mtop(0), mright(0), mbottom(0)
        {}


        int left() const { return mleft; }
        int top() const { return mtop; }
        int right() const { return mright; }
        int bottom() const { return mbottom; }

        Position left_top_offs() const {
            return Position(mleft, mtop);
        }

        int horz() const { return mleft+mright; }
        int vert() const { return mtop+mbottom; }

        void set_left(int l) { mleft = l; }
        void set_top(int t) { mtop = t; }
        void set_right(int r) { mright = r; }
        void set_bottom(int b) { mbottom = b; }

        void set_horz(int horz) {
            mleft = mright = horz;
        }
        void set_vert(int vert) {
            mtop = mbottom = vert;
        }

        void set(int everywhere) {
            mleft = mtop = mright = mbottom = everywhere;
        }

        void set(int horz, int vert) {
            mleft = mright = horz;
            mtop = mbottom = vert;
        }

        void set(int l, int t, int r, int b)
        {
            mleft = l; mtop = t; mright = r; mbottom = b;
        }

        Size add(const Size& s) const
        {
            Size ret;
            ret.set_w(s.w()+mleft+mright);
            ret.set_h(s.h()+mtop+mbottom);
            return ret;
        }

        MeasureResults add(const MeasureResults& mrs) const
        {
            return MeasureResults(mrs.w() + horz(), mrs.h() + vert(),
                                  mrs.is_width_too_small(), mrs.is_height_too_small());
        }

        Size sub(const Size& s)  const
        {
            Size ret;
            ret.set_w(s.w()-mleft-mright);
            ret.set_h(s.h()-mtop-mbottom);
            return ret;
        }

        Rect add(const Rect& r) const {
            return Rect(r.x()-mleft, r.y()-mtop,
                        r.w()+mleft+mright, r.h()+mtop+mbottom);
        }

        Rect sub(const Rect& r) const {
            return Rect(r.x()+mleft, r.y()+mtop,
                        r.w()-mleft-mright, r.h()-mtop-mbottom);
        }

        /** Return a new size that will be adjusted to use this padding
         *  instead of old padding */
        Size get_same_content_size(const Size& old_size,
                                   const Padding& old_padding) const {
            return add(old_padding.sub(old_size));
        }

        friend Size operator+(const Size& s, const Padding& p) {
            return Size(s.w() + p.horz(), s.h() + p.vert());
        }

        friend Size operator+(const Padding& p, const Size& s) {
            return Size(s.w() + p.horz(), s.h() + p.vert());
        }

        friend Size operator-(const Size& s, const Padding& p) {
            return Size(s.w() - p.horz(), s.h() - p.vert());
        }

        friend Size operator-(const Padding& p, const Size& s) {
            return Size(s.w() - p.horz(), s.h() - p.vert());
        }

    private:
        int mleft, mtop, mright, mbottom;
};

using Margin = Padding;

inline void Size::operator+=(const Padding& p) {
    mw += p.horz();
    mh += p.vert();
}

inline void Size::operator-=(const Padding& p) {
    mw -= p.horz();
    mh -= p.vert();
}


/** Force size (being a width or height) to respect the size constraint sc.
 *  @return a MeasureResult with the too_small flag set if the size exceeded the space allowed by the constraint.
*/
MeasureResult force_size_constraint(int size, SizeConstraint sc);

/** Force size (being a width or height) to respect the size constraint sc. The too small flag of the result
 *  will always be set (i.e. overriden through logical OR) if too_small_accum is true.
 *  @return a MeasureResult with the too_small flag set if the size exceeded the space allowed by the
 *          constraint or if the flag passed in was set
*/
MeasureResult force_size_constraint(int size, SizeConstraint sc, bool too_small_accum);

/** Force r (being a width or height) to respect the size constraint sc. This overload will preserve the
 *  too-small flag of the measure result passed as argument.
 *  @return a MeasureResult with the too_small flag set if the size exceeded the space allowed by the
 *          constraint or if the too-small flag of the argument was set
*/
MeasureResult force_size_constraint(MeasureResult r, SizeConstraint sc);

/** Force s to respect the size constraints wc and hc.
 *  @return MeasureResults with the too_small flags set if the size exceeded the space allowed by the
 *          constraints.
*/
MeasureResults force_size_constraints(Size s, SizeConstraint wc, SizeConstraint hc);

/** Force s to respect the size constraints wc and hc. The too small flags of the result will always be set
 *  when the flag for the corresponding dimension is set in accum.
 *  @return MeasureResults with the too_small flags set if the size exceeded the space allowed by the
 *          constraints or if the flag for the corresponding dimension was passed in via accum
*/
MeasureResults force_size_constraints(Size s, SizeConstraint wc, SizeConstraint hc, TooSmallAccumulator accum);

/** Force s to respect the size constraints wc and hc. The too-small flags of will be preserved.
 *  @return MeasureResults with the too_small flags set if the size exceeded the space allowed by the
 *          constraints or if the flag for the corresponding dimension was already set in s.
*/
MeasureResults force_size_constraints(MeasureResults s, SizeConstraint wc, SizeConstraint hc);

/** Return a position to align an object of dimension `size` within `space` according to `align`.  */
Position do_alignment(const Rect& space, const Size& size, Align align);

}

#endif // LGUI_H

