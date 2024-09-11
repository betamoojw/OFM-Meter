### Berechnung

Beim Impulszähler besteht die Option, die Leistung bzw. den Durchfluss berechnen zu lassen. Hierbei wird die Zeit zwischen den Impulsen gemessen, und die Leistung bzw. der Durchfluss wird berechnet.

#### Impulse je Einheit

Hier wird angegeben, wie viele Impulse eine Einheit repräsentieren. Viele Zähler arbeiten mit 1000 Impulsen je Einheit, was einem Impuls pro Wh oder Liter entspricht. Der genaue Wert steht in der Regel direkt auf dem Zähler.

#### Mindestwartezeit

Dabei verhindert die Mindestwartezeit bei hoher Leistung bzw. Durchfluss, dass die Ergebnisse ungenau werden (zu kurze Messstrecke). Außerdem verhindert es, dass zu viele Telegramme gesendet werden.

#### Abbruchzeit

Sollten keine Impulse mehr eintreffen, da der Verbrauch gerade 0 ist, wird der letzte Wert nicht mehr aktualisiert. Daher wird nach Ablauf der Abbruchzeit davon ausgegangen, dass gerade kein Verbrauch stattfindet, und es wird ein Wert von 0 für Leistung bzw. Durchfluss gesendet. Das Ganze ist ein prinzipbedingter Kompromiss, da ein Verbrauch von 0 nicht signalisiert werden kann.

