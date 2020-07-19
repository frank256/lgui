#ifndef LGUI_VECTOR_UTILS_H
#define LGUI_VECTOR_UTILS_H

#include <vector>
#include <algorithm>

namespace lgui {

template<typename T>
void erase_remove(std::vector<T>& vec, const T& elem) {
    vec.erase(std::remove(vec.begin(), vec.end(), elem), vec.end());
}

template<typename T, typename Pred>
void erase_remove_if(std::vector<T>& vec, const Pred& pred) {
    vec.erase(std::remove_if(vec.begin(), vec.end(), pred), vec.end());
}

template<typename T>
bool contains(const std::vector<T>& vec, const T& elem) {
    return std::find(vec.begin(), vec.end(), elem) != vec.end();
}

}

#endif //LGUI_VECTOR_UTILS_H
