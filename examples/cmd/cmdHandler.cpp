#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <exception>
#include <type_traits>
#include <cstring>
#include <tuple>

// ================== 编译期工具函数（constexpr） ==================

constexpr int find_closing(const char* s, int p) {
    return s[p] == '>' ? p : find_closing(s, p + 1);
}

constexpr bool is_type_impl(const char* s, int p, const char* type, int i) {
    return type[i] == '\0'
               ? (s[p + i] == '>' || s[p + i] == ':')
               : (s[p + i] == type[i] && is_type_impl(s, p, type, i + 1));
}

/// @brief 检查命令参数是否为指定类型（C++11 兼容）
constexpr bool is_type(const char* s, int p, const char* type) {
    return is_type_impl(s, p, type, 0);
}

constexpr int count_args(const char* s, int p = 0) {
    return s[p] == '\0' ? 0 :
           s[p] == '<'  ? 1 + count_args(s, p + 1) :
           count_args(s, p + 1);
}

// 类型码：int=1, float=2, string=3, quoted=4  基数=5
constexpr int get_parameter_tag(const char* s, int p = 0) {
    return s[p] == '\0' ? 0 :
           s[p] == '<'  ? (
               is_type(s, p+1, "int")    ? get_parameter_tag(s, find_closing(s, p)+1) * 5 + 1 :
               is_type(s, p+1, "float")  ? get_parameter_tag(s, find_closing(s, p)+1) * 5 + 2 :
               is_type(s, p+1, "string") ? get_parameter_tag(s, find_closing(s, p)+1) * 5 + 3 :
               is_type(s, p+1, "quoted") ? get_parameter_tag(s, find_closing(s, p)+1) * 5 + 4 :
               throw std::logic_error("invalid parameter type")
           ) :
           get_parameter_tag(s, p + 1);
}

// ================== 编译期参数对与类型列表 ==================

template <int Pos, typename T>
struct arg_pair {
    static constexpr int pos = Pos;
    using type = T;
};

// 根据 tag 最低位生成单个 arg_pair 类型
template <int Tag, int Pos>
struct make_arg_pair {
    using type = typename std::conditional<
        (Tag % 5) == 1, arg_pair<Pos, int>,
        typename std::conditional<
            (Tag % 5) == 2, arg_pair<Pos, float>,
            typename std::conditional<
                (Tag % 5) == 3, arg_pair<Pos, std::string>,
                arg_pair<Pos, std::string>  // quoted 也映射为 string
            >::type
        >::type
    >::type;
};

// 递归解码，用 std::tuple 累积 arg_pair 类型（避免 seq 可变参数包问题）
template <int Tag, int Pos, int Count>
struct decode_impl {
    using type = decltype(
        std::tuple_cat(
            std::tuple<typename make_arg_pair<Tag % 5, Pos>::type>(),
            typename decode_impl<Tag / 5, Pos + 1, Count - 1>::type()
        )
    );
};

template <int Tag, int Pos>
struct decode_impl<Tag, Pos, 0> {
    using type = std::tuple<>;
};

// 对外接口：给定 tag 和参数个数 N，得到 std::tuple<arg_pair<Pos, Type>...>
template <int Tag, int N>
using decode_tag = typename decode_impl<Tag, 0, N>::type;

// ================== 运行时字符串转类型 ==================

template<typename T>
T from_string(const std::string& s) {
    std::istringstream iss(s);
    T val;
    if (!(iss >> val)) {
        throw std::runtime_error("Failed to convert string to target type");
    }
    return val;
}

template<>
std::string from_string<std::string>(const std::string& s) {
    return s;
}

// 参数取值包装
template<typename T>
T convert_token(const std::string& token) {
    return from_string<T>(token);
}

// ================== 命令处理器与 TaggedRule ==================

class CommandHandlerBase {
public:
    virtual void handle(const std::vector<std::string>& tokens) const = 0;
    virtual ~CommandHandlerBase() = default;
};

// TaggedRule 直接使用 std::tuple 作为参数列表
template <typename F, typename Tuple>
class TaggedRule : public CommandHandlerBase {
    F handler_;
public:
    explicit TaggedRule(F f) : handler_(std::move(f)) {}

