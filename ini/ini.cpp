#include "ini.h"

namespace Ini{
    Section& Document::AddSection(string name) {
        return sections[move(name)];
    }

    const Section& Document::GetSection(const string &name) const {
            return sections.at(name);
    }

    size_t Document::SectionCount() const {
        return sections.size();
    }

    Document Load(istream &input){
        string line;
        Document res;
        Section* current_section;
        while (getline(input, line)){
            if(line.front() == '[' && line.back() == ']'){
                current_section = &res.AddSection(line.substr(1, line.size() - 2));
            } else if (!line.empty() && current_section){
                size_t equal_sign_pos = line.find('=');
                current_section -> emplace(line.substr(0, equal_sign_pos),
                                           line.substr(equal_sign_pos + 1));
            }
        }
        return res;
    }
}
