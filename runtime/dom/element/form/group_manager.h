/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_GROUP_MANAGER_ELEMENT_H
#define HARMONY_LIBRARY_GROUP_MANAGER_ELEMENT_H
#include <map>
#include <arkui/native_node.h>

#include "runtime/dom/element/element.h"

/*
使用方法： 在TaroContext中创建一个
*/
namespace TaroRuntime {
namespace TaroDOM {
    class FormGroup {
        public:
        FormGroup(std::string name);
        void AppendChild(const std::shared_ptr<TaroNode> &node);
        void DeleteChild(const std::shared_ptr<TaroNode> &node);
        std::string GetName();

        private:
        bool CheckChildExist(const std::shared_ptr<TaroNode> &node);

        private:
        std::string name_;
        std::vector<std::shared_ptr<TaroNode>> nodes_;
    };

    class FormGroupManager {
        public:
        FormGroupManager();
        std::shared_ptr<FormGroup> CreateGroup(std::string name);
        void DelGroup(std::string name);
        void AppendChildForGroup(std::string name, const std::shared_ptr<TaroNode> &node);
        void AppendChildWithID(std::string id, const std::shared_ptr<TaroNode> &node);
        void DelChildForGroup(std::string name, const std::shared_ptr<TaroNode> &node);
        std::shared_ptr<TaroNode> GetNodeById(const std::string &id);
        void Clear();

        private:
        std::map<std::string, std::shared_ptr<FormGroup>> group_manager_;
        std::map<std::string, std::shared_ptr<TaroNode>> node_map_;
    };
} // namespace TaroDOM
} // namespace TaroRuntime
#endif
