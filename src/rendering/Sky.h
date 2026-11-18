#ifndef SKY_H
#define SKY_H

#include <glm/glm.hpp>

struct SkyState {
    glm::vec3 sunDir, sunColor, zenith, horizon, fog;
};

struct SkyParams {
    float horizonOffset = -0.15f;
    float gradientPower = 0.6f;
    glm::vec3 nightZenith{0.01f, 0.015f, 0.05f}, nightHorizon{0.03f, 0.04f, 0.09f};
    glm::vec3 duskZenith {0.18f, 0.25f, 0.50f},  duskHorizon {0.95f, 0.45f, 0.20f};
    glm::vec3 dayZenith  {72/256.f, 195/256.f, 231/256.f};
    glm::vec3 dayHorizon {64/256.f, 160/256.f, 200.f/256.f};
};

inline SkyState computeSky(float timeOfDay, const SkyParams& p = {}) {
    using namespace glm;
    const float a = timeOfDay * 2.f * pi<float>();

    SkyState s;
    s.sunDir = normalize(vec3(cos(a), sin(a), 0.2f));

    const float h = s.sunDir.y;
    const float twilightT = smoothstep(-0.25f, 0.0f, h); // 0 = night, 1 = sun at horizon
    const float dayT      = smoothstep( 0.0f, 0.35f, h); // 0 = sun at horizon, 1 = full day

    s.zenith  = mix(mix(p.nightZenith,  p.duskZenith,  twilightT), p.dayZenith,  dayT);
    s.horizon = mix(mix(p.nightHorizon, p.duskHorizon, twilightT), p.dayHorizon, dayT);
    s.sunColor = mix(vec3(1.0f, 0.5f, 0.2f), vec3(1.0f, 0.96f, 0.85f), dayT);

    const float t0 = std::pow(-p.horizonOffset / (1.0f - p.horizonOffset), p.gradientPower);
    s.fog = mix(s.horizon, s.zenith, t0);
    return s;
}

#endif //SKY_H
