from PIL import Image
import os


list = os.listdir('./2')
for imgname in list:
    imgname = "./2/"+imgname
    img = Image.open(imgname)
    name = imgname[0:-3]
    img.save(name+"png")
