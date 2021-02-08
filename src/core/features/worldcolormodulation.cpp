#include "features.hpp"

void Features::WorldColorModulate::updateColorModulation() {
    for(auto i = Interfaces::materialSystem->FirstMaterial(); i != Interfaces::materialSystem->InvalidMaterial(); i = Interfaces::materialSystem->NextMaterial(i)) {
        IMaterial* material = Interfaces::materialSystem->GetMaterial(i);
        if (material) {
            if(strstr(material->GetTextureGroupName(), "World")) {
                material->AlphaModulate(CONFIGCOL("Visuals>World>World>World Color Modulation").Value.w);
                material->ColorModulate(CONFIGCOL("Visuals>World>World>World Color Modulation").Value.x, CONFIGCOL("Visuals>World>World>World Color Modulation").Value.y, CONFIGCOL("Visuals>World>World>World Color Modulation").Value.z);
            }
        }
    }
}