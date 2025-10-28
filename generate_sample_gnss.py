import math
import csv
import time

# ===============================
# 🔧 PARAMÈTRES GÉNÉRATION
# ===============================

output_file = "gps_loop_test.csv"

# Centre du circuit (latitude / longitude de référence)
center_lat = 45.123000
center_lon = 5.678000
center_alt = 250.0

# Forme du circuit : ellipse
radius_lat_m = 100.0  # "hauteur" (mètres)
radius_lon_m = 200.0  # "largeur" (mètres)

# Simulation temporelle
total_time = 60.0      # secondes
dt = 0.1               # pas de temps (s)
num_points = int(total_time / dt)

# Conversion m -> degrés
deg_per_meter_lat = 1.0 / 111320.0
deg_per_meter_lon = 1.0 / (111320.0 * math.cos(math.radians(center_lat)))

# ===============================
# 🧮 GÉNÉRATION DES POINTS
# ===============================

rows = []
start_timestamp = 1698200000.0
alt_variation = 1.0

for i in range(num_points):
    t = i * dt
    timestamp = start_timestamp + t

    # Angle autour de la boucle (1 tour en total_time)
    angle = 2 * math.pi * (t / total_time)

    # Calcul de la position
    lat_offset_m = radius_lat_m * math.sin(angle)
    lon_offset_m = radius_lon_m * math.cos(angle)

    lat = center_lat + lat_offset_m * deg_per_meter_lat
    lon = center_lon + lon_offset_m * deg_per_meter_lon
    alt = center_alt + alt_variation * math.sin(2 * angle)

    # Calcul de la direction (heading en degrés)
    heading = math.degrees(angle + math.pi / 2) % 360.0

    # Vitesse variable pour plus de réalisme
    speed = 10.0 + 2.0 * math.sin(3 * angle)

    rows.append([timestamp, lat, lon, alt, heading, speed])

# ===============================
# 💾 ÉCRITURE DU FICHIER CSV
# ===============================

with open(output_file, "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(["timestamp", "lat", "lon", "alt", "heading", "speed"])
    writer.writerows(rows)

print(f"✅ Fichier généré : {output_file} ({num_points} points)")
