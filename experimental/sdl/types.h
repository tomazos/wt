//#pragma once

//#include "eigen/Dense"

// namespace gl {

// using vec2 = Eigen::Vector2f;
// using vec3 = Eigen::Vector3f;
// using vec4 = Eigen::Vector4f;
// STATIC_ASSERT(sizeof(vec2) == 8);
// STATIC_ASSERT(sizeof(vec3) == 12);
// STATIC_ASSERT(sizeof(vec4) == 16);

// using dvec2 = Eigen::Vector2d;
// using dvec3 = Eigen::Vector3d;
// using dvec4 = Eigen::Vector4d;
// STATIC_ASSERT(sizeof(dvec2) == 16);
// STATIC_ASSERT(sizeof(dvec3) == 24);
// STATIC_ASSERT(sizeof(dvec4) == 32);

// using bvec2 = Eigen::Vector2b;
// using bvec3 = Eigen::Vector3b;
// using bvec4 = Eigen::Vector4b;
// STATIC_ASSERT(sizeof(bvec2) == 2);
// STATIC_ASSERT(sizeof(bvec3) == 3);
// STATIC_ASSERT(sizeof(bvec4) == 4);

// using ivec2 = Eigen::Vector2i;
// using ivec3 = Eigen::Vector3i;
// using ivec4 = Eigen::Vector4i;
// STATIC_ASSERT(sizeof(ivec2) == 8);
// STATIC_ASSERT(sizeof(ivec3) == 12);
// STATIC_ASSERT(sizeof(ivec4) == 16);

// using uvec2 = Eigen::Vector2u;
// using uvec3 = Eigen::Vector3u;
// using uvec4 = Eigen::Vector4u;
// STATIC_ASSERT(sizeof(uvec2) == 8);
// STATIC_ASSERT(sizeof(uvec3) == 12);
// STATIC_ASSERT(sizeof(uvec4) == 16);

// using mat2x2 = Eigen::Matrix<float32, 2, 2>;
// using mat3x2 = Eigen::Matrix<float32, 2, 3>;
// using mat4x2 = Eigen::Matrix<float32, 2, 4>;
// using mat2x3 = Eigen::Matrix<float32, 3, 2>;
// using mat3x3 = Eigen::Matrix<float32, 3, 3>;
// using mat4x3 = Eigen::Matrix<float32, 3, 4>;
// using mat2x4 = Eigen::Matrix<float32, 4, 2>;
// using mat3x4 = Eigen::Matrix<float32, 4, 3>;
// using mat4x4 = Eigen::Matrix<float32, 4, 4>;
// using dmat2x2 = Eigen::Matrix<float64, 2, 2>;
// using dmat3x2 = Eigen::Matrix<float64, 2, 3>;
// using dmat4x2 = Eigen::Matrix<float64, 2, 4>;
// using dmat2x3 = Eigen::Matrix<float64, 3, 2>;
// using dmat3x3 = Eigen::Matrix<float64, 3, 3>;
// using dmat4x3 = Eigen::Matrix<float64, 3, 4>;
// using dmat2x4 = Eigen::Matrix<float64, 4, 2>;
// using dmat3x4 = Eigen::Matrix<float64, 4, 3>;
// using dmat4x4 = Eigen::Matrix<float64, 4, 4>;

// using mat2 = mat2x2;
// using mat3 = mat3x3;
// using mat4 = mat4x4;
// using dmat2 = dmat2x2;
// using dmat3 = dmat3x3;
// using dmat4 = dmat4x4;

// STATIC_ASSERT(sizeof(mat2x2) == 4 * 2 * 2);
// STATIC_ASSERT(sizeof(mat3x2) == 4 * 3 * 2);
// STATIC_ASSERT(sizeof(mat4x2) == 4 * 4 * 2);
// STATIC_ASSERT(sizeof(mat2x3) == 4 * 2 * 3);
// STATIC_ASSERT(sizeof(mat3x3) == 4 * 3 * 3);
// STATIC_ASSERT(sizeof(mat4x3) == 4 * 4 * 3);
// STATIC_ASSERT(sizeof(mat2x4) == 4 * 2 * 4);
// STATIC_ASSERT(sizeof(mat3x4) == 4 * 3 * 4);
// STATIC_ASSERT(sizeof(mat4x4) == 4 * 4 * 4);
// STATIC_ASSERT(sizeof(dmat2x2) == 8 * 2 * 2);
// STATIC_ASSERT(sizeof(dmat3x2) == 8 * 3 * 2);
// STATIC_ASSERT(sizeof(dmat4x2) == 8 * 4 * 2);
// STATIC_ASSERT(sizeof(dmat2x3) == 8 * 2 * 3);
// STATIC_ASSERT(sizeof(dmat3x3) == 8 * 3 * 3);
// STATIC_ASSERT(sizeof(dmat4x3) == 8 * 4 * 3);
// STATIC_ASSERT(sizeof(dmat2x4) == 8 * 2 * 4);
// STATIC_ASSERT(sizeof(dmat3x4) == 8 * 3 * 4);
// STATIC_ASSERT(sizeof(dmat4x4) == 8 * 4 * 4);

