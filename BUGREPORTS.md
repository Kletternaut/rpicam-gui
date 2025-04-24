# BUGREPORTS
**BUGNUMBER**
eine vortlaufende Nummerierung mit 4 Digits (0001-9999)

**status**
[o] = open
[c] = closed

**date**
date of bug reporting

**s-description**
eine kurze Beschreibung des Fehlers wann dieser und wie auftritt.

**l-description**
eine im Bedarfsfall ausführliche Beschreibung des Fehlers am Ende der BUGREPORTLIST
mit vorangestellter **BUGNUMBER**

**BUGREPORTLIST**
No.  state  date    -    description - 

**0001**  [o] 2025.04.25  qt-preview wird wieder als preview=--qt-preview in die config geschrieben

**0002**  [o] 2025.04.25  beim laden aus der config wird qt-preview= nicht übernommen

**0003**  [o] 2025.04.25  preview=0,100,200,200 ->Vorschaufenstermaße werden nicht in config gespeichert

**0004**  [o] 2025.04.25  dateinamen -o werden nicht aus der config geladen und ins output file: lineedit feld übertragen

**0005**  [o] 2025.04.25  beim jeweiligen browse button von post-process-file, load-config und save config stimmen die speicherpfade interhalb Dialoge nicht mit dem setup überein

**0006**  [c] 2025.04.25  
**0007**  [c] 2025.04.25  
**0008**  [c] 2025.04.25  
**0009**  [c] 2025.04.25  
**0010**  [c] 2025.04.25  
**0011**  [c] 2025.04.25  
**0012**  [c] 2025.04.25  
**0013**  [c] 2025.04.25  
**0014**  [c] 2025.04.25  
**0015**  [c] 2025.04.25  
**0016**  [c] 2025.04.25  
**0017**  [c] 2025.04.25  
**0018**  [c] 2025.04.25  
**0019**  [c] 2025.04.25  
**0020**  [c] 2025.04.25  
**0021**  [c] 2025.04.25  ##### diese zeile nicht verwenden #####







**0003** preview=0,100,200,200 also die Vorschaufenstermaße werden nicht mehr gespeichert,werden aber korrekt geladen, wenn sie in der config parametrisiert sind

**0005** beim jeweiligen browse button von post-process-file, load-config und save config stimmen die speicherpfade interhalb Dialoge nicht mit dem setup überein 

Um es zu definieren, 
das Overlay: lineedit korrespondiert mit config preview=

das preview: dropdown korrespondiert mit:
wenn --qt-preview = config qt-preview=
wenn --nopreview = config  nopreview=1
wenn --fullscreen = config fullscreen=1