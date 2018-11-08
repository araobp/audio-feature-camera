# Testing the oscilloscope

## Linear chirp spectrogram

Condition:
- Linear chirp
- 1.5sec length
- 50Hz(amplitude 0.8)-9000Hz(amplitude 0.1) sweep range
- generated by Audacity

#### Linear chirp spectrogram

![Linear chirp spectrogram (pre-emphasis enabled)](./calibration/chirp_spectrogram-32-128.png)

Note: two resonance components can be seen on the picture above.

#### Linear chirp mel-spectrogram

![Linear chirp mel-scale spectrogram (pre-emphasis enabled)](./calibration/chirp_mel_spectrogram-32-128.png)

# Calibrating MEMS mic and DFSDM

Condition: 7000 Hz sine wave is generated by a tone generator running on my PC, and the MEMS mic receives the tone via air.

## Measurement result

"System clock 80_000_000 / clock divider 32 / FOSR 128" outperformed System "clock 80_000_000 / clock divider 64 / FOSR 64": SNR is better at 7000Hz.

### System clock 80_000_000 / clock divider 64 / FOSR 64

#### Pre-emphasis disabled

![FFT pre-emphasis disabled](./calibration/fft-pre_emphasis_disabled-64-64.png)

#### Pre-emphasis enabled

![FFT pre-emphasis enabled](./calibration/fft-pre_emphasis_enabled-64-64.png)

![Spectrogram pre-emphasis enabled](./calibration/spectrogram-pre_emphasis_enabled-64-64.png)

![Mel-spectrogram pre-emphasis enabled](./calibration/mel_spectrogram-pre_emphasis_enabled-64-64.png)

### System clock 80_000_000 / clock divider 32 / FOSR 128

#### Pre-emphasis disabled

![FFT pre-emphasis disabled](./calibration/fft-pre_emphasis_disabled-32-128.png)

#### Pre-emphasis enabled

![FFT pre-emphasis enabled](./calibration/fft-pre_emphasis_enabled-32-128.png)

![Spectrogram pre-emphasis enabled](./calibration/spectrogram-pre_emphasis_enabled-32-128.png)

![Mel-spectrogram pre-emphasis enabled](./calibration/mel_spectrogram-pre_emphasis_enabled-32-128.png)