// STATIC_ASSERT(mat2x2::RowsAtCompileTime == 2);
// STATIC_ASSERT(mat2x3::RowsAtCompileTime == 3);
// STATIC_ASSERT(mat2x4::RowsAtCompileTime == 4);
// STATIC_ASSERT(mat3x2::RowsAtCompileTime == 2);
// STATIC_ASSERT(mat3x3::RowsAtCompileTime == 3);
// STATIC_ASSERT(mat3x4::RowsAtCompileTime == 4);
// STATIC_ASSERT(mat4x2::RowsAtCompileTime == 2);
// STATIC_ASSERT(mat4x3::RowsAtCompileTime == 3);
// STATIC_ASSERT(mat4x4::RowsAtCompileTime == 4);
// STATIC_ASSERT(mat2::RowsAtCompileTime == 2);
// STATIC_ASSERT(mat3::RowsAtCompileTime == 3);
// STATIC_ASSERT(mat4::RowsAtCompileTime == 4);

// STATIC_ASSERT(mat2x2::ColsAtCompileTime == 2);
// STATIC_ASSERT(mat2x3::ColsAtCompileTime == 2);
// STATIC_ASSERT(mat2x4::ColsAtCompileTime == 2);
// STATIC_ASSERT(mat3x2::ColsAtCompileTime == 3);
// STATIC_ASSERT(mat3x3::ColsAtCompileTime == 3);
// STATIC_ASSERT(mat3x4::ColsAtCompileTime == 3);
// STATIC_ASSERT(mat4x2::ColsAtCompileTime == 4);
// STATIC_ASSERT(mat4x3::ColsAtCompileTime == 4);
// STATIC_ASSERT(mat4x4::ColsAtCompileTime == 4);
// STATIC_ASSERT(mat2::ColsAtCompileTime == 2);
// STATIC_ASSERT(mat3::ColsAtCompileTime == 3);
// STATIC_ASSERT(mat4::ColsAtCompileTime == 4);

// template <typename V>
// typename V::Scalar dot(const V& a, const V& b) {
//  return a.dot(b);
//}

// template <typename V>
// V cross(const V& a, const V& b) {
//  return a.cross(b);
//}

// template <typename V>
// typename V::Scalar length(const V& v) {
//  return v.norm();
//}

// template <typename V>
// V normalize(const V& v) {
//  return v / length(v);
//}

// template <typename V>
// typename V::Scalar angle(const V& a, const V& b) {
//  using S = typename V::Scalar;
//  S s = dot(a, b) / (length(a) * length(b));
//  if (s <= S(-1))
//    return std::acos(S(-1));
//  else if (s >= S(1))
//    return std::acos(S(1));
//  else
//    return std::acos(s);
//}

// template <typename V>
// V projection(const V& source, const V& destination) {
//  return (dot(source, destination) / std::pow(length(destination), 2)) *
//         destination;
//}

// template <typename M>
// auto transpose(const M& m) {
//  return m.transpose();
//}

// template <typename M>
// M inverse(const M& m) {
//  return m.inverse();
//}

// template <typename To, typename From>
// To trim(const From& m) {
//  STATIC_ASSERT(size_t(To::RowsAtCompileTime) <=
//                size_t(From::RowsAtCompileTime));
//  STATIC_ASSERT(size_t(To::ColsAtCompileTime) <=
//                size_t(From::ColsAtCompileTime));
//  return m
//      .template topLeftCorner<To::RowsAtCompileTime, To::ColsAtCompileTime>();
//}

//}  // namespace gl
