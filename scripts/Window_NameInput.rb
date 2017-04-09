#==============================================================================
# ** Window_NameInput
#------------------------------------------------------------------------------
#  This window is used to select text characters on the input name screen.
#==============================================================================

class NameInputMode
  attr_reader :names
  attr_reader :size
  attr_accessor :index
  attr_accessor :count

  def initialize(names, bitmap)
    @names = names
    @size = names.map { |n| bitmap.text_size(n).width }.max
    @index = 0
    @count = 0
  end

  def cycle
    @index = (@index + 1) % @count
  end

  def label
    @names[(@index + 1) % @count]
  end
end

class Window_NameInput < Window_Base
  BUTTON_BUFFER = 8
  BUTTON_PADDING = 16

  attr_accessor :index
  attr_accessor :ok_text
  attr_accessor :character_tables
  attr_accessor :table_height
  attr_accessor :char_w
  attr_accessor :char_h

  #--------------------------------------------------------------------------
  # * Object Initialization
  #--------------------------------------------------------------------------
  def initialize
    super(0, 128, 640, 352)
    self.contents = Bitmap.new(width - 32, height - 32)
    @index = 0
    @ok_text = "OK"
    @char_w = 28
    @char_h = 32
  end
  def init
    # Create dimension information
    @table_size = @character_tables.values.first.size
    @table_width = @table_size / @table_height
    @start_x = ((width - 32) - @table_width * @char_w) / 2
    @start_y = ((height - 32) - (@table_height + 1) * @char_h + BUTTON_BUFFER) / 2
    @ok_text_size = self.contents.text_size(@ok_text).width

    # Initialize mode information
    @character_tables.each_key do |key|
      key.each_with_index do |new_max, i|
        @modes[i].count = new_max + 1 if @modes[i].count < new_max + 1
      end
    end

    refresh
    update_cursor_rect
    self
  end
  #--------------------------------------------------------------------------
  # * Properties
  #--------------------------------------------------------------------------
  def set_mode_buttons(buttons)
    @modes = buttons.map { |n| NameInputMode.new(n, self.contents) }
  end
  #--------------------------------------------------------------------------
  # * Text Character Acquisition
  #--------------------------------------------------------------------------
  def character
    return current_table[@index]
  end
  #--------------------------------------------------------------------------
  # * Mode Button Cursor Positioning
  #--------------------------------------------------------------------------
  def mode_button_slot
    pos = @index % @table_width
    x = 0
    @modes.each_with_index do |mode, i|
      size = mode.size + BUTTON_PADDING * 2
      if pos * @char_w >= x && pos * @char_w <= x + size
        if (pos + 1) * @char_w - BUTTON_PADDING <= x + size || i < @modes.size - 1
          return i
        end
      end
      x += size
    end
    return -1
  end
  #--------------------------------------------------------------------------
  # * Cycle Current Mode
  #--------------------------------------------------------------------------
  def cycle_mode(slot)
    @modes[slot].cycle
    refresh
  end
  #--------------------------------------------------------------------------
  # * Get Character Table for Mode
  #--------------------------------------------------------------------------
  def current_table
    @character_tables[@modes.map { |m| m.index }]
  end
  #--------------------------------------------------------------------------
  # * Refresh
  #--------------------------------------------------------------------------
  def refresh
    self.contents.clear
	delete_text = tr("Press X to delete characters (default controls)")
	delete_width = self.contents.text_size(delete_text).width
	delete_height = self.contents.text_size(delete_text).height
	self.contents.draw_text((self.contents.width - delete_width)/2, self.contents.height - (delete_height+2), delete_width, delete_height, delete_text, 1)
    table = current_table
    @table_size.times do |i|
      x = @start_x + (i % @table_width) * @char_w
      y = @start_y + (i / @table_width) * @char_h
      self.contents.draw_text(x, y, @char_w, @char_h, table[i], 1)
    end
    # OK button
    self.contents.draw_text(@start_x + @table_width * @char_w - @ok_text_size - BUTTON_PADDING,
                            @start_y + @table_height * @char_h + BUTTON_BUFFER,
                            @ok_text_size, @char_h, @ok_text, 1)
    # Mode buttons
    x = @start_x + BUTTON_PADDING
    @modes.each do |mode|
      self.contents.draw_text(x, @start_y + @table_height * @char_h + BUTTON_BUFFER,
                              mode.size, @char_h, mode.label, 1)
      x += BUTTON_PADDING * 2 + mode.size
    end
  end
  #--------------------------------------------------------------------------
  # * Cursor Rectangle Update
  #--------------------------------------------------------------------------
  def update_cursor_rect
    if @index >= @table_size
      # Cursor is positioned on OK or mode buttons
      slot = mode_button_slot
      if slot >= 0
        # Cursor is positioned on mode button
        x = @start_x
        @modes.each_with_index do |mode, i|
          if slot == i
            self.cursor_rect.set(x, @start_y + @table_height * @char_h + BUTTON_BUFFER,
                                 mode.size + BUTTON_PADDING * 2, @char_h)
            break
          end
          x += BUTTON_PADDING * 2 + mode.size
        end
      else
        # Cursor is positioned on OK button
        self.cursor_rect.set(@start_x + @table_width * @char_w - @ok_text_size - BUTTON_PADDING * 2,
                             @start_y + @table_height * @char_h + BUTTON_BUFFER,
                             @ok_text_size + 32, @char_h)
      end
    else
      # Cursor is not positioned on OK or mode buttons
      x = @start_x + (@index % @table_width) * @char_w
      y = @start_y + (@index / @table_width) * @char_h
      self.cursor_rect.set(x, y, @char_w, @char_h)
    end
  end
  #--------------------------------------------------------------------------
  # * Frame Update
  #--------------------------------------------------------------------------
  def update
    super
    if @index >= @table_size
      # Cursor is positioned on OK or mode buttons
      if Input.repeat?(Input::LEFT)
        # If directional button pressed down is not a repeat, or
        # cursor is not positioned on the left edge
        old_slot = mode_button_slot
        if Input.trigger?(Input::LEFT) || old_slot != 0
          $game_system.se_play($data_system.cursor_se)
          if old_slot == 0
            @index = @table_size + @table_width - 1
          else
            loop do
              @index -= 1
              break if old_slot != mode_button_slot
            end
          end
        end
      end
      if Input.repeat?(Input::RIGHT)
        # If directional button pressed down is not a repeat, or
        # cursor is not positioned on the right edge
        old_slot = mode_button_slot
        if Input.trigger?(Input::RIGHT) || old_slot != -1
          $game_system.se_play($data_system.cursor_se)
          if old_slot == -1
            @index = @table_size
          else
            loop do
              @index += 1
              break if old_slot != mode_button_slot
            end
          end
        end
      end
      if Input.trigger?(Input::DOWN)
        $game_system.se_play($data_system.cursor_se)
        @index -= @table_size
      end
      if Input.repeat?(Input::UP)
        $game_system.se_play($data_system.cursor_se)
        @index -= @table_width
      end
    else
      # Cursor is not positioned on OK or mode buttons
      if Input.repeat?(Input::RIGHT)
        # If directional button pressed down is not a repeat, or
        # cursor is not positioned on the right edge
        if Input.trigger?(Input::RIGHT) || @index % @table_width < @table_width - 1
          $game_system.se_play($data_system.cursor_se)
          if @index % @table_width < @table_width - 1
            @index += 1
          else
            @index -= @table_width - 1
          end
        end
      end
      if Input.repeat?(Input::LEFT)
        # If directional button pressed down is not a repeat, or
        # cursor is not positioned on the left edge
        if Input.trigger?(Input::LEFT) || @index % @table_width > 0
          $game_system.se_play($data_system.cursor_se)
          if @index % @table_width > 0
            @index -= 1
          else
            @index += @table_width - 1
          end
        end
      end
      if Input.repeat?(Input::DOWN)
        $game_system.se_play($data_system.cursor_se)
        @index += @table_width
      end
      if Input.repeat?(Input::UP)
        # If directional button pressed down is not a repeat, or
        # cursor is not positioned on the upper edge
        if Input.trigger?(Input::UP) || @index >= @table_width
          $game_system.se_play($data_system.cursor_se)
          if @index >= @table_width
            @index -= @table_width
          else
            @index += @table_size
          end
        end
      end
    end
    update_cursor_rect
  end
end
