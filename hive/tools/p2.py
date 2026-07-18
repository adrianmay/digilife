#!/usr/bin/python3

import numpy as np
import matplotlib.pyplot as plt

# Load the data file
data = np.loadtxt("o2")

# Extract columns
x = data[:, 0]
y1 = data[:, 1]
y2 = data[:, 2]
y3 = data[:, 3]
y4 = data[:, 4]
y5 = data[:, 5]

# Plot
plt.scatter(x, y1, s=1, label="Mobs")
plt.scatter(x, y2, s=1, label="Msgs")
plt.scatter(x, y3, s=1, label="R0")
plt.scatter(x, y4, s=1, label="R1")
plt.scatter(x, y5, s=1, label="Cash")

plt.xlabel("Iteration")
plt.ylabel("Count")
plt.legend()
plt.grid(True)

plt.show()
