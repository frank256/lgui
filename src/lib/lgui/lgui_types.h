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

#ifndef LGUI_TYPES_H
#define LGUI_TYPES_H

#include <algorithm>
#include <cmath>

namespace lgui {

enum Orientation {
    Vertical, Horizontal
};

class PointF;

/** A basic point type, int version. */
class Point {
    public:
        using Scalar = int;

        Point(Scalar x, Scalar y)
                : mx(x), my(y) {}
        Point()
                : mx(0), my(0) {}

        Scalar x() const { return mx; }
        Scalar y() const { return my; }
        void set_x(Scalar x) { mx = x; }
        void set_y(Scalar y) { my = y; }
        void set(Scalar x, Scalar y) {
            mx = x;
            my = y;
        }

        void operator+=(const Point& other) {
            mx += other.mx;
            my += other.my;
        }
        void operator-=(const Point& other) {
            mx -= other.mx;
            my -= other.my;
        }

        Point operator-() const {
            return {-mx, -my};
        }

        bool operator==(const Point& other) const {
            return (mx == other.mx) && (my == other.my);
        }
        bool operator!=(const Point& other) const {
            return !operator==(other);
        }

        Scalar lengthsq() const {
            return mx * mx + my * my;
        }

        friend inline Point operator+(const Point& a, const Point& b) {
            return {a.x() + b.x(), a.y() + b.y()};
        }

        friend inline Point operator-(const Point& a, const Point& b) {
            return {a.x() - b.x(), a.y() - b.y()};
        }

        explicit operator PointF() const;

    private:
        Scalar mx, my;
};

/** A basic point type, float version */
class PointF {
    public:
        using Scalar = float;

        PointF(Scalar x, Scalar y)
                : mx(x), my(y) {}
        PointF()
                : mx(0), my(0) {}

        Scalar x() const { return mx; }
        Scalar y() const { return my; }
        void set_x(Scalar x) { mx = x; }
        void set_y(Scalar y) { my = y; }
        void set(Scalar x, Scalar y) {
            mx = x;
            my = y;
        }

        Scalar lengthsq() const {
            return mx * mx + my * my;
        }

        void operator+=(const PointF& other) {
            mx += other.mx;
            my += other.my;
        }
        void operator-=(const PointF& other) {
            mx -= other.mx;
            my -= other.my;
        }

        PointF operator-() const {
            return {-mx, -my};
        }

        bool operator==(PointF other) const {
            return (mx == other.mx) && (my == other.my);
        }
        bool operator!=(PointF other) const {
            return !operator==(other);
        }

        void operator*=(float scalar) {
            mx *= scalar;
            my *= scalar;
        }

        friend inline PointF operator+(PointF a, PointF b) {
            return {a.x() + b.x(), a.y() + b.y()};
        }

        friend inline PointF operator-(PointF a, PointF b) {
            return {a.x() - b.x(), a.y() - b.y()};
        }

        friend PointF operator*(PointF p, float scalar) {
            return {p.mx * scalar, p.my * scalar};
        }

        friend PointF operator*(float scalar, PointF p) {
            return {p.mx * scalar, p.my * scalar};
        }

        Point to_point() const {
            return {Point::Scalar(mx), Point::Scalar(my)};
        }

        Point to_point_rounded() const {
            return {Point::Scalar(std::roundf(mx)), Point::Scalar(std::roundf(my))};
        }

    private:
        Scalar mx, my;
};

inline Point::operator PointF() const {
    return {PointF::Scalar(mx), PointF::Scalar(my)};
}

/** A basic size class. */
class Size {
    public:
        using Scalar = int;

        Size()
                : mw(0), mh(0) {}
        Size(Scalar w, Scalar h)
                : mw(w), mh(h) {}
        explicit Size(Point p)
                : mw(p.x()), mh(p.y()) {}

        Scalar w() const { return mw; }
        Scalar h() const { return mh; }

        void set_w(Scalar w) { mw = w; }
        void set_h(Scalar h) { mh = h; }
        void set(Scalar w, Scalar h) {
            mw = w;
            mh = h;
        }

