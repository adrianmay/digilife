
import numpy as np
import matplotlib.pyplot as plt

# Load the data file
data = np.loadtxt("out")

# Extract columns
x = data[:, 0]
y1 = data[:, 1]
y2 = data[:, 2]

# Plot
plt.plot(x, y1, label="Mobs")
plt.plot(x, y2, label="Msgs")

plt.xlabel("Iteration")
plt.ylabel("Count")
plt.legend()
plt.grid(True)

plt.show()
