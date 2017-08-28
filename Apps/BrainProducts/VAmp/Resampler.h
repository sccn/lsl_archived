/*
Copyright (c) 2009, Motorola, Inc

All Rights Reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are
met:

* Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright 
notice, this list of conditions and the following disclaimer in the 
documentation and/or other materials provided with the distribution.

* Neither the name of Motorola nor the names of its contributors may be 
used to endorse or promote products derived from this software without 
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,  
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef RESAMPLER_H
#define RESAMPLER_H


#include <stdexcept>
#include <complex>
#include <vector>

using namespace std;

template<class S1, class S2, class C>
class Resampler {
public:
    typedef    S1 inputType;
    typedef    S2 outputType;
    typedef    C coefType;

    // create new resampler
    Resampler(int upRate, int downRate, const C *coefs, int coefCount, S1 *initConds=0,int t=0,int xoff=0);
    virtual ~Resampler();

	// resample a chunk of multi-channel samples
	void apply_multichannel(const vector<vector<S1> > &in, vector<vector<S2> > &out);

    // process input samples
    int        apply(S1* in, int inCount, S2* out, int outCount);
    // number of output samples produced for given input
    int        neededOutCount(int inCount);
    
    // length of initial/final conditions
    int        coefsPerPhase() const { return _coefsPerPhase; }
    
    // final conditions
    inputType *finalConds() const { return _state; }
    
    // pointer into fractional data stream
    int get_t() const { return _t; }
    int get_xoff() const {return _xOffset; }
    
//private:
	vector<vector<S1> > _temp_in,_temp_out;
	int _tempChannels;

    int        _upRate;
    int        _downRate;

    coefType   *_transposedCoefs; // reorganized coefficient matrix
    inputType  *_state;           // state vector (in case past data is needed for next sample)
    inputType  *_stateEnd;        // end ptr for state vector
    
    int        _paddedCoefCount;  // ceil(len(coefs)/upRate)*upRate
    int        _coefsPerPhase;    // _paddedCoefCount / upRate
    
    int        _t;                // "time" (modulo upRate)
    int        _xOffset;          // offset into the fractional data stream
    
};


#include <iostream>
#include <cmath>

/*
using std::cout;
using std::endl;

using std::fill;
using std::copy;
*/

using std::invalid_argument;

template<class S1, class S2, class C>
Resampler<S1, S2, C>::Resampler(int upRate, int downRate, const C *coefs,
                                int coefCount, S1 *initConds, int t, int xoff):
  _upRate(upRate), _downRate(downRate), _t(t), _xOffset(xoff)
/*
  The coefficients are copied into local storage in a transposed, flipped
  arrangement.  For example, suppose upRate is 3, and the input number
  of coefficients coefCount = 10, represented as h[0], ..., h[9].
  Then the internal buffer will look like this:
        h[9], h[6], h[3], h[0],   // flipped phase 0 coefs
           0, h[7], h[4], h[1],   // flipped phase 1 coefs (zero-padded)
           0, h[8], h[5], h[2],   // flipped phase 2 coefs (zero-padded)
*/
{
    _paddedCoefCount = coefCount;
    while (_paddedCoefCount % _upRate) {
        _paddedCoefCount++;
    }
    _coefsPerPhase = _paddedCoefCount / _upRate;

    /* initialize state */
    _state = new inputType[_coefsPerPhase - 1];
    _stateEnd = _state + _coefsPerPhase - 1;
    if (initConds) {
        /* if initial conditions have been specified, use them */
        copy(initConds,initConds + _coefsPerPhase - 1, _state);
    } else {
        /* otherwise assume that they are zero... */
        fill(_state, _stateEnd, 0.);
    }

    /* This both transposes, and "flips" each phase, while
     * copying the defined coefficients into local storage.
     * There is probably a faster way to do this
     */
    _transposedCoefs = new coefType[_paddedCoefCount];
    fill(_transposedCoefs, _transposedCoefs + _paddedCoefCount, 0.);
    for (int i=0; i<_upRate; ++i) {
        for (int j=0; j<_coefsPerPhase; ++j) {
            if (j*_upRate + i  < coefCount)
                _transposedCoefs[(_coefsPerPhase-1-j) + i*_coefsPerPhase] =
                                                coefs[j*_upRate + i];
        }
    }
}

