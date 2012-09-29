from cv2 import *
import matplotlib.pyplot as plt
import numpy as np
import os

def do_flow(a,b):
    flow = np.empty_like(a,dtype='float32')
    return calcOpticalFlowFarneback(a,b, flow, 0.5, 4, 9,20,7,1.1,0)

a = imread("..\\Data\\SP\\frame000361.png", 0)
b = imread("..\\Data\\SP\\frame000362.png", 0)
flow = do_flow(b,a)
stdimg = GaussianBlur(a.astype('float32'),(23,23),0)
stdimg -= a
stdimg *= stdimg
stdimg = np.sqrt(stdimg, out=stdimg)
power = np.sqrt((flow[:,:,0]-3)**2 + flow[:,:,1]**2)
fixedPower = power.copy()
fixedSDV = stdimg.copy()
fixedPower[power < (np.mean(power) + 3*np.std(power))] = 0
fixedSDV[stdimg < 15] = 0
se = getStructuringElement(MORPH_ELLIPSE ,(4,4))
fixedSDV = dilate(fixedSDV,se,iterations=3)
fixedSDV = erode(fixedSDV,se,iterations=3)
namedWindow("Flow")
namedWindow("SDV")
fixedPower[fixedPower > 0] = 1
fixedSDV[fixedSDV > 0] = 1
print plt.hist([f for f in (power[fixedSDV == 1]).ravel()],100)
print np.mean(power), np.std(power)
imshow("Flow",fixedPower)
imshow("SDV",fixedSDV)
plt.ylim(0,2000)
plt.show()
waitKey()

