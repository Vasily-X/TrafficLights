#include "trafficlights.inc"

main() {
    new light = CreateTrafficLight(TL_TYPE_1, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0);
    SetTrafficLightColor(light, TLC_RED);
    GetTrafficLightColor(light);
    DisableTrafficLight(light);
    RemoveTrafficLight(light);
    RemoveAllTrafficLights();
    GetPostRequiredLights(TL_TYPE_1);
    GetPostOffsetsIndex(TL_TYPE_1);
    CheckPostUpperLight(TL_TYPE_1);
    IsValidTrafficLightColor(TLC_GREEN);
    IsValidTrafficLightPost(TL_TYPE_1);
    // no 0.3.8 in sampctl tests yet
    // LoadTrafficLightsModels();
}
