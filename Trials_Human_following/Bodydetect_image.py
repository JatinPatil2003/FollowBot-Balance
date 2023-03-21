import cv2
from cvzone.PoseModule import PoseDetector
# size =(720,1024)
size = (1024, 720)
detector = PoseDetector()
img = cv2.imread('Data/pedestrians_5.jpg')
# img = cv2.resize(img, size)

img = detector.findPose(img)
cv2.imshow("raw", img)
lmlist,bbox = detector.findPosition(img)
print(type(bbox))
print(type(lmlist))
print(bbox)
print(lmlist)
print(bbox["center"])
cv2.imshow("POSE", img)
cv2.waitKey(0)
    # k = cv2.waitKey(1) & 0xff
    # if k == 27 : 
    #     break

