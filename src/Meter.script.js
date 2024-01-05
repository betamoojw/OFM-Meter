
function updateMeterModifier(device, online, progress, context) {
    if (context.InModifier) {
        device.getParameterByName("MTR_Channel" + context.Channel + "InModifier").value = context.InModifier;
    }
    if (context.OutModifier) {
        device.getParameterByName("MTR_Channel" + context.Channel + "OutModifier").value = context.OutModifier;
    }
}
