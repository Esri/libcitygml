#pragma once

#include <string>

#include <citygml/citygml_api.h>
#include <citygml/attributesmap.h>
#include <citygml/warnings.h>

namespace citygml {
    /**
     * @brief The base object associated with an unique id and a set of attributes (key-value pairs)
     */
    class LIBCITYGML_EXPORT Object
    {
    public:
        Object( const std::string& id );

        const std::string& getId() const;

        std::string getAttribute( const std::string& name ) const;

        const AttributesMap& getAttributes() const;

        AttributesMap& getAttributes();

        virtual ~Object() {}

        void setAttribute(const std::string& name, const std::string& value, AttributeType type = AttributeType::String, bool overwrite = true );

    protected:

        PRAGMA_WARN_DLL_BEGIN
        std::string m_id;

        AttributesMap m_attributes;
        PRAGMA_WARN_DLL_END
    };

    LIBCITYGML_EXPORT std::ostream& operator<<( std::ostream&, const citygml::Object& );

}
