# Syst�me de Physique R�aliste pour le Replay GNSS

## Vue d'ensemble

Le syst�me de replay GNSS a �t� am�lior� pour utiliser la **physique r�aliste du v�hicule Chaos** au lieu de t�l�porter la voiture entre les points GPS. Cela permet d'avoir des collisions, des suspensions, et un comportement physique r�aliste.

## Changements Principaux

### 1. Mode Physique Activ� par D�faut
- **`bUsePhysicsMovement = true`** : La voiture utilise maintenant le moteur physique Chaos avec toutes ses fonctionnalit�s (suspensions, collisions, friction des pneus, etc.)
- Les anciens modes de t�l�portation sont toujours disponibles mais marqu�s comme "Legacy"

### 2. Contr�leur PID Complet

Le syst�me utilise un contr�leur PID (Proportionnel-Int�gral-D�riv�) pour deux aspects :

#### Contr�le de Direction (Steering)
- **Kp (Proportional)** : R�agit � l'erreur de cap actuelle
- **Ki (Integral)** : �limine les erreurs de suivi � long terme
- **Kd (Derivative)** : R�duit les oscillations et l'overshoot

#### Contr�le de Vitesse (Throttle/Brake)
- **Kp, Ki, Kd** : Similaire au contr�le de direction
- Le syst�me ajuste automatiquement le throttle et le frein selon la vitesse cible
- Freinage automatique dans les virages serr�s

### 3. Syst�me de Look-Ahead

Au lieu de viser directement le prochain waypoint, le v�hicule regarde plus loin sur la trajectoire :
- **`LookAheadDistance`** : Distance de pr�vision (par d�faut 1000 cm = 10 m�tres)
- Permet des trajectoires plus lisses et naturelles

### 4. Lissage du Steering

- **`MaxSteeringRate`** : Limite la vitesse de changement du volant
- �vite les changements brusques qui d�stabiliseraient le v�hicule

## Param�tres R�glables (dans l'�diteur Unreal)

### Cat�gorie "GNSS Replay"
- **Enable GNSS Replay** : Active/d�sactive le mode replay
- **Use Physics Movement** : Active la physique r�aliste (recommand�)

### Cat�gorie "GNSS Replay|PID Control"

#### Gains de Contr�le de Direction
- **Kp Yaw** (0.5) : R�activit� de la direction
- **Ki Yaw** (0.01) : Correction des erreurs persistantes
- **Kd Yaw** (0.1) : Amortissement des oscillations

#### Gains de Contr�le de Vitesse
- **Kp Speed** (0.1) : R�activit� � l'erreur de vitesse
- **Ki Speed** (0.005) : Correction des erreurs de vitesse
- **Kd Speed** (0.05) : Amortissement de l'acc�l�ration

#### Param�tres de Trajectoire
- **Accept Radius** (500 cm) : Distance pour consid�rer un waypoint atteint
- **Look Ahead Distance** (1000 cm) : Distance de pr�vision sur la trajectoire
- **Max Steering Rate** (2.0) : Vitesse maximale de rotation du volant

### Cat�gorie "GNSS Replay|Status" (Lecture seule)
- **Current Trajectory Index** : Point GPS actuel
- **Target Waypoint Index** : Point GPS vis� (look-ahead)
- **Current Speed** : Vitesse actuelle en km/h
- **Target Speed** : Vitesse cible en km/h

## Comment Utiliser

### Dans l'�diteur Unreal

1. S�lectionnez votre v�hicule dans la sc�ne
2. Dans le panneau "Details", trouvez la section "GNSS Replay"
3. Activez **Enable GNSS Replay**
4. V�rifiez que **Use Physics Movement** est activ�
5. Ajustez les gains PID si n�cessaire (les valeurs par d�faut fonctionnent bien pour la plupart des cas)

### Ajustement des Gains PID

Si le v�hicule :
- **Oscille trop** : Augmentez Kd (d�riv�) ou r�duisez Kp
- **R�agit trop lentement** : Augmentez Kp (proportionnel)
- **D�rive constamment** : Augmentez Ki (int�gral)
- **Tourne trop brusquement** : R�duisez Max Steering Rate

### Modes Legacy (D�conseill�s)

Pour revenir � l'ancien comportement de t�l�portation :
1. D�sactivez **Use Physics Movement**
2. Les param�tres "Legacy" deviennent disponibles
3. Choisissez entre :
 - **Use Timestamp Playback** : Replay bas� sur les timestamps GPS
   - Mode fixe : T�l�portation � intervalle fixe

## Architecture Technique

### Boucle de Contr�le (Tick)

```
1. Obtenir position/vitesse actuelle du v�hicule
2. Trouver le waypoint cible (avec look-ahead)
3. Calculer erreur de cap ? PID Steering
4. Calculer erreur de vitesse ? PID Speed
5. Convertir en commandes Throttle/Brake/Steering
6. Appliquer � ChaosVehicleMovement
7. Le moteur physique Chaos g�re le reste (suspensions, collisions, etc.)
```

### Anti-Windup

Les termes int�graux sont limit�s pour �viter l'accumulation excessive d'erreurs :
- Int�gral Yaw : [-100, 100]
- Int�gral Speed : [-50, 50]

### Freinage Intelligent

Le syst�me freine automatiquement si :
- La vitesse cible est inf�rieure � la vitesse actuelle
- L'angle de virage est sup�rieur � 45� ET vitesse > 20 km/h

## Avantages du Mode Physique

? **R�alisme** : Comportement physique authentique avec masse, inertie, suspensions
? **Collisions** : Le v�hicule interagit avec l'environnement
? **Suspension** : R�actions r�alistes aux bosses et aux variations du terrain
? **Friction** : Adh�rence des pneus et comportement dans les virages
? **Debugging** : Vitesse et comportement observables en temps r�el

## Compatibilit�

- ? Fonctionne avec tous les v�hicules d�riv�s de `ArcsimPawn`
- ? Compatible avec le syst�me Chaos Vehicle d'Unreal Engine 5
- ? Pr�serve les anciennes fonctionnalit�s (modes Legacy)
- ? Fonctionne avec ou sans donn�es de vitesse GPS

## D�pannage

### Le v�hicule ne bouge pas
- V�rifiez que **Enable GNSS Replay** est activ�
- V�rifiez qu'un **TrajectoryActor** avec des donn�es GPS est pr�sent
- V�rifiez les logs : "GNSS Physics: WP=..."

### Le v�hicule oscille beaucoup
- R�duisez **Kp Yaw** de 0.5 � 0.3
- Augmentez **Kd Yaw** de 0.1 � 0.2
- R�duisez **Look Ahead Distance**

### Le v�hicule va trop lentement
- V�rifiez que les donn�es GPS contiennent des vitesses
- Augmentez **Kp Speed**
- V�rifiez la configuration du moteur dans le Blueprint du v�hicule

### Le v�hicule rate les virages
- Augmentez **Look Ahead Distance** (1500-2000 cm)
- R�duisez **Max Steering Rate** pour des virages plus amples
- Augmentez **Kp Yaw** pour une direction plus r�active

## Fichiers Modifi�s

- `Source/rcsim/rcsimPawn.h` : D�claration des nouveaux param�tres PID
- `Source/rcsim/rcsimPawn.cpp` : Impl�mentation du contr�leur PID et du syst�me physique
