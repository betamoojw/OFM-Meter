### Maximale Differenz

Hierbei wird der neue Referenzzählwert vom letzten Referenzzählwert subtrahiert. Ist die Differenz größer als der erlaubte Abstand, wird dieser Wert ignoriert.

Dies verhindert, dass der initiale Zählerstand des Referenzzählers als neuer Zählerstand übernommen wird. Bei einem Zählertausch wird die Differenz ebenfalls nicht mit dem internen Zähler verrechnet. Liefert ein Zähler aufgrund eines technischen Fehlers längere Zeit keine Werte, können diese ebenfalls ignoriert werden.

Dies ist besonders hilfreich, wenn der Zählerstand an ein Smart-Home-System übergeben wird. Hier würde in der Statistik sonst die große Differenz als Tagesverbrauch verbucht werden. Gleiches gilt für Zwischenzähler, die z.B. täglich zurückgesetzt werden.

Ist die maximale Differenz mit 0 angegeben, so ist die Funktion deaktiviert.