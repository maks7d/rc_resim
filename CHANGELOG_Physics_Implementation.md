# Résumé des Changements - Système de Physique Réaliste GNSS

## Problème Résolu
La voiture suivait les points GPS mais utilisait la téléportation au lieu de la physique réaliste, ce qui ne permettait pas d'avoir des collisions, des suspensions, ou un comportement physique authentique.

## Solution Implémentée
Remplacement du système de téléportation par un **contrôleur PID complet** qui utilise le moteur physique **Chaos Vehicle** d'Unreal Engine.

## Fichiers Modifiés

### 1. `Source/rcsim/rcsimPawn.h`
**Changements principaux :**
- ? `bUsePhysicsMovement = true` par défaut (au lieu de false)
- ? Ajout de paramètres PID complets (Kp, Ki, Kd) pour direction et vitesse
- ? Ajout de `LookAheadDistance` pour un suivi de trajectoire plus lisse
- ? Ajout de `MaxSteeringRate` pour éviter les changements brusques
- ? Ajout de variables d'état PID (intégrale, dérivée, lissage)
- ? Nouveaux paramètres de debug (CurrentSpeed, TargetSpeed, TargetWaypointIndex)
- ? Organisation des paramètres en catégories (PID Control, Legacy, Status)

**Nouveaux paramètres ajoutés :**
```cpp
// Direction (Steering)
float KpYaw = 0.5f;         // Gain proportionnel
float KiYaw = 0.01f;    // Gain intégral
float KdYaw = 0.1f;// Gain dérivé

// Vitesse (Speed)
float KpSpeed = 0.1f;       // Gain proportionnel
float KiSpeed = 0.005f;     // Gain intégral
float KdSpeed = 0.05f;      // Gain dérivé

// Contrôle de trajectoire
float LookAheadDistance = 1000.0f;  // 10 mètres
float MaxSteeringRate = 2.0f;       // Lissage du volant
float AcceptRadius = 500.0f;        // 5 mètres

// État du contrôleur (privé)
float IntegralErrorYaw = 0.0f;
float PreviousErrorYaw = 0.0f;
float IntegralErrorSpeed = 0.0f;
float PreviousErrorSpeed = 0.0f;
float SmoothedSteeringInput = 0.0f;
```

### 2. `Source/rcsim/rcsimPawn.cpp`
**Changements principaux dans `Tick()` :**

#### A. Contrôle de Direction (PID Complet)
```cpp
// Calcul de l'erreur de cap
float YawError = FMath::FindDeltaAngleDegrees(CurrentYaw, DesiredYaw);

// Terme Proportionnel
float P = KpYaw * YawError;

// Terme Intégral (avec anti-windup)
IntegralErrorYaw += YawError * Delta;
IntegralErrorYaw = FMath::Clamp(IntegralErrorYaw, -100.0f, 100.0f);
float I = KiYaw * IntegralErrorYaw;

// Terme Dérivé
float DerivativeYaw = (YawError - PreviousErrorYaw) / Delta;
float D = KdYaw * DerivativeYaw;

// Commande finale
float SteeringCommand = P + I + D;
```

#### B. Système de Look-Ahead
Au lieu de viser directement le prochain waypoint, le véhicule cherche un point plus loin sur la trajectoire :
```cpp
TargetWaypointIndex = CurrentTrajectoryIndex;
float AccumulatedDistance = 0.0f;

for (int32 i = CurrentTrajectoryIndex; i < TrajectoryRef->Positions.Num() - 1; ++i)
{
    float SegmentLength = FVector::Dist(TrajectoryRef->Positions[i], TrajectoryRef->Positions[i + 1]);
    AccumulatedDistance += SegmentLength;
    
    if (AccumulatedDistance >= LookAheadDistance)
    {
        TargetWaypointIndex = i + 1;
        break;
    }
}
```

#### C. Contrôle de Vitesse avec Données GPS
```cpp
// Utilise la vitesse des données GNSS si disponible
if (TrajectoryRef->Speeds.IsValidIndex(TargetWaypointIndex))
{
    TargetSpeed = TrajectoryRef->Speeds[TargetWaypointIndex];
}

// PID pour calculer throttle/brake
float SpeedError = TargetSpeed - CurrentSpeed;
float SpeedCommand = (KpSpeed * SpeedError) + (KiSpeed * IntegralErrorSpeed) + (KdSpeed * DerivativeSpeed);

// Conversion en commandes moteur
if (SpeedCommand > 0.0f)
{
    ThrottleInput = FMath::Clamp(SpeedCommand, 0.0f, 1.0f);
    BrakeInput = 0.0f;
}
else
{
    ThrottleInput = 0.0f;
    BrakeInput = FMath::Clamp(-SpeedCommand * 0.5f, 0.0f, 1.0f);
}
```

#### D. Freinage Intelligent dans les Virages
```cpp
// Freine automatiquement si virage serré à haute vitesse
if (FMath::Abs(YawError) > 45.0f && CurrentSpeed > 20.0f)
{
    BrakeInput = FMath::Max(BrakeInput, 0.3f);
    ThrottleInput = 0.0f;
}
```

