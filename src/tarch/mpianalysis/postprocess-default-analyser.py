from PIL import Image, ImageDraw, ImageFont

import csv
import sys

if len(sys.argv) != 2:
    print 'usage: python tobiDomain.py <filename>'
    sys.exit(0)

filename = sys.argv[1]
csvfile = open(filename, 'rb')
peanoreader_csv = csv.reader(csvfile, delimiter=",")

# generate domain data
width = 1000
height = 1000

img = Image.new("RGB", (width,height), color="white")
draw = ImageDraw.Draw(img)
 
scale = (width/1.0,height/1.0)

p = 0
color = 200 + 200*256 + 200*256*256
for domain_line in peanoreader_csv:
    print domain_line[0],domain_line[1],domain_line[2],domain_line[3]

    offset1 = (
             float(domain_line[0])*scale[0], 
      height-float(domain_line[1])*scale[1]
    )
    offset2 = (
             (float(domain_line[0])+float(domain_line[2])) * scale[0], 
      height-(float(domain_line[1])+float(domain_line[3])) * scale[1]
    )

    draw.rectangle([offset1, offset2],fill=color,outline='white')
    
    color = color - 16
    p     = p+1
 
print 'number of partitions: '+str(p)

img.save(sys.argv[1]+'.png',"PNG")
