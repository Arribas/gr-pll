/* -*- c++ -*- */

#define PLL_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "pll_swig_doc.i"

%{
#include "pll/sdrpll.h"
%}

%include "pll/sdrpll.h"
GR_SWIG_BLOCK_MAGIC2(pll, sdrpll);
