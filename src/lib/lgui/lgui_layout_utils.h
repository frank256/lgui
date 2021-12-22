/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015-22 frank256
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

#ifndef LGUI_LAYOUT_UTILS_H
#define LGUI_LAYOUT_UTILS_H

#include "lgui_layout_types.h"

namespace lgui {

/** Return a position to align an object of dimension `size` within `space` according to `align`.  */
Position do_alignment(const Rect& space, const Size& size, Align align);

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

}

#endif // LGUI_LAYOUT_UTILS_H
