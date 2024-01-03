
function meterUpdate(device, online, progress, context) {
    if (context.inunit) {
        device.getParameterByName("MTR_Channel" + context.channel + "InUnit").value = context.inunit;
    }
    if (context.inpulses) {
        device.getParameterByName("MTR_Channel" + context.channel + "InPulses").value = context.inpulses;
    }
    if (context.inmodifier) {
        device.getParameterByName("MTR_Channel" + context.channel + "InModifier").value = context.inmodifier;
    }
    if (context.outmodifier) {
        device.getParameterByName("MTR_Channel" + context.channel + "OutModifier").value = context.outmodifier;
    }
    if (context.powermodifier) {
        device.getParameterByName("MTR_Channel" + context.channel + "PowerModifier").value = context.powermodifier;
    }
}
