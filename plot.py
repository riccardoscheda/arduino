import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv("dati2.txt", delimiter="\t")
df.columns = ["a", "b", "c", "d", "e", "f", "g", "h", "i", "l"]
df = df[1000:]
alpha = 0.95
df = df.reset_index()
# Inizializza i dati filtrati per i tre assi
filteredX = np.zeros_like(df["a"])
filteredY = np.zeros_like(df["b"])
filteredZ = np.zeros_like(df["c"])

# Imposta il primo valore
filteredX[0] = df["a"][0]
filteredY[0] = df["b"][0]
filteredZ[0] = df["c"][0]

# Applica il filtro passa-basso
for i in range(1, len(df)):
    filteredX[i] = alpha * filteredX[i-1] + (1 - alpha) * df["a"][i]
    filteredY[i] = alpha * filteredY[i-1] + (1 - alpha) * df["b"][i]
    filteredZ[i] = alpha * filteredZ[i-1] + (1 - alpha) * df["c"][i]

# Crea una figura con sei subplot (tre righe, due colonne)
fig, axs = plt.subplots(3, 2, figsize=(12, 8))

# Titoli per le colonne
axs[0, 0].set_title("Raw Data")
axs[0, 1].set_title("Filtered Data")

# Lista degli assi e delle etichette
axes = ["X", "Y", "Z"]
raw_data = [df["a"], df["b"], df["c"]]
filtered_data = [filteredX, filteredY, filteredZ]
colors = ['b', 'g', 'r']

# Popoliamo i subplot
for i in range(3):
    # Dati originali
    axs[i, 0].plot(raw_data[i], label=f"Raw {axes[i]}", color=colors[i])
    axs[i, 0].grid()
    axs[i, 0].set_ylim(-2, 2)
    axs[i, 0].legend()

    # Dati filtrati
    axs[i, 1].plot(filtered_data[i], label=f"Filtered {axes[i]}", color=colors[i])
    axs[i, 1].grid()
    axs[i, 1].set_ylim(-2, 2)
    axs[i, 1].legend()

# Layout per evitare sovrapposizioni
plt.tight_layout()
plt.savefig("comparison_axes.png")
plt.show()

