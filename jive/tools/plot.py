#!/usr/bin/python3

import os
import sys
import numpy as np
import matplotlib.pyplot as plt

fn = "out"
# Load the data file
if not os.path.exists(fn):
    sys.exit(0);

data = np.loadtxt(fn)

# Extract columns
x = data[:, 0]
y1 = data[:, 1]
y2 = data[:, 2]
y3 = data[:, 3]
y4 = data[:, 4]
y5 = data[:, 5]

# Plot
plt.scatter(x, y5, s=1, label="Cash")
plt.scatter(x, y1, s=1, label="Mobs")
plt.scatter(x, y2, s=1, label="Msgs")
plt.scatter(x, y3, s=1, label="Thresh")

plt.xlabel("Iteration")
plt.ylabel("Count")
plt.legend()
plt.grid(True)

plt.show()
