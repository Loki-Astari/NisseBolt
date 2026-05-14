#ifndef THORSANVIL_SLACK_API_BLOCKACTIONS_STATE_H
#define THORSANVIL_SLACK_API_BLOCKACTIONS_STATE_H

#include "API.h"
#include "SlackBlockKit.h"
#include <string>
#include <memory>

namespace BK = ThorsAnvil::Slack::BlockKit;

namespace ThorsAnvil::Slack::API
{

using NullVecElOption   = std::unique_ptr<BK::VecElOption>;
using NullElText        = std::unique_ptr<BK::ElText>;
using NullElOption      = std::unique_ptr<BK::ElOption>;
using NullString        = std::unique_ptr<std::string>;
using NullTime          = std::unique_ptr<std::time_t>;

// The value for a CheckBox
struct ElActCheckboxValue
{
    // std::string                         type;               // checkboxes Have Example
    NullVecElOption                     selected_options;
    NullVecElOption const&  getValue() const {return selected_options;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActCheckboxValue, checkboxes);
    ThorsAnvil_TypeFieldName(type);
};

struct ElActDatePickerValue
{
    // std::string                         type;               // datepicker Have Example
    NullString                          selected_date;
    NullString const&  getValue() const {return selected_date;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActDatePickerValue, datepicker);
    ThorsAnvil_TypeFieldName(type);
};

struct ElActDatetimePickerValue
{
    // std::string                         type;               // datetimepicker Have Example
    NullTime                            selected_date_time;
    NullTime const&  getValue() const {return selected_date_time;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActDatetimePickerValue, datetimepicker);
    ThorsAnvil_TypeFieldName(type);
};


struct ElActEMailValue
{
    // std::string                         type;               // email_text_input => NEED EXAMPLE <=
    NullString                          initial_value;
    NullString const&  getValue() const {return initial_value;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActEMailValue, email_text_input);
    ThorsAnvil_TypeFieldName(type);
};

struct ElActNumberInputValue
{
    // std::string                         type;               // number_input => NEED EXAMPLE <=
    NullString                          initial_value;
    NullString const&  getValue() const {return initial_value;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActNumberInputValue, number_input);
    ThorsAnvil_TypeFieldName(type);
};

// The value for a text input field.
struct ElActPlainTextInputValue
{
    //std::string                         type;               // plain_text_input
    NullString                          value;

    NullString const&  getValue() const {return value;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActPlainTextInputValue, plain_text_input);
    ThorsAnvil_TypeFieldName(type);
};

// The value for a RadioButton
struct ElActRadioButtonValue
{
    // std::string                         type;               // radio_buttons: Have Example
    BK::ElOption                        selected_option;
    BK::ElOption const&  getValue() const {return selected_option;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActRadioButtonValue, radio_buttons);
    ThorsAnvil_TypeFieldName(type);
};

struct ElActSelectMenuValue
{
    // std::string                         type;               // static_select: Have Example
    NullElOption                        selected_option;
    NullElOption const&  getValue() const {return selected_option;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActSelectMenuValue, static_select);
    ThorsAnvil_TypeFieldName(type);
};

struct ElActTimePickerValue
{
    // std::string                         type;               // timepicker: Have Example
    NullString                          selected_time;
    NullString const&  getValue() const {return selected_time;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActTimePickerValue, timepicker);
    ThorsAnvil_TypeFieldName(type);
};

// InputElement = std::variant<ElActURLInput>;
struct ElActURLInputValue
{
    // std::string                         type;               // url_text_input: => NEED EXAMPLE <=
    NullString                          initial_value;
    NullString const&  getValue() const {return initial_value;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::ElActURLInputValue, url_text_input);
    ThorsAnvil_TypeFieldName(type);
};


// See Also InputElement.
// This set of variants should be kept in sync with the type InputElement.
using InputValue = std::variant<ElActCheckboxValue, ElActDatePickerValue, ElActDatetimePickerValue, ElActEMailValue, ElActNumberInputValue, ElActPlainTextInputValue, ElActRadioButtonValue, /*ElActRichTextInputValue,*/ ElActSelectMenuValue, ElActTimePickerValue, ElActURLInputValue>;


// action_id of the "InputElement" object
// Note The "InputElement" is part of the "Input" object.
using SlackValue  = std::map<std::string, InputValue>;

// block_id of the "Input" object
using SlackValues = std::map<std::string, SlackValue>;
struct SlackState
{
    SlackValues                         values;
    template<typename T>
    typename T::ValueReturnType const& getValue(std::string const& block_id, std::string const& action_id) const
    {
        auto findBlock = values.find(block_id);
        if (findBlock != std::end(values)) {
            auto const& actionMap = findBlock->second;
            auto findAction = actionMap.find(action_id);
            if (findAction != std::end(actionMap)) {
                return std::get<typename T::ValueStorageType>(findAction->second).getValue();
            }
        }
        static typename T::ValueReturnType nullOption;
        return nullOption;
    }
};
using NullSlackState = std::unique_ptr<SlackState>;

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::ElActCheckboxValue, selected_options);         //
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::ElActDatePickerValue, selected_date);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::ElActDatetimePickerValue, selected_date_time);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::ElActEMailValue, initial_value);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::ElActNumberInputValue, initial_value);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::ElActPlainTextInputValue, value);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::ElActRadioButtonValue, selected_option);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::ElActSelectMenuValue, selected_option);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::ElActTimePickerValue, selected_time);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::ElActURLInputValue, initial_value);

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::SlackState, values);

#endif
