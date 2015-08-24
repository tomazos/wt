// Injection point for custom user configurations.
// The following macros can be defined:
//
//   Flag related macros:
//     GTEST_FLAG(flag_name)
//     GTEST_USE_OWN_FLAGFILE_FLAG_  - Define to 0 when the system provides its
//                                     own flagfile flag parsing.
//     GTEST_DECLARE_bool_(name)
//     GTEST_DECLARE_int32_(name)
//     GTEST_DECLARE_string_(name)
//     GTEST_DEFINE_bool_(name, default_val, doc)
//     GTEST_DEFINE_int32_(name, default_val, doc)
//     GTEST_DEFINE_string_(name, default_val, doc)
//
//   Test filtering:
//     GTEST_TEST_FILTER_ENV_VAR_ - The name of an environment variable that
//                                  will be used if --GTEST_FLAG(test_filter)
//                                  is not provided.
//
//   Logging:
//     GTEST_LOG_(severity)
//     GTEST_CHECK_(condition)
//     Functions LogToStderr() and FlushInfoLog() have to be provided too.
//
//   Threading:
//     GTEST_HAS_NOTIFICATION_ - Enabled if Notification is already provided.
//     GTEST_HAS_MUTEX_AND_THREAD_LOCAL_ - Enabled if Mutex and ThreadLocal are
//                                         already provided.
//     Must also provide GTEST_DECLARE_STATIC_MUTEX_(mutex) and
//     GTEST_DEFINE_STATIC_MUTEX_(mutex)
//
//     GTEST_EXCLUSIVE_LOCK_REQUIRED_(locks)
//     GTEST_LOCK_EXCLUDED_(locks)
//
// ** Custom implementation starts here **

#ifndef GTEST_INCLUDE_GTEST_INTERNAL_CUSTOM_GTEST_PORT_H_
#define GTEST_INCLUDE_GTEST_INTERNAL_CUSTOM_GTEST_PORT_H_

#endif  // GTEST_INCLUDE_GTEST_INTERNAL_CUSTOM_GTEST_PORT_H_
