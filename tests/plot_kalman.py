from matplotlib import pyplot as plt
import numpy as np
import pandas as pd
import os

file = open("./tests/TestingData/kalman_output.txt", "r")
data = file.readlines()
file.close()

pd_data = pd.DataFrame([x.strip().split("\t") for x in data[1:]], columns=["Iteration", "Real X", "Noisy X", "Estimated X", "Real V", "Estimated V"])
pd_data = pd_data.astype(float)

plt.figure()
plt.plot(pd_data["Iteration"], pd_data["Real X"], label="Real X", color="g")
plt.plot(pd_data["Iteration"], pd_data["Noisy X"], label="Noisy X", color="r", linestyle="dotted")
plt.plot(pd_data["Iteration"], pd_data["Estimated X"], label="Estimated X", color="b", linestyle="dashed")
plt.xlabel("Iteration")
plt.ylabel("Position")
plt.title("Kalman Filter Position Estimation")
plt.legend()
plt.grid()
plt.show()