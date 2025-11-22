import scipy.signal as sps
import numpy as np

def bandpass(sig, fs=250, low=0.5, high=45, order=4):
    ny = 0.5 * fs
    b, a = sps.butter(order, [low/ny, high/ny], btype='band')
    return sps.filtfilt(b, a, sig)
