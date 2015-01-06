#include "alsoundprocessing.h"
#include <alcommon/alproxy.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include <complex>
#include <valarray>

const double PI = 3.141592653589793238460;

#define M_PI 3.14159265358979323846
#define MAX_FRAME_LENGTH 8192

ALSoundProcessing::ALSoundProcessing(boost::shared_ptr<ALBroker> pBroker,
                                     std::string pName)
  : ALSoundExtractor(pBroker, pName)
{
  setModuleDescription("This module processes the data collected by the "
                       "microphones and output in the ALMemory the RMS power "
                       "of each of the four channels.");

}




typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

// Cooley–Tukey FFT (in-place)
void fftRosetta(CArray& x)
{
    const size_t N = x.size();
    if (N <= 1) return;

    // divide
    CArray even = x[std::slice(0, N/2, 2)];
    CArray  odd = x[std::slice(1, N/2, 2)];

    // conquer
    fftRosetta(even);
    fftRosetta(odd);

    // combine
    for (size_t k = 0; k < N/2; ++k)
    {
        Complex t = std::polar(1.0, -2 * PI * k / N) * odd[k];
        x[k    ] = even[k] + t;
        x[k+N/2] = even[k] - t;
    }
}



void fft(float *fftBuffer, long fftFrameSize, long sign)
/*
	FFT routine, (C)1996 S.M.Bernsee. Sign = -1 is FFT, 1 is iFFT (inverse)
	Fills fftBuffer[0...2*fftFrameSize-1] with the Fourier transform of the
	time domain data in fftBuffer[0...2*fftFrameSize-1]. The FFT array takes
	and returns the cosine and sine parts in an interleaved manner, ie.
	fftBuffer[0] = cosPart[0], fftBuffer[1] = sinPart[0], asf. fftFrameSize
	must be a power of 2. It expects a complex input signal (see footnote 2),
	ie. when working with 'common' audio signals our input signal has to be
	passed as {in[0],0.,in[1],0.,in[2],0.,...} asf. In that case, the transform
	of the frequencies of interest is in fftBuffer[0...fftFrameSize].
*/
{
	float wr, wi, arg, *p1, *p2, temp;
	float tr, ti, ur, ui, *p1r, *p1i, *p2r, *p2i;
	long i, bitm, j, le, le2, k;
    std::cout<<"Starting: "<<std::endl;
	for (i = 2; i < 2*fftFrameSize-2; i += 2) {
		for (bitm = 2, j = 0; bitm < 2*fftFrameSize; bitm <<= 1) {
			if (i & bitm) j++;
			j <<= 1;
		}
		if (i < j) {
			p1 = fftBuffer+i; p2 = fftBuffer+j;
			temp = *p1; *(p1++) = *p2;
			*(p2++) = temp; temp = *p1;
			*p1 = *p2; *p2 = temp;
		}
	}
	std::cout<<"First loop completed. Next loop: "<<std::endl;
	for (k = 0, le = 2; k < (long)(log(fftFrameSize)/log(2.)+.5); k++) {
		le <<= 1;
		le2 = le>>1;
		ur = 1.0;
		ui = 0.0;
		arg = M_PI / (le2>>1);
		wr = cos(arg);
		wi = sign*sin(arg);
		for (j = 0; j < le2; j += 2) {
			p1r = fftBuffer+j; p1i = p1r+1;
			p2r = p1r+le2; p2i = p2r+1;
			for (i = j; i < 2*fftFrameSize; i += le) {
				tr = *p2r * ur - *p2i * ui;
				ti = *p2r * ui + *p2i * ur;
				*p2r = *p1r - tr; *p2i = *p1i - ti;
				*p1r += tr; *p1i += ti;
				p1r += le; p1i += le;
				p2r += le; p2i += le;
			}
			tr = ur*wr - ui*wi;
			ui = ur*wi + ui*wr;
			ur = tr;
		}
	}
	std::cout<<"FFT completed. "<<std::endl;

}

void ALSoundProcessing::init()
{
  fALMemoryKeys.push_back("ALSoundProcessing/leftMicEnergy");
  fALMemoryKeys.push_back("ALSoundProcessing/rightMicEnergy");
  fALMemoryKeys.push_back("ALSoundProcessing/frontMicEnergy");
  fALMemoryKeys.push_back("ALSoundProcessing/rearMicEnergy");
  fALMemoryKeys.push_back("ALSoundProcessing/numChannels");
  fALMemoryKeys.push_back("ALSoundProcessing/samplesPerChannel");
  fALMemoryKeys.push_back("ALSoundProcessing/timestamp");

  static const float arr[] = {0.0,0.0,0.0,0.0};
  std::vector<int> vec (arr, arr + sizeof(arr) / sizeof(arr[0]) );
  fProxyToALMemory.insertData(fALMemoryKeys[0],vec);
  fProxyToALMemory.insertData(fALMemoryKeys[1], vec);
  fProxyToALMemory.insertData(fALMemoryKeys[2], vec);
  fProxyToALMemory.insertData(fALMemoryKeys[3], vec);


  audioDevice->callVoid("setClientPreferences",
                        getName(),                //Name of this module
                        48000,                    //48000 Hz requested
                        (int)ALLCHANNELS,         //4 Channels requested
                        1                         //Deinterleaving requested
                        );

  startDetection();
}

