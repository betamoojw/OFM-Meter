
function updateMeterModifier(device, online, progress, context) {
    if (context.in) {
        device.getParameterByName("MTR_Channel" + context.Channel + "InModifier").value = context.in;
    }
    if (context.out) {
        device.getParameterByName("MTR_Channel" + context.Channel + "OutModifier").value = context.out;
    }
}
