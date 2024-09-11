# **SML**

<!-- DOC HelpContext="Dokumentation" -->

<!-- DOCCONTENT
Eine vollständige Applikationsbeschreibung ist unter folgendem Link verfügbar: https://github.com/openknx/OFM-Meter/blob/v1/doc/Applikationsbeschreibung-Zaehler.md
DOCCONTENT -->

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

<!-- DOCEND -->

### **Verfügbare Kanäle**

Um die Applikation übersichtlicher zu gestalten, kann hier ausgewählt werden, wie viele Kanäle in der Applikation verfügbar und editierbar sind. Die Maximalanzahl der Kanäle hängt von der Firmware des Gerätes ab, dass dieses Modul verwendet.

Die ETS ist auch schneller in der Anzeige, wenn sie weniger (leere) Kanäle darstellen muss. Insofern macht es Sinn, nur so viele Kanäle anzuzeigen, wie man wirklich braucht.

<!-- DOC -->
### **Sperrobjekt**

Durch die Verwendung des Sperrobjekts kannst du vorübergehend das Erhöhen des Zählers pausieren. Sobald der Zähler wieder entsperrt wurde, zählt dieser normal weiter. Durch die Verwendung mehrerer Zähler in Kombination mit diesem Sperrobjekt, können beliebige Tarifzähler erstellt werden. Einheiten, die während der Sperrung gezählt wurden, werden somit nicht nachträglich aufsummiert.

<!-- DOC -->
### **Zählertyp**

Es gibt drei verschiedene Zählertypen:

#### Standardzähler

Der Standardzähler erhält als Eingang einen vorhandenen Zählerstand. Auf Grundlage dieses Zählerstands wird der interne Zähler entsprechend erhöht.

Dies ermöglicht nicht nur die Abbildung von Zwischenzählern, sondern auch von Tarifzählern. Hierbei werden einfach mehrere Zähler aus dem gleichen übergeordneten Zähler befüllt. Mithilfe des Reset-KO kann der Zwischenzähler zurückgesetzt werden. Das Sperr-KO ermöglicht die gezielte Aktivierung bzw. Deaktivierung bestimmter Zähler (Tarife). In Kombination mit dem Logikmodul (Zeitschaltuhr) können somit Zähler für Tages-, Wochen- und/oder Monatsverbrauch erstellt werden, ohne ein aktives Archiv zu haben.

**Hinweis**: Die internen Zähler arbeiten ausschließlich mit Ganzzahlen. Falls ein DPT14-Wert verwendet wird, sollte eine entsprechende Umrechnung erfolgen. Hierfür kann der Multiplikator genutzt werden.

#### Impulszähler

Der Impulszähler wertet die eingehenden EIN-Telegramme aus und erhöht den internen Zähler entsprechend. Diese Funktion ist besonders nützlich für S0-Zähler, die keine eigenen Zählerwerte liefern können. Eine zusätzliche Funktion besteht darin, aus dem Zählwert die Leistung bzw. den Durchfluss zu berechnen.

#### Betriebsstundenzähler

Der Betriebsstundenzähler erhält über ein Schalt-KO den Status eines zu überwachenden Gerätes und zählt intern die Sekunden, wie lange dieses im Betrieb ist. Auch hier können mehrere Zähler parallel arbeiten und bedarfsweise per Sperr- oder Reset-KO gesteuert werden.


<!-- DOC -->
### **Bestehendes KO**

Es besteht die Möglichkeit, ein bestehendes KO als Eingang zu nutzen. Der Hintergrund ist, dass unsere Module eigenständige und flexible Einheiten sind, die keinen direkten Bezug zu anderen Modulen haben.

Verfügt die verwendete Hardware jedoch über Binäreingänge oder SML-Reader, möchte man möglicherweise vermeiden, dass die Eingangsdaten auf dem Bus übertragen werden. In diesem Fall kann das Zählermodul direkt auf den Ausgang eines Binäreingangs oder SML-Readers zugreifen, um die Buslast gering zu halten.

