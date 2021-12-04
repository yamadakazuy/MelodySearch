'''
Created on 2021/12/04

@author: Sin Shimozono
'''
### Imports ###
import sys
import math
import time
import soundfile as sf
import IPython.display as ipd
import matplotlib.pyplot as plt
import numpy as np
from scipy import signal as sg

### For estimating the pitch based on harmonic summation model ###
def get_fundamental_freq(wavdata, Fs, f0_min, f0_max, resolution, L_max):
    
    #Parameter check
    N = len(wavdata)
    if f0_min < Fs / N:
        print('The f_min should be more than the frequency resolution(Fs / N).')
        sys.exit()
    if f0_max > Fs:
        print('The f_max should be less than sampling rate Fs.')
        sys.exit()
    
    #Define the range of frequency
    f_list = np.arange(f0_min, f0_max, resolution)
    
    #Generate the z-vector and harmonic summation list
    z = np.arange(0, N)
    Harmonic_sum = []
    Z_list = []
    
    #Repeat for f one by one
    for i in range(len(f_list)):
        
        #Construct the zc and zs matrix
        zc = []
        zs = []
        for L in range(1, L_max + 1):
            zc.append(np.cos(2*np.pi*f_list[i]*L*z/Fs))
            zs.append(np.sin(2*np.pi*f_list[i]*L*z/Fs))
        
        #Stack the zc and zs to construct Z-matrix
        zc = np.array(zc).T
        zs = np.array(zs).T
        Z = np.concatenate([zc, zs], axis=1) #row=N,column=2L
        
        #Compute the harmonic summation
        Zx = Z.T @ wavdata
        Harmonic_sum.append(Zx.T @ Zx)
        Z_list.append(Z)
    
    #Search for argument max (maximize the harmonic summation value)
    Harmonic_sum = np.array(Harmonic_sum)
    arg_i = np.argmax(Harmonic_sum)
    
    #Fundamental frequency
    arg_f = f_list[arg_i]
    #Maximum value
    Harmonic_max = Harmonic_sum[arg_i]
    #Z(0) N x 2L matrix
    Z0 = Z_list[arg_i]
    
    return arg_f, Harmonic_max, Z0



