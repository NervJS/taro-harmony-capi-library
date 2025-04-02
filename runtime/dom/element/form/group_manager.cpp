/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "group_manager.h"

namespace TaroRuntime {
namespace TaroDOM {

    FormGroup::FormGroup(std::string name) {
        name_ = name;
    }

    void FormGroup::AppendChild(const std::shared_ptr<TaroNode> &node) {
        if (CheckChildExist(node)) {
            return;
        }
        nodes_.push_back(node);
    }

    void FormGroup::DeleteChild(const std::shared_ptr<TaroNode> &node) {
        if (CheckChildExist(node)) {
            std::remove(nodes_.begin(), nodes_.end(), node);
        }
    }

    std::string FormGroup::GetName() {
        return name_;
    }

    bool FormGroup::CheckChildExist(const std::shared_ptr<TaroNode> &node) {
        return std::find(nodes_.begin(), nodes_.end(), node) != nodes_.end();
    }

    FormGroupManager::FormGroupManager() {
    }

    std::shared_ptr<FormGroup> FormGroupManager::CreateGroup(std::string name) {
        auto iter = group_manager_.find(name);
        if (iter == group_manager_.end()) {
            auto group = std::make_shared<FormGroup>(name);
            group_manager_.emplace(name, group);
        }
        return group_manager_[name];
    }

    void FormGroupManager::DelGroup(std::string name) {
        auto iter = group_manager_.find(name);
        if (iter != group_manager_.end()) {
            group_manager_.erase(iter);
        }
    }

    void FormGroupManager::AppendChildForGroup(std::string name, const std::shared_ptr<TaroNode> &node) {
        if (group_manager_.find(name) == group_manager_.end()) {
            CreateGroup(name);
        }
        group_manager_[name]->AppendChild(node);
    }

    void FormGroupManager::DelChildForGroup(std::string name, const std::shared_ptr<TaroNode> &node) {
        if (group_manager_.find(name) == group_manager_.end()) {
            return;
        }
        group_manager_[name]->DeleteChild(node);
    }

    std::shared_ptr<TaroNode> FormGroupManager::GetNodeById(const std::string &id) {
        if (node_map_.find(id) == node_map_.end()) {
            return nullptr;
        }
        return node_map_[id];
    }

    void FormGroupManager::Clear() {
        if (group_manager_.size() > 0) {
            group_manager_.clear();
        }

        if (node_map_.size() > 0) {
            node_map_.clear();
        }
    }

    void FormGroupManager::AppendChildWithID(std::string id, const std::shared_ptr<TaroNode> &node) {
        node_map_.emplace(id, node);
    }

} // namespace TaroDOM
} // namespace TaroRuntime
