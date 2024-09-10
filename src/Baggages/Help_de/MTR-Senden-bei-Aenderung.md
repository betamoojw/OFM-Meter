### Senden bei Änderung

Hier kann angegeben werden, in welchem Abstand die Änderungen gesendet werden sollen.

Der Standardwert für Standardzähler beträgt 1 und bedeutet, dass bei jeder Änderung des Zählerstands ein Telegramm gesendet wird. Bei Zählern in Wh können so viele Telegramme zusammenkommen. Mit dem Wert 1000 würde nur noch bei jeder kWh ein Telegramm gesendet werden. Soll überhaupt keine Änderung gesendet werden, so kann mit 0 das Senden deaktiviert werden.

Der Standardwert für den Impulszähler liegt bei 1000 (wie der Standardwert für "Impulse je Einheit"). Das ist wichtig, da der interne Zähler die Impulse und nicht die kWh zählt. In Kombination mit einem DPT14 und einem Wert von 100 wird somit jede 0,1 kWh gesendet.

Alternativ kann mithilfe des Logikmoduls (Zeitschaltuhr) der Zählerstand mittels Read-Request auch zeitabhängig abgefragt und damit gesendet werden.

