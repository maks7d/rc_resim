# ??? Outils GNSS

Ce dossier contient des utilitaires pour travailler avec les fichiers GNSS CSV.

## convert_gnss_csv.py

Convertit les anciens fichiers CSV (6 colonnes avec `speed`) vers le nouveau format (5 colonnes sans `speed`).

### Utilisation

```bash
# Conversion basique (cr�e input_file_new.csv)
python convert_gnss_csv.py gps_loop_test.csv

# Sp�cifier le fichier de sortie
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

La colonne `speed` est **redondante** car la vitesse peut �tre calcul�e automatiquement � partir de :
- **Position** (lat, lon, alt)
- **Timestamp**

**Vitesse = Distance parcourue / Temps �coul�**

Avantages :
- ? **Coh�rent** : vitesse toujours exacte
- ? **Simple** : pas de confusion sur les unit�s (m/s vs km/h)
- ? **Fiable** : une seule source de v�rit�

Voir [../Docs/Speed_Column_Removal.md](../Docs/Speed_Column_Removal.md) pour plus de d�tails.

### Options

- **input_file** : Fichier CSV � convertir (obligatoire)
- **output_file** : Fichier CSV de sortie (optionnel, par d�faut : `input_file_new.csv`)

### Exemples

#### Exemple 1 : Conversion simple
```bash
python convert_gnss_csv.py trajectory_2024.csv
# Cr�e : trajectory_2024_new.csv
```

#### Exemple 2 : Avec nom personnalis�
```bash
python convert_gnss_csv.py old_data.csv clean_data.csv
# Cr�e : clean_data.csv
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

**Succ�s :**
```
Success: Converted 600 data lines
Input:  gps_loop_test.csv
Output: gps_loop_test_new.csv
```

**D�j� au bon format :**
```
Info: gps_loop_test.csv is already in new format (5 columns)
```

**Erreur :**
```
Error: Unexpected number of columns: 7
Expected: timestamp,lat,lon,alt,heading,speed
Got: timestamp,lat,lon,alt,heading,speed,extra
```

### V�rification Apr�s Conversion

1. Ouvrir le fichier converti dans un �diteur de texte
2. V�rifier l'en-t�te : `timestamp,lat,lon,alt,heading`
3. V�rifier que chaque ligne a bien 5 valeurs s�par�es par des virgules

### Utilisation dans Unreal Engine

Apr�s conversion :
1. Compiler le projet C++ (si pas d�j� fait)
2. Lancer Unreal Engine
3. Charger le fichier CSV converti via l'interface GNSS Loader
4. V�rifier les logs :
   ```
   TrajectoryActor: Successfully loaded X GNSS points
   TrajectoryActor: Calculated speeds range: Y - Z km/h
   ```

Les vitesses sont maintenant calcul�es automatiquement et devraient �tre **coh�rentes** avec les d�placements GPS.

## Autres Outils (� venir)

- **validate_gnss_csv.py** : Valider format et coh�rence des donn�es
- **resample_gnss.py** : R�-�chantillonner la trajectoire (changer fr�quence)
- **smooth_gnss.py** : Lisser les donn�es GPS bruit�es
- **gnss_stats.py** : Statistiques sur une trajectoire (vitesse min/max/moyenne, dur�e, distance)

## Contribution

Pour ajouter un nouvel outil :
1. Cr�er un script Python dans ce dossier
2. Ajouter la documentation dans ce README
3. Fournir des exemples d'utilisation
