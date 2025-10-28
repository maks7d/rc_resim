# Guide de Démarrage Rapide - Physique Réaliste GNSS

## ?? Objectif
Faire fonctionner votre véhicule avec la physique réaliste (suspensions, collisions, etc.) au lieu de la téléportation.

## ? Démarrage en 3 Minutes

### Étape 1: Compiler le Code (30 secondes)
```
1. Ouvrir le projet dans Unreal Engine
2. Menu: Build ? Compile rcsim C++ Code
3. Attendre la fin de la compilation
```

### Étape 2: Configurer le Véhicule (1 minute)
```
1. Dans la scène, sélectionner votre véhicule (ex: BP_SportsCar)
2. Panneau Details ? Section "GNSS Replay"
3. Cocher: ? Enable GNSS Replay
4. Vérifier: ? Use Physics Movement (doit être déjà coché par défaut)
5. Laisser les autres valeurs par défaut
```

### Étape 3: Charger des Données GPS (1 minute)
```
1. Lancer le jeu (bouton Play)
2. Cliquer sur le bouton "?? Import GNSS File" dans l'interface
3. Sélectionner votre fichier CSV GNSS
4. Le véhicule démarre automatiquement en mode autonome !
```

## ? Vérification Visuelle

### Ce que vous DEVEZ voir:
- ? La voiture roule de manière fluide (pas de téléportation)
- ? Les roues tournent et se suspendent
- ? Le véhicule penche dans les virages
- ? Réaction réaliste aux bosses du terrain
- ? Logs dans la console: "GNSS Physics: WP=..."

### Ce que vous NE devez PAS voir:
- ? Téléportation saccadée
- ? Roues qui ne tournent pas
- ? Véhicule qui flotte sans toucher le sol
- ? Logs d'erreur rouges

## ?? Ajustements de Base

### Si le véhicule oscille (zigzague):
```
Panneau Details ? GNSS Replay | PID Control
- Réduire "Kp Yaw" de 0.5 ? 0.3
- Augmenter "Look Ahead Distance" de 1000 ? 1500
```

### Si le véhicule va trop lentement:
```
Panneau Details ? GNSS Replay | PID Control
- Augmenter "Kp Speed" de 0.1 ? 0.15
```

### Si le véhicule rate les virages:
```
Panneau Details ? GNSS Replay | PID Control
- Augmenter "Look Ahead Distance" de 1000 ? 1500
- Augmenter "Kp Yaw" de 0.5 ? 0.6
```

## ?? Comprendre les Paramètres (Simple)

### Les 3 Paramètres Principaux

#### 1. Look Ahead Distance (Distance de Prévision)
```
Valeur: 1000 cm = 10 mètres

?? Qu'est-ce que c'est?
Au lieu de viser le prochain point GPS, la voiture regarde plus loin sur la trajectoire.

?? Plus bas (500) ? Virages plus serrés mais risque de zigzag
?? Plus haut (2000) ? Trajectoire plus lisse mais virages moins précis

?? Recommandation: 1000-1500 pour conduite urbaine
```

#### 2. Kp Yaw (Réactivité de Direction)
```
Valeur: 0.5

?? Qu'est-ce que c'est?
Contrôle la force avec laquelle le volant tourne pour corriger la direction.

?? Plus bas (0.3) ? Conduite plus douce, moins de corrections
?? Plus haut (0.8) ? Conduite plus agressive, corrections rapides

?? Recommandation: 0.3-0.5 pour conduite normale
```

#### 3. Kp Speed (Réactivité de Vitesse)
```
Valeur: 0.1

?? Qu'est-ce que c'est?
Contrôle la force avec laquelle l'accélérateur/frein réagit.

?? Plus bas (0.05) ? Accélération/freinage doux
?? Plus haut (0.2) ? Accélération/freinage agressif

?? Recommandation: 0.1-0.15 pour conduite normale
```

## ?? Modes Disponibles

### Mode 1: Physique Réaliste (RECOMMANDÉ) ?
```
Use Physics Movement: ? Coché

Avantages:
+ Physique complète (suspensions, collisions, friction)
+ Comportement réaliste
+ Réagit au terrain
+ Peut percuter des obstacles

Idéal pour:
? Simulation réaliste
? Test de véhicules
? Environnements avec obstacles
```

### Mode 2: Téléportation avec Timestamps
```
Use Physics Movement: ? Décoché
Use Timestamp Playback: ? Coché

Avantages:
+ Suit exactement les timestamps GPS
+ Pas d'écart de trajectoire

Inconvénients:
- Pas de physique réelle
- Téléportation visible
- Pas de collisions

Idéal pour:
? Visualisation de données GPS brutes
? Débuggage de trajectoires
```

### Mode 3: Téléportation à Intervalle Fixe
```
Use Physics Movement: ? Décoché
Use Timestamp Playback: ? Décoché
Trajectory Point Delay: 0.1 sec

?? Mode legacy, non recommandé
```

## ?? Exemples de Configuration Prédéfinis

### Configuration 1: Ville (30-50 km/h)
```
[GNSS Replay | PID Control]
Kp Yaw: 0.5
Look Ahead Distance: 1000
Accept Radius: 500
Kp Speed: 0.1

Résultat: Conduite fluide en ville avec virages modérés
```

