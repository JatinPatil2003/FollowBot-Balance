import cv2
from cvzone.PoseModule import PoseDetector
import requests
import time

prev_time =0
now_time = 0
count =0
# size=(720,1024)
size=(1280, 720)
midX = int(size[0]/2)
detector = PoseDetector()
url = "http://192.168.94.107:81/coord?"
# 400 
# 550
# 
vcap = cv2.VideoCapture('http://192.168.94.107:81/stream')
c_x, c_y = midX, int(size[1]/2)
def map(x, in_min, in_max, out_min, out_max):
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min


while True:
  ret, frame = vcap.read()
  frame = cv2.resize(frame, size)
  # frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
  cv2.imshow("frame", frame)
  img = detector.findPose(frame)
  lmlist,bbox = detector.findPosition(img)
  now_time = time.time()
  fps = 1/(now_time - prev_time)
  prev_time = now_time
  fps = int(fps)
  fps = str(fps)
  cv2.putText(img, "fps : " + fps, (10, 90), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
  if bbox:
    (c_x, c_y) = bbox['center']
  cv2.line(img,(midX-60,0),(midX-60,720),(255,0,0),2)
  cv2.line(img,(midX,0),(midX,720),(0,255,0),1)
  cv2.line(img,(midX+60,0),(midX+60,720),(255,0,0),2)
  cv2.line(img,(0,455),(1280,455),(255,0,0),2)
  cv2.line(img,(0,475),(1280,475),(0,255,0),1)
  cv2.line(img,(0,495),(1280,495),(255,0,0),2)
  cv2.putText(img,"Center : "+str(c_x)+" "+str(c_y),(10,50),cv2.FONT_HERSHEY_SIMPLEX, 1,(0,0,255),2)
  # url = "http://192.168.137.45/coord?X="+str(c_x)+"&Y="+str(c_y)
  # print(url)
  # requests.post(url)

  # if c_y >= 360 and c_y <= 590:
  #   if(c_y < 455):
  #     mov = map(c_y, 400, 455, -3, 0)
  #     m = 'Reverse'
  #   elif (c_y > 495):
  #     mov = map(c_y, 495, 550, 0, 3)
  #     m = 'Forward'
  #   else:
  #     m = 'OK'
  #     mov = 0
  # else:
  #   m = 'OK'
  #   mov = 0
  # cv2.putText(img,"Movement : "+m+" "+str(mov),(10,650),cv2.FONT_HERSHEY_SIMPLEX, 1,(0,0,255),2)
  #
  # if(c_x < 580):
  #   dir = map(c_x, 100, 580, -80, 0)
  #   d = 'Left'
  # elif (c_x > 700):
  #   dir = map(c_x, 700, 1180, 0, 80)
  #   d = 'Right'
  # else:
  #   d = 'OK'
  #   dir = 0
  # cv2.putText(img,"Direction : "+d+" "+str(dir),(10,690),cv2.FONT_HERSHEY_SIMPLEX, 1,(0,0,255),2)
  cv2.imshow("detect", img)
  k = cv2.waitKey(1) & 0xff
  if k == 27 : 
    break


vcap.release()
cv2.destroyAllWindows()
print("Video stop")

















