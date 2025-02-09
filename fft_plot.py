import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# === PARAMETRI ===
filename = "dati.txt"  # Sostituisci con il tuo file
sampling_rate = 20  # Hz (dalla tua acquisizione)

# === CARICAMENTO DATI ===
data = pd.read_csv(filename, delimiter="\t")  # Separa i dati con tabulazione
#time = data.iloc[:, 0]  # Prima colonna: tempo
accel_z = data.iloc[:, 2]  # Quarta colonna: accelerazione Z

# === FFT ===
N = len(accel_z)  # Numero di campioni
frequencies = np.fft.fftfreq(N, 1/sampling_rate)  # Calcola le frequenze
fft_values = np.fft.fft(accel_z)  # Esegui la FFT
fft_magnitude = np.abs(fft_values) / N  # Normalizza

# === GRAFICI ===
plt.figure(figsize=(12, 5))

# Segnale originale
plt.subplot(2, 1, 1)
plt.plot(accel_z, label="Accelerazione Z", color='b')
plt.xlabel("Tempo (s)")
plt.ylabel("Accelerazione (g)")
plt.title("Segnale dell'Accelerometro")
plt.legend()
plt.grid()

# Spettro di frequenza (solo parte positiva)
plt.subplot(2, 1, 2)
plt.stem(frequencies[:N//2], fft_magnitude[:N//2], 'r', markerfmt=" ", basefmt="-r")
plt.xlabel("Frequenza (Hz)")
plt.ylabel("Ampiezza")
plt.title("Spettro di Frequenza (FFT)")
plt.grid()

plt.tight_layout()
plt.show()
