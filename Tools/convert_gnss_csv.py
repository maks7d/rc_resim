#!/usr/bin/env python3
"""
Convert old GNSS CSV format (6 columns with speed) to new format (5 columns without speed).

Old format: timestamp,lat,lon,alt,heading,speed
New format: timestamp,lat,lon,alt,heading

The speed column is removed because speeds are now calculated automatically
from positions and timestamps in the C++ code.
"""

import sys
import os


def convert_csv(input_file, output_file=None):
    """
    Convert GNSS CSV from 6-column format to 5-column format.
    
    Args:
   input_file: Path to input CSV file
        output_file: Path to output CSV file (optional, defaults to input_file with _new suffix)
    """
    if output_file is None:
        base, ext = os.path.splitext(input_file)
   output_file = f"{base}_new{ext}"
    
    try:
        with open(input_file, 'r', encoding='utf-8') as f_in:
 lines = f_in.readlines()
        
   if not lines:
     print(f"Error: {input_file} is empty")
            return False
        
        # Check header
        header = lines[0].strip().split(',')
        
        if len(header) == 5 and header == ['timestamp', 'lat', 'lon', 'alt', 'heading']:
     print(f"Info: {input_file} is already in new format (5 columns)")
       return True
   
        if len(header) != 6:
  print(f"Error: Unexpected number of columns: {len(header)}")
            print(f"Expected: timestamp,lat,lon,alt,heading,speed")
         print(f"Got: {','.join(header)}")
            return False
    
   # Process lines
     output_lines = []
        output_lines.append('timestamp,lat,lon,alt,heading\n')

        converted_count = 0
  for i, line in enumerate(lines[1:], start=2):  # Skip header
            line = line.strip()
        if not line:  # Skip empty lines
   continue

        fields = line.split(',')
            if len(fields) == 6:
  # Remove last field (speed)
      new_line = ','.join(fields[:5]) + '\n'
 output_lines.append(new_line)
     converted_count += 1
            elif len(fields) == 5:
            # Already in new format
           output_lines.append(line + '\n')
             converted_count += 1
            else:
            print(f"Warning: Line {i} has {len(fields)} fields, skipping")
     
        # Write output
        with open(output_file, 'w', encoding='utf-8') as f_out:
            f_out.writelines(output_lines)
        
   print(f"Success: Converted {converted_count} data lines")
        print(f"Input:  {input_file}")
        print(f"Output: {output_file}")
        return True
  
    except FileNotFoundError:
        print(f"Error: File not found: {input_file}")
     return False
    except Exception as e:
     print(f"Error: {e}")
        return False


def main():
    if len(sys.argv) < 2:
      print("Usage: python convert_gnss_csv.py <input_file> [output_file]")
     print()
        print("Convert GNSS CSV from old format (6 columns with speed)")
    print("to new format (5 columns without speed).")
        print()
      print("Examples:")
   print("  python convert_gnss_csv.py gps_loop_test.csv")
        print("  python convert_gnss_csv.py gps_loop_test.csv gps_new.csv")
      sys.exit(1)
    
  input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else None
    
    success = convert_csv(input_file, output_file)
    sys.exit(0 if success else 1)


if __name__ == '__main__':
    main()
