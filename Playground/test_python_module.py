import sys
import os
import numpy as np
import matplotlib.pyplot as plt;
os.system('cd')
os.system(r'copy ..\x64\Release\*.pyd .')

raw_input('press enter')

import video_mosaic as vm
a = np.array([[1,2],[3,4]])
vm.Test(a)
print vm.Test2()
params = vm.Parameters(r'..\Data\INI\cvm.ini')
params.Set("ImageToMosaic.RenderImpl", "OPENCV")
params.Set("ImageToMosaic.TileSize", "18")
params.Set("TopographicToLocations.PreciseTiling", "0")
params.Set("TopographicToLocations.MaxOverlapPercent", "0.05")
v = vm.ImageToMosaic(params)
r = v.ProcessFile(r'..\Data\Images\BlueDragon.jpg')
plt.imshow(r)
plt.show()

raw_input('press enter')