#include "gtest/gtest.h"

#if !(defined(DISABLE_SLACKTEST) && (DISABLE_SLACKTEST == 1))

#include "Environment.h"
#include "SlackClient.h"
#include "APIChatMessage.h"
#include "SlackBlockKit.h"

#include <variant>

using namespace std::literals::string_literals;
namespace BK = ThorsAnvil::Slack::BlockKit;

using ThorsAnvil::Slack::SlackClient;
using ThorsAnvil::Slack::API::Chat::POSTMessage;

extern SlackClient             client;
extern Environment             environment;

TEST(SlackBlockKitTest, Block_Section_ElText)
{
    POSTMessage::Reply      reply;
    client.sendMessage(POSTMessage{
                            .channel = environment.slackChannel,
                            .blocks = BK::Blocks{
                                BK::makeSectionMarkD("Section With Text"),
                                BK::Divider{},
                                BK::makeSectionPlain("More Text after a divider")
                            }
                       }, reply, true);
    ThorsAnvil::Slack::BlockKit::TextGetter     text;

    ASSERT_TRUE(reply.ok);
    ASSERT_EQ(3, reply.message.blocks.size());
    ASSERT_TRUE(std::holds_alternative<BK::Section>(reply.message.blocks[0]));
    BK::Section&            section1 = std::get<BK::Section>(reply.message.blocks[0]);
    ASSERT_TRUE(section1.text.has_value());
    EXPECT_EQ("Section With Text", std::visit(text, section1.text.value()));

    ASSERT_TRUE(std::holds_alternative<BK::Divider>(reply.message.blocks[1]));

    ASSERT_TRUE(std::holds_alternative<BK::Section>(reply.message.blocks[2]));
    BK::Section&            section2 = std::get<BK::Section>(reply.message.blocks[2]);
    ASSERT_TRUE(section2.text.has_value());
    EXPECT_EQ("More Text after a divider", std::visit(text, section2.text.value()));
}

TEST(SlackBlockKitTest, Block_RichText_Section_Text)
{
    POSTMessage::Reply      reply;
    client.sendMessage(POSTMessage{
                            .channel = environment.slackChannel,
                            .blocks = BK::Blocks{BK::RichText{}.addSection({{"Rich Text: Section", {true, true, true, true}}})}
                       }, reply, true);
    ASSERT_TRUE(reply.ok);
    ASSERT_EQ(1, reply.message.blocks.size());
    ASSERT_TRUE(std::holds_alternative<BK::RichText>(reply.message.blocks[0]));
    BK::RichText&           text = std::get<BK::RichText>(reply.message.blocks[0]);

    ASSERT_TRUE(std::holds_alternative<BK::RichTextSection>(text.elements[0]));
    BK::RichTextSection&    section = std::get<BK::RichTextSection>(text.elements[0]);

    ASSERT_TRUE(std::holds_alternative<BK::Text>(section.elements[0]));
    BK::Text&               rtext = std::get<BK::Text>(section.elements[0]);

    EXPECT_EQ("Rich Text: Section", rtext.text);
    EXPECT_TRUE(rtext.style->bold);
    EXPECT_TRUE(rtext.style->italic);
    EXPECT_TRUE(rtext.style->strike);
    EXPECT_TRUE(rtext.style->code);
}

TEST(SlackBlockKitTest, Block_RichText_ListBullet_Section_Text)
{
    POSTMessage::Reply      reply;
    client.sendMessage(POSTMessage{
                            .channel = environment.slackChannel,
                            .blocks = BK::Blocks{BK::RichText{}.addList(BK::bullet, {{"Rich Text: List-bullet",{true,false,true,false}}})}
                       }, reply, true);
    ASSERT_TRUE(reply.ok);
    ASSERT_EQ(1, reply.message.blocks.size());
    ASSERT_TRUE(std::holds_alternative<BK::RichText>(reply.message.blocks[0]));
    BK::RichText&           text = std::get<BK::RichText>(reply.message.blocks[0]);

    ASSERT_TRUE(std::holds_alternative<BK::RichTextList>(text.elements[0]));
    BK::RichTextList&       list = std::get<BK::RichTextList>(text.elements[0]);
    EXPECT_EQ(BK::bullet, list.style);

    BK::RichTextSection&    section = list.elements[0];

    ASSERT_TRUE(std::holds_alternative<BK::Text>(section.elements[0]));
    BK::Text&               rtext = std::get<BK::Text>(section.elements[0]);

    EXPECT_EQ("Rich Text: List-bullet", rtext.text);
    EXPECT_TRUE(rtext.style->bold);
    EXPECT_FALSE(rtext.style->italic);
    EXPECT_TRUE(rtext.style->strike);
    EXPECT_FALSE(rtext.style->code);
}

