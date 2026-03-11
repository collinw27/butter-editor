#ifndef MATH_H
#define MATH_H

// Doesn't validate that a < b

template<typename T>
T clamp(T value, T min_val, T max_val)
{
    return std::max(std::min(value, max_val), min_val);
}

#endif