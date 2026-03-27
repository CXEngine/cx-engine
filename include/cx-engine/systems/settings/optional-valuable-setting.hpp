#pragma once

#include <cx-engine/systems/settings/setting.hpp>
#include <cx-engine/systems/settings/validator.hpp>

#include <cx-engine/systems/settings/valuable-setting.hpp>

namespace cx {

class AnyOptionalValuableSetting: public AnyValuableSetting {
public:
    using AnyValuableSetting::AnyValuableSetting;

    virtual bool hasValue() const = 0;
    virtual void emplaceValue() = 0;
    virtual void resetValue() = 0;
};

template <typename T>
class OptionalValuableSetting: public AnyOptionalValuableSetting {
protected:
    Optional<T> value;
    Optional<T> defaultValue;

public:
    OptionalValuableSetting(
        StringView key,
        StringView displayName,
        StringView description,
        Optional<T> defaultValue = std::nullopt,
        SettingFlags flags = SettingFlags::None
    )
        : AnyOptionalValuableSetting(key, displayName, description, flags)
        , value(defaultValue)
        , defaultValue(defaultValue)
    {}

    virtual Pair<TypeIndex, TypeIndex> innerTypes() const override {
        return { typeid(T), typeid(void) };
    }

    bool has() const {
        return value.has_value();
    }

    bool hasValue() const override {
        return value.has_value();
    }

    void emplaceValue() override {
        if (!value.has_value()) {
            value.emplace();
        }
    }

    void resetValue() override {
        value.reset();
    }

    T& get() {
        return *value;
    }

    const T& get() const {
        return *value;
    }

    const Optional<T>& getOptional() const {
        return value;
    }

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
        if (!value.has_value())
            return true;
        return true;
    }

    bool setFromString(StringView sv) override {
        try {
            return set(fromString<T>(sv));
        } catch (...) {
            return false;
        }
    }

    String toString() const override {
        if (!value.has_value()) return "None";
        return strconv::toString(*value);
    }

    void save(toml::table& tbl) const override {
        toml::table optTbl;
        optTbl.insert_or_assign("has-value", value.has_value());
        if (value.has_value()) {
            optTbl.insert_or_assign("value", *value);
        }
        tbl.insert_or_assign(key, optTbl);
    }

    void load(const toml::table& tbl) override {
        const auto* node = tbl.get(key);
        if (!node) { reset(); return; }

        const auto* sub = node->as_table();
        if (!sub) { reset(); return; }

        const auto hvNode = sub->get("has-value");
        if (!hvNode) { reset(); return; }

        const auto hv = hvNode->template value<bool>();
        if (!hv) { reset(); return; }
        if (!*hv) {
            value = std::nullopt;
            return;
        }

        const auto* vNode = sub->get("value");
        if (!vNode) { reset(); return; }

        auto v = vNode->template value<T>();
        if (!v) { reset(); return; }

        setOrReset(*v);
    }
};

template <typename T, ValidatorFor<T> TValidator>
class ValidatedOptionalValuableSetting: public OptionalValuableSetting<T> {
protected:
    TValidator validator;

public:
    ValidatedOptionalValuableSetting(
        StringView key,
        StringView displayName,
        StringView description,
        Optional<T> defaultValue = std::nullopt,
        TValidator validator = {},
        SettingFlags flags = SettingFlags::None
    )
        : OptionalValuableSetting<T>(key, displayName, description, defaultValue, flags)
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
        if (!this->value.has_value())
            return true;
        return validator.validate(*this->value);
    }
};

}
