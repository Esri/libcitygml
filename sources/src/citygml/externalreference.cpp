#include <citygml/externalreference.h>

namespace citygml {
    ExternalObjectReference::ExternalObjectReference() : name("") {
    }

    ExternalObjectReference::~ExternalObjectReference() {
    }

    ExternalReference::ExternalReference(std::string const& id) : Object(id) {
    }
}
