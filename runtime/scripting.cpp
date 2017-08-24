
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include "utils/Global.h"
#include "scripting.h"
#include "unicode.hpp"
#include "regex.hpp"
#include "include/signal_lua.hpp"

namespace runtime {

static inline size_t base0(int i)
{
    if (i <= 0) {
        throw error("index must be positive");
    }

    return size_t(i - 1);
}

static sol::table array_to_table(sol::state_view lua, const std::vector<string> &strings)
{
    sol::table table = lua.create_table();

    for (auto &s : strings) {
        table.add(s);
    }

    return table;
}

static std::vector<string> table_to_array(const sol::table &table)
{
    std::vector<string> tmp;

    for (auto &it : table) {
        tmp.emplace_back(it.second.as<const char*>());
    }

    return tmp;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int generate_signal()
{
    static int id = 100000;
    return id++;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_string_library(sol::state_view lua)
{
    lua.open_libraries(sol::lib::string);
    sol::table str = lua["string"];


    str["trim"] = [](std::string_view self) {
        return string::trim(self, string::option::both);
    };

    str["ltrim"] = [](std::string_view self) {
        return string::trim(self, string::option::left);
    };

    str["rtrim"] = [](std::string_view self) {
        return string::trim(self, string::option::right);
    };

    str["to_upper"] = [](string self) {
        return self.to_upper();
    };

    str["to_lower"] = [](string self) {
        return self.to_lower();
    };

    lua.script("string.byte_count = string.len");

    str["len"] = [](std::string_view s) {
        return (int) unicode::strlen(s.data(), s.size());
    };

    str["contains"] = [](const std::string_view self, const std::string_view other) {
        return string::contains(self, other);
    };

    str["starts_with"] = [](const std::string_view self, const std::string_view other) {
        return string::starts_with(self, other);
    };

    str["ends_with"] = [](const std::string_view self, const std::string_view other) {
        return string::ends_with(self, other);
    };

    str["insert"] = [](string self, int pos, const std::string_view other) {
        auto i = base0(pos);
        return self.insert(i, other);
    };

    str["reverse"] = [](string self) {
        return self.reverse();
    };

    str["remove"] = [](string self, const std::string_view other) {
        return self.remove(other);
    };

    str["remove_at"] = [](string self, int at, int len) {
        return self.replace(at, len, "");
    };

    str["remove_first"] = [](string self, const std::string_view other) {
        return self.remove_first(other);
    };

    str["remove_last"] = [](string self, const std::string_view other) {
        return self.remove_last(other);
    };

    str["replace"] = [](string self, const std::string_view before, const std::string_view after) {
        return self.replace(before, after);
    };    

    str["replace_first"] = [](string self, const std::string_view before, const std::string_view after) {
        return self.replace_first(before, after);
    };

    str["replace_last"] = [](string self, const std::string_view before, const std::string_view after) {
        return self.replace_last(before, after);
    };

    str["replace_at"] = [](string self, int at, int len, std::string_view after) {
        return self.replace(at, len, after);
    };

    str["count"] = [](const std::string_view self, const std::string_view other) {
        return string::count(self, other);
    };

    str["substr"] = [](string self, int from, int to) {
        auto f = base0(from);
        size_t t = (to == -1) ? string::npos : base0(to);
        return self.substr(f, t);
    };

    str["left"] = [](string self, int n) {
        auto i = base0(n);
        return self.left(i);
    };

    str["right"] = [](string self, int n) {
        auto i = base0(n);
        return self.right(i);
    };

    str["first"] = [](string self) {
        auto it = self.front();
        return unicode::encode(*it);
    };

    str["last"] = [](const string &self) {
        auto it = self.back();
        return unicode::encode(*it);
    };

    str["at"] = [](string self, int pos) {
        auto i = base0(pos);
        auto it = self.at(i);

        return unicode::encode(*it);
    };

    str["split"] = [&lua](sol::this_state s, string self, const std::string_view delim) {
        sol::state_view lua(s);

        if (delim.empty())
        {
            sol::table strings = lua.create_table();

            for (auto c : self.codepoints()) {
                strings.add(unicode::encode(c));
            }

            return strings;
        }
        else
        {
            return array_to_table(lua, self.split(delim));

        }
    };

    str["join"] = [](const sol::table &strings, const std::string_view delim) {
        return string::join(table_to_array(strings), delim);
    };
}

static void set_regex_library(sol::state_view lua)
{
    lua.new_usertype<regex>( "regex",
        sol::constructors<regex(const string&), regex(const string&, const string&)>(),
                             "match", sol::overload(&regex::match,
                                                    [](regex &self, const string &pattern) { return self.match(pattern); }),
        "has_match", &regex::has_match,
        "capture", &regex::capture,
        "count", &regex::capture_count,
        "first", &regex::start_offset,
        "last", &regex::end_offset,
        "pattern", &regex::pattern,
        sol::meta_function::to_string, &regex::to_string
    );
}

static void set_filesystem_library(sol::state_view lua)
{
//    lua["filesystem"] = lua.create_table();
//    sol::table fs = lua["filesystem"];

//    fs["join"] = [](QString path, sol::variadic_args va, QString second) {

//        for (size_t i = 0; i < va.size(); i++)
//        {
//            auto s = va.get<QString>(i);
//            path.append(QDir::separator());
//            path.append(s);
//        }

//        return path;
//    };

//    fs["full_path"] = [](const String &path) {
//        return Path::full_path(path);
//    };
}

static void set_shell_library(sol::state_view lua)
{
    lua["shell"] = lua.create_table();
    sol::table shell = lua["shell"];

    shell["warning"] = [](const QString &msg) {
        QMessageBox dlg(QMessageBox::Warning, "Warning", msg);
        dlg.exec();
    };

    shell["alert"] = [](const QString &msg) {
        QMessageBox dlg(QMessageBox::Critical, "Error", msg);
        dlg.exec();
    };

    shell["open_file_dialog"] = [](const char *s) {
        return QFileDialog::getOpenFileName(nullptr, QString(s));
    };

    shell["open_directory_dialog"] = [](const char *s) {
        return QFileDialog::getExistingDirectory(nullptr, QString(s));
    };

    shell["save_file_dialog"] = [](const char *s) {
        return QFileDialog::getSaveFileName(nullptr, QString(s));
    };

    shell["status"] = [](const char *msg, int timeout) {
        if (Global::Output)
        {
            // Convert timeout from seconds to milliseconds
            timeout *= 1000;
            Global::Output->showMessage(msg, timeout);
        }
    };
}

static void set_signal_library(sol::state_view lua)
{
    lua.script(signal_script);
    sol::table sig = lua["signal"];
    sig["new"] = &generate_signal;

    lua.script(R"_(
               EVENT_ANNOTATION_LOADED = signal.new()
               EVENT_DOCUMENT_LOADED = signal.new()
               EVENT_SOUND_LOADED = signal.new()
               )_");
}

void initialize(sol::state_view lua)
{
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table);
    set_string_library(lua);
    set_regex_library(lua);
    set_shell_library(lua);
    set_signal_library(lua);
}

} // runtime