        Point to_point() const {
            return Point{mw, mh};
        }

        bool operator==(const Size& other) const {
            return (mw == other.mw) && (mh == other.mh);
        }
        bool operator!=(const Size& other) const {
            return !operator==(other);
        }

        friend Size operator+(Size a, Size b) {
            return {a.mw + b.mw, a.mh + b.mh};
        }

    private:
        Scalar mw, mh;
};

/** A basic rectangle class. */
class Rect {
    public:
        using Scalar = int;
        using PointT = Point;
        using SizeT = Size;

        Rect()
                : mpos(0, 0), msize(0, 0) {}
        Rect(Scalar x, Scalar y, Scalar w, Scalar h)
                : mpos(x, y), msize(w, h) {}
        Rect(Scalar x, Scalar y, SizeT s)
                : mpos(x, y), msize(s) {}
        Rect(PointT p, Scalar w, Scalar h)
                : mpos(p), msize(w, h) {}
        Rect(PointT p, SizeT s)
                : mpos(p), msize(s) {}

        Scalar x() const { return mpos.x(); }
        Scalar y() const { return mpos.y(); }
        Scalar w() const { return msize.w(); }
        Scalar h() const { return msize.h(); }

        Scalar x1() const { return x(); }
        Scalar y1() const { return y(); }
        Scalar x2() const { return x() + msize.w() - 1; }
        Scalar y2() const { return y() + msize.h() - 1; }

        PointT pos() const { return mpos; }
        SizeT size() const { return msize; }

        bool contains(PointT p) const {
            return contains(p.x(), p.y());
        }

        bool contains(PointF p) const {
            return contains(p.x(), p.y());
        }

        bool contains(Scalar x, Scalar y) const {
            return (x >= mpos.x() && x < mpos.x() + msize.w()) &&
                   (y >= mpos.y() && y < mpos.y() + msize.h());
        }

        bool contains(float x, float y) const {
            return (x >= float(mpos.x()) && x < float(mpos.x()) + msize.w()) &&
                   (y >= float(mpos.y()) && y < float(mpos.y()) + msize.h());
        }

        bool overlaps(const Rect& other) const {
            if (x() >= other.x() + other.w())
                return false;
            if (y() >= other.y() + other.h())
                return false;
            if (x() + w() < other.x())
                return false;
            if (y() + h() < other.y())
                return false;
            return true;
        }

        void translate(Scalar x, Scalar y) {
            mpos.set(mpos.x() + x, mpos.y() + y);
        }

        void translate(PointT p) {
            mpos.set(mpos.x() + p.x(), mpos.y() + p.y());
        }

        Rect translated(PointT p) const {
            return Rect(mpos + p, msize);
        }

        Rect translated(Scalar x, Scalar y) const {
            return Rect(mpos.x() + x, mpos.y() + y, msize);
        }

        /** Ensure that the rect is within `frame`, cutting it off in all directions if it isn't. */
        void clip_to(const Rect& frame) {
            if (x() < frame.x()) {
                Scalar dx = frame.x() - x();
                set_pos_x(frame.x());
                set_width(std::max(w() - dx, 0));
            }
            if (y() < frame.y()) {
                Scalar dy = frame.y() - y();
                set_pos_y(frame.y());
                set_height(std::max(h() - dy, 0));
            }
            if (x2() > frame.x2())
                set_width(std::max(frame.x2() - x() + 1, 0));

            if (y2() > frame.y2())
                set_height(std::max(frame.y2() - y() + 1, 0));
        }

        /** Ensure that p is within the rectangle. */
        void clip_point(PointT& p) const {
            if (p.x() < x1())
                p.set_x(x1());
            else if (p.x() > x2())
                p.set_x(x2());
            if (p.y() < y1())
                p.set_y(y1());
            else if (p.y() > y2())
                p.set_y(y2());
        }

        /** Return the point clipped to the rectangle. */
        PointT clipped_point(PointT p) const {
            PointT r = p;
            clip_point(r);
            return r;
        }

