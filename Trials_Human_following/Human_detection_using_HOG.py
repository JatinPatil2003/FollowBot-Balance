import cv2
# Make sure the image file is in the same directory as your code
filename = 'Data/pedestrians_1.jpg'
 
def detect_human():
 
  # Create a HOGDescriptor object
    hog = cv2.HOGDescriptor()
     
  # Initialize the People Detector
    hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())
     
  # Load an image
    image = cv2.imread(filename)
    (bounding_boxes, weights) = hog.detectMultiScale(image, 
                                                   winStride=(4, 4),
                                                   padding=(8, 8), 
                                                   scale=1.05)
 
    arr = bounding_boxes.argmax(axis=0)

    closest_human = bounding_boxes[arr[3],:]
    print(bounding_boxes)
    print(type(bounding_boxes))
    print(arr)
    print(closest_human)
    (x, y, w, h) = closest_human
    
    
    cv2.rectangle(image, 
                  (x, y),  
                  (x + w, y + h),  
                  (0, 0, 255), 
                   4)
    cv2.imshow("ghgh", image)
    cv2.waitKey(0)
    return closest_human,w
 
closest_human,w = detect_human()
print(closest_human)
print(w)
