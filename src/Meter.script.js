
function updateMeterUI(device, online, progress, context) {
    if (context.InModifier) {
        device.getParameterByName("MTR_Channel" + context.Channel + "InModifier").value = context.InModifier;
    }
    if (context.OutModifier) {
        device.getParameterByName("MTR_Channel" + context.Channel + "OutModifier").value = context.OutModifier;
    }
    if (context.SendOnChange) {
        device.getParameterByName("MTR_Channel" + context.Channel + "SendOnChange").value = context.SendOnChange;
    }
}

function readMeter(device, online, progress, context) {
    progress.setText("Zähler "  + context.Channel + ": Abrufen der Zählerdaten...");
    online.connect();

    var resp = online.invokeFunctionProperty(0xAF, 1, [context.Channel - 1]);
    if (resp[0] != 0) {
        throw new Error("Zähler "  + context.Channel + ": Es ist ein unbekannter Fehler aufgetreten!");
    }

    var counterSigned = resp[1];
    var counter = (resp[2] << 24) | (resp[3] << 16) | (resp[4] << 8) | resp[5];
    var referenceSigned = resp[6];
    var reference = (resp[7] << 24) | (resp[8] << 16) | (resp[9] << 8) | resp[10];

    // Convert int-> uint
    if (!counterSigned) { counter = counter >>> 0; }
    if (!referenceSigned) { reference = reference >>> 0; }

    if (counterSigned) {
        device.getParameterByName("MTR_Channel" + context.Channel + "Counter").value = 0;
        device.getParameterByName("MTR_Channel" + context.Channel + "CounterSigned").value = counter;
    } else {
        device.getParameterByName("MTR_Channel" + context.Channel + "Counter").value = counter;
        device.getParameterByName("MTR_Channel" + context.Channel + "CounterSigned").value = 0;
    }
    device.getParameterByName("MTR_Channel" + context.Channel + "Reference").value = reference.toString();

    online.disconnect();
    progress.setText("Zähler "  + context.Channel + ": Zählerdaten wurden abgerufen.");
}

function resetMeter(device, online, progress, context) {
    progress.setText("Zähler "  + context.Channel + ": Resete Zählerdaten...");
    online.connect();

    var resp = online.invokeFunctionProperty(0xAF, 2, [context.Channel - 1, context.Full]);
    if (resp[0] != 0) {
        throw new Error("Zähler "  + context.Channel + ": Es ist ein unbekannter Fehler aufgetreten!");
    }

    device.getParameterByName("MTR_Channel" + context.Channel + "Counter").value = 0;
    device.getParameterByName("MTR_Channel" + context.Channel + "CounterSigned").value = 0;
    if (context.Full) {
        device.getParameterByName("MTR_Channel" + context.Channel + "Reference").value = "0";
    }

    online.disconnect();
    progress.setText("Zähler "  + context.Channel + ": Zählerdaten wurden zurückgesetzt.");
}

function writeMeter(device, online, progress, context) {
    progress.setText("Zähler "  + context.Channel + ": Übermittle neuen Zählerstand...");
    online.connect();

    data = [];
    data[0] = context.Channel - 1;

    mode = device.getParameterByName("MTR_Channel" + context.Channel + "Mode").value;
    backstop = device.getParameterByName("MTR_Channel" + context.Channel + "Backstop").value;

    data[1] = (mode == 1 && !backstop);

    if (data[1]) {
        counter = device.getParameterByName("MTR_Channel" + context.Channel + "CounterSigned").value;
    } else {
        counter = device.getParameterByName("MTR_Channel" + context.Channel + "Counter").value;
    }

    data[2] = (counter >> 24) & 0xFF;
    data[3] = (counter >> 16) & 0xFF;
    data[4] = (counter >> 8) & 0xFF;
    data[5] = counter & 0xFF;

    var resp = online.invokeFunctionProperty(0xAF, 3, data);
    if (resp[0] != 0) {
        throw new Error("Zähler "  + context.Channel + ": Es ist ein unbekannter Fehler aufgetreten!");
    }

    online.disconnect();
    progress.setText("Zähler "  + context.Channel + ": Zählerstand wurde übermittelt.");
}