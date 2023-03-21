import cv2
import requests

# Open a sample video available in sample-videos
url = "http://192.168.137.45/coord?"
vcap = cv2.VideoCapture('http://192.168.137.45:81/stream')
#if not vcap.isOpened():
#    print "File Cannot be Opened"

size=(1024, 720)
count = 0
url += "X="
url += str(size[0])
url += "&"
url += "Y="
url += str(size[1])

while(True):
    # Capture frame-by-frame
    ret, frame = vcap.read()
    count += 1
    if(count == 5):
        # print(url)
        requests.post(url)
        count = 0
    #print cap.isOpened(), ret
    if frame is not None:
        # Display the resulting frame
        cv2.imshow('frame',cv2.resize(frame, size))
        # Press q to close the video windows before it ends if you want
        if cv2.waitKey(22) & 0xFF == ord('q'):
            break
    else:
        print("Frame is None")
        break

# When everything done, release the capture
vcap.release()
cv2.destroyAllWindows()
print("Video stop")