import cv2
from cvzone.PoseModule import PoseDetector
detector = PoseDetector()
cap = cv2.VideoCapture(0)
while True:
    sucsess, img = cap.read()
    img = detector.findPose(img)
    lmlist,bbox = detector.findPosition(img)
    cv2.imshow("POSE", img)
    k = cv2.waitKey(1) & 0xff
    if k == 27 : 
        break
