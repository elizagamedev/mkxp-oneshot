# The main menu window
class Window_MainMenu < Window_Selectable
  def initialize
    super(16, 16, 608, 64)

    # Set up menu options
    @commands = Array.new
    @commands << 'Travel'
    @commands << 'Notes'
    @commands << 'Settings'
    @item_max = @commands.size
    @column_max = @item_max

    # Make invisible by default
    self.visible = false
    self.active = false
    self.back_opacity = 230
    @fade_in = false
    @fade_out = false

    # Render menu
    self.contents = Bitmap.new(width - 32, 32)
    Language.register_text_sprite(self.class.name + "_contents", self.contents)
    for i in 0...@item_max
      draw_item(i, normal_color)
    end
    self.z = 9998
  end
  #--------------------------------------------------------------------------
  # * Dispose
  #--------------------------------------------------------------------------
  def dispose
    # Dispose of windows
    super
  end
  #--------------------------------------------------------------------------
  # * Draw Item
  #     index : item number
  #     color : text color
  #--------------------------------------------------------------------------
  def draw_item(index, color)
    # Set color
    self.contents.font.color = color

    # Get width of text and cap
    w = self.width / @item_max

    # Update item
    rect = Rect.new(w * index, 0, w - 32, 32)
    self.contents.fill_rect(rect, Color.new(0, 0, 0, 0))
    self.contents.draw_text(rect, tr(@commands[index]), 1)
  end
  #--------------------------------------------------------------------------
  # * Disable Item
  #     index : item number
  #--------------------------------------------------------------------------
  def disable_item(index)
    draw_item(index, disabled_color)
  end

  # Open/show the menu
  def open
    # redraw in case language has been updated
    for i in 0...@item_max
      draw_item(i, normal_color)
    end
    self.opacity = 0
    self.contents_opacity = 0
    self.visible = true
    @fade_in = true
    self.index = 0
    self.active = true
  end

  # Update
  def update
    super

    # Handle fade-in effect
    if @fade_in
      if Input.trigger?(Input::ITEMS)
        @fade_in = false
        @fade_out = true
      else
        self.opacity += 48
        self.contents_opacity += 48
        if self.contents_opacity == 255
          @fade_in = false
        end
        return
      end
    end

    # Handle fade-out effect
    if @fade_out
      self.opacity -= 48
      self.contents_opacity -= 48
      if self.contents_opacity == 0
        @fade_out = false
        self.visible = false
        self.active = false
      end
      return
    end

    # Don't do anything if not active
    if !self.active || $game_system.map_interpreter.running?
      return
    end


    # Cancel menu
    if Input.trigger?(Input::CANCEL) ||
        Input.trigger?(Input::MENU) ||
        Input.trigger?(Input::ITEMS)
      unless Input.trigger?(Input::ITEMS)
        $game_system.se_play($data_system.cancel_se)
      end
      @fade_out = true
      return
    end
	
    # Select menu item
    if Input.trigger?(Input::ACTION)
      self.active = false
      self.opacity = 127
      case @index
      when 0
        if $game_fasttravel.enabled?
          $game_system.se_play($data_system.decision_se)
          $game_temp.travel_menu_calling = true
          @fade_out = true
        else
          $game_system.se_play($data_system.buzzer_se)
          $game_temp.message_ed_text = tr("You cannot fast travel right now.")
          $game_temp.message_proc = Proc.new { self.active = true; self.opacity = 255 }
        end
      when 1
        $game_temp.common_event_id = 15
        @fade_out = true
      else
        $game_system.se_play($data_system.decision_se)
        $game_temp.window_settings_calling = true
        @fade_out = true
      end
      return
    end

  end
end
