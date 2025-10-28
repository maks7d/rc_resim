# Guide de D�marrage Rapide - Physique R�aliste GNSS

## ?? Objectif
Faire fonctionner votre v�hicule avec la physique r�aliste (suspensions, collisions, etc.) au lieu de la t�l�portation.

## ? D�marrage en 3 Minutes

### �tape 1: Compiler le Code (30 secondes)
```
1. Ouvrir le projet dans Unreal Engine
2. Menu: Build ? Compile rcsim C++ Code
3. Attendre la fin de la compilation
```

### �tape 2: Configurer le V�hicule (1 minute)
```
1. Dans la sc�ne, s�lectionner votre v�hicule (ex: BP_SportsCar)
2. Panneau Details ? Section "GNSS Replay"
3. Cocher: ? Enable GNSS Replay
4. V�rifier: ? Use Physics Movement (doit �tre d�j� coch� par d�faut)
5. Laisser les autres valeurs par d�faut
```

### �tape 3: Charger des Donn�es GPS (1 minute)
```
1. Lancer le jeu (bouton Play)
2. Cliquer sur le bouton "?? Import GNSS File" dans l'interface
3. S�lectionner votre fichier CSV GNSS
4. Le v�hicule d�marre automatiquement en mode autonome !
```

## ? V�rification Visuelle

### Ce que vous DEVEZ voir:
- ? La voiture roule de mani�re fluide (pas de t�l�portation)
- ? Les roues tournent et se suspendent
- ? Le v�hicule penche dans les virages
- ? R�action r�aliste aux bosses du terrain
- ? Logs dans la console: "GNSS Physics: WP=..."

### Ce que vous NE devez PAS voir:
- ? T�l�portation saccad�e
- ? Roues qui ne tournent pas
- ? V�hicule qui flotte sans toucher le sol
- ? Logs d'erreur rouges

## ?? Ajustements de Base

### Si le v�hicule oscille (zigzague):
```
Panneau Details ? GNSS Replay | PID Control
- R�duire "Kp Yaw" de 0.5 ? 0.3
- Augmenter "Look Ahead Distance" de 1000 ? 1500
```

### Si le v�hicule va trop lentement:
```
Panneau Details ? GNSS Replay | PID Control
- Augmenter "Kp Speed" de 0.1 ? 0.15
```

### Si le v�hicule rate les virages:
```
Panneau Details ? GNSS Replay | PID Control
- Augmenter "Look Ahead Distance" de 1000 ? 1500
- Augmenter "Kp Yaw" de 0.5 ? 0.6
```

## ?? Comprendre les Param�tres (Simple)

### Les 3 Param�tres Principaux

#### 1. Look Ahead Distance (Distance de Pr�vision)
```
Valeur: 1000 cm = 10 m�tres

?? Qu'est-ce que c'est?
Au lieu de viser le prochain point GPS, la voiture regarde plus loin sur la trajectoire.

?? Plus bas (500) ? Virages plus serr�s mais risque de zigzag
?? Plus haut (2000) ? Trajectoire plus lisse mais virages moins pr�cis

?? Recommandation: 1000-1500 pour conduite urbaine
```

#### 2. Kp Yaw (R�activit� de Direction)
```
Valeur: 0.5

?? Qu'est-ce que c'est?
Contr�le la force avec laquelle le volant tourne pour corriger la direction.

?? Plus bas (0.3) ? Conduite plus douce, moins de corrections
?? Plus haut (0.8) ? Conduite plus agressive, corrections rapides

?? Recommandation: 0.3-0.5 pour conduite normale
```

#### 3. Kp Speed (R�activit� de Vitesse)
```
Valeur: 0.1

?? Qu'est-ce que c'est?
Contr�le la force avec laquelle l'acc�l�rateur/frein r�agit.

?? Plus bas (0.05) ? Acc�l�ration/freinage doux
?? Plus haut (0.2) ? Acc�l�ration/freinage agressif

?? Recommandation: 0.1-0.15 pour conduite normale
```

## ?? Modes Disponibles

### Mode 1: Physique R�aliste (RECOMMAND�) ?
```
Use Physics Movement: ? Coch�

Avantages:
+ Physique compl�te (suspensions, collisions, friction)
+ Comportement r�aliste
+ R�agit au terrain
+ Peut percuter des obstacles

Id�al pour:
? Simulation r�aliste
? Test de v�hicules
? Environnements avec obstacles
```

### Mode 2: T�l�portation avec Timestamps
```
Use Physics Movement: ? D�coch�
Use Timestamp Playback: ? Coch�

Avantages:
+ Suit exactement les timestamps GPS
+ Pas d'�cart de trajectoire

Inconv�nients:
- Pas de physique r�elle
- T�l�portation visible
- Pas de collisions

Id�al pour:
? Visualisation de donn�es GPS brutes
? D�buggage de trajectoires
```

### Mode 3: T�l�portation � Intervalle Fixe
```
Use Physics Movement: ? D�coch�
Use Timestamp Playback: ? D�coch�
Trajectory Point Delay: 0.1 sec

?? Mode legacy, non recommand�
```

## ?? Exemples de Configuration Pr�d�finis

### Configuration 1: Ville (30-50 km/h)
```
[GNSS Replay | PID Control]
Kp Yaw: 0.5
Look Ahead Distance: 1000
Accept Radius: 500
Kp Speed: 0.1

R�sultat: Conduite fluide en ville avec virages mod�r�s
```

