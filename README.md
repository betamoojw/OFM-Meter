# OFM-Meter

Zählermodul für Werte, die nicht über ein eigenes Protokoll (wie SML
oder Wireless M-Bus) eingelesen werden, sondern direkt am KNX-Bus als
Wert-, Impuls- oder zeitgesteuerter Zähler geführt werden sollen.

## Funktionsumfang

Jeder Kanal arbeitet in einem von drei Modi:

- **Wert-Eingang** – ein KO liefert direkt den aktuellen Zählerstand.
- **Impulszähler** – Impulse (z. B. von einem S0-Zähler) werden
  gezählt und in einen Zählerstand umgerechnet.
- **Zeitgesteuert** – der Zähler läuft, solange ein Eingang aktiv ist
  (z. B. Betriebsstundenzähler).

Zusätzlich verfügbar: Sperr-Eingang (inkl. Polung), Referenzwert,
Reset, ein konfigurierbarer Ausgabefaktor sowie eine Konsolenausgabe
zur Kontrolle der aktuellen Zählerstände.

## Webseite

Auf Geräten mit aktiviertem Webserver ergänzt das Modul den Menüpunkt
*Zählermodul* (`/meter`) um eine Tabelle aller Kanäle: Modus (Standard-,
Impuls- oder Zeitzähler), interner Zählerstand und Referenzzähler.
Unkonfigurierte Kanäle werden als solche ausgewiesen. Die Seite ist rein
informativ — dort lässt sich nichts verändern.
