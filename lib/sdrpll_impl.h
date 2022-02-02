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

#ifndef INCLUDED_PLL_SDRPLL_IMPL_H
#define INCLUDED_PLL_SDRPLL_IMPL_H

#include <pll/sdrpll.h>

namespace gr
{
namespace pll
{
class sdrpll_impl : public sdrpll
{
private:
    // FLL + PLL filter parameters
    float d_pll_w{0.0};
    float d_pll_w0p3{0.0};
    float d_pll_w0f2{0.0};
    float d_pll_x{0.0};
    float d_pll_a2{0.0};
    float d_pll_w0f{0.0};
    float d_pll_a3{0.0};
    float d_pll_w0p2{0.0};
    float d_pll_b3{0.0};
    float d_pll_w0p{0.0};
    int d_order{0};
    float d_period_s;
    float d_hz_to_nco_phase_step_rads_per_sample;

    float d_nco_base_freq_hz;
    float d_k_o;
    float d_k_d;
    int d_detector_type;

    float d_phase_rad;
    float mod_2pi(float in);
    void phase_wrap();
    float phase_detector_rads(gr_complex sample, float ref_phase_rads, int type);
    float get_carrier_error_cycles(float PLL_discriminator_cycles);

public:
    sdrpll_impl(float bw_hz, int detector_type, int filt_order, float period, float nco_base_freq, float k_d, float k_o, bool custom_params, float w0, float a2, float a3, float b3);
    ~sdrpll_impl();

    // Where all the action really happens
    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items);
};

}  // namespace pll
}  // namespace gr

#endif /* INCLUDED_PLL_SDRPLL_IMPL_H */
