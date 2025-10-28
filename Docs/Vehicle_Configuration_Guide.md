# Configuration Recommandée pour le Véhicule GNSS

## Paramètres du Véhicule dans Unreal Editor

### Section "GNSS Replay"

#### Mode de Fonctionnement
```
Enable GNSS Replay: ? (Coché)
Trajectory Ref: (Auto-détecté ou sélectionner manuellement)
Use Physics Movement: ? (Coché - RECOMMANDÉ)
```

### Section "GNSS Replay | PID Control"

#### Configuration pour Terrain Urbain (Vitesse Moyenne 30-50 km/h)
```
KpYaw: 0.5
KiYaw: 0.01
KdYaw: 0.1

KpSpeed: 0.1
KiSpeed: 0.005
KdSpeed: 0.05

Accept Radius: 500 cm (5 mètres)
Look Ahead Distance: 1000 cm (10 mètres)
Max Steering Rate: 2.0
```

#### Configuration pour Autoroute (Vitesse > 80 km/h)
```
KpYaw: 0.3          (Moins agressif pour éviter zigzags)
KiYaw: 0.005        (Réduit)
KdYaw: 0.15         (Plus de stabilité)

KpSpeed: 0.15     (Plus réactif)
KiSpeed: 0.01
KdSpeed: 0.08

Accept Radius: 800 cm (8 mètres)
Look Ahead Distance: 2000 cm (20 mètres - plus loin = plus lisse)
Max Steering Rate: 1.5 (Changements plus graduels)
```

#### Configuration pour Circuit Sportif (Virages Serrés)
```
KpYaw: 0.8          (Très réactif)
KiYaw: 0.02
KdYaw: 0.2          (Fort amortissement)

KpSpeed: 0.2    (Accélération/freinage agressif)
KiSpeed: 0.01
KdSpeed: 0.1

Accept Radius: 300 cm (3 mètres)
Look Ahead Distance: 800 cm (8 mètres)
Max Steering Rate: 3.0 (Permet des corrections rapides)
```

#### Configuration pour Tout-Terrain (Terrain Accidenté)
```
KpYaw: 0.4
KiYaw: 0.015        (Compense déviations du terrain)
KdYaw: 0.05         (Moins de dérivé car terrain chaotique)

KpSpeed: 0.08 (Moins agressif)
KiSpeed: 0.003
KdSpeed: 0.03

Accept Radius: 600 cm (6 mètres - plus tolérant)
Look Ahead Distance: 1200 cm (12 mètres)
Max Steering Rate: 1.5
```

### Section "GNSS Replay | Legacy" (Si physique désactivée)
```
Use Physics Movement: ? (Décoché)
Use Timestamp Playback: ? (Recommandé si legacy)
Playback Speed: 1.0 (Vitesse réelle)
Trajectory Point Delay: 0.1 (secondes - si timestamp désactivé)
```

## Configuration du Véhicule Chaos (Blueprint)

### Moteur (Engine Setup)
```
Max Torque: 750 N?m (Pour voiture sportive)
Max RPM: 7000
Engine Idle RPM: 900
Engine Brake Effect: 0.2
Engine Rev Up MOI: 5.0
Engine Rev Down Rate: 600
```

### Transmission (Transmission Setup)
```
Use Automatic Gears: ? (Oui)
Use Auto Reverse: ? (Oui)
Final Ratio: 2.81
Change Up RPM: 6000
Change Down RPM: 2000
Gear Change Time: 0.2s
Transmission Efficiency: 0.9

Forward Gear Ratios:
  [0]: 4.25 (1ère)
  [1]: 2.52 (2ème)
  [2]: 1.66 (3ème)
  [3]: 1.22 (4ème)
  [4]: 1.0  (5ème)

Reverse Gear Ratios:
  [0]: 4.04 (Marche arrière)
```

### Direction (Steering Setup)
```
Steering Type: Ackermann
Angle Ratio: 0.7
```

### Châssis (Chassis)
```
Mass: 1500 kg (Voiture moyenne)
Drag Coefficient: 0.31 (Sportive)
Downforce Coefficient: 0.3
Chassis Height: 144 cm
```

### Roues (Wheel Setup)

#### Roues Avant (Front Wheels)
```
Wheel Class: rcsimSportsWheelFront
Bone Names: Phys_Wheel_FL, Phys_Wheel_FR
Radius: 35 cm
Width: 20 cm
Mass: 20 kg

Suspension:
  - Max Raise: 10 cm
  - Max Drop: 10 cm
  - Spring Rate: 25000 N/m
  - Damping Rate: 5000 N?s/m
  - Rest Offset: -5 cm

Tire:
  - Friction Force Multiplier: 3.0
  - Lateral Slip Graph: (Courbe personnalisée)
  - Max Brake Torque: 1500 N?m
  - Handbrake Torque: 0 N?m (Pas de frein à main avant)
```