TEST(SlackBlockKitTest, Block_RichText_ListOrder_Section_Text)
{
    POSTMessage::Reply      reply;
    client.sendMessage(POSTMessage{
                            .channel = environment.slackChannel,
                            .blocks = BK::Blocks{BK::RichText{}.addList(BK::ordered, {{"Rich Text: List-ordered",{false,true,false,true}}})}
                       }, reply, true);
    ASSERT_TRUE(reply.ok);
    ASSERT_EQ(1, reply.message.blocks.size());
    ASSERT_TRUE(std::holds_alternative<BK::RichText>(reply.message.blocks[0]));
    BK::RichText&           text = std::get<BK::RichText>(reply.message.blocks[0]);

    ASSERT_TRUE(std::holds_alternative<BK::RichTextList>(text.elements[0]));
    BK::RichTextList&       list = std::get<BK::RichTextList>(text.elements[0]);
    EXPECT_EQ(BK::ordered, list.style);

    BK::RichTextSection&    section = list.elements[0];

    ASSERT_TRUE(std::holds_alternative<BK::Text>(section.elements[0]));
    BK::Text&               rtext = std::get<BK::Text>(section.elements[0]);

    EXPECT_EQ("Rich Text: List-ordered", rtext.text);
    EXPECT_FALSE(rtext.style->bold);
    EXPECT_TRUE(rtext.style->italic);
    EXPECT_FALSE(rtext.style->strike);
    EXPECT_TRUE(rtext.style->code);
}

TEST(SlackBlockKitTest, Block_RichText_Pre_Formatted)
{
    POSTMessage::Reply  reply;
    POSTMessage         message = {
                            .channel = environment.slackChannel,
                            .blocks = BK::Blocks{BK::RichText{}.addPreForm(1, {{"Rich Text PreFormaatted Test", {true, true, false,false}}})}
                        };
    client.sendMessage(message, reply, true);
    ASSERT_TRUE(reply.ok);
}

TEST(SlackBlockKitTest, Block_RichText_QUOTE)
{
    POSTMessage::Reply      reply;
    client.sendMessage(POSTMessage{
                            .channel = environment.slackChannel,
                            .blocks = BK::Blocks{BK::RichText{}.addQuote(1, {{"Rich Text Quote", {true, false, false, true}}})}
                       }, reply, true);
    ASSERT_TRUE(reply.ok);
}

TEST(SlackBlockKitTest, BlockAllRichTextElements)
{
    POSTMessage::Reply      reply;
    client.sendMessage(POSTMessage{
                            .channel = environment.slackChannel,
                            .blocks = BK::Blocks{
                                        BK::RichText{
                                            .elements = {
                                                BK::RichTextSection{
                                                    .elements = {
                                                        BK::Text{.text = "Rich Text: All ELEMENTS TEST",}
                                                        ,BK::Text{.text = "Broadcast",}
                                                        ,BK::Broadcast{.range = BK::here}
                                                        ,BK::Text{.text = "Color",}
                                                        ,BK::Color{.value = "#FF0000"}
                                                        ,BK::Text{.text = "Channel",}
                                                        ,BK::Channel{.channel_id="C09SGCEFLPJ", .style=BK::InfoStyle{true, false, false, true, false, false}}
                                                        ,BK::Text{.text = "Date",}
                                                        ,BK::Date{.timestamp=-21636000, .format="{date_long_pretty}",.fallback="Date Failed to parse"}
                                                        ,BK::Text{.text = "Emoji",}
                                                        ,BK::Emoji{.name="wave"}
                                                        ,BK::Text{.text = "Link",}
                                                        ,BK::Link{.url="https://github.com/Loki-Astari/ThorsMongo", .text= "Thors Mongo Lib"}
                                                        ,BK::Text{.text = "Text",}
                                                        ,BK::Text{.text = "For Completness another text item",}
                                                        ,BK::Text{.text = "User",}
                                                        ,BK::User{.user_id = "U095XJHJ1J5"}
                                                        ,BK::Text{.text = "UserGroup",}
#if 0
                                                        // Can't test UserGroups with the free version of slack.
                                                        ,BK::UserGroup{.usergroup_id = "U095XJHJ1J5"}
#endif
                                                    }
                                                }
                                            }
                                        }
                            }
                       }, reply, true);
    ASSERT_TRUE(reply.ok);
}

