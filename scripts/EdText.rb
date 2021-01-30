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
      if Oneshot::OS == "macos"
        sleep 0.65
      else
        sleep 0.2
      end
      Graphics.update
    end
    result = nil
    # so here, we strip unescaped newlines from the untranslated text
    # for the translator to find the right string. Then from the
    # translated text, we replace escaped newlines with empty strings.
    # yeah.
    text = Language.tr(text.to_s.gsub(/\s*\n\s*/, " ").strip)
        .to_s.gsub(/\s*\\n\s*/, "").strip
    thread = Thread.new do
      result = Oneshot.msgbox(type, text
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