### Configuration 2: Autoroute (80-120 km/h)
```
[GNSS Replay | PID Control]
Kp Yaw: 0.3              ? Moins r�actif = plus stable
Look Ahead Distance: 2000 ? Regarde plus loin
Accept Radius: 800
Kp Speed: 0.15           ? Plus r�actif en vitesse

R�sultat: Conduite stable � haute vitesse
```

### Configuration 3: Circuit Sportif
```
[GNSS Replay | PID Control]
Kp Yaw: 0.8     ? Tr�s r�actif
Look Ahead Distance: 800  ? Vision courte = virages serr�s
Accept Radius: 300
Kp Speed: 0.2    ? Acc�l�ration/freinage agressif

R�sultat: Conduite sportive avec virages serr�s
```

## ?? Probl�mes Courants et Solutions

### Probl�me: "Le v�hicule ne bouge pas du tout"

**Diagnostic:**
```
1. V�rifier dans Output Log:
   ? "GNSS Replay: Auto-found TrajectoryActor"
   ? "TrajectoryActor: Loaded X GNSS points"
   
2. Si aucun log ? Pas de donn�es GPS charg�es
```

**Solution:**
```
1. Cliquer sur "?? Import GNSS File"
2. Charger un fichier CSV valide
3. V�rifier format: timestamp,lat,lon,alt,heading,speed
```

### Probl�me: "Le v�hicule t�l�porte au lieu de rouler"

**Diagnostic:**
```
Panneau Details ? GNSS Replay
? "Use Physics Movement" n'est pas coch�
```

**Solution:**
```
? Cocher "Use Physics Movement"
```

### Probl�me: "Le v�hicule traverse le sol"

**Diagnostic:**
```
Le niveau n'a pas de collision configur�e
```

**Solution:**
```
1. V�rifier que le terrain a "Collision Enabled"
2. Dans le v�hicule: Details ? Mesh ? Collision Preset = "Vehicle"
```

### Probl�me: "Logs d'erreur: No TrajectoryActor found"

**Diagnostic:**
```
Aucun TrajectoryActor n'existe dans la sc�ne
```

**Solution:**
```
1. Dans Content Browser: rechercher "TrajectoryActor"
2. Glisser-d�poser dans la sc�ne
3. OU laisser le syst�me le cr�er automatiquement (GNSSGameMode)
```

## ?? Checklist Compl�te de V�rification

### Avant de Lancer le Jeu

- [ ] Code compil� sans erreur
- [ ] V�hicule plac� dans la sc�ne
- [ ] "Enable GNSS Replay" coch�
- [ ] "Use Physics Movement" coch�
- [ ] TrajectoryActor pr�sent dans la sc�ne
- [ ] Fichier CSV GNSS pr�par�

### Pendant le Jeu

- [ ] Interface GNSS Loader visible
- [ ] Fichier CSV charg� avec succ�s
- [ ] Logs "GNSS Physics: WP=..." apparaissent
- [ ] V�hicule roule de mani�re fluide
- [ ] Roues tournent et se suspendent

### Si Probl�me

- [ ] Consulter Output Log (Window ? Developer Tools ? Output Log)
- [ ] V�rifier les valeurs PID
- [ ] Tester avec configuration pr�d�finie "Ville"
- [ ] D�sactiver/r�activer GNSS Replay

## ?? Comprendre les Logs

### Log Normal (Tout fonctionne)
```
LogTemp: Log: GNSS Physics: WP=15/150, Speed=35.2/40.0 km/h, YawErr=12.3�, Steer=0.25, Throttle=0.45, Brake=0.00
```

**Signification:**
- WP=15/150 ? Au waypoint 15 sur 150 total
- Speed=35.2/40.0 ? Vitesse actuelle / vitesse cible (km/h)
- YawErr=12.3� ? Erreur de direction (angle � corriger)
- Steer=0.25 ? Volant tourn� � 25% vers la droite
- Throttle=0.45 ? Acc�l�rateur � 45%
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

**Action:** Ajouter un TrajectoryActor ou laisser GNSSGameMode le cr�er

## ?? Objectifs de Performance

### Cible � Atteindre

```
? Erreur de cap (YawErr): < 20� en moyenne
? Erreur de vitesse: < 5 km/h
? Distance au waypoint: D�croissante jusqu'� Accept Radius
? Framerate: > 60 FPS
? Pas de t�l�portation visible
```

### Si Non Atteint

```
? Ajuster les gains PID
? V�rifier qualit� des donn�es GPS
? Augmenter Look Ahead Distance
? R�duire complexit� de la sc�ne (optimisation)
```

## ?? Support

Si vous rencontrez des probl�mes apr�s avoir suivi ce guide:

1. **V�rifier les fichiers de documentation:**
   - `Docs/GNSS_Physics_System.md` ? D�tails techniques
   - `Docs/Vehicle_Configuration_Guide.md` ? Configurations avanc�es
   - `CHANGELOG_Physics_Implementation.md` ? Liste des changements

2. **V�rifier les logs Unreal:**
   - Window ? Developer Tools ? Output Log
   - Chercher "GNSS" ou "Trajectory"

3. **Tester en mode manuel:**
   - D�sactiver GNSS Replay
   - Conduire manuellement (ZQSD)
   - V�rifier que la physique fonctionne

4. **R�initialiser la configuration:**
   - Supprimer le v�hicule de la sc�ne
   - Re-drag depuis Content Browser
   - Reconfigurer depuis z�ro

Bonne simulation ! ????
