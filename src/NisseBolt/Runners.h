#ifndef  THORSANVIL_NISSE_BOLT_HANDLERS_H
#define  THORSANVIL_NISSE_BOLT_HANDLERS_H

#include "NisseBoltConfig.h"
#include "Ack.h"
#include "Say.h"
#include "Response.h"

#include "ThorsSlack/EventHandler.h"

#include <functional>
#include <variant>
#include <string>

namespace ThorsAnvil::Nisse::Bolt
{
namespace Event = ThorsAnvil::Slack::Event;

template<typename T>
using EventRunner          = std::function<void(T const& event, Say const& say)>;
using AnyEventRunner       = std::variant<
                                        EventRunner<Event::AppDeleted>, EventRunner<Event::AppHomeOpened>, EventRunner<Event::AppInstalled>, EventRunner<Event::AppRateLimited>, EventRunner<Event::AppRequested>, EventRunner<Event::AppUninstalledTeam>, EventRunner<Event::AppUninstalled>, EventRunner<Event::AppMentioned>,
                                        EventRunner<Event::AssistantThreadContextChanged>, EventRunner<Event::AssistantThreadStarted>,
                                        EventRunner<Event::CallRejected>,
                                        EventRunner<Event::ChannelArchive>, EventRunner<Event::ChannelCreated>, EventRunner<Event::ChannelDeleted>, EventRunner<Event::ChannelHistoryChanged>, EventRunner<Event::ChannelIdChanged>, EventRunner<Event::ChannelLeft>, EventRunner<Event::ChannelPostingPermissions>, EventRunner<Event::ChannelRename>, EventRunner<Event::ChannelShared>, EventRunner<Event::ChannelUnshared>,
                                        EventRunner<Event::DndUpdated>, EventRunner<Event::DndUpdatedUser>,
                                        EventRunner<Event::EmailDomainChanged>,
                                        EventRunner<Event::EmojiChanged>,
                                        EventRunner<Event::EntityDetailsRequested>,
                                        EventRunner<Event::FileChange>, EventRunner<Event::FileCommentAdded>, EventRunner<Event::FileCommentDeleted>, EventRunner<Event::FileCommentEdited>, EventRunner<Event::FileCreated>, EventRunner<Event::FileDeleted>, EventRunner<Event::FilePublic>, EventRunner<Event::FileShared>, EventRunner<Event::FileUnshared>,
                                        EventRunner<Event::FunctionExecuted>,
                                        EventRunner<Event::GridMigrationFinished>, EventRunner<Event::GridMigrationStarted>,
                                        EventRunner<Event::GroupClose>, EventRunner<Event::GroupDeleted>, EventRunner<Event::GroupHistoryChanged>, EventRunner<Event::GroupLeft>, EventRunner<Event::GroupOpen>, EventRunner<Event::GroupRename>,
                                        EventRunner<Event::ImClose>, EventRunner<Event::ImCreated>, EventRunner<Event::ImHistoryChanged>, EventRunner<Event::ImOpen>,
                                        EventRunner<Event::InviteRequested>,
                                        EventRunner<Event::LinkShared>,
                                        EventRunner<Event::MemberJoinedChannel>, EventRunner<Event::MemberLeftChannel>,
                                        // Don't include Event::Message
                                        // We automate the addition of the event handlers via this type.
                                        // EventRunner<Event::Message>,
                                        EventRunner<Event::MessageMetadataPosted>, EventRunner<Event::MessageMetadataUpdated>, EventRunner<Event::MessageMetadataDeleted>,
                                        EventRunner<Event::PinAdded>, EventRunner<Event::PinRemoved>,
                                        EventRunner<Event::ReactionAdded>, EventRunner<Event::ReactionRemoved>,
                                        EventRunner<Event::SharedChannelInviteAccepted>, EventRunner<Event::SharedChannelInviteApproved>, EventRunner<Event::SharedChannelInviteDeclined>, EventRunner<Event::SharedChannelInviteReceived>, EventRunner<Event::SharedChannelInviteRequested>,
                                        EventRunner<Event::StarAdded>, EventRunner<Event::StarRemoved>,
                                        EventRunner<Event::SubteamCreated>, EventRunner<Event::SubteamMembersChanged>, EventRunner<Event::SubteamSelfAdded>, EventRunner<Event::SubteamSelfRemoved>, EventRunner<Event::SubteamUpdated>,
                                        EventRunner<Event::TeamAccessGranted>, EventRunner<Event::TeamAccessRevoked>, EventRunner<Event::TeamDomainChange>, EventRunner<Event::TeamJoin>, EventRunner<Event::TeamRename>,
                                        EventRunner<Event::TokensRevoked>,
                                        EventRunner<Event::UserChange>, EventRunner<Event::UserConnection>, EventRunner<Event::UserHuddleChanged>
                              >;

using Filter                = std::function<bool(ThorsAnvil::Slack::Event::Message const&)>;
using MessageRunner         = EventRunner<ThorsAnvil::Slack::Event::Message>;

using SlashCommandRunner    = std::function<void(Ack const&, Response const&, ThorsAnvil::Slack::SlashCommand const&)>;

using ActionRunner          = std::function<void(Ack const&, Response const&, ThorsAnvil::Slack::API::BlockActions const&, std::string const& value)>;

using ViewSubmitRunner      = std::function<void(Ack const&, Response const&, ThorsAnvil::Slack::API::Views::ViewSubmission const&)>;
using ViewClosedRunner      = std::function<void(Ack const&, Response const&, ThorsAnvil::Slack::API::Views::ViewClosed const&)>;

using ShortcutRunner        = std::function<void(Ack const&, Response const&, ThorsAnvil::Slack::API::ShortCutMessage const&)>;

}

#endif
