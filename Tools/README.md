# ??? Outils GNSS

Ce dossier contient des utilitaires pour travailler avec les fichiers GNSS CSV.

## convert_gnss_csv.py

Convertit les anciens fichiers CSV (6 colonnes avec `speed`) vers le nouveau format (5 colonnes sans `speed`).

### Utilisation

```bash
# Conversion basique (crée input_file_new.csv)
python convert_gnss_csv.py gps_loop_test.csv

# Spécifier le fichier de sortie
python convert_gnss_csv.py gps_loop_test.csv gps_new.csv
```

### Ancien Format (6 colonnes)
```csv
timestamp,lat,lon,alt,heading,speed
1698200000.0,45.123,5.680546,250.0,90.0,10.0
```

### Nouveau Format (5 colonnes)
```csv
timestamp,lat,lon,alt,heading
1698200000.0,45.123,5.680546,250.0,90.0
```

### Pourquoi Supprimer `speed` ?

La colonne `speed` est **redondante** car la vitesse peut être calculée automatiquement à partir de :
- **Position** (lat, lon, alt)
- **Timestamp**

**Vitesse = Distance parcourue / Temps écoulé**

Avantages :
- ? **Cohérent** : vitesse toujours exacte
- ? **Simple** : pas de confusion sur les unités (m/s vs km/h)
- ? **Fiable** : une seule source de vérité

Voir [../Docs/Speed_Column_Removal.md](../Docs/Speed_Column_Removal.md) pour plus de détails.

### Options

- **input_file** : Fichier CSV à convertir (obligatoire)
- **output_file** : Fichier CSV de sortie (optionnel, par défaut : `input_file_new.csv`)

### Exemples

#### Exemple 1 : Conversion simple
```bash
python convert_gnss_csv.py trajectory_2024.csv
# Crée : trajectory_2024_new.csv
```

#### Exemple 2 : Avec nom personnalisé
```bash
python convert_gnss_csv.py old_data.csv clean_data.csv
# Crée : clean_data.csv
```

#### Exemple 3 : Conversion par lot (Windows)
```powershell
Get-ChildItem *.csv | ForEach-Object {
  python convert_gnss_csv.py $_.Name
}
```

#### Exemple 4 : Conversion par lot (Linux/Mac)
```bash
for file in *.csv; do
    python convert_gnss_csv.py "$file"
done
```

### Messages de Sortie

**Succès :**
```
Success: Converted 600 data lines
Input:  gps_loop_test.csv
Output: gps_loop_test_new.csv
```

**Déjà au bon format :**
```
Info: gps_loop_test.csv is already in new format (5 columns)
```

**Erreur :**
```
Error: Unexpected number of columns: 7
Expected: timestamp,lat,lon,alt,heading,speed
Got: timestamp,lat,lon,alt,heading,speed,extra
```

### Vérification Après Conversion

1. Ouvrir le fichier converti dans un éditeur de texte
2. Vérifier l'en-tête : `timestamp,lat,lon,alt,heading`
3. Vérifier que chaque ligne a bien 5 valeurs séparées par des virgules

### Utilisation dans Unreal Engine

Après conversion :
1. Compiler le projet C++ (si pas déjà fait)
2. Lancer Unreal Engine
3. Charger le fichier CSV converti via l'interface GNSS Loader
4. Vérifier les logs :
   ```
   TrajectoryActor: Successfully loaded X GNSS points
   TrajectoryActor: Calculated speeds range: Y - Z km/h
   ```

Les vitesses sont maintenant calculées automatiquement et devraient être **cohérentes** avec les déplacements GPS.

## Autres Outils (À venir)

- **validate_gnss_csv.py** : Valider format et cohérence des données
- **resample_gnss.py** : Ré-échantillonner la trajectoire (changer fréquence)
- **smooth_gnss.py** : Lisser les données GPS bruitées
- **gnss_stats.py** : Statistiques sur une trajectoire (vitesse min/max/moyenne, durée, distance)

## Contribution

Pour ajouter un nouvel outil :
1. Créer un script Python dans ce dossier
2. Ajouter la documentation dans ce README
3. Fournir des exemples d'utilisation
