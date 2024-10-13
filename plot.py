import pandas as pd
import matplotlib.pyplot as plt 
import numpy as np


df = pd.read_csv("Provadati.csv",delimiter="\t")
df.columns= ["a","b","c","d","e","f","g"]
print(df)
df = df[1320:1700]
#t = np.linspace(0,len(df))


plt.plot(df["a"])
plt.plot(df["b"])
plt.plot(df["c"])
plt.grid()
plt.savefig("acc.png")
plt.figure()
plt.plot(df["d"])
plt.plot(df["e"])
plt.plot(df["f"])
plt.grid()
plt.savefig("giroscopio.png")