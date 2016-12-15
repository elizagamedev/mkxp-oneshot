module EdText
  def self.blue_understand
    self.msgbox(Oneshot::Msg::YESNO, tr("Do you understand what this means?"))
  end

  def self.info(text)
    # TODO tr text
    self.msgbox(Oneshot::Msg::INFO, text)
  end

  def self.yesno(text)
    # TODO tr text
    self.msgbox(Oneshot::Msg::YESNO, text)
  end

  def self.err(text)
    # TODO tr text
    self.msgbox(Oneshot::Msg::ERR, text)
  end

  def self.msgbox(type, text)
    if Graphics.fullscreen
      Graphics.fullscreen = false
      $console = false
      sleep 0.2
    end
    result = nil
    thread = Thread.new do
      result = Oneshot.msgbox(type, text
          .gsub(/\s+\n\s+/, " ")
          .gsub("\\p", $game_oneshot.player_name) +
          " " * 10)
          # HACK: Fuck
    end
    while thread.alive?
      Graphics.update
    end
    result
  end
end
