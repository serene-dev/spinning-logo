
# Convert given logo image to data.h text format

import sys, json
from PIL import Image

def convert(file):
    # Resize to 128x128, and get alpha channel
    img = Image.open(file).resize((128, 128)).split()[-1]
    w, h = img.width, img.height
    data = [i > 128 for i in img.getdata()]
    empty_line = ' ' * 128

    result = [empty_line]
    for y in range(1, h - 1):
        line = ''
        for x in range(1, w - 1):
            i = x + y * h
            if not data[i]:
                line+= ' '
                continue

            l = [data[i + t] for t in [
                1, h + 1, h, h - 1, -1, -h - 1, -h, -h + 1
            ]]
            if not True in l or not False in l:
                # not edge
                line+= '!'
                continue

            # fine angle for edge pixel, and pack it into character
            s = l.index(False)
            while not l[s - 1]:
                s = (s - 1) % len(l)

            ang = s * 0.125
            total, n = 0, 0
            while not l[s]:
                total+= ang
                ang+= 0.125
                n+= 1
                s = (s + 1) % len(l)
            line+= chr(0x30 + int(round(32 * (total / n % 1))))
        result.append(f" {line} ")
    result.append(empty_line)

    with open('data.h', 'w+') as f:
        f.write(f"const char *data =\n")
        f.write("\n".join(map(json.dumps, result)))
        f.write(";")

if len(sys.argv) > 1:
    convert(sys.argv[1])

