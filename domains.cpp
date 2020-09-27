#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

using namespace std;

using BanDomains = unordered_map<char, vector<string>>;

vector<string> ReadDomainsToCheck(istream& is = cin) {
    size_t count;
    is >> count;
    vector<string> domains(count);
    for (size_t i = 0; i < count; ++i) {
        is >> domains[i];
    }
    return domains;
}

BanDomains ReadDomainsToBan(istream& is = cin){
    size_t count;
    is >> count;
    BanDomains banned_domains;
    for (size_t i = 0; i < count; ++i) {
        string domain;
        is >> domain;
        banned_domains[domain.back()].push_back(move(domain));
    }
    return banned_domains;
}

bool isSubdomain(string_view subdomain, string_view domain) {
    if(subdomain.empty()){
        return true;
    }
    if(domain.empty() || domain.size() < subdomain.size()){
        return false;
    }
    int sub = static_cast<int>(subdomain.size()) - 1;
    int dom = static_cast<int>(domain.size()) - 1;
    while(sub >= 0){
        if(subdomain[sub--] != domain[dom--]){
            return false;
        }
    }
    return domain.size() == subdomain.size() || domain[domain.size() - subdomain.size() - 1] == '.';
}

void processDomain(string_view domain, const BanDomains& banned_domains){
    if(!banned_domains.count(domain.back())){
        cout << "Good\n";
        return;
    }
    for(const auto& ban_domain: banned_domains.at(domain.back())){
        if(isSubdomain(ban_domain, domain)){
            cout << "Bad\n";
            return;
        }
    }
    cout << "Good\n";

}

int main() {
    ios::sync_with_stdio(false);
    const BanDomains banned_domains = ReadDomainsToBan();
    const vector<string> domains_to_check = ReadDomainsToCheck();
    for(const auto& domain: domains_to_check){
        processDomain(domain, banned_domains);
    }
    return 0;
}
