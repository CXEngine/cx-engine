#pragma once

#include <cx-engine/systems/settings/setting.hpp>
#include <cx-engine/systems/settings/validator.hpp>

#include <cx-engine/utils/enumstr.hpp>

#include <algorithm>

namespace cx {

template <typename Enum>
class EnumSetting;

template <typename Enum, ValidatorFor<Enum> TValidator>
class ValidatedEnumSetting;

template <typename Enum>
struct EnumValidator {
    bool validate(Enum v) const {
        int i = static_cast<int>(v);
        return i >= 0 && i < (int) EnumStrings<Enum>::names().size();
    }
};

class AnyEnumSetting: public Setting {
public:
    virtual Pair<TypeIndex, TypeIndex> innerTypes() const = 0;
    using Setting::Setting;
};

template <typename Enum>
class EnumSetting: public AnyEnumSetting {
protected:
    Enum value;
    Enum defaultValue;

public:
    EnumSetting(
        StringView key,
        StringView displayName,
        StringView description,
        Enum defaultValue,
        SettingFlags flags = SettingFlags::None
    )
        : AnyEnumSetting(key, displayName, description, flags)
        , value(defaultValue)
        , defaultValue(defaultValue)
    {}

    virtual Pair<TypeIndex, TypeIndex> innerTypes() const override {
        return { typeid(Enum), typeid(void) };
    }

    virtual bool set(Enum v) {
        value = v;
        return true;
    }

    void setOrReset(Enum v) {
        value = v;
        if (!validate())
            reset();
    }

    Enum& get() { return value; }

    void reset() override {
        value = defaultValue;
    }

    bool validate() const override {
        return true;
    }

    void save(toml::table& tbl) const override {
        tbl.insert_or_assign(key, EnumStrings<Enum>::names()[(int) value]);
    }

    void load(const toml::table& tbl) override {
        if (auto node = tbl[key]) {
            if (auto s = node.template value<String>()) {
                auto names = EnumStrings<Enum>::names();
                auto it = std::find(names.begin(), names.end(), StringView(*s));
                if (it != names.end()) {
                    const Enum v = static_cast<Enum>(
                        std::distance(names.begin(), it)
                    );
                    setOrReset(v);
                }
            }
        }
    }

    String toString() const override {
        auto names = EnumStrings<Enum>::names();
        auto idx = static_cast<usize>(value);
        if (idx < names.size())
            return String(names[idx]);
        return String();
    }

    bool setFromString(StringView s) override {
        auto names = EnumStrings<Enum>::names();
        auto it = std::find(names.begin(), names.end(), s);
        if (it != names.end()) {
            value = static_cast<Enum>(std::distance(names.begin(), it));
            return true;
        }
        return false;
    }
};

template <typename Enum, ValidatorFor<Enum> TValidator>
class ValidatedEnumSetting: public EnumSetting<Enum> {
protected:
    TValidator validator;
public:
    ValidatedEnumSetting(
        StringView key,
        StringView displayName,
        StringView description,
        Enum defaultValue,
        TValidator validator = {},
        SettingFlags flags = SettingFlags::None
    )
        : EnumSetting<Enum>(key, displayName, description, defaultValue, flags)
        , validator(validator)
    {}

    virtual Pair<TypeIndex, TypeIndex> innerTypes() const override {
        return { typeid(Enum), typeid(TValidator) };
    }

    bool set(Enum v) override {
        if (!validator.validate(v))
            return false;
        this->value = v;
        return true;
    }

    bool validate() const override {
        return validator.validate(this->value);
    }
};

} // namespace cx
