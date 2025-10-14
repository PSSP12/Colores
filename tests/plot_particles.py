import pandas as pd
import matplotlib.pyplot as plt

# --- Load CSV data ---
df = pd.read_csv("tests/TestingData/pf_results.csv")

# --- Plot particle trajectories ---
plt.figure(figsize=(10, 6))
for p in df['particle'].unique():
    particle_data = df[df['particle'] == p]
    plt.plot(particle_data['x_left'], particle_data['y_left'], color='blue', alpha=0.3)
    plt.plot(particle_data['x_right'], particle_data['y_right'], color='red', alpha=0.3)

plt.title("Particle trajectories (blue=left sensor, red=right sensor)")
plt.xlabel("X position")
plt.ylabel("Y position")
plt.grid(True)
plt.axis('equal')

# --- Plot stepwise particle positions (optional heatmap style) ---
plt.figure(figsize=(10, 6))
plt.scatter(df['x_left'], df['y_left'], c=df['step'], cmap='Blues', s=10, label='Left sensor')
plt.scatter(df['x_right'], df['y_right'], c=df['step'], cmap='Reds', s=10, label='Right sensor')
plt.colorbar(label="Step")
plt.title("Particle positions over time")
plt.xlabel("X position")
plt.ylabel("Y position")
plt.grid(True)
plt.axis('equal')
plt.legend()

# --- Plot timing per step ---
timing_per_step = df.groupby('step')['dt'].mean()
plt.figure(figsize=(8, 4))
plt.plot(timing_per_step.index, timing_per_step.values, marker='o')
plt.title("Average computation time per step")
plt.xlabel("Step")
plt.ylabel("Time (s)")
plt.grid(True)
plt.show()
