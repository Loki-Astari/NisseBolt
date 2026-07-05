#include <gtest/gtest.h>
#include "EventMessage.h"
#include "API.h"
#include "Client.h"
#include "APIConversationsHistory.h"

#include "NisseHTTP/ClientResponse.h"
#include "NisseHTTP/StreamInput.h"
#include "ThorSerialize/Traits.h"
#include "ThorSerialize/SerUtil.h"
#include "ThorSerialize/JsonThor.h"



TEST(EventMessageTest, HistoryResponse)
{
    std::stringstream  reply("HTTP/1.1 200 OK\r\n"
                             "Content-Length: 4395\r\n"
                             "\r\n"
                             R"({"ok":true,"latest":"1782890500.000000","oldest":"1782890100.000000","messages":[)"
                                    R"({"user":"U095XJHJ1J5","type":"message","ts":"1782890437.133269","client_msg_id":"dce45cb7-6bd7-44ff-8ce2-87d2eedd7715","text":"Thsis is a game 4","team":"T095XJHH589","blocks":[{"type":"rich_text","block_id":"N6O3j","elements":[{"type":"rich_text_section","elements":[{"type":"text","text":"Thsis is a game 4"}]}]}]},)"
                                    R"({"text":"","files":[{"id":"F0BEDBQ3WR0","created":1782890249,"timestamp":1782890249,"mimetype":"image\/jpeg","filetype":"jpg","pretty_type":"JPEG","user":"U095XJHJ1J5","user_team":"T095XJHH589","editable":false,"size":2771637,"mode":"hosted","is_external":false,"external_type":"","is_public":true,"public_url_shared":false,"display_as_bot":false,"username":"","name":"IMG_0750.jpg","title":"IMG_0750.jpg","is_modified_by_ai":false,"url_private":"https:\/\/files.slack.com\/files-pri\/T095XJHH589-F0BEDBQ3WR0\/img_0750.jpg","url_private_download":"https:\/\/files.slack.com\/files-pri\/T095XJHH589-F0BEDBQ3WR0\/download\/img_0750.jpg","media_display_type":"unknown","thumb_64":"https:\/\/files.slack.com\/files-tmb\/T095XJHH589-F0BEDBQ3WR0-af7e7a432f\/img_0750_64.jpg","thumb_80":"https:\/\/files.slack.com\/files-tmb\/T095XJHH589-F0BEDBQ3WR0-af7e7a432f\/img_0750_80.jpg","thumb_360":"https:\/\/files.slack.com\/files-tmb\/T095XJHH589-F0BEDBQ3WR0-af7e7a432f\/img_0750_360.jpg","thumb_360_w":270,"thumb_360_h":360,"thumb_480":"https:\/\/files.slack.com\/files-tmb\/T095XJHH589-F0BEDBQ3WR0-af7e7a432f\/img_0750_480.jpg","thumb_480_w":360,"thumb_480_h":480,"thumb_160":"https:\/\/files.slack.com\/files-tmb\/T095XJHH589-F0BEDBQ3WR0-af7e7a432f\/img_0750_160.jpg","thumb_720":"https:\/\/files.slack.com\/files-tmb\/T095XJHH589-F0BEDBQ3WR0-af7e7a432f\/img_0750_720.jpg","thumb_720_w":540,"thumb_720_h":720,"thumb_800":"https:\/\/files.slack.com\/files-tmb\/T095XJHH589-F0BEDBQ3WR0-af7e7a432f\/img_0750_800.jpg","thumb_800_w":800,"thumb_800_h":1067,"thumb_960":"https:\/\/files.slack.com\/files-tmb\/T095XJHH589-F0BEDBQ3WR0-af7e7a432f\/img_0750_960.jpg","thumb_960_w":720,"thumb_960_h":960,"thumb_1024":"https:\/\/files.slack.com\/files-tmb\/T095XJHH589-F0BEDBQ3WR0-af7e7a432f\/img_0750_1024.jpg","thumb_1024_w":768,"thumb_1024_h":1024,"original_w":3024,"original_h":4032,"thumb_tiny":"AwAwACRGjtt33SPof8aetqjjcjAj0qCJg2S1SqShDL1PBxUFjZAYm2sMenvVWdsuD7Venk8y03FWODwxxVWKRhu6YIx+FUiWRiFyAcEZ9qXyW9\/yq4kse35lwRTvNh9KLisVYv3ZO71qZ1JO1OcDJ9KZ5DYzkbR3NTKQi4z3GSaksljGyHZwc84qhLiKQgjA7VeALlQDwKz5xiVweSGPNMRC5LNmm4p+KMU7isX49xHAxnnmnpall+dyO+eKkRcDNS7gBmkMRIgo6Z9zVa8t+soz2BH9ama6VeCwqKdvMgLKOMjnPWgRS2j1o2j1p2Pb9aMew\/OgZ\/\/Z","permalink":"https:\/\/thorsanvilworkspace.slack.com\/files\/U0A6EA93KEE\/F0BEDBQ3WR0\/img_0750.jpg","permalink_public":"https:\/\/slack-files.com\/T095XJHH589-F0BEDBQ3WR0-503afda18b","is_starred":false,"skipped_shares":true,"has_rich_preview":false,"file_access":"visible"}],"upload":false,"user":"U095XJHJ1J5","display_as_bot":false,"type":"message","ts":"1782890253.841419","client_msg_id":"a388ee56-6f66-4225-ae34-dade1c83560e"},)"
                                    R"({"user":"U095XJHJ1J5","type":"message","ts":"1782890181.392749","client_msg_id":"add00c73-88d6-4ec3-828c-c5f841965cb3","text":"Long time 3","team":"T095XJHH589","blocks":[{"type":"rich_text","block_id":"saJcd","elements":[{"type":"rich_text_section","elements":[{"type":"text","text":"Long time 3"}]}]}]},)"
                                    R"({"user":"U095XJHJ1J5","type":"message","ts":"1782890168.479749","client_msg_id":"e19b2144-0619-46a7-a09d-5fb046b57eb3","text":"Some more stuff","team":"T095XJHH589","blocks":[{"type":"rich_text","block_id":"jcxlw","elements":[{"type":"rich_text_section","elements":[{"type":"text","text":"Some more stuff"}]}]}]},)"
                                    R"({"user":"U095XJHJ1J5","type":"message","ts":"1782890148.538769","client_msg_id":"c91e44e5-2fe7-49f1-9bdd-1fee86dfd651","text":"Anither point 2","team":"T095XJHH589","blocks":[{"type":"rich_text","block_id":"HG3hb","elements":[{"type":"rich_text_section","elements":[{"type":"text","text":"Anither point 2"}]}]}]},)"
                                    R"({"user":"U095XJHJ1J5","type":"message","ts":"1782890110.511419","client_msg_id":"46ec467f-4cc1-4f42-8361-8d25b5c85202","text":"Point and click","team":"T095XJHH589","blocks":[{"type":"rich_text","block_id":"DOtvs","elements":[{"type":"rich_text_section","elements":[{"type":"text","text":"Point and click"}]}]}]})"
                                    R"(],)"
                                    R"("has_more":false,"is_limited":true,"pin_count":0,"channel_actions_ts":null,"channel_actions_count":0})"
                            );
    ThorsAnvil::Nisse::HTTP::ClientResponse   response(reply);
    ThorsAnvil::Nisse::HTTP::StreamInput      input(reply, response.getContentSize());

    using OutputType = std::variant<ThorsAnvil::Slack::API::Error, ThorsAnvil::Slack::API::Conversation::HistoryReply>;
    OutputType      result;
    input >> ThorsAnvil::Serialize::jsonImporter(result, ThorsAnvil::Serialize::ParserConfig{}.setIdentifyDynamicClass([&](ThorsAnvil::Serialize::DataInputStream&){return ThorsAnvil::Slack::Client::getEventType<ThorsAnvil::Slack::API::Conversation::HistoryReply>(input);}));


    ASSERT_TRUE(std::holds_alternative<ThorsAnvil::Slack::API::Conversation::HistoryReply>(result));
    ThorsAnvil::Slack::API::Conversation::HistoryReply&  history = std::get<ThorsAnvil::Slack::API::Conversation::HistoryReply>(result);

    ASSERT_EQ(6, history.messages.size());
    EXPECT_EQ("U095XJHJ1J5", history.messages[0].user);
    ASSERT_EQ(1, history.messages[1].files.value().size());

}
