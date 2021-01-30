def demo_entity_message
  Oneshot.msgbox Oneshot::Msg::INFO, "[...Who am I kidding. Of course I don't.]"
  Oneshot.msgbox Oneshot::Msg::ERR, "[If you DO decide to visit this world again, #{$game_oneshot.player_name}... I won't be looking forward to it at all.]"
  Oneshot.msgbox Oneshot::Msg::ERR, "[I'm so tired.]"
  Oneshot.msgbox Oneshot::Msg::ERR, "[Goodbye.]"
  Graphics.frame_reset
end
