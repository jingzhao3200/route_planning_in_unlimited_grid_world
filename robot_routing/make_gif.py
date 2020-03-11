from PIL import Image
import glob

def make_gif_for_problem(problem_file = "sample", total_ts = 15):
    # Create the frames
    frames = []
    imgs = glob.glob(problem_file + "plots/*.png")
    for i in range(total_ts):
        filename = problem_file + "/plots/" + str(i) + ".png"
        new_frame = Image.open(filename)
        frames.append(new_frame)

    # Save into a GIF file that loops forever
    frames[0].save(problem_file + "/" + problem_file + ".gif", format='GIF',
                   append_images=frames[1:],
                   save_all=True,
                   duration=700, loop=0)
    return
