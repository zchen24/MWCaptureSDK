#!/usr/bin/env python3

from PIL import Image
import matplotlib.pyplot as plt
import sys

if len(sys.argv) == 1:
    img_file = 'tmp.png'
else:
    img_file = sys.argv[1]

print('Opening {}'.format(img_file))
img = Image.open(img_file)
plt.figure()
plt.imshow(img)
plt.title(img_file)
plt.show()
