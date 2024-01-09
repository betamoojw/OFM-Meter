### Rückfallzeit

Beim Betriebsstundenzähler ist es wichtig, den Betriebszustand zu kennen. Würde nun ein AUS-Telegramm verloren gehen, könnte das bedeuten, dass der Zähler dauerhaft weiterläuft. Um das zu verhindern, muss das EIN-Telegramm regelmäßig gesendet werden. Bleibt dies aus, schaltet der Betriebsstundenzähler nach Ablauf der Rückfallzeit automatisch ab. Die Zeit sollte so gewählt werden, dass mindestens 2 Status-Telegramme in dem Zeitraum eintreffen sollten.

**Tipp**: Sollte der Aktor kein zyklisches Senden unterstützen, kannst du mit dem Logikmodul regelmäßig Read-Requests auslösen.