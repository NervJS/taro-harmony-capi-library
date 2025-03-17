//
// Created by zhutianjian on 24-8-26.
//

#ifndef TARO_HARMONY_CLASS_LIST_H
#define TARO_HARMONY_CLASS_LIST_H

#include <string>
#include <vector>

namespace TaroRuntime {
namespace TaroDOM {
    class TaroElement;
    class ClassList : public std::vector<std::string> {
        public:
        ClassList() = default;
        ClassList(const std::string &class_name);
        ~ClassList();

        void bindElement(std::weak_ptr<TaroElement> el);
        bool is_match(const ClassList &class_list) const;

        std::string value() const;
        int length() const;

        void add(const std::vector<std::string> &args);
        int add(const std::string &token);
        void remove(const std::vector<std::string> &args);
        int remove(const std::string &token);
        int contains(const std::string &token) const;
        int toggle(const std::string &token);
        int toggle(const std::string &token, bool force);
        int replace(const std::string &token, const std::string &new_token);
        static constexpr const char *const delimiter = " ";

        void reset(const std::string &class_name);
        void reset(const std::vector<std::string> &class_names);

        private:
        std::weak_ptr<TaroElement> element_ref_;
        int checkTokenIsValid(std::string token) const;
        std::string to_string() const;
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_HARMONY_CLASS_LIST_H