Bei der Verwendung bestehender KOs ist es jedoch von großer Bedeutung, dass der Datentyp mit dem des bestehenden KOs übereinstimmt. Erwartet das Zählermodul beispielsweise einen DPT13, muss auch das bestehende KO diesen Datentyp verwenden. 
Die Applikation selbst kann dies nicht überprüfen. Bei falscher Verwendung kann dies sogar zu einem Absturz der Hardware führen.

<!-- DOC -->
### **Datentyp

Hier kann festgelegt werden, welcher Datentyp verwendet werden soll. Um so generisch wie möglich zu bleiben, wurde auf spezifische Datentypen verzichtet. Lediglich die Grundtypen DPT12 (ohne Vorzeichen), DPT13 (mit Vorzeichen) sowie DPT14 (Fließkomma) können ausgewählt werden. Technisch ist das jedoch unerheblich, da die Subtypen untereinander kompatibel sind. Mithilfe des Multiplikators können zudem die Zählerstände entsprechend umgerechnet bzw. konvertiert werden.

<!-- DOC -->
### Datentyp für Impulsezähler

Hier wird festgelegt, um welchen Typ von Impulszähler es sich handelt. Neben den beiden gängigen Datentypen kWh und m³ kann auch Individuell gewählt werden. Allerdings bieten nur die ersten beiden Datentypen die Möglichkeit, die Leistung bzw. den Durchfluss zu berechnen.

<!-- DOC -->
### Datentyp für Betriebsstundenzähler

Hier kannst du auswählen, welchen Datentyp du zur Ausgabe der Zeit verwenden möchtest.

<!-- DOC -->
### **Einheit**

Hier kann optional eine Einheit als Gedankestütze hinterlegt werden.

<!-- DOC -->
### **Multiplikator**

Mit Hilfe des Multiplikators kann der Eingangs- oder Ausgangswert umgerechnet bzw. konvertiert werden.

Hier einige Beispiele:

* Ein Eingangswert wird immer in eine Ganzzahl konvertiert. Der Wert 123,456 kWh würde so intern als 123 kWh betrachtet. Multipliziere ich nun den Wert mit 1000, erhalte ich 123456 Wh.
* Erhalte ich als Eingangswert 123456 Wh und möchte jedoch nur in kWh zählen, multipliziere ich den Wert mit 0,001 (entspricht einer Teilung durch 1000) und erhalte 123 kWh.
* Beim Ausgang verhält es sich genauso. Habe ich einen Zähler mit 123456 Wh, kann ich diesen mit 0,001 multiplizieren und erhalte bei DPT13 123 kWh bzw. bei DPT14 123,456 kWh.
* Auch kann man den Gasverbrauch (m³) direkt in kWh umrechnen und zählen lassen. Dazu wird der Brennwert (z.B. 8) mit der Zustandszahl (z.B. 0,95) multipliziert (=7,6) und erhält den Umrechnungsfaktor für den Multiplikator.

<!-- DOC -->
### **Rücklaufsperre**

Zähler mit einer Rücklaufsperre können nur positiv zählen. Änderungen, bei denen der Referenzzähler rückwärts läuft, werden dabei nicht berücksichtigt.

<!-- DOC -->
### **Maximale Differenz**

Hierbei wird der neue Referenzzählwert vom letzten Referenzzählwert subtrahiert. Ist die Differenz größer als der erlaubte Abstand, wird dieser Wert ignoriert.

Dies verhindert, dass der initiale Zählerstand des Referenzzählers als neuer Zählerstand übernommen wird. Bei einem Zählertausch wird die Differenz ebenfalls nicht mit dem internen Zähler verrechnet. Liefert ein Zähler aufgrund eines technischen Fehlers längere Zeit keine Werte, können diese ebenfalls ignoriert werden.

Dies ist besonders hilfreich, wenn der Zählerstand an ein Smart-Home-System übergeben wird. Hier würde in der Statistik sonst die große Differenz als Tagesverbrauch verbucht werden. Gleiches gilt für Zwischenzähler, die z.B. täglich zurückgesetzt werden.

Ist die maximale Differenz mit 0 angegeben, so ist die Funktion deaktiviert.