if __name__ == "__main__":
    
    #Set up
    segTime = 0.5             #Segment time (seconds)
    overlap_ratio = 0.5       #Overlap ratio of segments (0 < overlap < 1)
    f0_min, f0_max = 110, 350 #Range of fundamental frequency (Hz)
    f_delta = 1               #Resolution of frequency (Hz)
    L_max = 3                 #Degree of Harmonic summation (integer)
    
    #Read the sound data
    #[wavdata, Fs] = read_wav_file('./data/record1.wav')
    wavdata, Fs = sf.read('./data/record1.wav')
    #[wavdata, Fs] = generate_sinwave(f0=440) #for test use
    
    #Crop the audio
    wavdata = wavdata[round(0.5*Fs) : len(wavdata)-round(0.5*Fs)]
    wavLen = len(wavdata)
    ipd.display(ipd.Audio(data=wavdata, rate=Fs))
    
    #Calculate the parameter
    segLen = round(segTime * Fs)
    N_seg = math.floor((wavLen - overlap_ratio * segLen) / ((1-overlap_ratio) * segLen))
    stride = math.floor((1-overlap_ratio) * segLen)
    time_vector = (segLen/2 + stride * np.arange(0, N_seg)) / Fs
    
    #Do the analysis
    f0_vector = np.zeros((N_seg)) #F0 is defined as a vector
    for i in range(N_seg):
        #Measure time
        start = time.time()
        
        #Trim the wave-data into each segment
        crop_wav = wavdata[round(i*stride) : round(i*stride + segLen)] #Crop the wave-data for each segment
        [arg_f, Harmonic_max, Z0] = get_fundamental_freq(crop_wav, Fs, f0_min, f0_max, f_delta, L_max) #Call my function
        f0_vector[i] = arg_f #Divide the sampling rate (Fs) by the tau to get fundamental frequency
        
        #Report time
        print('Time={:.2f}sec: F0 estimates={:.1f}Hz, Process_time={:.1f}sec'.format((i*stride+segLen/2)/Fs, f0_vector[i], time.time() - start))
        
        #Preserve the second segment for plotting
        if i == round(N_seg)/2:
            X = crop_wav
            Z = Z0
    
    #Construct the Harmonic model S from Z
    S1 = Z.T @ X
    S2 = np.linalg.inv(Z.T @ Z)
    S = Z @ S2 @ S1
    
    #Plot the results
    plot_wavs(X, S, Fs, xmin=0, xmax=segTime/10)
    plot_STFT(wavdata, Fs, time_vector, f0_vector, f0_min, f0_max, win_size=segLen, win_overlap=segLen*0.75)
    print('Averaged F0 estimates: {:.1f}Hz'.format(np.average(f0_vector)))

    #IPythonライブラリを使ってjupyter notebook上に再生バーを表示
    print('Original Sound')
    #ipd.display(ipd.Audio(data=wavdata, rate=Fs))
    
    #横軸用の時間を作っておく
    time = np.arange(0, len(wavdata))/Fs
    
    #matplotというライブラリを使って音源の波形グラフを表示
    plt.figure(figsize=(16, 5))
    plt.plot(time, wavdata)
    plt.title('Wave data of the original music', fontsize=12)
    plt.xlabel('Time [sec]')
    plt.ylabel('Amplitude')
    plt.xlim(0, 5)
    plt.show()
    
    #短時間フーリエ変換(STFT)を実行
    #窓関数の種類はハニング窓,窓の大きさは20msec,窓の重なり幅は10mecで設定
    F, T, Adft = sg.stft(wavdata, fs=Fs, window='hann', nperseg=0.02*Fs, noverlap=0.01*Fs)
    
    #振幅をdBに変換
    P = 10 * np.log(np.abs(Adft))
    
    #STFTのスペクトログラムを表示
    plt.figure(figsize=(16, 5))
    plt.title('Spectrogram of the original music', fontsize=12)
    plt.xlabel('Time [sec]')
    plt.ylabel('Frequency [kHz]')
    plt.xlim(0, 5)
    plt.pcolormesh(T, F/1000, P, cmap = 'rainbow')
    plt.colorbar(orientation='horizontal').set_label('Amplitude [dB]')
    plt.show()
    
    #自己定義関数でReverbを付加したサウンドを生成する
    reverb_wavdata = Schroeder_Reverb(wavdata, Fs)
    
    #IPythonライブラリを使ってリバーブ付加後の波形グラフを表示
    print('Reverbed sound')
    #ipd.display(ipd.Audio(data=reverb_wavdata, rate=Fs))
    
    #matplotというライブラリを使って音源の波形グラフを表示
    plt.figure(figsize=(16, 5))
    plt.plot(time, reverb_wavdata)
    plt.title('Wave data of the reverbed music', fontsize=12)
    plt.xlabel('Time [sec]')
    plt.ylabel('Amplitude')
    plt.xlim(0, 5)
    plt.show()
    
    #短時間フーリエ変換(STFT)を実行
    #窓関数の種類はハニング窓,窓の大きさは20msec,窓の重なり幅は10mecで設定
    F2, T2, Adft2 = sg.stft(reverb_wavdata, fs=Fs, window='hann', nperseg=0.02*Fs, noverlap=0.01*Fs)
    
    #振幅をdBに変換
    P2 = 10 * np.log(np.abs(Adft2))
    
    #STFTのスペクトログラムを表示
    plt.figure(figsize=(16, 5))
    plt.title('Spectrogram of the reverbed music', fontsize=12)
    plt.xlabel('Time [sec]')
    plt.ylabel('Frequency [kHz]')
    plt.xlim(0, 5)
    plt.pcolormesh(T2, F2/1000, P2, cmap = 'rainbow')
    plt.colorbar(orientation='horizontal').set_label('Amplitude [dB]')
    plt.show()
    