    void handle(const std::vector<std::string>& tokens) const override {
        call(tokens, Tuple{});
    }

private:
    // 展开 tuple<arg_pair<Pos,Type>...>
    template <typename... Args>
    void call(const std::vector<std::string>& toks, std::tuple<Args...>) const {
        handler_(convert_token<typename Args::type>(toks[Args::pos])...);
    }
};

// ================== 命令调度器 ==================

class CommandDispatcher {
public:
    // 注册命令：使用 integral_constant 技巧绕过 decltype(lambda) 的未计算上下文限制
    template <typename F, int Tag, int N>
    void register_command(const std::string& name, F handler,
                          std::integral_constant<int, Tag>,
                          std::integral_constant<int, N>)
    {
        using ArgTuple = decode_tag<Tag, N>;
        auto rule = std::make_shared<TaggedRule<F, ArgTuple>>(std::move(handler));
        commands_[name] = std::move(rule);
    }

    void execute(const std::string& input) {
        auto tokens = split(input);
        if (tokens.empty()) return;

        auto it = commands_.find(tokens[0]);
        if (it != commands_.end()) {
            tokens.erase(tokens.begin());
            it->second->handle(tokens);
        } else {
            std::cerr << "Unknown command: " << tokens[0] << "\n";
        }
    }

private:
    std::unordered_map<std::string, std::shared_ptr<CommandHandlerBase>> commands_;

    static std::vector<std::string> split(const std::string& str) {
        std::vector<std::string> result;
        std::string token;
        bool in_quotes = false;
        for (size_t i = 0; i < str.size(); ++i) {
            char c = str[i];
            if (c == '\"') {
                in_quotes = !in_quotes;
            } else if (c == ' ' && !in_quotes) {
                if (!token.empty()) {
                    result.push_back(token);
                    token.clear();
                }
            } else {
                token += c;
            }
        }
        if (!token.empty()) result.push_back(token);
        return result;
    }
};

// ================== 辅助函数与注册宏 ==================

inline std::string extract_command_name(const char* tmpl) {
    std::string name;
    while (*tmpl && *tmpl != ' ' && *tmpl != '<') {
        name += *tmpl++;
    }
    return name;
}

#define REGISTER_COMMAND(dispatcher, cmd_template, handler) \
    do { \
        constexpr int tag = get_parameter_tag(cmd_template);  \
        constexpr int n   = count_args(cmd_template);         \
        std::string name = extract_command_name(cmd_template);\
        (dispatcher).register_command(name, handler, \
            std::integral_constant<int, tag>{}, \
            std::integral_constant<int, n>{}); \
    } while(0)

// ================== 使用示例 ==================
int main() {
    CommandDispatcher dispatcher;

    REGISTER_COMMAND(dispatcher,
        "ImportTLEFile <string> <quoted>",
        [](std::string scenario, std::string filePath) {
            std::cout << "Importing TLE file:\n"
                      << "  Scenario: " << scenario << "\n"
                      << "  FilePath: " << filePath << "\n";
        }
    );

    REGISTER_COMMAND(dispatcher,
        "SetUnits <string> <string>",
        [](std::string appPath, std::string unit) {
            std::cout << "Setting units:\n"
                      << "  Application: " << appPath << "\n"
                      << "  Unit: " << unit << "\n";
        }
    );

    REGISTER_COMMAND(dispatcher,
        "GenerateReport <string> <string>",
        [](std::string objectPath, std::string style) {
            std::cout << "Generating report:\n"
                      << "  Object: " << objectPath << "\n"
                      << "  Style: " << style << "\n";
        }
    );

    std::cout << "=== Executing commands ===\n";
    dispatcher.execute("ImportTLEFile  Scenario1  \"C:\\data\\tle.txt\"");
    dispatcher.execute("SetUnits /App/Default km");
    dispatcher.execute("GenerateReport Satellite1 \"Detailed\"");

    // 编译期类型错误
    // 错误：参数类型不匹配
    // REGISTER_COMMAND(dispatcher,
    //     "ImportTLEFile <int> <quoted>",
    //     [](std::string scenario, std::string filePath) {});

    return 0;
}