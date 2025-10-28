# Système de Physique Réaliste pour le Replay GNSS

## Vue d'ensemble

Le système de replay GNSS a été amélioré pour utiliser la **physique réaliste du véhicule Chaos** au lieu de téléporter la voiture entre les points GPS. Cela permet d'avoir des collisions, des suspensions, et un comportement physique réaliste.

## Changements Principaux

### 1. Mode Physique Activé par Défaut
- **`bUsePhysicsMovement = true`** : La voiture utilise maintenant le moteur physique Chaos avec toutes ses fonctionnalités (suspensions, collisions, friction des pneus, etc.)
- Les anciens modes de téléportation sont toujours disponibles mais marqués comme "Legacy"

### 2. Contrôleur PID Complet

Le système utilise un contrôleur PID (Proportionnel-Intégral-Dérivé) pour deux aspects :

#### Contrôle de Direction (Steering)
- **Kp (Proportional)** : Réagit à l'erreur de cap actuelle
- **Ki (Integral)** : Élimine les erreurs de suivi à long terme
- **Kd (Derivative)** : Réduit les oscillations et l'overshoot

#### Contrôle de Vitesse (Throttle/Brake)
- **Kp, Ki, Kd** : Similaire au contrôle de direction
- Le système ajuste automatiquement le throttle et le frein selon la vitesse cible
- Freinage automatique dans les virages serrés

### 3. Système de Look-Ahead

Au lieu de viser directement le prochain waypoint, le véhicule regarde plus loin sur la trajectoire :
- **`LookAheadDistance`** : Distance de prévision (par défaut 1000 cm = 10 mètres)
- Permet des trajectoires plus lisses et naturelles

### 4. Lissage du Steering

- **`MaxSteeringRate`** : Limite la vitesse de changement du volant
- Évite les changements brusques qui déstabiliseraient le véhicule

## Paramètres Réglables (dans l'éditeur Unreal)

### Catégorie "GNSS Replay"
- **Enable GNSS Replay** : Active/désactive le mode replay
- **Use Physics Movement** : Active la physique réaliste (recommandé)

### Catégorie "GNSS Replay|PID Control"

#### Gains de Contrôle de Direction
- **Kp Yaw** (0.5) : Réactivité de la direction
- **Ki Yaw** (0.01) : Correction des erreurs persistantes
- **Kd Yaw** (0.1) : Amortissement des oscillations

#### Gains de Contrôle de Vitesse
- **Kp Speed** (0.1) : Réactivité à l'erreur de vitesse
- **Ki Speed** (0.005) : Correction des erreurs de vitesse
- **Kd Speed** (0.05) : Amortissement de l'accélération

#### Paramètres de Trajectoire
- **Accept Radius** (500 cm) : Distance pour considérer un waypoint atteint
- **Look Ahead Distance** (1000 cm) : Distance de prévision sur la trajectoire
- **Max Steering Rate** (2.0) : Vitesse maximale de rotation du volant

### Catégorie "GNSS Replay|Status" (Lecture seule)
- **Current Trajectory Index** : Point GPS actuel
- **Target Waypoint Index** : Point GPS visé (look-ahead)
- **Current Speed** : Vitesse actuelle en km/h
- **Target Speed** : Vitesse cible en km/h

## Comment Utiliser

### Dans l'Éditeur Unreal

1. Sélectionnez votre véhicule dans la scène
2. Dans le panneau "Details", trouvez la section "GNSS Replay"
3. Activez **Enable GNSS Replay**
4. Vérifiez que **Use Physics Movement** est activé
5. Ajustez les gains PID si nécessaire (les valeurs par défaut fonctionnent bien pour la plupart des cas)

### Ajustement des Gains PID

Si le véhicule :
- **Oscille trop** : Augmentez Kd (dérivé) ou réduisez Kp
- **Réagit trop lentement** : Augmentez Kp (proportionnel)
- **Dérive constamment** : Augmentez Ki (intégral)
- **Tourne trop brusquement** : Réduisez Max Steering Rate

### Modes Legacy (Déconseillés)

Pour revenir à l'ancien comportement de téléportation :
1. Désactivez **Use Physics Movement**
2. Les paramètres "Legacy" deviennent disponibles
3. Choisissez entre :
 - **Use Timestamp Playback** : Replay basé sur les timestamps GPS
   - Mode fixe : Téléportation à intervalle fixe

## Architecture Technique

### Boucle de Contrôle (Tick)

```
1. Obtenir position/vitesse actuelle du véhicule
2. Trouver le waypoint cible (avec look-ahead)
3. Calculer erreur de cap ? PID Steering
4. Calculer erreur de vitesse ? PID Speed
5. Convertir en commandes Throttle/Brake/Steering
6. Appliquer à ChaosVehicleMovement
7. Le moteur physique Chaos gère le reste (suspensions, collisions, etc.)
```

### Anti-Windup

Les termes intégraux sont limités pour éviter l'accumulation excessive d'erreurs :
- Intégral Yaw : [-100, 100]
- Intégral Speed : [-50, 50]

### Freinage Intelligent

Le système freine automatiquement si :
- La vitesse cible est inférieure à la vitesse actuelle
- L'angle de virage est supérieur à 45° ET vitesse > 20 km/h

## Avantages du Mode Physique

? **Réalisme** : Comportement physique authentique avec masse, inertie, suspensions
? **Collisions** : Le véhicule interagit avec l'environnement
? **Suspension** : Réactions réalistes aux bosses et aux variations du terrain
? **Friction** : Adhérence des pneus et comportement dans les virages
? **Debugging** : Vitesse et comportement observables en temps réel

## Compatibilité

- ? Fonctionne avec tous les véhicules dérivés de `ArcsimPawn`
- ? Compatible avec le système Chaos Vehicle d'Unreal Engine 5
- ? Préserve les anciennes fonctionnalités (modes Legacy)
- ? Fonctionne avec ou sans données de vitesse GPS

## Dépannage

### Le véhicule ne bouge pas
- Vérifiez que **Enable GNSS Replay** est activé
- Vérifiez qu'un **TrajectoryActor** avec des données GPS est présent
- Vérifiez les logs : "GNSS Physics: WP=..."

### Le véhicule oscille beaucoup
- Réduisez **Kp Yaw** de 0.5 à 0.3
- Augmentez **Kd Yaw** de 0.1 à 0.2
- Réduisez **Look Ahead Distance**

### Le véhicule va trop lentement
- Vérifiez que les données GPS contiennent des vitesses
- Augmentez **Kp Speed**
- Vérifiez la configuration du moteur dans le Blueprint du véhicule

### Le véhicule rate les virages
- Augmentez **Look Ahead Distance** (1500-2000 cm)
- Réduisez **Max Steering Rate** pour des virages plus amples
- Augmentez **Kp Yaw** pour une direction plus réactive

## Fichiers Modifiés

- `Source/rcsim/rcsimPawn.h` : Déclaration des nouveaux paramètres PID
- `Source/rcsim/rcsimPawn.cpp` : Implémentation du contrôleur PID et du système physique