#### Roues Arrière (Rear Wheels)
```
Wheel Class: rcsimSportsWheelRear
Bone Names: Phys_Wheel_BL, Phys_Wheel_BR
Radius: 35 cm
Width: 25 cm (Plus large)
Mass: 22 kg

Suspension:
  - Max Raise: 10 cm
  - Max Drop: 10 cm
  - Spring Rate: 28000 N/m (Plus rigide)
  - Damping Rate: 5500 N?s/m
  - Rest Offset: -5 cm

Tire:
  - Friction Force Multiplier: 3.5 (Plus d'adhérence)
  - Lateral Slip Graph: (Courbe personnalisée)
  - Max Brake Torque: 2000 N?m
  - Handbrake Torque: 3000 N?m (Frein à main actif)
```

## Vérification de la Configuration

### Dans l'Éditeur Unreal

1. **Sélectionner le véhicule** dans la scène
2. **Panneau Details ? Components ? Mesh**
   - ? Simulate Physics: Coché
   - ? Collision Preset: "Vehicle"

3. **Panneau Details ? Vehicle Movement Component**
   - Vérifier que toutes les roues sont bien configurées
   - Vérifier les bone names correspondent au skeletal mesh

4. **Panneau Details ? GNSS Replay**
   - Activer Enable GNSS Replay
   - Activer Use Physics Movement
   - Vérifier Trajectory Ref pointe vers un TrajectoryActor

### Test de Base

#### Test 1: Physique Fonctionnelle
```
1. Placer le véhicule en l'air (Z = 1000)
2. Lancer le jeu
3. Le véhicule doit tomber et rebondir sur les suspensions
```

#### Test 2: Conduite Manuelle
```
1. Désactiver Enable GNSS Replay
2. Lancer le jeu
3. Utiliser ZQSD pour conduire manuellement
4. Vérifier que les roues tournent et se suspendent
```

#### Test 3: Replay GNSS
```
1. Charger un fichier CSV GNSS
2. Activer Enable GNSS Replay
3. Activer Use Physics Movement
4. Lancer le jeu
5. Observer la conduite autonome
```

## Logs à Vérifier

### Au Démarrage
```
LogTemp: Warning: GNSSGameMode: Creating GNSS Loader Widget
LogTemp: Warning: GNSSGameMode: Found and assigned TrajectoryActor
LogTemp: Warning: TrajectoryActor: Loaded 150 GNSS points
LogTemp: Warning: GNSS Replay: Auto-found TrajectoryActor: TrajectoryActor_0
LogTemp: Warning: GNSS Replay: Reset to start position (Physics Mode: Enabled)
```

### Pendant le Jeu (chaque seconde)
```
LogTemp: Log: GNSS Physics: WP=15/150, Speed=35.2/40.0 km/h, YawErr=12.3°, Steer=0.25, Throttle=0.45, Brake=0.00
```

### Atteinte de Waypoint
```
LogTemp: Log: GNSS Physics Replay: Reached waypoint 15/150
```

## Dépannage Rapide

### Problème: Le véhicule ne bouge pas
**Solution:**
1. Vérifier que Enable GNSS Replay est coché
2. Vérifier que Use Physics Movement est coché
3. Vérifier qu'il y a des données GPS chargées
4. Vérifier les logs : doit voir "GNSS Physics: WP=..."

### Problème: Le véhicule oscille
**Solution:**
1. Réduire KpYaw de 0.5 ? 0.3
2. Augmenter KdYaw de 0.1 ? 0.2
3. Augmenter Look Ahead Distance de 1000 ? 1500

### Problème: Le véhicule est trop lent
**Solution:**
1. Augmenter KpSpeed de 0.1 ? 0.15
2. Vérifier Max Torque du moteur (750+)
3. Vérifier que les vitesses GPS sont présentes

### Problème: Le véhicule rate les virages
**Solution:**
1. Augmenter Look Ahead Distance (1500-2000 cm)
2. Augmenter KpYaw pour réaction plus rapide
3. Vérifier Max Steering Rate n'est pas trop faible

### Problème: Le véhicule traverse le sol
**Solution:**
1. Vérifier Collision Preset = "Vehicle"
2. Vérifier que le niveau a de la collision
3. Ajuster Suspension Max Drop/Raise

## Astuces d'Optimisation

### Pour Trajectoire Lisse
- Augmenter Look Ahead Distance
- Réduire Max Steering Rate
- Augmenter Accept Radius

### Pour Conduite Agressive
- Augmenter KpYaw et KpSpeed
- Réduire Look Ahead Distance
- Augmenter Max Steering Rate

### Pour Terrain Difficile
- Réduire tous les gains de 30%
- Augmenter Accept Radius
- Augmenter amortissement suspensions

## Valeurs Extrêmes à Éviter

? **Ne PAS faire:**
```
KpYaw > 2.0      (Trop instable)
KiYaw > 0.1      (Accumulation excessive)
KdYaw > 0.5      (Trop de bruit)

Look Ahead < 300 cm     (Trop court, zigzags)
Look Ahead > 5000 cm    (Trop loin, rate virages)

Max Steering Rate > 10  (Changements brutaux)
Max Steering Rate < 0.5 (Trop lent)
```

## Conclusion

Cette configuration devrait donner un comportement réaliste et fluide. Ajustez les paramètres selon vos besoins spécifiques et le type de trajectoire GPS que vous utilisez.
