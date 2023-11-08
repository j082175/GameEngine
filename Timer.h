#pragma once

#include <chrono>
#include "D3D11Utils.h"

using Microsoft::WRL::ComPtr;

namespace hlab {
class Timer {
  public:
    Timer() {}
    Timer(ComPtr<ID3D11Device> &device);

    void Start(ComPtr<ID3D11DeviceContext> &context, bool measureGPU);

    void End(ComPtr<ID3D11DeviceContext> &context);

  public:
    double m_elapsedTimeCPU = 0.0f;
    double m_elapsedTimeGPU = 0.0;

  protected:
    ComPtr<ID3D11Query> m_startQuery, m_stopQuery, m_disjointQuery;

    decltype(std::chrono::high_resolution_clock::now()) m_startTimeCPU;

    bool m_measureGPU = false;
};
} // namespace hlab