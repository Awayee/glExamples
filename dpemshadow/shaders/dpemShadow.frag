#version 330 core
in float posZ;
void main(){
    if (posZ < -1 || posZ > 1)discard;
}