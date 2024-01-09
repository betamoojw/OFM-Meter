### Multiplikator

Mit Hilfe des Multiplikators kann der Eingangs- oder Ausgangswert umgerechnet bzw. konvertiert werden.

Hier einige Beispiele:

* Ein Eingangswert wird immer in eine Ganzzahl konvertiert. Der Wert 123,456 kWh würde so intern als 123 kWh betrachtet. Multipliziere ich nun den Wert mit 1000, erhalte ich 123456 Wh.
* Erhalte ich als Eingangswert 123456 Wh und möchte jedoch nur in kWh zählen, multipliziere ich den Wert mit 0,001 (entspricht einer Teilung durch 1000) und erhalte 123 kWh.
* Beim Ausgang verhält es sich genauso. Habe ich einen Zähler mit 123456 Wh, kann ich diesen mit 0,001 multiplizieren und erhalte bei DPT13 123 kWh bzw. bei DPT14 123,456 kWh.
* Auch kann man den Gasverbrauch (m³) direkt in kWh umrechnen und zählen lassen. Dazu wird der Brennwert (z.B. 8) mit der Zustandszahl (z.B. 0,95) multipliziert (=7,6) und erhält den Umrechnungsfaktor für den Multiplikator.