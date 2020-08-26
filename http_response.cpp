#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <utility>
#include <map>
#include <optional>
#include <unordered_set>

using namespace std;

struct HttpRequest {
    string method, path, body;
    map<string, string> get_params;
};

pair<string, string> SplitBy(const string& what, const string& by) {
    size_t pos = what.find(by);
    if (by.size() < what.size() && pos < what.size() - by.size()) {
        return {what.substr(0, pos), what.substr(pos + by.size())};
    } else {
        return {what, {}};
    }
}

template<typename T>
T FromString(const string& s) {
    T x;
    istringstream is(s);
    is >> x;
    return x;
}

pair<size_t, string> ParseIdAndContent(const string& body) {
    auto [id_string, content] = SplitBy(body, " ");
    return {FromString<size_t>(id_string), content};
}

struct LastCommentInfo {
    size_t user_id, consecutive_count;
};
struct HttpHeader {
    string name, value;
};

ostream& operator<<(ostream& output, const HttpHeader& h) {
    return output << h.name << ": " << h.value;
}

bool operator==(const HttpHeader& lhs, const HttpHeader& rhs) {
    return lhs.name == rhs.name && lhs.value == rhs.value;
}

struct ParsedResponse {
    int code;
    vector<HttpHeader> headers;
    string content;
};
enum class HttpCode {
    Ok = 200,
    NotFound = 404,
    Found = 302,
};

class HttpResponse {
public:
    explicit HttpResponse(HttpCode code): http_code(code){}

    HttpResponse& AddHeader(string name, string value){
        headers.push_back({move(name), move(value)});
        return *this;
    }
    HttpResponse& SetContent(string a_content){
        body = move(a_content);
        return *this;
    }
    HttpResponse& SetCode(HttpCode a_code){
        http_code = a_code;
        return *this;
    }

    friend ostream& operator << (ostream& output, const HttpResponse& resp);
private:
    HttpCode http_code;
    vector<HttpHeader> headers;
    string body;
};
ostream& operator << (ostream& output, const HttpResponse& resp){
    output << "HTTP/1.1 " << static_cast<int>(resp.http_code) << " ";
    switch (resp.http_code) {
        case HttpCode::Ok:
            output << "OK\n";
            break;
        case HttpCode::Found:
            output << "Found\n";
            break;
        case HttpCode::NotFound:
            output << "Not found\n";
            break;
        default:
            output << "Undefined HTTP code\n";
            break;
    }
    for(const auto& header: resp.headers){
        output << header << "\n";
    }
    if (!resp.body.empty()){
        output << "Content-Length: " << resp.body.size() << "\n\n" << resp.body;
    } else {
        output << "\n";
    }
    return output;
}

class CommentServer {
private:
    vector<vector<string>> comments_;
    std::optional<LastCommentInfo> last_comment;
    unordered_set<size_t> banned_users;
    HttpResponse ServePost(const HttpRequest& req){
        if (req.path == "/add_user"){
            comments_.emplace_back();
            return HttpResponse(HttpCode::Ok).SetContent(to_string(comments_.size() - 1));
        } else if (req.path == "/add_comment") {
            auto [user_id, comment] = ParseIdAndContent(req.body);
            if (!last_comment || last_comment->user_id != user_id) {
                last_comment = LastCommentInfo {user_id, 1};
            } else if (++last_comment->consecutive_count > 3) {
                banned_users.insert(user_id);
            }
            if (banned_users.count(user_id) == 0) {
                comments_[user_id].push_back(string(comment));
                return HttpResponse(HttpCode::Ok);
            } else {
                return HttpResponse(HttpCode::Found).AddHeader("Location", "/captcha");
            }
        } else if (req.path == "/checkcaptcha") {
            if (auto [id, response] = ParseIdAndContent(req.body); response == "42") {
                banned_users.erase(id);
                if (last_comment && last_comment->user_id == id) {
                    last_comment.reset();
                }
                return HttpResponse(HttpCode::Ok);
            } else {
                return HttpResponse(HttpCode::Found).AddHeader("Location", "/captcha");
            }
        } else {
            return HttpResponse(HttpCode::NotFound);
        }
    }

    HttpResponse ServerGet(const HttpRequest& req){
        if (req.path == "/user_comments") {
            auto user_id = FromString<size_t>(req.get_params.at("user_id"));
            string response;
            for (const string& c : comments_[user_id]) {
                response += c + '\n';
            }
            return HttpResponse(HttpCode::Ok).SetContent(response);
        } else if (req.path == "/captcha") {
            return HttpResponse(HttpCode::Ok).
            SetContent("What's the answer for The Ultimate Question of Life, the Universe, and Everything?");
        } else {
            return HttpResponse(HttpCode::NotFound);
        }
    }
public:
    HttpResponse ServeRequest(const HttpRequest& req) {
        if (req.method == "POST") {
            return ServePost(req);
        } else if (req.method == "GET") {
            return ServerGet(req);
        } else {
            return HttpResponse(HttpCode::NotFound);
        }
    }
};
