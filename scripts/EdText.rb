module EdText
  def self.blue_understand
    Oneshot.msgbox Oneshot::Msg::YESNO, tr("Do you understand what this means?")
  end
  def self.info(text)
    text.gsub!("\\p", $game_oneshot.player_name)
    Oneshot.msgbox Oneshot::Msg::INFO, tr(text)
  end
  def self.yesno(text)
    text.gsub!("\\p", $game_oneshot.player_name)
    Oneshot.msgbox Oneshot::Msg::YESNO, tr(text)
  end
end
