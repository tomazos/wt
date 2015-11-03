//#pragma once

//#include <array>
//#include <utility>

// namespace gl {

// struct vec2;
// struct vec3;
// struct vec4;
// struct dvec2;
// struct dvec3;
// struct dvec4;
// struct bvec2;
// struct bvec3;
// struct bvec4;
// struct ivec2;
// struct ivec3;
// struct ivec4;
// struct uvec2;
// struct uvec3;
// struct uvec4;
// struct mat2x2;
// struct mat3x2;
// struct mat4x2;
// struct mat2x3;
// struct mat3x3;
// struct mat4x3;
// struct mat2x4;
// struct mat3x4;
// struct mat4x4;
// struct dmat2x2;
// struct dmat3x2;
// struct dmat4x2;
// struct dmat2x3;
// struct dmat3x3;
// struct dmat4x3;
// struct dmat2x4;
// struct dmat3x4;
// struct dmat4x4;

// using mat2 = mat2x2;
// using mat3 = mat3x3;
// using mat4 = mat4x4;
// using dmat2 = dmat2x2;
// using dmat3 = dmat3x3;
// using dmat4 = dmat4x4;

// template <typename R, typename F, typename P, size_t N, size_t... I>
// constexpr R binary_operation_kv_impl(F f, P k, const std::array<P, N>& v,
//                                     std::index_sequence<I...>) {
//  return R(f(k, v[I])...);
//}

// template <typename R, typename F, typename P, size_t N,
//          typename Is = std::make_index_sequence<N>>
// constexpr R binary_operation_kv(F f, P k, const std::array<P, N>& v) {
//  return binary_operation_kv_impl<R>(f, k, v, Is());
//}

// template <typename R, typename F, typename P, size_t N, size_t... I>
// constexpr R binary_operation_vk_impl(F f, const std::array<P, N>& v, P k,
//                                     std::index_sequence<I...>) {
//  return R(f(v[I], k)...);
//}

// template <typename R, typename F, typename P, size_t N,
//          typename Is = std::make_index_sequence<N>>
// constexpr R binary_operation_vk(F f, const std::array<P, N>& v, P k) {
//  return binary_operation_vk_impl<R>(f, v, k, Is());
//}

// template <typename R, typename F, typename P, size_t N, size_t... I>
// constexpr R binary_operation_vv_impl(F f, const std::array<P, N>& a,
//                                     const std::array<P, N>& b,
//                                     std::index_sequence<I...>) {
//  return R(f(a[I], b[I])...);
//}

// template <typename R, typename F, typename P, size_t N,
//          typename Is = std::make_index_sequence<N>>
// constexpr R binary_operation_vv(F f, const std::array<P, N>& a,
//                                const std::array<P, N>& b) {
//  return binary_operation_vv_impl<R>(f, a, b, Is());
//}

//}  // namespace gl
