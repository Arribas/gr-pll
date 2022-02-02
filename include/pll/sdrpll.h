/* -*- c++ -*- */
/*
 * Copyright 2022 Javier Arribas.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_PLL_SDRPLL_H
#define INCLUDED_PLL_SDRPLL_H

#include <gnuradio/sync_block.h>
#include <pll/api.h>

namespace gr
{
namespace pll
{
/*!
 * \brief <+description of block+>
 * \ingroup pll
 *
 */
class PLL_API sdrpll : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<sdrpll> sptr;

    /*!
   * \brief Return a shared_ptr to a new instance of pll::sdrpll.
   *
   * To avoid accidental use of raw pointers, pll::sdrpll's
   * constructor is in a private implementation
   * class. pll::sdrpll::make is the public interface for
   * creating new instances.
   */
    static sptr make(float bw_hz, int detector_type, int filt_order, float period, float nco_base_freq, float k_d, float k_o,
        bool custom_params, float w0, float a2, float a3, float b3);
};

}  // namespace pll
}  // namespace gr

#endif /* INCLUDED_PLL_SDRPLL_H */
