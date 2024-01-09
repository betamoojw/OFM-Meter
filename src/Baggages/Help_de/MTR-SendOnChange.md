### Senden bei Änderung

Hier kann angegeben werden, in welchem Abstand die Änderungen gesendet werden sollen.

Der Standardwert beträgt 1 und bedeutet, dass bei jeder Änderung des Zählerstands ein Telegramm gesendet wird. Bei Zählern in Wh können so viele Telegramme zusammenkommen. Mit dem Wert 1000 würde nur noch bei jeder kWh ein Telegramm gesendet werden. Soll überhaupt keine Änderung gesendet werden, so kann mit 0 das Senden deaktiviert werden.

Alternativ kann mithilfe des Logikmoduls (Zeitschaltuhr) der Zählerstand mittels Read-Request auch zeitabhängig abgefragt werden.