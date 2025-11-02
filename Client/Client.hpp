#pragma once

#include <string>
#include <vector>

#include <Document.hpp>

class NoteClient {
    explicit NoteClient(const std::string& userName);


private:
    DocumentsMap m_documents;
};

using ClientPtr = std::unique_ptr<NoteClient>;
using ClientsMap = Map<Id, ClientPtr>;