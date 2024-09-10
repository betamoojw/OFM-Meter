### Dokumentation


Eine vollständige Applikationsbeschreibung ist unter folgendem Link verfügbar: https://github.com/openknx/OFM-Meter/blob/v1/doc/Applikationsbeschreibung-Zaehler.md

Das Zählermodul ist ein generisches Modul, das verschiedene Zählerlogiken bereitstellt. Aktuell gibt es drei verschiedene Zählertypen:

* **Standardzähler** für vorhandene Zähler
* **Impulszähler** zum Zählen von S0-Schnittstellen, einschließlich Leistungsermittlung
* **Betriebsstundenzähler** zur Ermittlung der Laufzeit von Geräten

Das Modul ist flexibel gestaltet, sodass es auch mit verschiedenen Datentypen umgehen kann.

Hier einige Beispiele, wofür das Modul verwendet werden kann:

* Verwende den Impulszähler in Kombination mit einem Binäreingang, um den Verbrauch einer S0-Schnittstelle zu messen.
* Nutze den Standardzähler, um einen eigenen Tarifzähler zu betreiben.
* Verwende den Standardzähler, um Werte von Wh in kWh umzurechnen.
* Nutze den Standardzähler, um Zwischenzähler (z. B. täglich) zu erhalten.
* Verwende den Betriebsstundenzähler, um die Laufzeit einer Pumpe oder einer Lampe zu ermitteln.
* Kaskadiere Zähler, um verschiedene Funktionen kombinieren zu können.

