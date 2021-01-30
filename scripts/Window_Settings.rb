class Window_Settings
  MARGIN = 30
  TITLE_TOP_MARGIN = 32
  TITLE_MARGIN = 100
  ITEM_SPACING = 28
  VALUE_MARGIN = 270
  ACTIVE_MARGIN = MARGIN * 2 + 20
  SETTINGS_FILE_NAME = Oneshot::SAVE_PATH + '/settings.conf'

  def save_settings
    $persistent.save
    File.open(SETTINGS_FILE_NAME, 'w') do |file|
      file.puts('bgm_volume=' + Audio.bgm_volume.to_s)
      file.puts('sfx_volume=' + Audio.sfx_volume.to_s)
      file.puts('fullscreen=' + Graphics.fullscreen.to_s)
      file.puts('default_run=' + $game_switches[251].to_s)
      file.puts('colorblind_mode=' + $game_switches[252].to_s)
      file.puts('automash_enabled=' + $game_switches[253].to_s)
      file.puts('frameskip=' + Graphics.frameskip.to_s)
    end
  end

  def self.load_settings
    return false if !FileTest.exist?(SETTINGS_FILE_NAME)
    File.foreach(SETTINGS_FILE_NAME).with_index do |line, line_num|
      if !line.include? "="
	    next
	  end
	  vals = line.strip.split("=")
	  case vals[0]
	  when "bgm_volume"
	    if !!(vals[1] =~ /\A[-+]?[0-9]+\z/) #is a number
		  number = vals[1].to_i
		  Audio.bgm_volume = number
		end
	  when "sfx_volume"
	    if !!(vals[1] =~ /\A[-+]?[0-9]+\z/) #is a number
		  number = vals[1].to_i
		  Audio.sfx_volume = number
		end
	  when "fullscreen"
	    if vals[1] == "true"
		  Graphics.fullscreen = true
		  $console = true
		elsif vals[1] == "false"
		  Graphics.fullscreen = false
		  $console = false
		end
	  when "default_run"
	    if vals[1] == "true"
		  $game_switches[251] = true
		elsif vals[1] == "false"
		  $game_switches[251] = false
		end
	  when "colorblind_mode"
	    if vals[1] == "true"
		  $game_switches[252] = true
		elsif vals[1] == "false"
		  $game_switches[252] = false
		end
	  when "automash_enabled"
	    if vals[1] == "true"
		  $game_switches[253] = true
		elsif vals[1] == "false"
		  $game_switches[253] = false
		end
	  when "frameskip"
	    if vals[1] == "true"
		  Graphics.frameskip = true
		elsif vals[1] == "false"
		  Graphics.frameskip = false
		end
	  end
    end
  end

  def initialize
    @viewport = Viewport.new(0, 0, 640, 480)
    @bg = Sprite.new(@viewport)
    @bg.bitmap = Bitmap.new(640, 480)
    @bg.bitmap.fill_rect(0, 0, 640, 480, Color.new(0, 0, 0, 128))
    @title = Sprite.new(@viewport)
    @title.bitmap = Bitmap.new(320, TITLE_MARGIN)
    @title.bitmap.font.size = 40
    @title.y = TITLE_TOP_MARGIN
    @title.x = MARGIN
    @version = Sprite.new(@viewport)
    @version.bitmap = Bitmap.new(60, 20)
    @version.x = 10
    @version.y = 450
	@version.opacity = 128
    Language.register_text_sprite(self.class.name + "_title", @title)
    Language.register_text_sprite(self.class.name + "_ver", @version)
    @data_sprites = []
    @viewport.z = 9998

	@left_hold_timer = 0
	@right_hold_timer = 0

    self.visible = false
    @index = 0
    @fade_in = false
    @fade_out = false

    @transfer_player = nil
	@visible = false
  end

  def dispose
    @bg.dispose
    @title.dispose
    @version.dispose
    @data_sprites.each do |spr|
      spr.dispose
    end
    @viewport.dispose
  end

  def open
    self.visible = true
    self.opacity = 0
    @fade_in = true
    @data = [
	         tr('BGM Volume'),
			 tr('SFX Volume'),
			 tr('Fullscreen'),
			 tr('Default movement'),
			 tr('Colorblind mode'),
			 tr('Skip Text (R)'),
			 tr('Frameskip'),
			 tr('Language'),
			 tr('Configure Controls (Press F1)'),
			]

    @index = 0
    # Load our language settings from persistent, stored differently
    @lang_index = Language::LANGUAGES.index($persistent.langcode)
    if @lang_index.nil?
        @lang_index = 0
    end

    # Create title
    @title.bitmap.clear
    @title.bitmap.draw_text(0, 0, @title.bitmap.width, @title.bitmap.height, tr("Settings"))

    # Create version string
    @version.bitmap.clear
    @version.bitmap.draw_text(0, 0, @version.bitmap.width, @version.bitmap.height, tr("POT_VERSION"))

    # Create menu options
    @data.each_with_index do |item, i|
      spr = Sprite.new(@viewport)
      spr.bitmap = Bitmap.new(400, ITEM_SPACING)
      spr.x = MARGIN
      spr.y = TITLE_MARGIN + TITLE_TOP_MARGIN + ITEM_SPACING * i
      spr.opacity = 0
      Language.register_text_sprite(self.class.name + "_option_#{i}", spr)
	    redraw_setting(spr, i)

      @data_sprites << spr
    end
  end

  def redraw_setting(spr, i)
      if @visible == false
	    return
	  end
      spr.bitmap.clear
      spr.bitmap.draw_text(0, 0, spr.bitmap.width, spr.bitmap.height, @data[i])
      spr.bitmap.draw_text(0, 0, spr.bitmap.width, spr.bitmap.height, @data[i])
	  case i
	    when 0
          spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, Audio.bgm_volume.to_s)
		when 1
          spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, Audio.sfx_volume.to_s)
		when 2 #fullscreen
		  if(Graphics.fullscreen == true)
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("ON"))
		  else
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("OFF"))
		  end
		when 3 #default movement
		  if($game_switches[251] == true)
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("RUN"))
		  else
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("WALK"))
		  end
		when 4 #colorblind mode
		  if($game_switches[252] == true)
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("ON"))
		  else
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("OFF"))
		  end
		when 5 #automash
		  if($game_switches[253] == true)
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("ON"))
		  else
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("OFF"))
		  end
		when 6 #frameskip
		  if(Graphics.frameskip == true)
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("ON"))
		  else
		    spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr("OFF"))
		  end
        when 7 # Language
          l = Language::LANGUAGES[@lang_index] rescue Language::LANGUAGES[0]
          spr.bitmap.draw_text(VALUE_MARGIN, 0, spr.bitmap.width, spr.bitmap.height, tr(l))
        end
  end


  def redraw_setting_index(i)
    if !@data_sprites.kind_of?(Array)
	  return
	end
	if @data_sprites.length - 1 < i
	  return
	end
    redraw_setting(@data_sprites[i], i)
  end


  def redraw_all_settings
    @title.bitmap.clear
    @title.bitmap.draw_text(0, 0, @title.bitmap.width, @title.bitmap.height, tr("Settings"))
    @version.bitmap.clear
    @version.bitmap.draw_text(0, 0, @version.bitmap.width, @version.bitmap.height, tr("POT_VERSION"))
    for i in 0..(@data_sprites.length - 1)
      redraw_setting_index(i)
    end
  end


  def update
    
	@version.opacity = 128
  
    if @fade_in
      self.opacity += 20
      active_spr = nil
      @data_sprites.each do |spr|
        spr.opacity += 10
        spr.opacity = 128 if spr.opacity > 128
        active_spr = spr if !active_spr && spr.x < MARGIN * 2
      end
      if active_spr
        active_spr.x += 6
        active_spr.x = MARGIN * 2 if active_spr.x > MARGIN * 2
      elsif self.opacity == 255
        @fade_in = false
      end
	  @version.opacity = 128
      return
    end

    if @fade_out
      self.opacity -= 20
      @data_sprites.each do |spr|
        spr.opacity -= 10
      end
      if self.opacity == 0
        @fade_out = false
        self.visible = false
        @data_sprites.each do |spr|
          spr.dispose
        end
        @data_sprites = []

        if @transfer_player
          $game_temp.player_transferring = true
          $game_temp.player_new_map_id = @transfer_player.id
          $game_temp.player_new_x = @transfer_player.x
          $game_temp.player_new_y = @transfer_player.y
          $game_temp.player_new_direction = @transfer_player.dir
          Graphics.freeze
          $game_temp.transition_processing = true
          $game_temp.transition_name = "black"
          @transfer_player = nil
        end
      end
      return
    end

    return if !self.visible

    # Adjust position and visibility
    @data_sprites.each_with_index do |spr, i|
      if i == @index
        if spr.x < ACTIVE_MARGIN
          spr.x += 6
          spr.x = ACTIVE_MARGIN if spr.x > ACTIVE_MARGIN
        end
        spr.opacity += 10 if spr.opacity < 255
      else
        if spr.x > MARGIN * 2
          spr.x -= 6
          spr.x = MARGIN * 2 if spr.x < MARGIN * 2
        end
        spr.opacity -= 10 if spr.opacity > 128
        spr.opacity = 128 if spr.opacity < 128
      end
    end

    if Input.trigger?(Input::UP)
      @index = (@index - 1) % @data.size
      $game_system.se_play($data_system.cursor_se)
    end
    if Input.trigger?(Input::DOWN)
      @index = (@index + 1) % @data.size
      $game_system.se_play($data_system.cursor_se)
    end

	if Input.press?(Input::LEFT)
	  @left_hold_timer += 1
	else
	  @left_hold_timer = 0
	end

	if Input.press?(Input::RIGHT)
	  @right_hold_timer += 1
	else
	  @right_hold_timer = 0
	end

	case @index
	  when 0 #bgm vol
		old_vol = Audio.bgm_volume
	    if Input.trigger?(Input::LEFT) || (Input.press?(Input::LEFT) && (@left_hold_timer >= 15))
		  @left_hold_timer -= 2
		  Audio.bgm_volume -= 1
		  if old_vol > 1
            Audio.se_play("Audio/SE/text_robot.wav", 70, (Audio.bgm_volume/2) + 75)
		    redraw_setting_index(0)
		  end
		elsif Input.trigger?(Input::RIGHT) || (Input.press?(Input::RIGHT) && (@right_hold_timer >= 15))
		  @right_hold_timer -= 2
		  Audio.bgm_volume += 1
		  if old_vol < 100
            Audio.se_play("Audio/SE/text_robot.wav", 70, (Audio.bgm_volume/2) + 75)
		    redraw_setting_index(0)
		  end
		end

	  when 1 #sfx vol
		old_vol = Audio.sfx_volume
	    if Input.trigger?(Input::LEFT) || (Input.press?(Input::LEFT) && (@left_hold_timer >= 15))
		  @left_hold_timer -= 2
		  Audio.sfx_volume -= 1
		  if old_vol > 1
            Audio.se_play("Audio/SE/text_robot.wav", 70, (Audio.sfx_volume/2) + 75)
		    redraw_setting_index(1)
		  end
		elsif Input.trigger?(Input::RIGHT) || (Input.press?(Input::RIGHT) && (@right_hold_timer >= 15))
		  @right_hold_timer -= 2
		  Audio.sfx_volume += 1
		  if old_vol < 100
            Audio.se_play("Audio/SE/text_robot.wav", 70, (Audio.sfx_volume/2) + 75)
		    redraw_setting_index(1)
		  end
		end

	  when 2 #fullscreen
	    if Input.trigger?(Input::ACTION) || Input.trigger?(Input::LEFT) || Input.trigger?(Input::RIGHT)

		  $game_system.se_play($data_system.decision_se)
          if Graphics.fullscreen == true
	        Graphics.fullscreen = false
		    $console = false
	      else
	        Graphics.fullscreen = true
		    $console = true
	      end
	      sleep(0.500)
		  redraw_setting_index(2)
	      sleep(0.500)
		end

	  when 3 #default movement
	    if Input.trigger?(Input::ACTION) || Input.trigger?(Input::LEFT) || Input.trigger?(Input::RIGHT)

		  $game_system.se_play($data_system.decision_se)
          if $game_switches[251] == true
	        $game_switches[251] = false
	      else
	        $game_switches[251] = true
	      end
		  redraw_setting_index(3)
		end

	  when 4 #colorblind mode
	    if Input.trigger?(Input::ACTION) || Input.trigger?(Input::LEFT) || Input.trigger?(Input::RIGHT)

		  $game_system.se_play($data_system.decision_se)
          if $game_switches[252] == true
	        $game_switches[252] = false
	      else
	        $game_switches[252] = true
	      end
		  redraw_setting_index(4)
		end

	  when 5 #automash
	    if Input.trigger?(Input::ACTION) || Input.trigger?(Input::LEFT) || Input.trigger?(Input::RIGHT)

		  $game_system.se_play($data_system.decision_se)
          if $game_switches[253] == true
	        $game_switches[253] = false
	      else
	        $game_switches[253] = true
	      end
		  redraw_setting_index(5)
		end

	  when 6 #frameskip
	    if Input.trigger?(Input::ACTION) || Input.trigger?(Input::LEFT) || Input.trigger?(Input::RIGHT)

		  $game_system.se_play($data_system.decision_se)
          if Graphics.frameskip == true
	        Graphics.frameskip = false
	      else
	        Graphics.frameskip = true
	      end
		  redraw_setting_index(6)
		end

	  when 7 #language
	    if Input.trigger?(Input::ACTION) || Input.trigger?(Input::RIGHT)
          @lang_index += 1
          if @lang_index >= Language::LANGUAGES.length
            @lang_index = 0
          end
          $persistent.lang = Language::LANGUAGES[@lang_index]
          redraw_all_settings()
		end
		if Input.trigger?(Input::LEFT)
		  @lang_index -= 1
		  if @lang_index < 0
		    @lang_index = Language::LANGUAGES.length - 1
		  end
          $persistent.lang = Language::LANGUAGES[@lang_index]
          redraw_all_settings()
		end
	end

    if Input.trigger?(Input::CANCEL)
      $game_system.se_play($data_system.cancel_se)
      @fade_out = true
	  save_settings
    end
  end

  # Attributes
  def visible
    return @visible
  end
  def visible=(val)
    @viewport.visible = val
	@visible = val
  end
  def opacity=(val)
    @bg.opacity = val
    @title.opacity = val
    @version.opacity = val
  end
  def opacity
    @bg.opacity
  end
end
