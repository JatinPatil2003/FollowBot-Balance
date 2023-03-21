import cv2
from cvzone.PoseModule import PoseDetector
import time
# size =(720,1024)
size = (1024, 720)
fps = 1000
detector = PoseDetector()
# cap = cv2.VideoCapture('Data/humans.mp4')
cap = cv2.VideoCapture(0)
# img = cv2.resize(img, size)
# while cap.isOpened():
while True:
    ret, frame = cap.read()
    if ret == True:
        time.sleep(1/fps)
        img = detector.findPose(frame)
        cv2.imshow("raw", frame)
        lmlist,bbox = detector.findPosition(img)
        # print(type(bbox))
        # print(type(lmlist))
        # print(bbox)
        # print(lmlist)
        cv2.imshow("POSE", img)
        k = cv2.waitKey(1) & 0xff
        if k == 27 : 
            break
    else:
        break

    

