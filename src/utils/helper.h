#pragma once
#include <functional>
#include <string>
#include <type_traits>

template <typename Function>
auto measureExecutionTime(const std::string& label, Function func, bool enabled)
    -> std::invoke_result_t<Function>;

#include "helper.tpp"