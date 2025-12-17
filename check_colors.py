from PIL import Image
import numpy as np
from collections import Counter

# Check Emerald source font
img = Image.open('graphics/fonts/latin_short.png')
arr = np.array(img)

# Sample first 256 pixels to see color distribution
colors = []
for y in range(min(128, arr.shape[0])):
    for x in range(min(128, arr.shape[1])):
        if arr.ndim == 3:
            colors.append(tuple(arr[y, x]))
        else:
            colors.append(arr[y, x])

color_counts = Counter(colors)
print('Most common colors in Emerald latin_short.png:')
for color, count in color_counts.most_common(10):
    print(f'{color}: {count} pixels')

# Check our generated font
print('\n---\nGenerated font3_lat_new.png palette and pixels:')
generated = Image.open('graphics/fonts/font3_lat_new.png')
palette = generated.getpalette()
print(f'Palette (first 9 values): {palette[:9]}')
print(f'Mode: {generated.mode}')

arr2 = np.array(generated)
print(f'Pixel value distribution: {Counter(arr2.flatten())}')
