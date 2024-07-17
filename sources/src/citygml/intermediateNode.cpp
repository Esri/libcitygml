#include "citygml/intermediateNode.h"

namespace citygml {
    const std::string& IntermediateNode::id() const
    {
        return m_id;
    }

    const std::string IntermediateNode::name() const
    {
        return m_prefix + ":" + m_name;
    }

    const std::string& IntermediateNode::prefix() const
    {
        return m_prefix;
    }

    const std::string& IntermediateNode::baseName() const
    {
        return m_name;
    }
}