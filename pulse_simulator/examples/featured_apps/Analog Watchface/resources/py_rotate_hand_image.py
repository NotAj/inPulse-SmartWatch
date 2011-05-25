from PIL import Image
import Image

# Opens the hour_original image from the folder backup, rotates it and crops it. In order for the script to work, this image will have to be the image of
# the hand at the 12 position placed in an image twice its size in height, with a bit of space around the sides. For this version of the example, trial and error is your best fiend. See sample images for clarification. This is used as a hack in order to rotate
# the image around a specific point - the bottom of the arrow.

imcrop=Image.new("RGB", (1, 1))
c=1;

# There are 12 positions for the hour hand
for k in range(0, 12):
	imc= Image.open("./source_images/hour_original.bmp")
	imc=imc.rotate(-30*k,Image.BICUBIC,True)
	im_size = imc.size

	x0=imc.size[0]
	y0=imc.size[1]
	x1=0
	y1=0
	

	#Gets the top, right, bottom, left dimensions of the actual arrow. 	
	for i in range(0,imc.size[0]):
		for j in range(0,imc.size[1]):
			if (imc.getpixel((i,j))!=(0,0,0)):
				if (i<x0):
					x0=i
					
				if (j<y0):
					y0=j
					
				if (i>x1):
					x1=i
					
				if (j>y1):
					y1=j
					
			
	imc_size = imc.size
	"""
	Crops the image appropriately. Note the addition or subtraction of the numbers 8, 1, 2. They are fudge factors. The number 8 is
	more or less the distance between the center of the original image and the sides. 1, 2 is quite a fudge factor with no actual origin.
	"""
	if(k==0):
		imcrop=imc.crop ((imc_size[0]/2-8, 0, x1, imc_size[1]/2+8))
	if ((k>=1) and (k<=3)): #(imc_size[1]/2+8)
		imcrop=imc.crop (((imc_size[0]/2-8), 0, (x1+2), ((imc_size[1]/2+8))))
	
	if ((k>=4) and (k<=6)):
		imcrop=imc.crop (((imc_size[0]/2-8), imc_size[1]/2-8, x1+1, imc_size[1]))

	if ((k>=7) and (k<=9)):
		imcrop=imc.crop ((x0, (imc_size[1]/2-8), x1, (y1+1)))

	if ((k>=10) and (k<=12)):
		imcrop=imc.crop ((x0, 0, x1, (imc_size[1]/2+8)))


	imcrop.save("hour_"+chr(64+c)+".bmp")
	c=c+1;

# Same process as for the hour hand
# Opens the hour_original image from the folder backup, rotates it and crops it. In order for the script to work, this image will have to be the image of
# the hand at the 12 position placed in an image twice its size in height, with a bit of space around the sides. For this version of the example, trial and error is your best fiend. See sample images for clarification. This is used as a hack in order to rotate
# the image around a specific point - the bottom of the arrow.


imcrop=Image.new("RGB", (1, 1))
# There are 60 positions for the hour hand
for k in range(0, 60):
	imc= Image.open("./source_images/minute_original.bmp")
	imc=imc.rotate(-6*k,Image.BICUBIC,True)
	im_size = imc.size

	x0=imc.size[0]
	y0=imc.size[1]
	x1=0
	y1=0
	
	#Gets the top, right, bottom, left dimensions of the actual arrow. 
	for i in range(0,imc.size[0]):
		for j in range(0,imc.size[1]):
			if (imc.getpixel((i,j))!=(0,0,0)):
				if (i<x0):
					x0=i
					
				if (j<y0):
					y0=j
				if (i>x1):
					x1=i
					
				if (j>y1):
					y1=j
					
	imc_size = imc.size
	"""
	Crops the image appropriately. Note the addition or subtraction of the numbers 8, 1, 2. They are fudge factors. The number 8 is
	more or less the distance between the center of the original image and the sides. 1, 2 is quite a fudge factor with no actual origin.
	"""
	if(k==0):
		imcrop=imc.crop (((imc_size[0]/2-8), 0, (x1), imc_size[1]/2+8))

	if ((k>=1) and (k<=15)): #(imc_size[1]/2+8)
		imcrop=imc.crop (((imc_size[0]/2-8), 0, (x1), ((imc_size[1]/2+8))))

	if ((k>=16) and (k<=30)):
		imcrop=imc.crop (((imc_size[0]/2-8), imc_size[1]/2-8, x1+1, imc_size[1]))

	if ((k>=31) and (k<=45)):
		imcrop=imc.crop ((x0, (imc_size[1]/2-8), x1, (y1+1)))

	if ((k>=46) and (k<=60)):
		imcrop=imc.crop ((x0, 0, x1, (imc_size[1]/2+8)))
	
	imcrop.save("minute_"+chr(65+k/10)+chr(48+k%10)+".bmp")
	



	
