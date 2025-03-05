#include <citygml/hierarchyTracker.h>


namespace citygml {

    std::string HierarchyTracker::getPreviousParentId(std::string currentParentId, std::shared_ptr<citygml::CityGMLLogger> logger) const
    {
        auto const map = m_idToNodeAndParentMap.find(currentParentId);
        if (map != m_idToNodeAndParentMap.end())
        {
            return map->second.second;
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
            auto const nodeAndParent = m_idToNodeAndParentMap.find(currentId);
            if (nodeAndParent == m_idToNodeAndParentMap.end())
                return pathToRoot;

            std::string idBlock = currentId.empty() ? "" : "[" + currentId + "]";
            pathToRoot = nodeAndParent->second.first.name() + idBlock + "\\" + pathToRoot;
            currentId = nodeAndParent->second.second;
        }

        return pathToRoot;
    }

    void HierarchyTracker::pushIntermediateNode(const IntermediateNode& node, const std::string& parentId)
    {
        m_idToNodeAndParentMap.emplace(node.id(), std::make_pair(node, parentId));

        // if replacing the root then the root element is moved to become a child of the new element
        if (parentId == "root")
        {
            if (!m_rootChild.empty())
            {
                m_idToNodeAndParentMap.at(m_rootChild).second = node.id();
            }

            m_rootChild = node.id();
        }
    }

}
