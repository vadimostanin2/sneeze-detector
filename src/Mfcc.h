/* Header file for Mfcc.cpp. Adapted for C++ by Tlacael Esparza
 * from original libMFCC
*/

/*
 * libmfcc.h - Header for libMFCC
 * Copyright (c) 2010 Jeremy Sawruk
 *
 * This code is released under the MIT License. 
 * For conditions of distribution and use, see the license in LICENSE
 */

#pragma once
#define PI 3.14159265358979323846264338327

namespace std {

class Mfcc {
    /* params     */
    int samplingRate;
    int numFilters;
    int binSize;
    int numCoeffs;
    float ** filterParams;
    float * normFactors;
    float * innerSum;
    /* functions */
    // Compute the normalization 
    float _normalizationFactor(int m);
    // Compute the filter parameter for the specified frequency and filter bands
    float _getFilterParameter(int frequencyBand, 
                                int filterBand);
    // Compute the band-dependent magnitude factor for the given filter band
    float _getMagnitudeFactor(int filterBand);
    // Compute the center frequency (fc) of the specified filter band (l)
    float _getCenterFrequency(int filterBand);
public:
    Mfcc(void);
    void init(int SamplingRate,
                int NumFilters,
                int BinSize,
                int NumCoeffs);
    ~Mfcc(void);
    // Returns the first m MFCCs
    void getCoefficients(float* spectralData, float*mfccs);
};

} //namespace
