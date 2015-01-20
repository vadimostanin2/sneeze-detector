# sneeze-detector
<b>
Sneeze detector use MFCC fetures to recognize sneezing
</b>
<p>
Steps: <p>
<b>
1.  Preprocess audio files with nose and sneezing sounds
</b>
<p>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1.1. Find peaks
</p>
<p>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1.2. Find bounds of sounds threshold
</p>
<p>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1.3. Normalize
</p>
<p>
<b>
2.  Post processing MFCC
</b>
<p>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2.1 Extract MFCC features from noises sounds and sneezing
</p>
<p>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2.2 Remove cross nearest features about 0.01 distance
</p>
<p>
<b>
3. Record audio from microphone and search nearest features distance
</b>
</p>
</p>
</p>
