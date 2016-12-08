module EdText
  def self.blue_understand
    Oneshot.msgbox Oneshot::Msg::YESNO, tr("Do you understand what this means?")
  end
  def self.info(text)
    timestart = Time.now
    if(Graphics.fullscreen == true)
	  Graphics.fullscreen = false
	  $console = false
	end
	sleep 0.2
	Graphics.update
    text.gsub!("\\p", $game_oneshot.player_name)
    Oneshot.msgbox Oneshot::Msg::INFO, tr(text)
    $game_temp.prompt_wait = ((Time.now - timestart) * 60).round
  end
  def self.yesno(text)
    timestart = Time.now
    if(Graphics.fullscreen == true)
	  Graphics.fullscreen = false
	  $console = false
	end
	sleep 0.2
	Graphics.update
    text.gsub!("\\p", $game_oneshot.player_name)
    result = Oneshot.msgbox Oneshot::Msg::YESNO, tr(text)
    $game_temp.prompt_wait = ((Time.now - timestart) * 60).round
    return result
  end
  def self.err(text)
    timestart = Time.now
    if(Graphics.fullscreen == true)
	  Graphics.fullscreen = false
	  $console = false
	end
	sleep 0.2
	Graphics.update
    text.gsub!("\\p", $game_oneshot.player_name)
    result = Oneshot.msgbox Oneshot::Msg::ERR, tr(text)
    $game_temp.prompt_wait = ((Time.now - timestart) * 60).round
    return result
  end
end
