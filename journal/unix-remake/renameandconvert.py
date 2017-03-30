import os
for filename in os.listdir('images'):
    if filename[-4:] == '.bmp': os.system("convert images/{} -alpha on -transparent '#00ff00' images/{}.png".format(filename, filename.strip('_')[:-4].lower()))
