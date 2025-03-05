#pragma once

#include <citygml/citygml_api.h>
#include <citygml/intermediateNode.h>
#include <citygml/citygmllogger.h>
#include <citygml/warnings.h>

#include <string>
#include <map>
#include <memory>
#include <unordered_map>


namespace citygml {

    class LIBCITYGML_EXPORT HierarchyTracker
    {
    public:
        void pushIntermediateNode(const IntermediateNode& node, const std::string& parentId);
        std::string getPreviousParentId(std::string currentParentId, std::shared_ptr<citygml::CityGMLLogger> logger) const;
        std::string getNodeStackPath(const std::string& startNodeId, std::shared_ptr<citygml::CityGMLLogger> logger) const;

    private:
        PRAGMA_WARN_DLL_BEGIN
        // Keyed on parent ids
        std::map<std::string, std::unordered_map<std::string, IntermediateNode>> m_NodeStack;
        PRAGMA_WARN_DLL_END
    };
    
}
