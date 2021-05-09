import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from random import uniform
import numpy as np

width = 255 
x = np.linspace(-1, 1, 255)
y = np.linspace(-1,1, 255)
X, Y = np.meshgrid(x, y)
F = np.array([[uniform(-1, 1) for _ in range(width)]])

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.plot_surface(X, Y, F)
plt.show()
