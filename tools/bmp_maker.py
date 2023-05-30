from PIL import Image
import sys

im = Image.open(sys.argv[1])
print(im.size)
pix = im.load()

b = 0
i_i = 0
s = ""
c = 0

for y in range(0, im.size[1], 5):
	for x in range(0, im.size[0], 8):
		for j in range (5):
			i_i = 0
			b = 0
			for i in range(7, -1, -1):
				try:
					b = b | (int((pix[x + i_i, y + j][0] >= 1)) << i)
				except:
					print("ERROR: ", end='')
				print("X: ",x + i_i, "Y: ", y + j, "[]: ", int(pix[x + i_i, y + j][0] >= 1), "(", i, i_i, j, x, y, ")")
				i_i = i_i + 1

			s += str(b) + ", "
			c = c + 1
print(s)
print("Count: ", c)