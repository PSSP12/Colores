import matplotlib.pyplot as plt
import numpy as np
import matplotlib.image as mpimg

filename = "tests/TestingData/ImageData.csv"
background_image = "assets/Images/MasterpieceMat.bmp"  # 👈 change this to your image file

# --- Read the file ---
with open(filename, "r") as f:
    first_line = f.readline().strip()  # parameters
    header = f.readline()              # skip header
    data = np.loadtxt(f)

# --- Parse parameters ---
params = {}
for item in first_line.replace(',', '\t').split('\t'):
    if '=' in item:
        key, value = item.split('=')
        value = value.strip()
        try:
            params[key.strip()] = float(value)
        except ValueError:
            if value.startswith('[') and value.endswith(']'):
                params[key.strip()] = [
                    float(v) for v in value.strip('[]').split(',') if v.strip()
                ]
            else:
                params[key.strip()] = value

X1, Y1 = params.get("X1", 0), params.get("Y1", 0)
X2, Y2 = params.get("X2", 0), params.get("Y2", 0)

Xi, Yi, Xd, Yd = data.T

# --- Load background image ---
img = mpimg.imread(background_image)
img_height, img_width = img.shape[:2]

# --- Plot ---
fig, ax = plt.subplots(figsize=(8, 6))

# Display the image (origin='lower' keeps (0,0) bottom-left)
ax.imshow(img, origin='lower', extent=[0, img_width, 0, img_height])

# Plot the reference line and points
ax.plot([X1, X2], [Y1, Y2], color='red', linewidth=1.5, label='Reference')
ax.scatter([X1, X2], [Y1, Y2], color='red', s=40, zorder=3)

# Plot all lines from data
for x1, y1, x2, y2 in zip(Xi, Yi, Xd, Yd):
    ax.plot([x1, x2], [y1, y2], color='yellow', linewidth=0.5, alpha=0.7)

ax.set_xlim(0, img_width)
ax.set_ylim(0, img_height)
ax.set_xlabel("X")
ax.set_ylabel("Y")
ax.set_title("Connections over background image")
ax.legend()
ax.grid(False)  # optional — turn off grid for clarity
plt.tight_layout()

# Automatically zoom to data
margin = 50  # padding around data in same units as your coordinates
xmin = min(Xi.min(), Xd.min()) - margin
xmax = max(Xi.max(), Xd.max()) + margin
ymin = min(Yi.min(), Yd.min()) - margin
ymax = max(Yi.max(), Yd.max()) + margin

ax.set_xlim(xmin, xmax)
ax.set_ylim(ymin, ymax)

plt.show()
