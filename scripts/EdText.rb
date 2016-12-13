module EdText
  def self.blue_understand
    Oneshot.msgbox Oneshot::Msg::YESNO, tr("Do you understand what this means?")
  end
  def self.info(text)
    if(Graphics.fullscreen == true)
	  Graphics.fullscreen = false
	  $console = false
	end
	sleep 0.2
	Graphics.update
    text.gsub!("\\p", $game_oneshot.player_name)
    Oneshot.msgbox Oneshot::Msg::INFO, tr(text)
    Graphics.frame_reset
  end
  def self.yesno(text)
    if(Graphics.fullscreen == true)
	  Graphics.fullscreen = false
	  $console = false
	end
	sleep 0.2
	Graphics.update
    text.gsub!("\\p", $game_oneshot.player_name)
    result = Oneshot.msgbox Oneshot::Msg::YESNO, tr(text)
    Graphics.frame_reset
    return result
  end
  def self.err(text)
    if(Graphics.fullscreen == true)
	  Graphics.fullscreen = false
	  $console = false
	end
	sleep 0.2
	Graphics.update
    text.gsub!("\\p", $game_oneshot.player_name)
    result = Oneshot.msgbox Oneshot::Msg::ERR, tr(text)
    Graphics.frame_reset
    return result
  end
end
