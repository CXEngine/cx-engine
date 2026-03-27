#pragma once

#include <cx-engine/systems/settings/setting.hpp>
#include <cx-engine/systems/settings/validator.hpp>

namespace cx {

class AnyValuableSetting: public Setting {
public:
    virtual Pair<TypeIndex, TypeIndex> innerTypes() const = 0;
    using Setting::Setting;
};

template <typename T>
class ValuableSetting: public AnyValuableSetting {
protected:
    T value;
    T defaultValue;

public:
    ValuableSetting(
        StringView key,
        StringView displayName,
        StringView description,
        T defaultValue,
        SettingFlags flags = SettingFlags::None
    )
        : AnyValuableSetting(key, displayName, description, flags)
        , value(defaultValue)
        , defaultValue(defaultValue)
    {}

    virtual Pair<TypeIndex, TypeIndex> innerTypes() const override {
        return { typeid(T), typeid(void) };
    }

    T& get() { return value; }
    const T& get() const { return value; }

    virtual bool set(const T& v) {
        value = v;
        return true;
    }

    void setOrReset(const T& v) {
        value = v;
        if (!validate())
            reset();
    }

    void reset() override {
        value = defaultValue;
    }

    bool validate() const override {
        return true;
    }

    void save(toml::table& tbl) const override {
        tbl.insert_or_assign(key, value);
    }

    void load(const toml::table& tbl) override {
        if (auto node = tbl[key]) {
            if (auto v = node.template value<T>()) {
                value = *v;
                if (!validate())
                    reset();
            }
        }
    }

    bool setFromString(StringView sv) override {
        try {
            return set(fromString<T>(sv));
        } catch (...) {
            return false;
        }
    }

    String toString() const override {
        return strconv::toString(value);
    }
};

template <typename T, ValidatorFor<T> TValidator>
class ValidatedValuableSetting: public ValuableSetting<T> {
protected:
    TValidator validator;

public:
    ValidatedValuableSetting(
        StringView key,
        StringView displayName,
        StringView description,
        T defaultValue,
        TValidator validator = {},
        SettingFlags flags = SettingFlags::None
    )
        : ValuableSetting<T>(key, displayName, description, defaultValue, flags)
        , validator(validator)
    {}

    virtual Pair<TypeIndex, TypeIndex> innerTypes() const override {
        return { typeid(T), typeid(TValidator) };
    }

    bool set(const T& v) override {
        if (validator.validate(v)) {
            this->value = v;
            return true;
        }
        return false;
    }

    bool validate() const override {
        return validator.validate(this->value);
    }
};

using BooleanSetting = ValuableSetting<bool>;

}
