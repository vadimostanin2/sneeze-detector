/*
 * =====================================================================================
 *
 *       Filename:  Mfcc.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/14/2014 15:56:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Tlacael Esparza, 
 *           Note:  Code adapted to C++ from libMFCC. see below
 *   Organization:  
 *
 * =====================================================================================
 */

/*
 * libmfcc.c - Code implementation for libMFCC
 * Copyright (c) 2010 Jeremy Sawruk
 *
 * This code is released under the MIT License. 
 * For conditions of distribution and use, see the license in LICENSE
 */

#include "Mfcc.h"
#include <math.h>
#include <cstring>
#include <iostream>

namespace std {

Mfcc::Mfcc(void){
    samplingRate = 0;
    numFilters = 0;
    binSize = 0;
    numCoeffs = 0;
    filterParams = 0;
    normFactors = 0;
    innerSum = 0;
}

void Mfcc::init(int SamplingRate,
                 int NumFilters,
                 int BinSize,
                 int NumCoeffs) {
    samplingRate = SamplingRate;
    numFilters = NumFilters;
    binSize = BinSize;
    numCoeffs = NumCoeffs;
    int i,l,k;

    filterParams = new float*[numFilters];
    for (l=0;l<numFilters;l++){
        filterParams[l] = new float[binSize];
    }
    normFactors = new float[numCoeffs];
    innerSum = new float[numFilters];

    // 0 <= m < L
    if(numCoeffs >= numFilters) {
        numCoeffs = numFilters - 1; // force m to good case
    }

   
    for (i=0; i<numCoeffs; i++) {
        normFactors[i] = _normalizationFactor(i);
    }

    // get filter paramters
    for (l=0; l < numFilters; l++) {
        for (k=0; k<binSize-1; k++) {
            filterParams[l][k] = _getFilterParameter(k,l+1);
        }
    }


}

Mfcc::~Mfcc(void) {
    if (filterParams != 0){
        for (int l=0;l<numFilters;l++){
            delete(filterParams[l]);
        }
        delete(filterParams);
    }
    if (normFactors != 0){
        delete(normFactors);
    }
    if (innerSum != 0){
        delete(innerSum);
    }
}

// Returns the first m MFCCs
void Mfcc::getCoefficients(float* spectralData, float* mfccs) {
    int k, l, m;
    memset(mfccs, 0, sizeof(float)*numCoeffs);

    for (l=0;l<numFilters;l++) {
        innerSum[l] = 0.0f;
        for (k=0;k<binSize-1;k++) {
            innerSum[l] += fabs(spectralData[k]) * filterParams[l][k];
        }
        for (m=0;m<numCoeffs;m++) {
            mfccs[m] += innerSum[l] * cos(((m*PI) / numFilters)* (l+1-0.5f));
            
        }
    }
    for (m=0;m<numCoeffs;m++) {
        mfccs[m] *= normFactors[m];
    }
    return;
}

// Compute the normalization 
float Mfcc::_normalizationFactor(int m) {
    float normalizationFactor = 0.0f;
    if(m == 0) {
        normalizationFactor = sqrt(1.0f / numFilters);
    }
    else {
        normalizationFactor = sqrt(2.0f / numFilters);
    }

    return normalizationFactor;
}

// Compute the filter parameter for the specified frequency and filter bands
float Mfcc::_getFilterParameter(int frequencyBand, 
                                int filterBand)    {
    float filterParameter = 0.0f;

    float boundary = (frequencyBand * samplingRate) / binSize;// k * Fs / N
    float prevCenterFrequency = _getCenterFrequency(filterBand - 1);// fc(l - 1) etc.
    float thisCenterFrequency = _getCenterFrequency(filterBand);
    float nextCenterFrequency = _getCenterFrequency(filterBand + 1);

    if(boundary >= 0 && boundary < prevCenterFrequency) {
        filterParameter = 0.0f;
    }
    else if(boundary >= prevCenterFrequency && boundary < thisCenterFrequency) {
        filterParameter = (boundary - prevCenterFrequency) / 
                        (thisCenterFrequency - prevCenterFrequency);
        filterParameter *= _getMagnitudeFactor(filterBand);
    }
    else if(boundary >= thisCenterFrequency && boundary < nextCenterFrequency) {
        filterParameter = (boundary - nextCenterFrequency) / 
                          (thisCenterFrequency - nextCenterFrequency);
        filterParameter *= _getMagnitudeFactor(filterBand);
    }
    else if(boundary >= nextCenterFrequency && boundary < samplingRate) {
        filterParameter = 0.0f;
    }

    return filterParameter;
}
// Compute the band-dependent magnitude factor for the given filter band
float Mfcc::_getMagnitudeFactor(int filterBand) {
    float magnitudeFactor = 0.0f;

    if(filterBand >= 1 && filterBand <= 14) {
        magnitudeFactor = 0.015;
    }
    else if(filterBand >= 15 && filterBand <= 48) {
        magnitudeFactor = 2.0f / (_getCenterFrequency(filterBand + 1) - \
                _getCenterFrequency(filterBand -1));
    }

    return magnitudeFactor;
}
// Compute the center frequency (fc) of the specified filter band (l)
float Mfcc::_getCenterFrequency(int filterBand) {
    float centerFrequency = 0.0f;
    float exponent;

    if(filterBand == 0){
        centerFrequency = 0;
    }
    else if(filterBand >= 1 && filterBand <= 14){
        centerFrequency = (200.0f * filterBand) / 3.0f;
    }
    else {
        exponent = filterBand - 14.0f;
        centerFrequency = pow(1.0711703, exponent);
        centerFrequency *= 1073.4;
    }
    return centerFrequency;
}


} // namespace