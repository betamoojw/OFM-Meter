
function meterUpdate(device, online, progress, context) {
    if (context.InUnit) {
        device.getParameterByName("MTR_Channel" + context.Channel + "InUnit").value = context.InUnit;
    }
    if (context.InPulses) {
        device.getParameterByName("MTR_Channel" + context.Channel + "InPulses").value = context.InPulses;
    }
    if (context.InModifier) {
        device.getParameterByName("MTR_Channel" + context.Channel + "InModifier").value = context.InModifier;
    }
    if (context.OutModifier) {
        device.getParameterByName("MTR_Channel" + context.Channel + "OutModifier").value = context.OutModifier;
    }
    if (context.CalcModifier) {
        device.getParameterByName("MTR_Channel" + context.Channel + "CalcModifier").value = context.CalcModifier;
    }
    if (context.PulseCalculation) {
        device.getParameterByName("MTR_Channel" + context.Channel + "PulseCalculation").value = context.PulseCalculation;
    }
}