TEST(SlackBlockKitTest, Block_RichText_Failure)
{
    POSTMessage::Reply      reply;
    client.sendMessage(POSTMessage{
                            .channel = environment.slackChannel,
                            .blocks = BK::Blocks{BK::RichText{}.addQuote(3, {{"Rich Text Quote", {true, true, true, true}}})}
                       }, reply);
    ASSERT_FALSE(reply.ok);
}

TEST(SlackBlockKitTest, Block_Action_Button)
{
    POSTMessage     message{
                        .channel = environment.slackChannel,
                        .blocks = BK::Blocks{
                            BK::Context{
                                .elements = {
                                    BK::ImageElement{
                                        .alt_text = "Alternative Text 1",
                                        .image_url = "https://images.unsplash.com/photo-1486365227551-f3f90034a57c?q=80&w=1740&auto=format&fit=crop&ixlib=rb-4.1.0&ixid=M3wxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8fA%3D%3D"
                                    },
                                    BK::ImageElement{
                                        .alt_text = "Alternative Text 2",
                                        .image_url = "https://images.unsplash.com/photo-1606567595334-d39972c85dbe?q=80&w=1287&auto=format&fit=crop&ixlib=rb-4.1.0&ixid=M3wxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8fA%3D%3D"
                                    }
                                }
                            },
                        }
                    };
#if 0
    POSTMessage     message{
                        .channel = environment.slackChannel,
                        .blocks = BK::Blocks{
                            BK::Actions{
                                .elements = {
                                    BK::Button{.text = BK::PlainText{.text = "Push"}, .value="Clicked"}
                                }
                            },
                        }
                    };
    BK::Context& actions = std::get<BK::Context>(message.blocks.value()[0]);

    ThorsAnvil::Serialize::PrinterConfig config{ThorsAnvil::Serialize::OutputType::Stream};
    POSTMessage::Reply      reply;
    client.sendMessage(message, reply, true);
    ASSERT_TRUE(reply.ok);
#endif
}
TEST(SlackBlockKitTest, Block_Section_All_Standard_Elements)
{
    POSTMessage::Reply      reply;
    client.sendMessage(POSTMessage{
                           .channel = environment.slackChannel,
                                // Actions, Context, Context_Actions, Divider, File, Header, Image, Input, Markdown, RichText, Section, Table, Video
                           .blocks = BK::Blocks{
                                BK::Section{.text = BK::PlainText{.text="Selection with all Elements"}},
                                BK::Divider{},
                                BK::Section{.text = BK::PlainText{.text="Actions"}},
                                BK::Actions{
                                    .elements = {
                                        BK::Button{.text = BK::PlainText{.text = "Push"}, .value="Clicked"}
                                        , BK::Checkboxes{}.addOption({
                                                BK::makeOptionPlain("Opt 1", "V1", "The description 1"),
                                                BK::makeOptionPlain("Opt 2", "V2", "The description 2"),
                                                BK::makeOptionPlain("Opt 3", "V3", "The description 3"),
                                        })
                                        , BK::DatePicker{.placeholder=BK::PlainText{.text="Pick a date"}}
                                        , BK::DateTimePicker{.initial_date_time=-21636000}
                                        , BK::Overflow{}.addOption(BK::makeOptionPlain("Opt 1", "V1", "The description 1")).addOption(BK::makeOptionPlain("Opt 2", "V2", "The description 2")).addOption(BK::makeOptionPlain("Opt 3", "V3", "The description 3"))
                                    }
                                },
                                BK::Divider{},
                                BK::Section{.text = BK::PlainText{.text="Actions"}},
                                BK::Actions{
                                    .elements = {
                                        BK::RadioButtons{}.addOption({
                                                BK::makeOptionPlain("Opt 1", "V1", "The description 1"),
                                                BK::makeOptionPlain("Opt 2", "V2", "The description 2"),
                                                BK::makeOptionPlain("Opt 3", "V3", "The description 3"),
                                        })
                                        , BK::StaticSelect{}.addOption({
                                                BK::makeOptionPlain("Opt 1", "V1", "The description 1"),
                                                BK::makeOptionPlain("Opt 2", "V2", "The description 2"),
                                                BK::makeOptionPlain("Opt 3", "V3", "The description 3"),
                                        })
                                        , BK::TimePicker{}
                                    }
                                },
                                BK::Divider{},
                                BK::Section{.text = BK::PlainText{.text="Context"}},
                                BK::Context{
                                    .elements = {
                                        BK::ImageElement{
                                            .alt_text = "Alternative Text 1",
                                            .image_url = "https://images.unsplash.com/photo-1486365227551-f3f90034a57c?q=80&w=1740&auto=format&fit=crop&ixlib=rb-4.1.0&ixid=M3wxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8fA%3D%3D"
                                        },
                                        BK::ImageElement{
                                            .alt_text = "Alternative Text 2",
                                            .image_url = "https://images.unsplash.com/photo-1606567595334-d39972c85dbe?q=80&w=1287&auto=format&fit=crop&ixlib=rb-4.1.0&ixid=M3wxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8fA%3D%3D"
                                        }
                                    }
                                },
                                BK::Divider{},
                                BK::Section{.text = BK::PlainText{.text="Context Actions"}},
                                BK::ContextActions{
                                    .elements = {
                                        BK::FeedbackButtons {
                                            .positive_button = BK::ElButton{
                                                .text = BK::PlainText{.text="Pos"},.value="Pos1Val"
                                            },
                                            .negative_button = BK::ElButton{
                                                .text = BK::PlainText{.text="Neg"},.value="Neg1Val"
                                            },
                                        },
                                        BK::FeedbackButtons {
                                            .positive_button = BK::ElButton{
                                                .text = BK::PlainText{.text="Pos"},.value="Pos2Val"
                                            },
                                            .negative_button = BK::ElButton{
                                                .text = BK::PlainText{.text="Neg"},.value="Neg2Val"
                                            },
                                        }
                                    }
                                },
                                BK::Divider{},
                                BK::Section{.text = BK::PlainText{.text="Divider"}},
                                BK::Divider{},
                                BK::Section{.text = BK::PlainText{.text="Header"}},
                                BK::Header{.text = BK::PlainText{.text = "Header Text"}},
                                BK::Divider{},
                                BK::Section{.text = BK::PlainText{.text="Image"}},
                                //BK::Image{.alt_text="An image", .image_url="https://marvelofficial.com/wp-content/uploads/2020/10/1030357-247x296.jpg", .title=BK::ElText{.text="Thors Hammer"}},
                                BK::Image{.alt_text="An image", .image_url="https://t3.ftcdn.net/jpg/05/45/12/44/240_F_545124419_HmeRrewyuFXnnoE6LdI9zrSnQ2CqcOTp.jpg", .title=BK::PlainText{.text="Thors Hammer"}},
                                BK::Divider{},
                                BK::Section{.text = BK::PlainText{.text="Input"}},
                                BK::Input{
                                    .label=BK::PlainText{.text="User Input: Checkbox"},
                                    .element=BK::Checkboxes{}.addOption({
                                            BK::makeOptionPlain("Option 1", "1", "The description1"),
                                            BK::makeOptionPlain("Option 2", "2", "The description2"),
                                            BK::makeOptionPlain("Option 3", "3", "The description3"),
                                    }),
                                    .hint=BK::PlainText{.text="Hinty"}
                                },
                                BK::Input{
                                    .label=BK::PlainText{.text="User Input: DatePicker"},
                                    .element=BK::DatePicker{},
                                    .hint=BK::PlainText{.text="Hinty"}
                                },
                                BK::Input{
                                    .label=BK::PlainText{.text="User Input: DateTimePicker"},
                                    .element=BK::DateTimePicker{},
                                    .hint=BK::PlainText{.text="Hinty"}
                                },
                                BK::Input{
                                    .label=BK::PlainText{.text="User Input: RadioButton"},
                                    .element=BK::RadioButtons{}.addOption({
                                            BK::makeOptionPlain("Option1", "1", "The description1"),
                                            BK::makeOptionPlain("Option2", "2", "The description2"),
                                            BK::makeOptionPlain("Option3", "3", "The description3"),
                                    }),
                                    .hint=BK::PlainText{.text="Hinty"}
                                },
                                BK::Input{
                                    .label=BK::PlainText{.text="User Input: Email"},
                                    .element=BK::EmailTextInput{},
                                    .hint=BK::PlainText{.text="Hinty"}
                                },
                                BK::Input{
                                    .label=BK::PlainText{.text="User Input: Number Input"},
                                    .element=BK::NumberInput{
                                        .min_value="10",
                                        .max_value="24"
                                    },
                                    .hint=BK::PlainText{.text="Hinty"}
                                },
                                BK::Input{
                                    .label=BK::PlainText{.text="User Input: Plain Text Input"},
                                    .element=BK::PlainTextInput{
                                        .min_length=10,
                                        .max_length=24
                                    },
                                    .hint=BK::PlainText{.text="Hinty"}
                                },
#if 0
                                BK::Input{
                                    .label=BK::PlainText{.text="User Input: Rich Text Input"},
                                    .element=BK::RichTextInput{
                                        .action_id="rich_text_input-action",
                                        .initial_value=BK::RichText{
                                            .elements ={
                                                BK::RichTextSection{
                                                    .elements={
                                                        BK::Color{.value="#00FF00"}
                                                    }
                                                }
                                            }
                                        }
                                    },
                                    .hint=BK::PlainText{.text="Hinty"}
                                },
#endif
                                BK::Input{
                                    .label=BK::PlainText{.text="User Input: Select Menu"},
                                    .element=BK::StaticSelect{}.addOption({
                                            BK::makeOptionPlain("Option1", "1", "The description1"),
                                            BK::makeOptionPlain("Option2", "2", "The description2"),
                                            BK::makeOptionPlain("Option3", "3", "The description3"),
                                    }),
                                    .hint=BK::PlainText{.text="Hinty"}
                                },
                                BK::Input{
                                    .label=BK::PlainText{.text="User Input: Time Picker"},
                                    .element=BK::TimePicker{},
                                    .hint=BK::PlainText{.text="Hinty"}
                                },
                                BK::Input{
                                    .label=BK::PlainText{.text="User Input: URL Input"},
                                    .element=BK::URLTextInput{},
                                    .hint=BK::PlainText{.text="Hinty"}
                                },
                                BK::Divider{},
                                BK::Section{.text = BK::PlainText{.text="Markdown"}},
                                BK::Markdown{.text="**MarkDown** Bold?"},
                                BK::Divider{},
                                BK::Section{.text = BK::PlainText{.text="RichText"}},
                                BK::RichText{}.addQuote({{"Rich Text Quote", {false,true,false,true}}}),
                                BK::Divider{},
                                BK::Section{.text = BK::PlainText{.text="Section"}},
                                BK::Section{.text = BK::PlainText{.text="This is the section test!!"}},
                                BK::Divider{},
                                BK::Section{.text = BK::PlainText{.text="Table"}},
                                BK::Table{
                                    .rows = {
                                        {
                                            BK::RichText{}.addSection({"C-1-1"}),
                                            BK::RichText{}.addSection({"C-1-2"}),
                                            BK::RichText{}.addSection({"C-1-3"}),
                                        },
                                        {
                                            BK::RichText{}.addSection({"C-2-1"}),
                                            BK::RichText{}.addSection({"C-2-2"}),
                                            BK::RichText{}.addSection({"C-2-3"}),
                                        },
                                        {
                                            BK::RichText{}.addSection({"C-3-1"}),
                                            BK::RichText{}.addSection({"C-3-2"}),
                                            BK::RichText{}.addSection({"C-3-3"}),
                                        }
                                    },
                                    .column_settings = BK::VecElColInfo{
                                        BK::ElColInfo{.align="left"},BK::ElColInfo{.align="center"},BK::ElColInfo{.align="right"}
                                    }
                                },
                                BK::Divider{},
                                BK::Section{.text = BK::PlainText{.text="Video:TODO"}},
#if 0
                                // Need to own the domain that the video is coming from.
                                BK::Video{
                                    .alt_text="A video",
                                    .author_name="Not me",
                                    .description=BK::PlainText{.text="The desction"},
                                    .provider_icon_url="",
                                    .provider_name="Provider Name",
                                    .title=BK::PlainText{.text="Title"},
                                    .thumbnail_url="https://i.ytimg.com/vi/0kNpnjPpf1c/oar2.jpg?sqp=-oaymwEoCJUDENAFSFqQAgHyq4qpAxcIARUAAIhC2AEB4gEKCBgQAhgGOAFAAQ==&rs=AOn4CLDSJuHZ9q0tZXS8Kz6QPXW-5NqG3g",
                                    .video_url="https://www.youtube.com/shorts/0kNpnjPpf1c"
                                },
#endif
                                BK::Divider{}
                            }
                       }, reply, true);
    ASSERT_TRUE(reply.ok);
}

#endif