        void set_pos(PointT pos) { mpos = pos; }
        void set_pos(Scalar x, Scalar y) { mpos = PointT(x, y); }
        void set_pos_x(Scalar x) { mpos.set_x(x); }
        void set_pos_y(Scalar y) { mpos.set_y(y); }
        void set_size(SizeT size) { msize = size; }
        void set_size(Scalar w, Scalar h) { msize = SizeT(w, h); }
        void set_width(Scalar w) { msize.set_w(w); }
        void set_height(Scalar h) { msize.set_h(h); }

        /** Return a rectangle that is grown exactly s pixels into all four directions. */
        Rect grown(Scalar s) const {
            return Rect(mpos.x() - s, mpos.y() - s, msize.w() + 2 * s, msize.h() + 2 * s);
        }

        /** Return a rectangle that is shrunk exactly s pixels into all four directions. */
        Rect shrunk(Scalar s) const {
            return Rect(mpos.x() + s, mpos.y() + s, msize.w() - 2 * s, msize.h() - 2 * s);
        }

        Point center() const {
            return mpos + Point(msize.w() / 2, msize.h() / 2);
        }

        bool operator==(const Rect& other) const {
            return mpos == other.mpos && msize == other.msize;
        }
        bool operator!=(const Rect& other) const {
            return !operator==(other);
        }

    private:
        PointT mpos;
        SizeT msize;
};

using Position = Point;
using PositionF = PointF;

/** A class representing basic padding in four directions. */
class Padding {
    public:
        Padding(int l, int t, int r, int b)
                : mleft(l), mtop(t), mright(r), mbottom(b) {}

        Padding(int horz, int vert)
                : mleft(horz), mtop(vert), mright(horz), mbottom(vert) {}

        explicit Padding(int everywhere)
                : mleft(everywhere), mtop(everywhere),
                  mright(everywhere), mbottom(everywhere) {}

        Padding()
                : mleft(0), mtop(0), mright(0), mbottom(0) {}

        int left() const { return mleft; }
        int top() const { return mtop; }
        int right() const { return mright; }
        int bottom() const { return mbottom; }

        Position left_top_offs() const {
            return {mleft, mtop};
        }

        int horz() const { return mleft + mright; }
        int vert() const { return mtop + mbottom; }

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

        void set(int l, int t, int r, int b) {
            mleft = l;
            mtop = t;
            mright = r;
            mbottom = b;
        }

        Size add(const Size& s) const {
            Size ret;
            ret.set_w(s.w() + mleft + mright);
            ret.set_h(s.h() + mtop + mbottom);
            return ret;
        }

        Size sub(const Size& s) const {
            Size ret;
            ret.set_w(s.w() - mleft - mright);
            ret.set_h(s.h() - mtop - mbottom);
            return ret;
        }

        Rect add(const Rect& r) const {
            return {r.x() - mleft, r.y() - mtop,
                    r.w() + mleft + mright, r.h() + mtop + mbottom};
        }

        Rect sub(const Rect& r) const {
            return {r.x() + mleft, r.y() + mtop,
                    r.w() - mleft - mright, r.h() - mtop - mbottom};
        }

        /** Return a new size that will be adjusted to use this padding
         *  instead of old padding */
        Size get_same_content_size(const Size& old_size,
                                   const Padding& old_padding) const {
            return add(old_padding.sub(old_size));
        }

        friend Size operator+(const Size& s, const Padding& p) {
            return {s.w() + p.horz(), s.h() + p.vert()};
        }

        friend Size operator+(const Padding& p, const Size& s) {
            return {s.w() + p.horz(), s.h() + p.vert()};
        }

        friend Size operator-(const Size& s, const Padding& p) {
            return {s.w() - p.horz(), s.h() - p.vert()};
        }

        friend Size operator-(const Padding& p, const Size& s) {
            return {s.w() - p.horz(), s.h() - p.vert()};
        }

    private:
        int mleft, mtop, mright, mbottom;
};

using Margin = Padding;

}


#endif // LGUI_TYPES_H
