import cv2

cap = cv2.VideoCapture('/home/xing/data/webwxgetvideo')
print(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
print(cap.get(7))
while(cap.isOpened()):
    ret, frame = cap.read()
    cv2.imshow('frame',frame)
    if cv2.waitKey(25) & 0xFF == ord('q'):
        break
cap.release()
#cv2.destroyAllWindows()

