### Modus

#### Standardzähler

Der Standardzähler erhält als Eingang einen vorhandenen Zählerstand. Auf Grundlage dieses Zählerstands wird der interne Zähler entsprechend erhöht.

Dies ermöglicht nicht nur die Abbildung von Zwischenzählern, sondern auch von Tarifzählern. Hierbei werden einfach mehrere Zähler aus dem gleichen übergeordneten Zähler befüllt. Mithilfe des Reset-KO kann der Zwischenzähler zurückgesetzt werden. Das Sperr-KO ermöglicht die gezielte Aktivierung bzw. Deaktivierung bestimmter Zähler (Tarife). In Kombination mit dem Logikmodul (Zeitschaltuhr) können somit Zähler für Tages-, Wochen- und/oder Monatsverbrauch erstellt werden, ohne ein aktives Archiv zu haben.

**Hinweis**: Die internen Zähler arbeiten ausschließlich mit Ganzzahlen. Falls ein DPT14-Wert verwendet wird, sollte eine entsprechende Umrechnung erfolgen. Hierfür kann der Multiplikator genutzt werden.

#### Impulszähler

Der Impulszähler wertet die eingehenden EIN-Telegramme aus und erhöht den internen Zähler entsprechend. Diese Funktion ist besonders nützlich für S0-Zähler, die keine eigenen Zählerwerte liefern können. Eine zusätzliche Funktion besteht darin, aus dem Zählwert die Leistung bzw. den Durchfluss zu berechnen.

#### Betriebsstundenzähler

Der Betriebsstundenzähler erhält über ein Schalt-KO den Status eines zu überwachenden Gerätes und zählt intern die Sekunden, wie lange dieses im Betrieb ist. Auch hier können mehrere Zähler parallel arbeiten und bedarfsweise per Sperr- oder Reset-KO gesteuert werden.
