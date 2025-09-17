import numpy as np
import matplotlib.pyplot as plt

# Parámetros
T = 5e-3          # Periodo del pulso (5 ms)
fs = 20000        # Frecuencia de muestreo (20 kHz)
Nperiodos = 4     # Número de periodos a mostrar
duty = 0.5        # Triangular simétrico

N = int(Nperiodos * T * fs)
t = np.arange(N) / fs

def tri_periodic(t, T, duty=0.5):
    tt = np.mod(t, T)
    y = np.zeros_like(t, dtype=float)
    mask_rise = tt < duty * T
    y[mask_rise] = tt[mask_rise] / (duty * T)
    y[~mask_rise] = (T - tt[~mask_rise]) / ((1.0 - duty) * T)
    return y

x = tri_periodic(t, T, duty)

win = np.hanning(len(x))
X = np.fft.rfft(x * win)
f = np.fft.rfftfreq(len(x), d=1/fs)
mag = np.abs(X) / len(x)

# Señal en el tiempo
plt.figure()
plt.plot(t * 1e3, x)
plt.title("Tren de pulsos triangulares")
plt.xlabel("Tiempo [ms]")
plt.ylabel("Amplitud")
plt.xlim(0, Nperiodos * T * 1e3)
plt.grid(True)

# Espectro en frecuencia
plt.figure()
markerline, stemlines, baseline = plt.stem(f, mag)
plt.setp(stemlines, linewidth=0.8)
plt.setp(markerline, markersize=3)
plt.title("Espectro de magnitud (FFT)")
plt.xlabel("Frecuencia [Hz]")
plt.ylabel("|X(f)|")
plt.xlim(0, 5000)
plt.grid(True)

plt.savefig("punto3_espectro_fft.png", dpi=200)
plt.figure(1)
plt.savefig("punto3_tiempo.png", dpi=200)

plt.show()
