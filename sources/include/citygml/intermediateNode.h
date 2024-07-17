#pragma once
#include <citygml/citygml_api.h>
#include <string>

namespace citygml {

    class LIBCITYGML_EXPORT IntermediateNode
    {
    public:
        IntermediateNode(std::string prefix, std::string name, std::string id)
            : m_prefix(prefix), m_name(name), m_id(id)
        {

        }

        const std::string& id() const;
        const std::string name() const;
        const std::string& prefix() const;
        const std::string& baseName() const;

    private:
        std::string m_prefix;
        std::string m_name;
        std::string m_id;
    };
}
