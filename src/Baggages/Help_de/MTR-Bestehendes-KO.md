### Bestehendes KO

Es besteht die Möglichkeit, ein bestehendes KO als Eingang zu nutzen. Der Hintergrund ist, dass unsere Module eigenständige und flexible Einheiten sind, die keinen direkten Bezug zu anderen Modulen haben.

Verfügt die verwendete Hardware jedoch über Binäreingänge oder SML-Reader, möchte man möglicherweise vermeiden, dass die Eingangsdaten auf dem Bus übertragen werden. In diesem Fall kann das Zählermodul direkt auf den Ausgang eines Binäreingangs oder SML-Readers zugreifen, um die Buslast gering zu halten.

Bei der Verwendung bestehender KOs ist es jedoch von großer Bedeutung, dass der Datentyp mit dem des bestehenden KOs übereinstimmt. Erwartet das Zählermodul beispielsweise einen DPT13, muss auch das bestehende KO diesen Datentyp verwenden. 
Die Applikation selbst kann dies nicht überprüfen. Bei falscher Verwendung kann dies sogar zu einem Absturz der Hardware führen.

