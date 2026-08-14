# AGENTS für OFM-Meter

## Ziel

Zählermodul für Werte, die direkt am KNX-Bus als Wert-, Impuls- oder
zeitgesteuerter Zähler geführt werden — im Gegensatz zu SML oder
Wireless M-Bus, die eigene Protokolle auswerten.

## Basis: OGM-Common

Dieses OFM baut auf `../OGM-Common/AGENTS.md` auf (Modulsystem, LED-/
Flash-/Zeit-API, Logging, Embedded-Rahmenbedingungen, Code-Konventionen)
— dort beschriebene Regeln und vorhandene Claude-Skills/Agents (z. B.
das Kanalauswahl-Muster) gelten auch hier.

## Prefix und Namenskonventionen

- **Modulprefix**: `MTR`
- **C++-Klassen**: `MeterModule` (`name()`: `"Meter"`), `MeterChannel`
- **Kanalanzahl**: `MTR_ChannelCount`, vorgegeben durch das OAM

## Kanal-Modi (`_mode`)

1. **Wert-Eingang**: `KoMTR_ChannelInput` liefert direkt den
   Zählerstand.
2. **Impulszähler** (nur wenn `ParamMTR_ChannelPulseType` gesetzt):
   `pulse()`/`loopPulse()`/`pulseCalculate()` rechnen Impulse in einen
   Zählerstand um; `abortPulseCalculate()` bricht die Berechnung ab,
   wenn zwischen zwei Impulsen zu viel Zeit vergeht.
3. **Zeitgesteuert**: `loopTimer()`/`startTimer()`/`stopTimer()`/
   `processTimerCalculation()` — der Zähler läuft, solange der Eingang
   aktiv ist.

`_outModifier` ist bewusst `double`, nicht `float` — bei einem
Multiplikator von 1 muss ein `uint32_t`-Zählerstand unverändert
durchlaufen, `float` hätte ab 16,7 Mio. durch die 24-Bit-Mantisse
gerundet.

## Sperre, Referenz, Reset

- `processInputKoLock()` setzt `_locked`; im Modus 3 (zeitgesteuert)
  stoppt eine aktive Sperre den Timer.
- `processInputKoReset()` setzt den Zähler auf den konfigurierten
  Referenzwert zurück (`reset(bool full)` — `full` setzt zusätzlich
  interne Zwischenwerte zurück, nicht nur den Zählerstand).
- `counterTypeSigned()`/`referenceTypeSigned()` legen fest, ob
  Zählerstand bzw. Referenzwert vorzeichenbehaftet interpretiert
  werden.

## Webseite (`/meter`)

Unter `#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))`
registriert `MeterModule::setup()` Menüpunkt und Route `/meter` — eine
read-only Kanaltabelle (Modus, interner Zählerstand, Referenzzähler),
gebaut direkt im Route-Lambda.

- Registriert wird nur bei `knx.configured()`, und das Lambda prüft es
  erneut: die Route bleibt registriert, wenn die Konfiguration zur
  Laufzeit wegfällt, und antwortet dann mit 404 statt auf ungültige
  Kanaldaten zuzugreifen.
- Vorzeichen folgen `counterTypeSigned()`/`referenceTypeSigned()` — die
  Anzeige muss dieselbe Interpretation nutzen wie der Sendepfad.
- Neue Modi brauchen einen Zweig im `switch (ch->mode())` der Seite,
  sonst steht dort `—`.

## Regeln für Weiterentwicklung

1. Neue Modi bekommen eine eigene Nummer in `_mode` und eigene
   `process*`-Methoden — keine Vermischung der Modus-Zweige in
   `loop()`/`processInputKo()`.
2. Rundung und Vorzeichenbehandlung sind an mehreren Stellen bewusst
   in `double` gehalten (siehe `_outModifier`) — nicht ohne Grund auf
   `float` zurückstufen.
3. `MeterModule` verwaltet nur das Channel-Array und leitet KOs/Konsole
   weiter — Zähler-Logik gehört in `MeterChannel`.

## Dokumentation und Hilfe

- Dokumentation liegt in `doc/Applikationsbeschreibung-Zaehler.md`
- Jede sichtbare `ParameterRefRef` bekommt einen `HelpContext` mit
  Prefix `MTR-` (Ausnahme: `Empty` für Parameter ohne eigene
  Hilfeseite)
- Baggages werden über VS Code Task "OpenKNXproducer Documentation"
  erzeugt (`.vscode/tasks.json`)
- Deutsche Texte mit echten Umlauten (ä, ö, ü, ß) schreiben

## Referenzen

- [README.md](README.md) — Kurzüberblick über die Modi
- [doc/Applikationsbeschreibung-Zaehler.md](doc/Applikationsbeschreibung-Zaehler.md) — vollständige Parameterbeschreibung
- [CHANGELOG.md](CHANGELOG.md) — Versionshistorie
