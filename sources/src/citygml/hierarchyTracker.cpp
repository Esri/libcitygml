#include <citygml/hierarchyTracker.h>


namespace citygml {

    std::string HierarchyTracker::getPreviousParentId(std::string currentParentId, std::shared_ptr<citygml::CityGMLLogger> logger) const
    {
        for (auto& pair : m_NodeStack)
        {
            if (pair.second.find(currentParentId) != pair.second.end())
                return pair.first;
        }

        if (currentParentId != "root")
        {
            CITYGML_LOG_ERROR(logger, "Failed to find parent id of " + currentParentId);
            return currentParentId;
        }
        
        return "";
    }

    std::string HierarchyTracker::getNodeStackPath(const std::string& startNodeId, std::shared_ptr<citygml::CityGMLLogger> logger) const
    {
        // searches up from the input id to the root
        std::string pathToRoot = "";
        std::string currentId = startNodeId;
        
        while (!currentId.empty() || currentId == "root")
        {
            std::string parentId = getPreviousParentId(currentId, logger);
            if (parentId.empty())
                return pathToRoot;

            // Now look for the details of the node from the parent
            auto parentChildren = m_NodeStack.at(parentId);
            auto nodeIt = parentChildren.find(currentId);
            if (nodeIt != parentChildren.end())
            {
                // Found the node in the parent's children
                std::string idBlock = nodeIt->first.empty() ? "" : "[" + nodeIt->first + "]";
                pathToRoot = nodeIt->second.name() + idBlock + "\\" + pathToRoot;
                currentId = parentId;
            }
            else
            {
                // force search to end
                currentId = "";
            }
        }

        return pathToRoot;
    }

    void HierarchyTracker::pushIntermediateNode(const IntermediateNode& node, const std::string& parentId)
    {
        auto const parentIter = m_NodeStack.emplace(parentId, std::unordered_map<std::string, citygml::IntermediateNode>{}).first;

        // if replacing the root then root elements are moved to become children of the new element
        if (parentId == "root")
        {
            auto const nodeIter = m_NodeStack.emplace(node.id(), std::unordered_map<std::string, citygml::IntermediateNode>{}).first;

            for (auto& rootElement : parentIter->second)
            {
                nodeIter->second.emplace(rootElement.first, rootElement.second);
            }

            parentIter->second.clear();
        }

        parentIter->second.emplace(node.id(), node);
    }

}
