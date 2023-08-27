from pyzbar import pyzbar
import argparse, datetime, imutils, time, cv2

ap = argparse.ArgumentParser()
ap.add_argument("-o", "--output", type = str, default = "barcodes.csv", help = "path to output CSV file containing barcodes")
args = vars(ap.parse_args())

csv = open(args["output"], "w")
found = set()

print("[INFO] starting video stream...")
cap = cv2.VideoCapture(0)
time.sleep(2.0)

while True:
	ret, frame = cap.read()
	frame = imutils.resize(frame, width = 600)
	barcodes = pyzbar.decode(frame)
	
	for barcode in barcodes:
		(x, y, w, h) = barcode.rect
		cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
		barcodeData = barcode.data.decode("utf-8")
		barcodeType = barcode.type
		text = "{} ({})".format(barcodeData, barcodeType)
		cv2.putText(frame, text, (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
		
		if barcodeData not in found:
			csv.write("{},{}\n".format(datetime.datetime.now(), barcodeData))
			csv.flush()
			found.add(barcodeData)
			
	cv2.imshow("Barcode Scanner", frame)
	key = cv2.waitKey(1) & 0xFF
 
	if key == ord("q"):
		break
	
print("[INFO] cleaning up...")
csv.close()
cv2.destroyAllWindows()