### Configuration 2: Autoroute (80-120 km/h)
```
[GNSS Replay | PID Control]
Kp Yaw: 0.3              ? Moins réactif = plus stable
Look Ahead Distance: 2000 ? Regarde plus loin
Accept Radius: 800
Kp Speed: 0.15           ? Plus réactif en vitesse

Résultat: Conduite stable à haute vitesse
```

### Configuration 3: Circuit Sportif
```
[GNSS Replay | PID Control]
Kp Yaw: 0.8     ? Très réactif
Look Ahead Distance: 800  ? Vision courte = virages serrés
Accept Radius: 300
Kp Speed: 0.2    ? Accélération/freinage agressif

Résultat: Conduite sportive avec virages serrés
```

## ?? Problèmes Courants et Solutions

### Problème: "Le véhicule ne bouge pas du tout"

**Diagnostic:**
```
1. Vérifier dans Output Log:
   ? "GNSS Replay: Auto-found TrajectoryActor"
   ? "TrajectoryActor: Loaded X GNSS points"
   
2. Si aucun log ? Pas de données GPS chargées
```

**Solution:**
```
1. Cliquer sur "?? Import GNSS File"
2. Charger un fichier CSV valide
3. Vérifier format: timestamp,lat,lon,alt,heading,speed
```

### Problème: "Le véhicule téléporte au lieu de rouler"

**Diagnostic:**
```
Panneau Details ? GNSS Replay
? "Use Physics Movement" n'est pas coché
```

**Solution:**
```
? Cocher "Use Physics Movement"
```

### Problème: "Le véhicule traverse le sol"

**Diagnostic:**
```
Le niveau n'a pas de collision configurée
```

**Solution:**
```
1. Vérifier que le terrain a "Collision Enabled"
2. Dans le véhicule: Details ? Mesh ? Collision Preset = "Vehicle"
```

### Problème: "Logs d'erreur: No TrajectoryActor found"

**Diagnostic:**
```
Aucun TrajectoryActor n'existe dans la scène
```

**Solution:**
```
1. Dans Content Browser: rechercher "TrajectoryActor"
2. Glisser-déposer dans la scène
3. OU laisser le système le créer automatiquement (GNSSGameMode)
```

## ?? Checklist Complète de Vérification

### Avant de Lancer le Jeu

- [ ] Code compilé sans erreur
- [ ] Véhicule placé dans la scène
- [ ] "Enable GNSS Replay" coché
- [ ] "Use Physics Movement" coché
- [ ] TrajectoryActor présent dans la scène
- [ ] Fichier CSV GNSS préparé

### Pendant le Jeu

- [ ] Interface GNSS Loader visible
- [ ] Fichier CSV chargé avec succès
- [ ] Logs "GNSS Physics: WP=..." apparaissent
- [ ] Véhicule roule de manière fluide
- [ ] Roues tournent et se suspendent

### Si Problème

- [ ] Consulter Output Log (Window ? Developer Tools ? Output Log)
- [ ] Vérifier les valeurs PID
- [ ] Tester avec configuration prédéfinie "Ville"
- [ ] Désactiver/réactiver GNSS Replay

## ?? Comprendre les Logs

### Log Normal (Tout fonctionne)
```
LogTemp: Log: GNSS Physics: WP=15/150, Speed=35.2/40.0 km/h, YawErr=12.3°, Steer=0.25, Throttle=0.45, Brake=0.00
```

**Signification:**
- WP=15/150 ? Au waypoint 15 sur 150 total
- Speed=35.2/40.0 ? Vitesse actuelle / vitesse cible (km/h)
- YawErr=12.3° ? Erreur de direction (angle à corriger)
- Steer=0.25 ? Volant tourné à 25% vers la droite
- Throttle=0.45 ? Accélérateur à 45%
- Brake=0.00 ? Pas de freinage

### Log de Waypoint Atteint
```
LogTemp: Log: GNSS Physics Replay: Reached waypoint 15/150
```

**Bon signe:** La voiture progresse sur la trajectoire

### Log d'Erreur
```
LogTemp: Error: GNSS Replay: Enabled but no TrajectoryActor found in level!
```

**Action:** Ajouter un TrajectoryActor ou laisser GNSSGameMode le créer

## ?? Objectifs de Performance

### Cible à Atteindre

```
? Erreur de cap (YawErr): < 20° en moyenne
? Erreur de vitesse: < 5 km/h
? Distance au waypoint: Décroissante jusqu'à Accept Radius
? Framerate: > 60 FPS
? Pas de téléportation visible
```

### Si Non Atteint

```
? Ajuster les gains PID
? Vérifier qualité des données GPS
? Augmenter Look Ahead Distance
? Réduire complexité de la scène (optimisation)
```

## ?? Support

Si vous rencontrez des problèmes après avoir suivi ce guide:

1. **Vérifier les fichiers de documentation:**
   - `Docs/GNSS_Physics_System.md` ? Détails techniques
   - `Docs/Vehicle_Configuration_Guide.md` ? Configurations avancées
   - `CHANGELOG_Physics_Implementation.md` ? Liste des changements

2. **Vérifier les logs Unreal:**
   - Window ? Developer Tools ? Output Log
   - Chercher "GNSS" ou "Trajectory"

3. **Tester en mode manuel:**
   - Désactiver GNSS Replay
   - Conduire manuellement (ZQSD)
   - Vérifier que la physique fonctionne

4. **Réinitialiser la configuration:**
   - Supprimer le véhicule de la scène
   - Re-drag depuis Content Browser
   - Reconfigurer depuis zéro

Bonne simulation ! ????
