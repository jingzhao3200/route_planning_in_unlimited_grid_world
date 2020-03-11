from PIL import Image
import glob

# Create the frames
frames = []
imgs = glob.glob("plots/*.png")
for i in range(11):
    filename = "plots/" + str(i) + ".png"
    new_frame = Image.open(filename)
    frames.append(new_frame)

# Save into a GIF file that loops forever
frames[0].save('png_to_gif.gif', format='GIF',
               append_images=frames[1:],
               save_all=True,
               duration=1000, loop=0)