ALSoundProcessing::~ALSoundProcessing()
{
  stopDetection();
}


/// Description: The name of this method should not be modified as this
/// method is automatically called by the AudioDevice Module.
void ALSoundProcessing::process(const int & nbOfChannels,
                                const int & nbOfSamplesByChannel,
                                const AL_SOUND_FORMAT * buffer,
                                const ALValue & timeStamp)
{
  /// Computation of the RMS power of the signal delivered by
  /// each microphone on a 170ms buffer
  /// init RMS power to 0
  std::vector<float> fMicsEnergy;
  std::vector<std::vector<float> > fMicsValues;
  static const float arr[] = {0.0,0.0,0.0,0.0};
  for(int i=0 ; i<nbOfChannels ; i++)
  {
    std::vector<float> vec (arr, arr + sizeof(arr) / sizeof(arr[0]) );
    fMicsValues.push_back(vec);
  }


  std::cout<<"num samples per channel: "<<nbOfSamplesByChannel<<" - which is: "<< 2*nbOfSamplesByChannel*2 <<" bytes"<<std::endl;
  float *fftBuffer = (float *)malloc(4*nbOfSamplesByChannel*2);

  std::cout<<"fftBuffer is len: "<<sizeof(fftBuffer)<<std::endl;
  std::cout<<"buffer is len: "<<sizeof(buffer)<<std::endl;
  for(int channelInd = 0 ; channelInd < 1 ; channelInd++)
  {
    std::cout<<"starting for channel "<<channelInd<<std::endl;
    for(int i = 0 ; i < nbOfSamplesByChannel ; i++)
    {
      fMicsValues[channelInd].push_back((float)buffer[nbOfSamplesByChannel*channelInd + i]);
      fftBuffer[i*2] = (float)buffer[nbOfSamplesByChannel*channelInd + i];
      //std::cout<<"Inserted: ["<< i*2 <<"] ="<< (float)buffer[nbOfSamplesByChannel*channelInd + i]<<std::endl;
      fftBuffer[i*2+1] = 0.0;
      //std::cout<<"Inserted: ["<< i*2+1 << "] =" << 0.0 <<std::endl;
    }
    std::cout << "About to fft" << std::endl;

    //fft(fftBuffer,nbOfSamplesByChannel*2,-1);
    fftRosetta(fftBuffer);
    //fProxyToALMemory.insertData(fALMemoryKeys[channelInd],fMicsValues[channelInd]);

  }
  for (int i = 0; i < nbOfSamplesByChannel; i++)
    {
        float t = sqrt(fftBuffer[i*2] + fftBuffer[i*2] + fftBuffer[i*2+1] + fftBuffer[i*2+1]);
        //std::cout<<"["<<i<<"] = "<<t<<std::endl;
    }
  free(fftBuffer);
  //fProxyToALMemory.insertData(fALMemoryKeys[4], nbOfChannels);
  //fProxyToALMemory.insertData(fALMemoryKeys[5], nbOfSamplesByChannel);
  //fProxyToALMemory.insertData(fALMemoryKeys[6], timeStamp);


//  /// Calculation of the RMS power
//  for(int channelInd = 0 ; channelInd < nbOfChannels ; channelInd++)
//  {
//    for(int i = 0 ; i < nbOfSamplesByChannel ; i++)
//    {
//      fMicsEnergy[channelInd] += (float)buffer[nbOfSamplesByChannel*channelInd + i]
//                                 *(float)buffer[nbOfSamplesByChannel*channelInd + i];
//    }
//    fMicsEnergy[channelInd] /= (float)nbOfSamplesByChannel;
//    fMicsEnergy[channelInd] = sqrtf(fMicsEnergy[channelInd]);
//  }

  /// Puts the result in ALMemory
  /// (for example to be easily retrieved by another module)
//  for(int i=0 ; i<nbOfChannels ; i++)
//  {
//    fProxyToALMemory.insertData(fALMemoryKeys[i],fMicsEnergy[i]);
//  }

  //fProxyToALMemory.insertData(fALMemoryKeys[0], *buffer);
  /// Displays the results on the Naoqi console
  //std::cout << "ALSoundProcessing Timestamp  = " <<  timeStamp << std::endl;
  // std::cout << "ALSoundProcessing Energy Right  = " << fMicsEnergy[1] << std::endl;
  // std::cout << "ALSoundProcessing Energy Front  = " << fMicsEnergy[2] << std::endl;
  // std::cout << "ALSoundProcessing Energy Rear  = " <<  fMicsEnergy[3] << std::endl;
}
