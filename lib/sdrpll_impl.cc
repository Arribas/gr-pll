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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sdrpll_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <gnuradio/sincos.h>
#include <math.h>

namespace gr
{
namespace pll
{
sdrpll::sptr sdrpll::make(float bw_hz, int detector_type, int filt_order, float period,
    float nco_base_freq, float k_d, float k_o,
    bool custom_params, float w0, float a2, float a3,
    float b3)
{
    return gnuradio::get_initial_sptr(new sdrpll_impl(
        bw_hz, detector_type, filt_order, period, nco_base_freq, k_d, k_o, custom_params, w0, a2, a3, b3));
}

/*
 * The private constructor
 */
sdrpll_impl::sdrpll_impl(float bw_hz, int detector_type, int filt_order, float period,
    float nco_base_freq, float k_d, float k_o, bool custom_params, float w0, float a2, float a3,
    float b3)
    : gr::sync_block("sdrpll", gr::io_signature::make(1, 1, sizeof(gr_complex)),
          gr::io_signature::make(1, 1, sizeof(gr_complex)))
{
    /*
     * Filter design (Kaplan 2nd ed., Pag. 181 Fig. 181)
     */
    d_order = filt_order;
    d_period_s = period;
    d_hz_to_nco_phase_step_rads_per_sample = GR_M_TWOPI * d_period_s;
    std::cout << "order: " << filt_order << ", detector: " << detector_type << ", period: " << period << ", bw: " << bw_hz << "\n";
    d_phase_rad = 0;
    d_k_o = k_o;
    d_k_d = k_d;
    d_nco_base_freq_hz = nco_base_freq;
    d_detector_type = detector_type;
    if (custom_params == false)
        {
            if (d_order == 3)
                {
                    /*
             *  3rd order PLL
             */
                    d_pll_b3 = 2.400;
                    d_pll_a3 = 1.100;
                    d_pll_a2 = 1.414;
                    d_pll_w0p = bw_hz / 0.7845F;
                    d_pll_w0p2 = d_pll_w0p * d_pll_w0p;
                    d_pll_w0p3 = d_pll_w0p2 * d_pll_w0p;
                }
            else
                {
                    /*
             * 2nd order PLL
             */
                    d_pll_a2 = 1.414;
                    d_pll_w0p = bw_hz / 0.53F;
                    d_pll_w0p2 = d_pll_w0p * d_pll_w0p;
                }
        }
    else
        {
            if (d_order == 3)
                {
                    /*
         *  3rd order PLL
         */
                    d_pll_b3 = b3;
                    d_pll_a3 = a3;
                    d_pll_a2 = a2;
                    d_pll_w0p = w0;
                    d_pll_w0p2 = d_pll_w0p * d_pll_w0p;
                    d_pll_w0p3 = d_pll_w0p2 * d_pll_w0p;
                }
            else
                {
                    /*
         * 2nd order PLL
         */
                    d_pll_a2 = a2;
                    d_pll_w0p = w0;
                    d_pll_w0p2 = d_pll_w0p * d_pll_w0p;
                }
        }
}

/*
 * Our virtual destructor.
 */
sdrpll_impl::~sdrpll_impl() {}

void sdrpll_impl::phase_wrap()
{
    while (d_phase_rad > GR_M_TWOPI)
        d_phase_rad -= GR_M_TWOPI;
    while (d_phase_rad < -GR_M_TWOPI)
        d_phase_rad += GR_M_TWOPI;
}


float sdrpll_impl::phase_detector_rads(gr_complex sample, int type)
{
    float sample_phase_rads;
    switch (type)
        {
        case 1:  //four quadrant arctan
            sample_phase_rads = gr::fast_atan2f(sample.imag(), sample.real());
            break;
        case 2:  //Two quadrant arctan (Costas Loop)
            sample_phase_rads = std::atan(sample.imag() / sample.real());
            break;
        default:
            sample_phase_rads = gr::fast_atan2f(sample.imag(), sample.real());
        }
    return sample_phase_rads;
}


float sdrpll_impl::get_carrier_error_cycles(float PLL_discriminator_cycles)
{
    float carrier_error_hz;
    float pll_w_new;
    switch (d_order)
        {
        case 3:

            /*
             * 3rd order PLL (2nd order filter)
             */
            d_pll_w = d_pll_w + d_period_s * (d_pll_w0p3 * PLL_discriminator_cycles);
            d_pll_x = d_pll_x + d_period_s * (0.5F * d_pll_w + d_pll_a3 * d_pll_w0p2 * PLL_discriminator_cycles);
            carrier_error_hz = 0.5F * d_pll_x + d_pll_b3 * d_pll_w0p * PLL_discriminator_cycles;
            break;
        case 2:

            /*
             * 2nd order PLL (first order filter)
             */
            pll_w_new = d_pll_w + PLL_discriminator_cycles * d_pll_w0p2 * d_period_s;
            carrier_error_hz = 0.5F * (pll_w_new + d_pll_w) + d_pll_a2 * d_pll_w0p * PLL_discriminator_cycles;
            d_pll_w = pll_w_new;
            break;
        case 1:  //1sr order PLL (no filter)
            carrier_error_hz = PLL_discriminator_cycles;
            break;
        default:
            carrier_error_hz = PLL_discriminator_cycles;
        }

    return carrier_error_hz;
}

int sdrpll_impl::work(int noutput_items, gr_vector_const_void_star &input_items,
    gr_vector_void_star &output_items)
{
    const gr_complex *iptr = (const gr_complex *)input_items[0];
    gr_complex *optr = (gr_complex *)output_items[0];


    float pll_discriminator_rads;
    float pll_filter_out_hz;
    float t_imag, t_real;
    gr_complex mix_out;
    int size = noutput_items;

    while (size-- > 0)
        {
            //NCO
            gr::sincosf(d_phase_rad, &t_imag, &t_real);
            *optr = gr_complex(t_real, t_imag);
            //MIXER
            mix_out = *iptr * gr_complex(t_real, -t_imag);

            //PHASE DETECTOR
            pll_discriminator_rads = d_k_d * phase_detector_rads(mix_out, d_detector_type);

            //LOOP FILTER
            pll_filter_out_hz = get_carrier_error_cycles(pll_discriminator_rads / GR_M_TWOPI);  //estimates error in Hz

            //NCO UPDATE
            d_phase_rad += d_hz_to_nco_phase_step_rads_per_sample * (d_nco_base_freq_hz + d_k_o * pll_filter_out_hz);
            phase_wrap();

            //NEXT SAMPLE
            iptr++;
            optr++;
        }
    return noutput_items;
}

} /* namespace pll */
} /* namespace gr */
