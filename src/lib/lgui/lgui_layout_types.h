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

#ifndef LGUI_LAYOUT_TYPES_H
#define LGUI_LAYOUT_TYPES_H

#include "lgui_types.h"

namespace lgui {

/** A class representing alignment in X and Y directions.
 *  Apart from others, this is used by LayoutItem behind the scenes. Behavior will vary depending on the layout used.
 *  Note that HStretch and VStretch are different from HTakeAll and VTakeAll: the TakeAll variants
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
            HTakeAll = 8u,  /**< greedily grab all horizontal space offered by the layout; applicability depends on the layout */

            Top = 16u,    /**< align top vertically */
            Bottom = 32u, /**< align bottom vertically */
            VStretch = 48u, /**< stretch to fill vertical space assigned by the layout; applicability depends on the
                                  *  layout; */
            VCenter = 64u, /**< center vertically */
            VTakeAll = 128u, /**< greedily grab all vertical space offered by the layout; applicability depends on the layout */

            HVCenter = HCenter | VCenter, /**< center horizontally and vertically */

            HVTakeAll = HTakeAll | VTakeAll,  /**< take all space offered both horizontally and vertically */
            HVStretch = HStretch | VStretch,  /**< stretch to fill assigned space both horizontally and vertically */

            HMask = HCenter | HStretch | HTakeAll, /**< bitmask to extract bits referring to horizontal alignment */
            VMask = VCenter | VStretch | VTakeAll   /**< bitmask to extract bits referring to vertical alignment */
        };

        Alignment(align_bits_t align)
                : malign(align) {}

        Alignment()
                : malign(Default) {}

        /** Return as bitfield. */
        align_bits_t align() const { return malign; }

        align_bits_t horz() const { return malign & HMask; }
        align_bits_t vert() const { return malign & VMask; }

        /** Return true if all bits are set. */
        bool is_set(align_bits_t align) const { return (malign & align) == align; }

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

enum SizeConstraintMode {
    Maximum = 0, NoLimits = 1 << 31, Exactly = 1 << 30
};

/** Represents a size constraint that is passed to measure().
 *  Each %SizeConstraint describes only one dimension. It consists of
 *  a value and a mode. The mode describes how the value is understood.
 *  Typically, it will be a maximum size.
 */
class SizeConstraint {
    public:
        explicit SizeConstraint(int value, SizeConstraintMode mode)
                : mvalue(mode == NoLimits ? mode : (value <= 0 ? 0 : value) | mode) {}

        SizeConstraint()
                : mvalue(0) {}

        int value() const { return mvalue & VALUE_MASK; }

        SizeConstraintMode mode() const {
            return SizeConstraintMode(mvalue & (~VALUE_MASK));
        }

        SizeConstraint add(int value) const {
            return SizeConstraint(this->value() + value, mode());
        }

        SizeConstraint sub(int value) const {
            return SizeConstraint(this->value() - value, mode());
        }

        SizeConstraint adapted_for_child(int space_used = 0) const;

        static inline SizeConstraint no_limits() {
            return SizeConstraint(0, SizeConstraintMode::NoLimits);
        }

        bool operator==(const SizeConstraint& other) const {
            return mvalue == other.mvalue;
        }

        bool operator!=(const SizeConstraint& other) const {
            return mvalue != other.mvalue;
        }

    private:
        static const int VALUE_MASK = (1 << 30) - 1;

        int mvalue;
};

/** Helper class to return the result of a measure operation for one dimension.
 *  Basically, this is a size (width or height) combined with an optional flag
 *  that indicates whether the constraint passed was found to be insufficient. */
class MeasureResult {
    public:
        MeasureResult()
                : mvalue(0) {}

        MeasureResult(int value, bool too_small = false)
                : mvalue(too_small ? ((value <= 0 ? 0 : value) | TOO_SMALL) : (value <= 0 ? 0 : value) & MASK) {}

        int value() const { return mvalue & MASK; }
        bool is_too_small() const { return (mvalue & TOO_SMALL) > 0; }

        void set(int value, int too_small = false) {
            mvalue = too_small ? (value | TOO_SMALL) : value & MASK;
        }

        void set_too_small(bool too_small = true) {
            if (too_small)
                mvalue = mvalue | TOO_SMALL;
            else
                mvalue &= MASK;
        }

    private:
        int mvalue;
        static const int TOO_SMALL = 1 << 31, MASK = ~TOO_SMALL;
};

/** Helper class to return the result of a measure operation for both dimensions. */
class MeasureResults {
    public:
        MeasureResults()
                : mwidth(), mheight() {}

        MeasureResults(int w, int h)
                : mwidth(w), mheight(h) {}

        MeasureResults(Size s)
                : mwidth(s.w()), mheight(s.h()) {}

        MeasureResults(int w, int h, bool w_too_small, bool h_too_small)
                : mwidth(w, w_too_small), mheight(h, h_too_small) {}

        MeasureResults(MeasureResult w, MeasureResult h)
                : mwidth(w), mheight(h) {}

        int w() const { return mwidth.value(); }
        int h() const { return mheight.value();; }

        Size to_size() const {
            return Size(mwidth.value(), mheight.value());
        }

        operator Size() const {
            return to_size();
        }

        bool is_width_too_small() const {
            return mwidth.is_too_small();
        }

        bool is_height_too_small() const {
            return mheight.is_too_small();
        }

        void set_width(int w, bool w_too_small = false) {
            mwidth.set(w, w_too_small);
        }

        void set_height(int h, bool h_too_small = false) {
            mheight.set(h, h_too_small);
        }

        MeasureResults add_padding(const Padding& padding) const {
            return {w() + padding.horz(), h() + padding.vert(),
                    is_width_too_small(), is_height_too_small()};
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
                : mdata(0) {
            or_bits(mr);
        }

        TooSmallAccumulator(const MeasureResults& mr1,
                            const MeasureResults& mr2)
                : mdata(0) {
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
        void or_bits(const MeasureResults& mr) {
            mdata |= mr.is_width_too_small() ? 1 : 0;
            mdata |= mr.is_height_too_small() ? 2 : 0;
        }
        int mdata;
};

}

#endif // LGUI_LAYOUT_TYPES_H