#### E. Application au Moteur Physique Chaos
```cpp
// Ces commandes sont appliquées au système physique complet
ChaosVehicleMovement->SetSteeringInput(SmoothedSteeringInput);
ChaosVehicleMovement->SetThrottleInput(ThrottleInput);
ChaosVehicleMovement->SetBrakeInput(BrakeInput);

// Chaos gère automatiquement :
// - Suspensions (roues indépendantes)
// - Friction des pneus
// - Collisions avec l'environnement
// - Transfert de poids
// - Anti-roulis
// - Etc.
```

**Changements dans `ResetGNSSReplay()` :**
```cpp
// Réinitialisation des états PID
IntegralErrorYaw = 0.0f;
PreviousErrorYaw = 0.0f;
IntegralErrorSpeed = 0.0f;
PreviousErrorSpeed = 0.0f;
SmoothedSteeringInput = 0.0f;

// Réinitialisation des inputs véhicule
ChaosVehicleMovement->SetSteeringInput(0.0f);
ChaosVehicleMovement->SetThrottleInput(0.0f);
ChaosVehicleMovement->SetBrakeInput(0.0f);
```

### 3. `Docs/GNSS_Physics_System.md` (Nouveau)
Documentation complète du système avec :
- Vue d'ensemble de l'architecture
- Explication des paramètres
- Guide d'ajustement des gains PID
- Conseils de dépannage
- Comparaison modes physique vs téléportation

## Fonctionnalités Physiques Activées

Avec ce système, le véhicule bénéficie maintenant de :

### 1. **Système de Suspension Réaliste**
- 4 roues indépendantes avec amortisseurs
- Réaction aux bosses et dépressions du terrain
- Transfert de poids dans les virages et freinages

### 2. **Système de Collision**
- Détection de collision avec l'environnement
- Réponse physique aux impacts
- Possibilité de heurter des obstacles

### 3. **Modèle de Pneu Avancé**
- Friction latérale et longitudinale
- Glissement contrôlé
- Adhérence variable selon le terrain

### 4. **Dynamique du Véhicule**
- Masse et inertie réalistes
- Centre de gravité
- Moments d'inertie
- Anti-roulis

### 5. **Moteur et Transmission**
- Courbe de couple moteur
- Boîte de vitesses automatique
- Frein moteur
- Différentiel

## Comparaison Avant/Après

### AVANT (Téléportation)
```cpp
// Ancien code
SetActorLocationAndRotation(TargetPos, TargetRot, false, nullptr, ETeleportType::TeleportPhysics);
GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);

// ? Pas de physique réelle
// ? Pas de collisions
// ? Pas de suspensions
// ? Téléportation visible
// ? Pas de comportement naturel
```

### APRÈS (Physique Réaliste)
```cpp
// Nouveau code
ChaosVehicleMovement->SetSteeringInput(SmoothedSteeringInput);
ChaosVehicleMovement->SetThrottleInput(ThrottleInput);
ChaosVehicleMovement->SetBrakeInput(BrakeInput);

// ? Physique complète
// ? Collisions actives
// ? Suspensions fonctionnelles
// ? Mouvement fluide et naturel
// ? Comportement réaliste dans les virages
// ? Réaction au terrain
```

## Utilisation dans Unreal Editor

1. **Ouvrir le projet** dans Unreal Engine
2. **Compiler** le code C++ (Build ? Compile)
3. **Sélectionner le véhicule** dans la scène
4. **Dans le panneau Details**, section "GNSS Replay" :
   - ? Activer "Enable GNSS Replay"
   - ? Vérifier que "Use Physics Movement" est activé (par défaut)
   - Ajuster les gains PID si nécessaire

5. **Lancer le jeu** (Play)
6. **Observer** : La voiture suit maintenant la trajectoire GPS avec physique réaliste !

## Logs de Debug

Le système affiche des logs chaque seconde :
```
GNSS Physics: WP=15/150, Speed=35.2/40.0 km/h, YawErr=12.3°, Steer=0.25, Throttle=0.45, Brake=0.00
```

Signification :
- **WP** : Waypoint actuel / total
- **Speed** : Vitesse actuelle / vitesse cible (km/h)
- **YawErr** : Erreur de cap en degrés
- **Steer** : Commande de direction (-1 à 1)
- **Throttle** : Accélérateur (0 à 1)
- **Brake** : Frein (0 à 1)

## Test Recommandé

Pour vérifier que tout fonctionne :

1. Charger un fichier CSV GNSS via l'interface
2. Activer GNSS Replay sur le véhicule
3. Démarrer le jeu
4. Observer :
   - ? La voiture doit suivre la trajectoire de manière fluide
   - ? Les roues doivent tourner et se suspendre
   - ? Pas de téléportation visible
   - ? Comportement réaliste dans les virages
   - ? Réaction aux variations du terrain

## Prochaines Améliorations Possibles

- ?? Ajout d'un mode "Record" pour enregistrer de nouvelles trajectoires
- ?? Visualisation des waypoints et du look-ahead en temps réel
- ?? Interface UI pour ajuster les gains PID en jeu
- ?? Mode "Comparaison" entre trajectoire réelle et trajectoire GPS
- ?? Auto-tuning des gains PID selon le type de terrain

## Support

Pour toute question ou problème :
1. Vérifier les logs Unreal Engine
2. Consulter `Docs/GNSS_Physics_System.md`
3. Ajuster les gains PID dans l'éditeur
4. Vérifier que les données GPS sont valides