<!-- DOC -->
### **Ignoriere 0**

Einige Zähler liefern zu Beginn statt des korrekten Zählwerts eine 0. Um sicherzustellen, dass die Differenz zu 0 nicht gezählt wird, kann die 0 ignoriert werden.

<!-- DOC -->
### **Senden bei Änderung**

Hier kann angegeben werden, in welchem Abstand die Änderungen gesendet werden sollen.

Der Standardwert für Standardzähler beträgt 1 und bedeutet, dass bei jeder Änderung des Zählerstands ein Telegramm gesendet wird. Bei Zählern in Wh können so viele Telegramme zusammenkommen. Mit dem Wert 1000 würde nur noch bei jeder kWh ein Telegramm gesendet werden. Soll überhaupt keine Änderung gesendet werden, so kann mit 0 das Senden deaktiviert werden.

Der Standardwert für den Impulszähler liegt bei 1000 (wie der Standardwert für "Impulse je Einheit"). Das ist wichtig, da der interne Zähler die Impulse und nicht die kWh zählt. In Kombination mit einem DPT14 und einem Wert von 100 wird somit jede 0,1 kWh gesendet.

Alternativ kann mithilfe des Logikmoduls (Zeitschaltuhr) der Zählerstand mittels Read-Request auch zeitabhängig abgefragt und damit gesendet werden.

<!-- DOC -->
### **Berechnung**

Beim Impulszähler besteht die Option, die Leistung bzw. den Durchfluss berechnen zu lassen. Hierbei wird die Zeit zwischen den Impulsen gemessen, und die Leistung bzw. der Durchfluss wird berechnet.

#### Impulse je Einheit

Hier wird angegeben, wie viele Impulse eine Einheit repräsentieren. Viele Zähler arbeiten mit 1000 Impulsen je Einheit, was einem Impuls pro Wh oder Liter entspricht. Der genaue Wert steht in der Regel direkt auf dem Zähler.

#### Mindestwartezeit

Dabei verhindert die Mindestwartezeit bei hoher Leistung bzw. Durchfluss, dass die Ergebnisse ungenau werden (zu kurze Messstrecke). Außerdem verhindert es, dass zu viele Telegramme gesendet werden.

#### Abbruchzeit

Sollten keine Impulse mehr eintreffen, da der Verbrauch gerade 0 ist, wird der letzte Wert nicht mehr aktualisiert. Daher wird nach Ablauf der Abbruchzeit davon ausgegangen, dass gerade kein Verbrauch stattfindet, und es wird ein Wert von 0 für Leistung bzw. Durchfluss gesendet. Das Ganze ist ein prinzipbedingter Kompromiss, da ein Verbrauch von 0 nicht signalisiert werden kann.

<!-- DOC -->
### **Rückfallzeit**

Beim Betriebsstundenzähler ist es wichtig, den Betriebszustand zu kennen. Würde nun ein AUS-Telegramm verloren gehen, könnte das bedeuten, dass der Zähler dauerhaft weiterläuft. Um das zu verhindern, muss das EIN-Telegramm regelmäßig gesendet werden. Bleibt dies aus, schaltet der Betriebsstundenzähler nach Ablauf der Rückfallzeit automatisch ab. Die Zeit sollte so gewählt werden, dass mindestens 2 Status-Telegramme in dem Zeitraum eintreffen sollten.

**Tipp**: Sollte der Aktor kein zyklisches Senden unterstützen, kannst du mit dem Logikmodul regelmäßig Read-Requests auslösen.

<!-- DOC -->
### **Zählerstand**

Diese Seite ermöglicht die Verwaltung des Zählerstands, ohne dies über ein KO tun zu müssen. Der aktuelle Zählerstand kann direkt aus dem Gerät ausgelesen werden. Zudem ist es möglich, den Zählerstand neu zu setzen bzw. zurückzusetzen.

Bei einem Standardzähler gibt es zusätzlich einen internen Referenzzähler, der benötigt wird, um den relativen Verbrauch zu ermitteln. Dieser kann ebenfalls zurückgesetzt werden, falls man den Zähler komplett zurücksetzen möchte.

