import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import to_rgba, LightSource

# def generate_height_map(rows, columns, min_height, max_height):
#     return np.random.randint(min_height, max_height+1, size=(rows, columns))

# height_map = generate_height_map(4, 4, 0, 5)

height_map = [
    [7, 9, 6, 8, 0],
    [4, 4, 8, 3, 9],
    [4, 5, 2, 2, 7],
    [3, 3, 1, 0, 4],
    [3, 9, 9, 7, 0],
]
height_map = [
    [7, 9, 6],
    [4, 2, 8],
    [4, 5, 2],
]
height_map = np.array(height_map)

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

x_size, y_size = height_map.shape

x = np.arange(x_size)
y = np.arange(y_size)
x, y = np.meshgrid(y, x)

# Generate random colors
colors = plt.cm.viridis(np.random.rand(x_size * y_size))

# Apply a height-based lightness adjustment
light_source = LightSource(azdeg=180, altdeg=45)
height_normalized = (height_map - np.min(height_map)) / (np.max(height_map) - np.min(height_map))
colors_adjusted = light_source.shade_rgb(colors.reshape(x_size, y_size, 4), height_normalized)

# Define bar width, depth, and gap
bar_width = 0.9
bar_depth = 0.9
gap = 9

# Plot the surface with different colors for each bar and a small gap between them
for i in range(x_size):
    for j in range(y_size):
        ax.bar3d(x[i, j] + gap/2, y[i, j] + gap/2, 0, bar_width, bar_depth, height_map[i, j], shade=True, color=colors_adjusted[i, j])

ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Height')

plt.show()
