#pragma once
#include <cx-engine/systems/settings/setting.hpp>
#include <cx-engine/systems/settings/valuable-setting.hpp>
#include <cx-engine/systems/settings/enum-setting.hpp>
#include <cx-engine/systems/settings/optional-valuable-setting.hpp>

#include <cx-engine/defs/types.hpp>
#include <cx-engine/defs/platform.hpp>
#include <cx-engine/defs/fs.hpp>

#include <cx-engine/extern/toml++/toml.hpp>

#include <variant>
#include <utility>
#include <fstream>

#if IS_WINDOWS
    #include <windows.h>
    #include <shlobj.h>
#elif IS_POSIX
    #include <sys/types.h>
    #include <pwd.h>
    #include <unistd.h>
#endif

namespace cx {

struct SettingsCategory {
    String key;
    String displayName;
    Vec<Setting*> items;

    SettingsCategory() = default;
    SettingsCategory(StringView k, StringView d, InitList<Setting*> init = {})
        : key(String(k)), displayName(String(d)), items(init) {}

    SettingsCategory(const SettingsCategory&) = delete;
    SettingsCategory& operator=(const SettingsCategory&) = delete;

    SettingsCategory(SettingsCategory&& o) noexcept
        : key(std::move(o.key)), displayName(std::move(o.displayName)), items(std::move(o.items)) {
        o.items.clear();
    }

    SettingsCategory& operator=(SettingsCategory&& o) noexcept {
        if (this != &o) {
            for (auto s : items) delete s;
            key = std::move(o.key);
            displayName = std::move(o.displayName);
            items = std::move(o.items);
            o.items.clear();
        }
        return *this;
    }

    ~SettingsCategory() {
        for (auto s : items) delete s;
    }

    void add(Setting* s) { if (!s) return; items.push_back(s); }
    void insertAt(usize pos, Setting* s) {
        if (!s) return;
        if (pos >= items.size()) items.push_back(s);
        else items.insert(items.begin() + pos, s);
    }
    Slice<Setting*> getItems() {
        return Slice(items.data(), items.size());
    }
    Slice<const Setting* const> getItems() const {
        return Slice(items.data(), items.size());
    }

    void loadAll(const toml::table& tbl) {
        for (auto s : items) if (s) s->load(tbl);
    }

    void saveAll(toml::table& tbl) const {
        for (auto s : items) if (s) s->save(tbl);
    }
};

using SettingsListEntry = Union<Setting*, UniquePtr<SettingsCategory>>;

class Settings {
    Vec<SettingsListEntry> entries;

public:
    Settings() = default;
    Settings(const Settings&) = delete;
    Settings(Settings&&) = default;

    ~Settings() {
        for (auto &e : entries) {
            if (auto p = std::get_if<Setting*>(&e)) {
                if (*p) delete *p;
            }
        }
    }

    virtual String getName() = 0;

    void add(Setting*&& s) {
        if (!s) return;
        entries.push_back(s);
    }

    void addCategory(StringView key, StringView displayName, InitList<Setting*> settings) {
        if (key == StringView()) return;
        for (auto& e: entries) {
            if (auto cp = std::get_if<UniquePtr<SettingsCategory>>(&e)) {
                if (cp && *cp && (*cp)->key == key) return;
            }
        }
        entries.push_back(std::make_unique<SettingsCategory>(key, displayName, settings));
    }

    Slice<SettingsListEntry> getList() {
        return Slice<SettingsListEntry>{ entries.data(), entries.size() };
    }
    Slice<const SettingsListEntry> getList() const {
        return Slice<const SettingsListEntry>{ entries.data(), entries.size() };
    }

    Setting* get(StringView key) const {
        for (auto& e: entries) {
            if (auto p = std::get_if<Setting*>(&e)) {
                if (*p && (*p)->key == key) return *p;
            } else if (auto cp = std::get_if<UniquePtr<SettingsCategory>>(&e)) {
                if (cp && *cp) {
                    for (auto s : (*cp)->getItems()) if (s && s->key == key) return s;
                }
            }
        }
        return nullptr;
    }

    template <typename T>
    ValuableSetting<T>* getValuable(StringView key) const {
        if (auto s = get(key))
            return dynamic_cast<ValuableSetting<T>*>(s);
        return nullptr;
    }

    template <typename T>
    OptionalValuableSetting<T>* getOptionalValuable(StringView key) const {
        if (auto s = get(key))
            return dynamic_cast<OptionalValuableSetting<T>*>(s);
        return nullptr;
    }

    template <typename Enum>
    EnumSetting<Enum>* getEnum(StringView key) const {
        if (auto s = get(key))
            return dynamic_cast<EnumSetting<Enum>*>(s);
        return nullptr;
    }

    bool open(fs::path file) {
        if (!fs::exists(file)) return false;
        try {
            toml::table tbl = toml::parse_file(file.string());
            for (auto& e: entries) {
                if (auto p = std::get_if<Setting*>(&e)) {
                    if (*p) (*p)->load(tbl);
                } else if (auto cp = std::get_if<UniquePtr<SettingsCategory>>(&e)) {
                    if (cp && *cp) {
                        const auto* node = tbl.get((*cp)->key);
                        if (node && node->is_table()) {
                            const toml::table& subtbl = *node->as_table();
                            (*cp)->loadAll(subtbl);
                        } else {
                            toml::table empty;
                            (*cp)->loadAll(empty);
                        }
                    }
                }
            }
            return true;
        } catch (const toml::parse_error&) {
            return false;
        }
    }

    bool save(fs::path file) const {
        toml::table tbl;
        for (auto& e: entries) {
            if (auto p = std::get_if<Setting*>(&e)) {
                if (*p) (*p)->save(tbl);
            } else if (auto cp = std::get_if<UniquePtr<SettingsCategory>>(&e)) {
                if (cp && *cp) {
                    toml::table catTbl;
                    (*cp)->saveAll(catTbl);
                    tbl.insert_or_assign((*cp)->key, std::move(catTbl));
                }
            }
        }
        std::ofstream os(file);
        if (!os.is_open()) return false;
        os << tbl << std::endl;
        return true;
    }
};

} // namespace cx