template<class S1, class S2, class C>
Resampler<S1, S2, C>::~Resampler() {
    delete [] _transposedCoefs;
    delete [] _state;
}

template<class S1, class S2, class C>
int Resampler<S1, S2, C>::neededOutCount(int inCount)
/* compute how many outputs will be generated for inCount inputs  */
{
    int np = inCount * _upRate;
    int need = np / _downRate;
    if ((_t + _upRate * _xOffset) < (np % _downRate))
        need++;
    return need;
}

// resample a chunk of multi-channel samples
template<class S1, class S2, class C>
void Resampler<S1,S2,C>::apply_multichannel(const vector<vector<S1> > &in, vector<vector<S2> > &out) {
	int insamples = in.size();
	if (insamples > 0) {
		int outchannels = in[0].size();
		int outsamples = neededOutCount(insamples);

		// copy to transposed input buffer _temp_in
		_temp_in.resize(outchannels);
		for (int c=0,e=outchannels;c<e;c++) {
			_temp_in[c].resize(insamples);
			for (int s=0,e=insamples;s<e;s++)
				_temp_in[c][s] = in[s][c];
		}

		// apply and store in output buffer _temp_out
		_temp_out.resize(outchannels);
		for (int c=0,e=outchannels;c<e;c++) {
			_temp_out[c].resize(outsamples);
			apply(&_temp_in[c][0],insamples,&_temp_out[c][0],outsamples);
		}

		// transpose back and store in out
		out.resize(outsamples);
		for (int s=0,e=outsamples;s<e;s++) {
			out[s].resize(outchannels);
			for (int c=0,e=outchannels;c<e;c++)
				out[s][c] = _temp_out[c][s];
		}
	} else {
		out.resize(0);
	}
}


template<class S1, class S2, class C>
int Resampler<S1, S2, C>::apply(S1* in, int inCount, 
                                S2* out, int outCount) {
    if (outCount < neededOutCount(inCount)) 
        throw invalid_argument("Not enough output samples");

    inputType *x = in + _xOffset;     // points to the latest processed input sample
    outputType *y = out;              // points to the next output sample
    inputType *end = in + inCount;    // end of the input
    
    // as long as there is input...
    while (x < end) {
        // init accumulator
        outputType acc = 0.;
        // pointer to current filter phase
        coefType *h = _transposedCoefs + _t*_coefsPerPhase;
        // pointer from where we read (may actually be before the beginning of the signal)
        inputType *xPtr = x - _coefsPerPhase + 1;
        // offset relative to beginning of the signal
        int offset = in - xPtr;
        // if we need samples from the past... (note: sometimes we don't!)
        if (offset > 0) {
            // pointer into state
            inputType *statePtr = _stateEnd - offset;
            // as long as we need to read from state...
            while (statePtr < _stateEnd) {
                // do so.
                acc += *statePtr++ * *h++;
            }
            // done; now increment the data pointer appropriately
            xPtr += offset;
        }
        // as long as we have to catch up...
        while (xPtr <= x) {
            // do so
            acc += *xPtr++ * *h++;
        }
        // and write out the result
        *y++ = acc;
        // also, advance phase time
        _t += _downRate;
        // and advance our position in X
        int advanceAmount = _t / _upRate;
        x += advanceAmount;
        // do the modulo computation
        _t %= _upRate;
    }
    // finally, remember where we are (in the data)
    _xOffset = x - end;

    // manage _state buffer
    // find number of samples retained in buffer:
    int retain = (_coefsPerPhase - 1) - inCount;
    if (retain > 0) {
        // for inCount smaller than state buffer, copy end of buffer
        // to beginning:
        copy(_stateEnd - retain, _stateEnd, _state);
        // Then, copy the entire (short) input to end of buffer
        copy(in, end, _stateEnd - inCount);
    } else {
        // just copy last input samples into state buffer
        copy(end - (_coefsPerPhase - 1), end, _state);
    }
    // number of samples computed
    return y - out;
}

#endif
