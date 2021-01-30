#==============================================================================
# ** Window_Item
#------------------------------------------------------------------------------
#  This window displays items in possession on the item and battle screens.
#==============================================================================

class Window_Item < Window_Selectable
  COLUMN_MAX = 2
  SPECIAL_COLOR = Color.new(128, 255, 128)

  #--------------------------------------------------------------------------
  # * Object Initialization
  #--------------------------------------------------------------------------
  def initialize
    super(16, 96, 608, 224)
    @column_max = 2
    refresh
    self.index = 0
    self.back_opacity = 230
    self.z = 9998
    self.visible = false
    self.active = false

    @fade_in = false
    @fade_out = false

    @help_window = Window_Help.new
  end
  #--------------------------------------------------------------------------
  # * Get Item
  #--------------------------------------------------------------------------
  def item
    return @data[self.index]
  end
  #--------------------------------------------------------------------------
  # * Refresh
  #--------------------------------------------------------------------------
  def refresh
    if self.contents != nil
      self.contents.dispose
      self.contents = nil
    end
    @data = []
    # Add item
    for i in 1...$data_items.size
      if $game_party.item_number(i) > 0
        @data.push($data_items[i])
      end
    end
    # If item count is not 0, make a bit map and draw all items
    @item_max = @data.size
    if @item_max > 0
      self.contents = Bitmap.new(width - 32, row_max * 32)
      for i in 0...@item_max
        draw_item(i)
      end
      self.index = 0 if self.index == -1
      self.index = @item_max - 1 if self.index >= @item_max
    else
      self.index = -1
    end
  end
  #--------------------------------------------------------------------------
  # * Draw Item
  #     index : item number
  #--------------------------------------------------------------------------
  def draw_item(index)
    item = @data[index]

    # Font color
    if $game_variables[1] == item.id
      self.contents.font.color = active_item_color
    elsif item.common_event_id > 0
      self.contents.font.color = SPECIAL_COLOR
    else
      self.contents.font.color = normal_color
    end

    # Geometry
    x = index % 2 * (self.width / COLUMN_MAX)
    y = index / 2 * 32
    rect = Rect.new(x + 32, y, self.width / COLUMN_MAX - 64, 32)
    self.contents.draw_text(rect, item.name, 1)

	translation_name = "#{$persistent.langcode}/#{item.icon_name}"
    if File.exists?("Graphics/Icons/#{translation_name}.png")
      bitmap = RPG::Cache.icon(translation_name)
	else
      bitmap = RPG::Cache.icon(item.icon_name)
	end
    self.contents.stretch_blt(Rect.new(x, y, 32, 32), bitmap, Rect.new(0, 0, bitmap.width, bitmap.height), self.contents.font.color.alpha)
  end
  #--------------------------------------------------------------------------
  # * Help Text Update
  #--------------------------------------------------------------------------
  def update_help
    self.contents.clear if self.contents
    (0...@item_max).each { |i| draw_item(i) }
    @help_window.set_text(self.item == nil ? tr("No item selected.") : self.item.description, 1)
  end
  #--------------------------------------------------------------------------
  # * Update
  #--------------------------------------------------------------------------
  def update
    if $game_system.map_interpreter.running?
      was_active = self.active
      self.active = false
      super
      self.active = was_active
      return
    end
    super

    # Handle fade-in effect
    if @fade_in
      if Input.trigger?(Input::MENU)
        @fade_in = false
        @fade_out = true
      else
        self.opacity += 48
        self.contents_opacity += 48
        @help_window.opacity += 48
        @help_window.contents_opacity += 48
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
      @help_window.opacity -= 48
      @help_window.contents_opacity -= 48
      if self.contents_opacity == 0
        @fade_out = false
        self.visible = false
        self.active = false
        @help_window.visible = false
      end
      return
    end

    # Don't do anything if not active
    if !self.active
      return
    end

    # Select menu item
    if Input.trigger?(Input::ACTION)
      if @item_max == 0
        $game_system.se_play($data_system.buzzer_se)
        return
      end

      # Run common event of item if valid
      item = @data[@index]
      if item.common_event_id > 0
        $game_temp.common_event_id = item.common_event_id
        $game_system.se_play($data_system.decision_se)
        return
      end

      # Select or combine items
      item_a = $game_variables[1]
      item_b = @data[@index].id
      if item_a == item_b
        $game_variables[1] = 0
        $game_system.se_play($data_system.cancel_se)
      else
        $game_system.se_play($data_system.decision_se)
        if item_a == 0
          $game_variables[1] = item_b
          draw_item(@index)
        else
          # Draw both as active
          $game_variables[1] = item_b
          draw_item(@index)
          # Combine items
          $game_variables[1] = 0
          $game_variables[2] = Item.combine(item_a, item_b)
          $game_switches[1] = true
        end
      end
    end

    # Close menu
    if Input.trigger?(Input::CANCEL) ||
        Input.trigger?(Input::MENU) ||
        Input.trigger?(Input::ITEMS)
      unless Input.trigger?(Input::MENU)
        $game_system.se_play($data_system.cancel_se)
      end
      @fade_out = true
      return
    end
  end
  #--------------------------------------------------------------------------
  # * Open
  #--------------------------------------------------------------------------
  def open
    update_help
    refresh
	
    # Search for selected item and auto-select
    if $game_variables[1] > 0
      @data.each_with_index do |item, i|
        if $game_variables[1] == item.id
          self.index = i
          break
        end
      end
    end

    self.opacity = 0
    self.contents_opacity = 0
    self.visible = true
    @help_window.opacity = 0
    @help_window.contents_opacity = 0
    @help_window.visible = true
    self.active = true
    @fade_in = true
  end
  #--------------------------------------------------------------------------
  # * Dispose
  #--------------------------------------------------------------------------
  def dispose
    super
    @help_window.dispose
  end
end
