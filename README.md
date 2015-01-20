# sneeze-detector

Sneeze detector use MFCC fetures to recognize sneezing

Steps:
1.  Preprocess audio files with nose and sneezing sounds
    1 .1. Find peaks
    1 .2. Find bounds of sounds threshold
    1.3. Normalize
2.  Post processing MFCC
    2.1 Extract MFCC features from noises sounds and sneezing
    2.2 Remove cross nearest features about 0.01 distance
3.1 Record audio from microphone and search nearest features distance
