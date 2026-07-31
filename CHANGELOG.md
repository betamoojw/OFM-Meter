# Changes

## 0.7.0

- Zählerstände und Referenzwerte werden an mehreren Stellen genauer
  berechnet (u. a. Rundung, Vorzeichenbehandlung bei negativen Werten).
- Das Sperr-Objekt eines Zählers wird jetzt inklusive seiner Polung
  korrekt berücksichtigt.
- Die maximale Dauer eines Impulses wird jetzt auf 120 Minuten
  begrenzt, damit ein hängender Impuls nicht dauerhaft einen falschen
  Zustand erzeugt.
- Kanalauswahl in der ETS überarbeitet.
- Bei Geräten mit aktiviertem Webserver zeigt eine neue Seite die
  Kanalinformationen im Browser an.
